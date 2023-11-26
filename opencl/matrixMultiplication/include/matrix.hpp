#ifndef MATRIX_HPP_
#define MATRIX_HPP_

#include <algorithm>
#include <chrono>
#include <complex>
#include <cstring>
#include <iostream>
#include <iterator>
#include <memory>
#include <random>

#include "App.hpp"

#if not defined(OPENCL_BASE) || not defined(OPENCL_CACHE)
#include <omp.h>
#endif
 

namespace matrix {
    const double EPSILON = 10E-15;
    const int maxCoefConst = 5;

    //-----------------------------------------------------------------------------------------------------

    template <typename T>
    void construct (T *p, const T &rhs)
    {
        new (p) T (rhs);
    }

    //-----------------------------------------------------------------------------------------------------

    template <typename T>
    void myDestroy (T *p) noexcept
    {
        p->~T ();
    }

    //-----------------------------------------------------------------------------------------------------

    template <typename FwdIt>
    void myDestroy (FwdIt begin, FwdIt end) noexcept
    {
        while (begin++ != end)
            myDestroy (&*begin);
    }

    //=====================================================================================================

    template <typename T = double>
    class MatrixBuf {
    protected:
        T *arr_;
        size_t size_;

        //-----------------------------------------------------------------------------------------------------

        MatrixBuf (const size_t size = 0)  // ctor
            : arr_ ((size == 0) ? nullptr : static_cast<T *> (::operator new (sizeof (T) * size))),
              size_ (size)
        {
        }

        //-----------------------------------------------------------------------------------------------------

        MatrixBuf (const MatrixBuf<T> &other) = delete;
        MatrixBuf &operator= (const MatrixBuf<T> &other) = delete;

        //-----------------------------------------------------------------------------------------------------

        MatrixBuf (MatrixBuf<T> &&other) noexcept
            : arr_ (other.arr_),
              size_ (other.size_)  // move ctor
        {
            other.arr_ = nullptr;
            other.size_ = 0;
        }

        //-----------------------------------------------------------------------------------------------------

        MatrixBuf &operator= (MatrixBuf<T> &&other) noexcept  // move operator
        {
            if (this == &other)
                return *this;

            std::swap (arr_, other.arr_);
            std::swap (size_, other.size_);

            return *this;
        }

        //-----------------------------------------------------------------------------------------------------

        ~MatrixBuf ()
        {
            myDestroy (arr_, arr_ + size_);
            ::operator delete (arr_);
        }
    };

    //=====================================================================================================

    template <typename T = double>
    class Matrix final : private MatrixBuf<T> {
    private:
        using MatrixBuf<T>::arr_;
        using MatrixBuf<T>::size_;

        size_t nRows_, nCols_;

        //-----------------------------------------------------------------------------------------------------

        int fakeSwapWithBiggest (std::vector<T *> &fakeRows, std::vector<int> &fakeCols, const size_t index, const bool param) const noexcept
        {
            int sign = 1;
            size_t withMax = index;

            if (param) {
                int realCol = fakeCols[index];
                for (size_t i = index + 1; i < nRows_; ++i)
                    if (std::abs (fakeRows[withMax][realCol]) < std::abs (fakeRows[i][realCol]))
                        withMax = i;
            }
            else {
                T *realRow = fakeRows[index];
                for (size_t i = index + 1; i < nCols_; ++i)
                    if (std::abs (realRow[fakeCols[withMax]]) < std::abs (realRow[fakeCols[i]]))
                        withMax = i;
            }

            if (withMax != index) {
                sign = -1;
                if (param)
                    std::swap (fakeRows[index], fakeRows[withMax]);
                else
                    std::swap (fakeCols[index], fakeCols[withMax]);
            }
            return sign;
        }

        //-----------------------------------------------------------------------------------------------------

        struct Proxy final {
            T *row_;
            size_t proxynCols_;

            Proxy (T *row, size_t nCols)
                : row_ (row),
                  proxynCols_ (nCols)
            {
            }

            //-----------------------------------------------------------------------------------------------------

            const T &operator[] (const size_t col) const
            {
                if (col >= proxynCols_)
                    throw std::length_error{"you tried to get data from nonexistent column"};

                return row_[col];
            }

            //-----------------------------------------------------------------------------------------------------

