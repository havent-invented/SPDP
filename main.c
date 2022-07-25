#include <mpi.h>
#include <stdlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double *A;
double *B;
double *C;

int min(int a, int b){
    if (a > b) {
        return b;
        } else {
        return a;
    }
}

int CPU_COUNT = 4;

void MatrixMultiplicationMPI_V(double *A, double *B, double *C, int Size) {
    MPI_Init(NULL, NULL);
    int ProcRank;
    int max_threads;
    MPI_Comm_size(MPI_COMM_WORLD, &max_threads);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    int num_threads = max_threads;
    int cur_thread = ProcRank;
    A = malloc(Size*Size*sizeof(double));
    B = malloc(Size*Size*sizeof(double));
    C = malloc(Size*Size*sizeof(double));
    if (ProcRank == 0) {
        A = malloc(Size*Size*sizeof(double));
        B = malloc(Size*Size*sizeof(double));
        C = malloc(Size*Size*sizeof(double));
         
        MPI_Alloc_mem(sizeof(double)*Size*Size, MPI_INFO_NULL, A); 
        MPI_Alloc_mem(sizeof(double)*Size*Size, MPI_INFO_NULL, B); 
        MPI_Alloc_mem(sizeof(double)*Size*Size, MPI_INFO_NULL, C); 
        for(int i = 0; i < Size; i++) {
            for(int j = 0; j < Size; j++) {
                A[i*Size + j] = 32;//j;
                B[i*Size + j] = 32;//i*Size + j;
                C[i*Size + j] = 0;
            }
        }
        for(int i = 1; i < max_threads; i++){
            MPI_Ssend(A, Size*Size, MPI_DOUBLE , i , 1 , MPI_COMM_WORLD);
            MPI_Ssend(B, Size*Size, MPI_DOUBLE , i , 2 , MPI_COMM_WORLD);
            MPI_Ssend(C, Size*Size, MPI_DOUBLE , i , 3 , MPI_COMM_WORLD);
        }

        //MPI_Free_mem(f); 
       //transpose_linear(B, Size);
    } else {
        MPI_Recv( A, Size*Size, MPI_DOUBLE , MPI_ANY_SOURCE , 1, MPI_COMM_WORLD , MPI_STATUS_IGNORE);
        MPI_Recv( B, Size*Size, MPI_DOUBLE , MPI_ANY_SOURCE , 2, MPI_COMM_WORLD , MPI_STATUS_IGNORE);
        MPI_Recv( C, Size*Size, MPI_DOUBLE , MPI_ANY_SOURCE , 3, MPI_COMM_WORLD , MPI_STATUS_IGNORE);
        
        int upper_limit = min(Size, (cur_thread+1) * Size / num_threads);
        for (int i = cur_thread * Size / num_threads; i < upper_limit; i++) {
            for (int j=0; j < Size; j++) {
                for (int k=0; k < Size; k++) {
                    C[i * Size + j] += A[i * Size + k] * B[j * Size + k];
                }
            }
        } 
    }
    if (ProcRank == 0) {
        for(int i = 0; i < Size; i++)
        {
            A[i] = 9;
        }
    }
    
    if (ProcRank != 0) {
        int upper_limit = min(Size, (cur_thread+1) * Size / num_threads) * Size;
        int lower_limit = (cur_thread * Size / num_threads) * Size;
        MPI_Ssend(C +  lower_limit, upper_limit-lower_limit , MPI_DOUBLE , 0 , ProcRank , MPI_COMM_WORLD);
    } else {
        for(int cur_th = 1; cur_th < max_threads; cur_th++) {
        int count;
        MPI_Status status;
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv( A, Size*Size, MPI_DOUBLE , cur_th , MPI_ANY_TAG, MPI_COMM_WORLD , MPI_STATUS_IGNORE);
        }
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    
    
    if (ProcRank == 3) {
        for(int i = 0; i < Size; i++)
        {
            printf("\n %f \n ", A[i]);
        }
    }
    if (ProcRank == 0) {
        for(int i = 0; i < Size; i++) {
            for (int j = 0; j < Size; j++ ) {
                printf("%f", C[i*Size + j]);
            }
            printf("\n");
        }
        MPI_Free_mem(A);
        MPI_Free_mem(B);
        MPI_Free_mem(C);
    }
    MPI_Finalize();
}

FILE *logfile;
int main(int argc, char **argv) {
    MatrixMultiplicationMPI_V(A, B, C, 8);
}