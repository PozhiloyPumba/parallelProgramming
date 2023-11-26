#ifndef APP_HPP__
#define APP_HPP__

#include "IOpenCLApp.hpp"

namespace OpenCLApp {

#ifndef LOCAL_SIZE
#define LOCAL_SIZE 64
#endif

template <typename T>
class MatrixMultiplier final: public IOpenclApp {

    std::string kernel_;

    using mul_t = cl::KernelFunctor<cl::Buffer, cl::Buffer, cl::Buffer, cl_int, cl_int>;

public:
    MatrixMultiplier(const std::string &kernel_source) : IOpenclApp() {
        kernel_ = getKernelExtension<T> () +
                    "#define KERNEL_TYPE " + getTypeName<T> () + "\n";

        std::ifstream in;
        in.exceptions (std::ifstream::failbit | std::ifstream::badbit);

        in.open (kernel_source);

        while (!in.eof ()) {
            std::string newLine;
            std::getline (in, newLine);

            kernel_ += newLine + "\n";
        }

        in.close ();
    }

    void run(const T *A, const T *B, T *C, int A_nCols, int A_nRows, int B_nCols);
};


template <typename T>
void MatrixMultiplier<T>::run(const T *A, const T *B, T *C, int A_nCols, int A_nRows, int B_nCols)
{
    profiling_.clear ();

    cl::Buffer clDataA (context_, CL_MEM_READ_ONLY, A_nCols * A_nRows * sizeof (T));
    cl::copy (queue_, A, A + A_nCols * A_nRows, clDataA);

    cl::Buffer clDataB (context_, CL_MEM_READ_ONLY, B_nCols * A_nCols * sizeof (T));
    cl::copy (queue_, B, B + B_nCols * A_nCols, clDataB);

    cl::Buffer clDataC (context_, CL_MEM_WRITE_ONLY, A_nRows * B_nCols * sizeof (T));
    cl::copy (queue_, C, C + A_nRows * B_nCols, clDataC);

    try {
        cl::NDRange GlobalRange (A_nRows, B_nCols);
        cl::NDRange LocalRange = getLocalRangeOptimal(GlobalRange, LOCAL_SIZE);

        // a little bit magic for getting tileSize in iterated space and stay in LOCAL_SIZE everywhere 
        cl::size_type tileSize = std::min(LocalRange[0], LocalRange[1]) + 1;
        while (A_nCols % (--tileSize));

        std::string kernel_cur = "#define TILE_SIZE_K " + std::to_string(tileSize) + "\n" +
                                 "#define TILE_SIZE_ROW_C " + std::to_string(LocalRange[1]) + "\n" +
                                 "#define TILE_SIZE_COL_C " + std::to_string(LocalRange[0]) + "\n" +
                                 kernel_;

        cl::Program program (context_, kernel_cur, true);
        mul_t kernel (program, "matmul");

        cl::EnqueueArgs Args (queue_, GlobalRange, LocalRange);
        cl::Event evt = kernel (Args, clDataA, clDataB, clDataC, A_nCols, B_nCols);

        profiling_.push_back (evt);

        evt.wait ();
    }
    catch (cl::BuildError &err) {
        std::cerr << "OCL BUILD ERROR: " << err.err () << ":" << err.what ()
                    << std::endl;
        std::cerr << "-- Log --\n";
        for (auto e : err.getBuildLog ())
            std::cerr << e.second;
        std::cerr << "-- End log --\n";
        return;
    }
    catch (cl::Error &e) {
        std::cerr << "OCL ERROR: " << e.err () << ":" << e.what ()
                    << std::endl;
        return;
    }

    cl::copy (queue_, clDataC, C, C + A_nRows * B_nCols);
}

}

#endif