            T &operator[] (const size_t col)
            {
                if (col >= proxynCols_)
                    throw std::length_error{"you tried to get data from nonexistent column"};

                return row_[col];
            }
        };

        //-----------------------------------------------------------------------------------------------------

        T det (std::true_type) const
        {
            Matrix<T> support (*this);
            T det = fakeGauss ();

            return det;
        }

        //-----------------------------------------------------------------------------------------------------

        T det (std::false_type) const
        {
            Matrix<double> support (*this);
            double det = support.det ();

            return std::round (det);
        }

        //-----------------------------------------------------------------------------------------------------

        T fakeGauss () const // only for square matrix
        {
            int sign = 1;
            std::vector<T *> fakeRows;  // for fake swap rows
            std::vector<int> fakeCols;  // for fake swap cols

            for (size_t i = 0; i < nRows_; ++i) {
                fakeRows.push_back (arr_ + nCols_ * i);
                fakeCols.push_back (i);
            }

            for (size_t i = 0; i < nRows_; ++i) {
                sign *= fakeSwapWithBiggest (fakeRows, fakeCols, i, true);
                sign *= fakeSwapWithBiggest (fakeRows, fakeCols, i, false);

                if (std::abs (fakeRows[i][fakeCols[i]]) <= EPSILON)
                    return T{};

                T max = fakeRows[i][fakeCols[i]];

                for (size_t j = i + 1; j < nRows_; ++j) {
                    T del = fakeRows[j][fakeCols[i]] / max;

                    for (size_t k = i + 1; k < nCols_; ++k)  // we can not nullify the column that we will not pay attention to and thn k = i + 1 (not i)
                        fakeRows[j][fakeCols[k]] -= del * fakeRows[i][fakeCols[k]];
                }
            }

            T determinant = fakeRows[0][fakeCols[0]];
            for (size_t i = 1; i < nCols_; ++i)
                determinant *= fakeRows[i][fakeCols[i]];

            return (sign == 1) ? determinant : -determinant;
        }

        //-----------------------------------------------------------------------------------------------------

        Matrix &mul (const Matrix<T> &other)  // multiply selector
        {
            if (nCols_ != other.nRows_)
                throw std::logic_error{"The number of columns of the first matrix does not match the number of rows of the second!"};

            Matrix<T> temporary_m (nRows_, other.nCols_, T{});
            Matrix<T> B (other);
            T *tmpArr = temporary_m.arr_;

            #if defined(OPENCL_BASE)

            OpenCLApp::MatrixMultiplier<T> mul ("../sources/kernels/simple.cl");
            mul.run(arr_, B.arr_, tmpArr, nCols_, nRows_, B.nCols_);

            #elif defined(OPENCL_CACHE)

            OpenCLApp::MatrixMultiplier<T> mul ("../sources/kernels/cache.cl");
            mul.run(arr_, B.arr_, tmpArr, nCols_, nRows_, B.nCols_);

            #elif defined(BLOCK_PARALLEL)
            // block parallel variant
            constexpr size_t block_size = 64; // < sqrt(L1_sz / 3)
            T *otherArr = B.arr_;

            #pragma omp parallel for
            for (size_t j = 0; j < other.nCols_; j += block_size)
                #pragma omp parallel for
                for (size_t k = 0; k < other.nRows_; k += block_size) {
                    for (size_t i = 0; i < nRows_; ++i) {
                        size_t row1 = i * nCols_;
                        size_t tmp_row = i * temporary_m.nCols_;
                        for (size_t kk = k, size_kk = std::min(k + block_size, other.nRows_); kk < size_kk; ++kk) {
                            size_t row2 = kk * B.nCols_;
                            T tmp = arr_[row1 + kk];
                            for (size_t jj = j, size_jj = std::min(j + block_size, other.nCols_); jj < size_jj; ++jj) {
                                tmpArr[tmp_row + jj] += tmp * otherArr[row2 + jj];
                            }
                        }
                    }
                }

            #else
            // base seq variant

            T *otherArr = B.arr_;
            for (size_t i = 0; i < nRows_; ++i) {
                size_t row1 = i * nCols_;
                size_t tmp_row = i * temporary_m.nCols_;
                for (size_t k = 0; k < other.nRows_; ++k) {
                    size_t row2 = k * B.nCols_;
                    T tmp = arr_[row1 + k];
                    for (size_t j = 0; j < other.nCols_; ++j)
                        tmpArr[tmp_row + j] += tmp * otherArr[row2 + j];
                }
            }
            #endif

            std::swap (temporary_m, *this);
            return *this;
        }

