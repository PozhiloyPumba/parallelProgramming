#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

void it_is_me (int id, int countPrev) {
    printf("hello from thread: %d, level %d, count Threads on prev levels = %d\n", 
            id, omp_get_level(), countPrev);
}

// #define SYNC

int main() {
    omp_set_dynamic(0);
    omp_set_max_active_levels(4);
    int countCur[] = {0, 0, 0, 0};
    int levelTeamSizes[] = {4, 3, 2, 2};

    #pragma omp parallel num_threads(levelTeamSizes[0])
    {
        int index = omp_get_thread_num();
        it_is_me(omp_get_thread_num(), 0);

        #ifdef SYNC
        #pragma omp atomic update
            ++countCur[0];

        int allDone = 0;
        while ( !allDone ) {
            allDone = (countCur[0] == levelTeamSizes[0]);
        }
        #endif

        #pragma omp parallel num_threads(levelTeamSizes[1]) firstprivate(index)
        {
            index = index * levelTeamSizes[1] + omp_get_thread_num();

            #ifdef SYNC
            it_is_me(index, countCur[0]);
            #pragma omp atomic update
                ++countCur[1];

            allDone = 0;
            while ( !allDone ) {
                allDone = (countCur[1] == levelTeamSizes[0] * levelTeamSizes[1]);
            }
            #else
            it_is_me(index, levelTeamSizes[0]);
            #endif

            #pragma omp parallel num_threads(levelTeamSizes[2]) firstprivate(index) 
            {
                index = index * levelTeamSizes[2] + omp_get_thread_num();

                #ifdef SYNC
                it_is_me(index, countCur[0] + countCur[1]);
                #pragma omp atomic update
                    ++countCur[2];

                allDone = 0;
                while ( !allDone ) {
                    allDone = (countCur[2] == levelTeamSizes[0] * levelTeamSizes[1] * levelTeamSizes[2]);
                }
                #else
                it_is_me(index, levelTeamSizes[0] + levelTeamSizes[0] * levelTeamSizes[1]);
                #endif

                #pragma omp parallel num_threads(levelTeamSizes[3])  firstprivate(index)
                {
                    index = index * levelTeamSizes[3] + omp_get_thread_num();

                    #ifdef SYNC
                    it_is_me(index, countCur[0] + countCur[1] + countCur[2]);
                    #else
                    it_is_me(index, levelTeamSizes[0] + levelTeamSizes[0] * levelTeamSizes[1] + 
                             levelTeamSizes[0] * levelTeamSizes[1] * levelTeamSizes[2]);
                    #endif
                }
            }
        }
    }

    return 0;

}