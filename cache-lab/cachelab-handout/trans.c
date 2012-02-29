/* 
 * trans - matrix transpose B = A^T
 */
#include <stdio.h>

#include "cachelab-tools.h"
#include "contracts.h"

// Please follow the same signature for the transpose functions
// void trans(int M, int N, int A[N][M], int B[M][N]);
// Note that:
//   A has size A[N][M]
//   B has size B[M][N]
// You have 1 KB of cache, directly mapped. Block size is 32 bytes.

// This function on the otherhand, is used to estimate the overhead of
// function call---even with just a return, we see some hits and misses.
// This function tells us the amount of overhead
char doNothing_desc[] = "A function that does nothing";
void doNothing(int M, int N, int A[N][M], int B[M][N]){
    (void)A;
    (void)B;
    return;
}

// This function checks if B is the transpose of A
int is_transpose(int M, int N, int A[N][M], int B[M][N]){
    int i, j;

    for (i = 0; i < N; i++){
        for(j = 0; j < M; ++j){
	    if (A[i][j] != B[j][i]) {
	        return 0;
	    }
	}
    }

    return 1;
}

// A simple transpose function; not optimized for cache
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N]){
//    int i, j, tmp;

    REQUIRES(M > 0);
    REQUIRES(N > 0);

    A = B;
/*    for (i = 0; i < N; i++){
        for (j = 0; j < M; j++){
            tmp = A[i][j];
            B[j][i] = tmp;
        }
      } */    

    ENSURES(is_transpose(M, N, A, B));
}



// Please fill in your solution here
// This function is evaluated by autolab to determine your score 
// for part (b)
// Please do not change this description
char transpose_submit_desc[] = "Part (b) Submit";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]){
	REQUIRES(M > 0);
	REQUIRES(N > 0);
	int i,j,k,l,temp;
	int block_size = 8;   
 //  printf("\nAddress of A = %p and Address of B = %p",(void *)A ,(void *) B);    
     
      if (( M == 32 ) || (M == 64)) {   

          if(M==32)
             block_size = 8;
          else
             block_size = 4;

	for( i = 0 ; i <N ;i += block_size) {

		for( j = 0 ; j< M ; j += block_size ) {


			for(k = i ; k < i + block_size ; k++) {

				for(l = j ; l < j + block_size ; l ++) {

					temp = A[k][l];
					B[l][k] = temp;


				}

			}

		}

	}

       }

      else {

        for(i = 0 ; i < N ; i++) {

         for(j = 0 ; j < M ; j++) { 
                temp = A[i][j];
                B[j][i] = temp;
            
                  }

}
           

                 }

	ENSURES(is_transpose(M, N, A, B));
}

////////////// Declare and test your own transpose functions here////////
//   Feel free to declare other transpose functions below. 
//   As long as you register your function with the driver, the driver
//   will print the number of misses. 
//   
//   You can register up to 100 functions with the driver. 
//
//   IMPORTANT: only the above transpose_submit() is evaluated by autolab 
//   to determine your score.
//


char trans_1_desc[] = "Part (b) 1st try";
void trans_1(int M, int N, int A[N][M], int B[M][N]){
	REQUIRES(M > 0);
	REQUIRES(N > 0);
	int i,j,k,l;
	int temp;
        int diagonal_case;
        int diagonal_case_index = 0;
        int block_size = 4;   
        int flag = 0;


	if (( M == 32 ) || (M == 64)) {   

		if(M==32)
			block_size = 8;
		else
			block_size = 8;

		for( i = 0 ; i <N ;i += block_size) {

			for( j = 0 ; j< M ; j += block_size ) {


				for(k = i ; k < i + block_size ; k++) {

                                        flag = 0;
					for(l = j ; l < j + block_size ; l ++) {
 
                                             if ((k==l)) {
                                               diagonal_case = A[k][l]; 
                                               diagonal_case_index = k;
                                                flag = 1;
                                                  }

                                              else {
						temp = A[k][l];
						B[l][k] = temp;
                                               }
					}

                                         if (flag) {
                                          
                                         B[diagonal_case_index][diagonal_case_index] = diagonal_case;

                                           }
				   

                                  }

			}

		}



	}

	else {

		for(i = 0 ; i < N ; i++) {

			for(j = 0 ; j < M ; j++) { 
				temp = A[i][j];
				B[j][i] = temp;

			}

		}


	}
	ENSURES(is_transpose(M, N, A, B));
}



////////////////////////////////////////////////////////////////////////


// This function registers all the transpose functions you define
// with the driver
void registerFunctions(){
	// Format:
	// registerTransFunction( function_name, function_description );
	registerTransFunction( doNothing, doNothing_desc);
	registerTransFunction( trans, trans_desc);
	registerTransFunction( transpose_submit, transpose_submit_desc);
	registerTransFunction( trans_1, trans_1_desc);

	//Following the above two examples, please register the transpose functions
	//you you want to test.
}


