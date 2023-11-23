#ifndef __SORT_HPP__
#define __SORT_HPP__

#include <omp.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cstring>

namespace Sort {

template <typename T>
void outPlaceInsertionSort(std::vector<T> &src, int begin, int end, std::vector<T> &dest, int off) {
    int len = end - begin;
    std::memcpy(dest.data() + off, src.data() + begin, len * sizeof(T));
    for (int i = off + 1, end = off + len; i < end; ++i) {
        int j = i;
        while((j > off) && (dest[j] < dest[j - 1])) {
            std::swap(dest[j], dest[j - 1]);
            --j;
        }
    }
}

template <typename T>
void mergeSequential(std::vector<T> &src, int begin, int mid, int mid_begin, int end, std::vector<T> &dest, int off) {
    while (mid_begin < end) {
        if (begin >= mid) {
            std::memcpy(dest.data() + off, src.data() + mid_begin, (end - mid_begin) * sizeof(T));
            return;
        }
        if(src[begin] < src[mid_begin]) {
            dest[off++] = src[begin++];
        }
        else {
            dest[off++] = src[mid_begin++];
        }
    }
    std::memcpy(dest.data() + off, src.data() + begin, (mid - begin) * sizeof(T));
    return;
}

template <typename T>
void mergeParallel(std::vector<T> &src, int begin, int mid, int mid_begin, int end, std::vector<T> &dest, int off, int level) {
    if (level >= 8) {
        mergeSequential(src, begin, mid, mid_begin, end, dest, off);
    }
    else {
        int m = mid - begin,
            n = end - mid_begin;

        if (m < n) {
            std::swap(begin, mid_begin);
            std::swap(mid, end);
            std::swap(m, n);
        }

        if(m <= 0)
            return;

        int r = (begin + mid) / 2;
        int s = static_cast<int>(std::lower_bound(src.begin() + mid_begin, src.begin() + end, src[r]) - src.begin());
        int t = off + (r - begin) + (s - mid_begin);

        dest[t] = src[r];
        #pragma omp parallel
        {
            #pragma omp single
            {
                #pragma omp task
                    mergeParallel(src, begin, r, mid_begin, s, dest, off, level + 1);

                #pragma omp task
                    mergeParallel(src, r + 1, mid, s, end, dest, t+1, level + 1);
            }
        }
    }
}

template <typename T>
void mergeSort(std::vector<T> &src, int begin, int end, std::vector<T> &dest, int off, int level) {
    int len = end - begin;

    #ifndef OPTIMIZED 
    if (len == 1) {
        dest[off] = src[begin];
        return;

    }
    #else
    if (len <= 16) {
        outPlaceInsertionSort(src, begin, end, dest, off);
        return;
    }
    #endif

    std::vector<T> tmp_vec(len);

    int mid = (begin + end) / 2;
    int tmp_mid = mid - begin;

    if (level >= 8) {
        mergeSort(src, begin, mid, tmp_vec, 0, level + 1);
        mergeSort(src, mid, end, tmp_vec, tmp_mid, level + 1);
    }
    else {
        #pragma omp parallel
        {
            #pragma omp single
            {
                #pragma omp task
                    mergeSort(src, begin, mid, tmp_vec, 0, level + 1);

                #pragma omp task
                    mergeSort(src, mid, end, tmp_vec, tmp_mid, level + 1);
            }
        }
    }

    // std::merge(tmp_vec.begin(), tmp_vec.begin() + tmp_mid, tmp_vec.begin() + tmp_mid, tmp_vec.end(), dest.begin() + off);
    mergeParallel(tmp_vec, 0, tmp_mid, tmp_mid, len, dest, off, level + 1);
}

template <typename T>
void sort(std::vector<T> &v) {
    std::vector<T> other(v.size());
    omp_set_nested(1);
    mergeSort(v, 0, v.size(), other, 0, 0);

    std::swap(v, other);
}

}

#endif
