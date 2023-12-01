#include <iostream>
#include <cmath>
#include <vector>
#include <mpi.h>

#include "common.hpp"

#define CHECK_CORRECT

int main(int argc, char **argv) {
	int size = 0, rank = 0;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Status st;

    int begin = JSIZE / size * rank, end = (rank == size - 1) ? JSIZE : JSIZE / size * (rank + 1);

    int size_j = end - begin + 1;
    std::vector<Type> tmp(ISIZE * size_j);

    for (int i = 0; i < ISIZE; ++i) {
        for (int j = begin; j < end + 1; ++j) {
            tmp[i * size_j + j - begin] = 10 * i + j;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double exec_time = -MPI_Wtime();

    int end_iteration = (rank != size - 1)?end:end-1;
    // D = {'>', '<'}
    // D_i = -1 anti
    for (int i = 1; i < ISIZE; ++i) {
        // D_j = 1 stream
        if (rank)
            MPI_Send(tmp.data() + (i - 1) * size_j, 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD);
        if (rank != size - 1)
            MPI_Recv(tmp.data() + (i - 1) * size_j + end - begin, 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &st);

        for (int j = begin; j < end_iteration; ++j)
            tmp[i * size_j + j - begin] = sin(2 * tmp[(i - 1) * size_j + j + 1 - begin]);
    }

    std::vector<Type> a;

    std::vector<int> displs;
    std::vector<int> rcounts;

    if(!rank) {
        a.resize(ISIZE * JSIZE);
        displs.resize(size);
        rcounts.resize(size);

        for (int k = 0; k < size; ++k) {
            int glob_begin = JSIZE / size * k, glob_end = (k == size - 1) ? JSIZE : JSIZE / size * (k + 1);
            rcounts[k] = glob_end - glob_begin;
            displs[k] = (k == 0) ? 0 : displs[k - 1] + rcounts[k - 1];
        }
    }

    for (int i = 0; i < ISIZE; ++i)
        MPI_Gatherv(tmp.data() + i * size_j, (!rank) ? JSIZE / size: size_j - 1, MPI_DOUBLE, a.data() + i * JSIZE, rcounts.data(), displs.data(), MPI_DOUBLE, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    exec_time += MPI_Wtime();

    if (!rank)
        std::cout << "parallel: " << exec_time << std::endl;

#ifdef CHECK_CORRECT
    if (!rank) {
        std::vector<Type> a_true(ISIZE * JSIZE);

        for (int i = 0; i < ISIZE; ++i)
            for (int j = 0; j < JSIZE; ++j)
                a_true[i * JSIZE + j] = 10 * i + j;

        exec_time = -MPI_Wtime();

        for (int i = 1; i < ISIZE; ++i)
            for (int j = 0; j < JSIZE - 1; ++j)
                a_true[i * JSIZE + j] = sin(2 * a_true[(i - 1) * JSIZE + j + 1]);

        exec_time += MPI_Wtime();

        std::cout << "seq: " << exec_time << std::endl;

        if(!std::equal(a_true.begin(), a_true.end(), a.begin(), a.end())) {
            std::cerr << "uncorrect work" << std::endl;
            // std::abort();
        }
        std::cout << "OK!" << std::endl;
    }
#endif

	MPI_Finalize();

    return 0;
}