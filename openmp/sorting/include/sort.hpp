#ifndef __SORT_HPP__
#define __SORT_HPP__

#include <omp.h>
#include <vector>
#include <algorithm>
#include <iostream>

template <typename T>
void merge(std::vector<T> &src, int begin, int mid, int mid_begin, int end, std::vector<T> &dest, int off) {
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
    int s = std::distance(src.begin(), std::lower_bound(src.begin() + mid_begin, src.begin() + end, src[r]));
    int t = off + (r - begin) + (s - mid_begin);

    dest[t] = src[r];

    #pragma omp parallel
    {
        #pragma omp single
        {
            #pragma omp task
                merge(src, begin, r, mid_begin, s, dest, off);

            #pragma omp task
                merge(src, r + 1, mid, s, end, dest, t+1);
        }

        #pragma omp taskwait
    }
}

template <typename T>
void mergeSort(std::vector<T> &src, int begin, int end, std::vector<T> &dest, int off) {
    int len = end - begin;

    if(len == 1) {
        dest[off] = src[begin];
        return;
    }
    std::vector<T> tmp_vec(len);

    int mid = (begin + end) / 2;
    int tmp_mid = mid - begin;
    #pragma omp parallel
    {
        #pragma omp single
        {
            #pragma omp task
                mergeSort(src, begin, mid, tmp_vec, 0);

            #pragma omp task
                mergeSort(src, mid, end, tmp_vec, tmp_mid);
        }

        #pragma omp taskwait
    }

    // std::merge(tmp_vec.begin(), tmp_vec.begin() + tmp_mid, tmp_vec.begin() + tmp_mid, tmp_vec.end(), dest.begin() + off);
    merge(tmp_vec, 0, tmp_mid, tmp_mid, len, dest, off);
}

template <typename T>
void sort(std::vector<T> &v) {
    std::vector<T> other(v.size());
    mergeSort(v, 0, v.size(), other, 0);

    std::swap(v, other);
}


#endif