        //-----------------------------------------------------------------------------------------------------

        template <typename distrType>
        static Matrix<T> rndMatr (const size_t size, const int det)
        {
            const int maxCoef = maxCoefConst;  // I don't want big coef
            unsigned seed = std::chrono::system_clock::now ().time_since_epoch ().count ();
            std::mt19937 generator (seed);

            int absDet = std::abs (det);
            distrType randUpperTriangle (-absDet, absDet);

            Matrix<T> rndMtrx (size, size);

            for (size_t i = 0; i < size - 1; ++i) {
                rndMtrx[i][i] = 1;

                for (size_t j = i + 1; j < size; ++j)
                    rndMtrx[i][j] = randUpperTriangle (generator);
            }

            rndMtrx[size - 1][size - 1] = det;

            distrType coefGen (-maxCoef, maxCoef);  // I don't want big coef
            int randCoef;

            for (size_t i = 1; i < size; ++i) {
                randCoef = coefGen (generator);

                for (size_t j = 0; j < size; ++j)
                    rndMtrx[i][j] += randCoef * rndMtrx[0][j];
            }

            for (size_t i = 0; i < size - 1; ++i) {
                randCoef = coefGen (generator);

                for (size_t j = 0; j < size; ++j)
                    rndMtrx[j][i] += randCoef * rndMtrx[j][size - 1];
            }

            return rndMtrx;
        }

    public:
        Matrix (const size_t nRows = 0, const size_t nCols = 0)  // ctor
            : MatrixBuf<T> (nRows * nCols),
              nRows_ (nRows),
              nCols_ (nCols)
        {
        }

        //-----------------------------------------------------------------------------------------------------

        Matrix (const size_t nRows, const size_t nCols, T val)  // ctor
            : MatrixBuf<T> (nRows * nCols),
              nRows_ (nRows),
              nCols_ (nCols)
        {
            std::fill_n (arr_, nRows_ * nCols_, val);
        }

        //-----------------------------------------------------------------------------------------------------

        Matrix (const Matrix<T> &other)
            : MatrixBuf<T> (other.nRows_ * other.nCols_),
              nRows_ (other.nRows_),
              nCols_ (other.nCols_)  // copy ctor
        {
            size_ = 0;
            while (size_ < other.size_) {
                construct<T> (arr_ + size_, other.arr_[size_]);
                ++size_;
            }
        }

        //-----------------------------------------------------------------------------------------------------

        template <typename otherT>
        Matrix (const Matrix<otherT> &other)
            : MatrixBuf<T> (other.getNRows () * other.getNCols ()),
              nRows_ (other.getNRows ()),
              nCols_ (other.getNCols ())  // copy ctor from other type
        {
            size_t otherSz = other.getNRows () * other.getNCols ();
            size_ = 0;
            while (size_ < otherSz) {
                construct<T> (arr_ + size_, other[size_ / nCols_][size_ % nRows_]);
                ++size_;
            }
        }

        //-----------------------------------------------------------------------------------------------------

        Matrix (Matrix<T> &&other) noexcept = default;             // move ctor
        Matrix &operator= (Matrix<T> &&other) noexcept = default;  // move operator

        //-----------------------------------------------------------------------------------------------------

        Matrix &operator= (const Matrix<T> &other)  // assignment operator
        {
            Matrix tmp (other);
            std::swap (tmp, *this);
            return *this;
        }

        //-----------------------------------------------------------------------------------------------------

        ~Matrix ()  // dtor
        {
        }

        //-----------------------------------------------------------------------------------------------------

        size_t getNRows () const noexcept
        {
            return nRows_;
        }

        //-----------------------------------------------------------------------------------------------------

        size_t getNCols () const noexcept
        {
            return nCols_;
        }

        //-----------------------------------------------------------------------------------------------------

        T det () const
        {
            if (nRows_ != nCols_)
                throw std::logic_error{"matrix is not square! I don't know what are you want from me"};

            if constexpr (std::is_floating_point<T> () || std::is_same<T, std::complex<double>> ())  // I really want the matrix to work at least for std::complex<double>
                return det (std::true_type ());
            else
                return det (std::false_type ());
        }

        //-----------------------------------------------------------------------------------------------------

        static Matrix<T> randomMatrix (const size_t size, const int det)
        {
            static_assert (std::is_fundamental<T> (), "I can't generate random matrix with this type");

            if constexpr (std::is_integral<T> ())
                return rndMatr<std::uniform_int_distribution<T>> (size, det);
            else
                return rndMatr<std::uniform_real_distribution<T>> (size, det);
        }

        //=====================================================================================================

        Proxy operator[] (const size_t row) const
        {
            if (row >= nRows_)
                throw std::length_error{"you tried to get data from nonexistent row"};

            return Proxy (arr_ + nCols_ * row, nCols_);
        }

        //=====================================================================================================

        void dump (std::ostream &out) const
        {
            for (size_t i = 0; i < nRows_; ++i) {
                size_t row = nCols_ * i;
                for (size_t j = 0; j < nCols_; ++j)
                    out << arr_[row + j] << " ";

                if (i + 1 != nRows_)
                    out << std::endl;
            }
        }

        //-----------------------------------------------------------------------------------------------------

        void input (std::istream &in)
        {
            for (size_t i = 0, size = nRows_ * nCols_; i < size; ++i)
                in >> arr_[i];
        }

        //-----------------------------------------------------------------------------------------------------

        bool equal (const Matrix<T> &other) const
        {
            if (nRows_ != other.nRows_ || nCols_ != other.nCols_)
                return false;

            return std::equal (arr_, arr_ + nRows_ * nCols_, other.arr_);
        }

        //-----------------------------------------------------------------------------------------------------

        Matrix &transpose ()
        {
            Matrix<T> trans (nCols_, nRows_);
            T *transArr = trans.arr_;

            for (size_t i = 0; i < nRows_; ++i) {
                size_t row = i * nCols_;
                for (size_t j = 0; j < nCols_; ++j)
                    transArr[j * nRows_ + i] = arr_[row + j];
            }

            std::swap (trans, *this);

            return *this;
        }

        //-----------------------------------------------------------------------------------------------------

        Matrix &operator*= (const Matrix<T> &other)
        {
            return mul (other);
        }

        //-----------------------------------------------------------------------------------------------------

        Matrix &operator+= (const Matrix<T> &other)
        {
            if (getNCols () != other.getNCols () || getNRows () != other.getNRows ())
                throw std::logic_error{"I can't add these matrix because their size isn't equal"};

            T *otherArr = other.arr_;
            for (int i = 0; i < nRows_; ++i) {
                size_t row = i * nCols_;
                for (int j = 0; j < nCols_; ++j)
                    arr_[row + j] += otherArr[row + j];
            }

            return (*this);
        }

        //-----------------------------------------------------------------------------------------------------

        template <typename delType>
        Matrix &operator/= (delType del)
        {
            if (del == delType{})
                throw std::logic_error{"Floating point exception\n"};

            for (int i = 0; i < nRows_; ++i) {
                size_t row = i * nCols_;
                for (int j = 0; j < nCols_; ++j)
                    arr_[row + j] /= del;
            }

            return (*this);
        }
    };

    //=====================================================================================================

    template <typename T = double>
    std::ostream &operator<< (std::ostream &out, const Matrix<T> &matrix)
    {
        matrix.dump (out);
        return out;
    }

    //-----------------------------------------------------------------------------------------------------

    template <typename T = double>
    std::istream &operator>> (std::istream &in, Matrix<T> &matrix)
    {
        matrix.input (in);
        return in;
    }

    //-----------------------------------------------------------------------------------------------------

    template <typename T = double>
    Matrix<T> operator+ (const Matrix<T> &first, const Matrix<T> &second)
    {
        Matrix<T> result (first);

        return result += second;
    }

    //-----------------------------------------------------------------------------------------------------

    template <typename T = double>
    bool operator== (const Matrix<T> &first, const Matrix<T> &second)
    {
        return first.equal (second);
    }

    //-----------------------------------------------------------------------------------------------------

    template <typename T = double>
    Matrix<T> operator~ (const Matrix<T> &matrix)
    {
        Matrix<T> other (matrix);

        return other.transpose ();
    }

    //-----------------------------------------------------------------------------------------------------

    template <typename T = double>
    Matrix<T> operator* (const Matrix<T> &first, const Matrix<T> &second)
    {
        Matrix<T> copy (first);
        return copy *= second;
    }

}  // namespace matrix

#endif
