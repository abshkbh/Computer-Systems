/* Andrew Id - abhisheb Name - Abhishek Bhardwaj 
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
    int i, j, tmp;

    REQUIRES(M > 0);
    REQUIRES(N > 0);

   for (i = 0; i < N; i++){
        for (j = 0; j < M; j++){
            tmp = A[i][j];
            B[j][i] = tmp;
        }
      }     

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
	int i,j,k,l,x_index,y_index;
        int block_size = 4;   
        int flag = 0;
        int temp_x,temp_y; 

       
	if(M==32) {

		block_size = 8;

		for( i = 0 ; i < M ;i += block_size) {

			for( j = 0 ; j < N; j += block_size ) {


				for(k = i ; k < i + block_size ; k++) {

					flag = 0;
					for(l = j ; l < j + block_size ; l ++) {

						if ((k==l)) {
							temp_x = k;
							flag = 1;
						}

						else {
							B[l][k] = A[k][l];
						}
					}

					if (flag) {

						B[temp_x][temp_x] = A[temp_x][temp_x];

					}


				}

			}

		}

	}

else if (M == 64) {

	block_size = 4;

	for( i = 0 ; i < M ;i +=  (block_size*2)) {

		for( j = 0 ; j < N ; j +=  (block_size*2) ) {


                          x_index = i + (M/16); 
                          y_index = j; 


			for(k = x_index ; k < x_index + block_size ; k++) {

				flag = 0;
				for(l = y_index ; l < y_index + block_size ; l ++) {

					if (((k-x_index)==(l-y_index))) {
						temp_x = k;
                                                temp_y = l;
                                                flag = 1;
					}

					else {
						B[l][k]   = A[k][l];
					}
				}

				if (flag) {

					B[temp_y][temp_x] = A[temp_x][temp_y];

				}

			}


                          x_index = i; 
                          y_index = j; 

			for(k = x_index ; k < x_index + block_size ; k++) {

				flag = 0;
				for(l = y_index ; l < y_index + block_size ; l ++) {

					if (((k-x_index)==(l-y_index))) {
				                temp_x = k;
                                                temp_y = l;
                                      		flag = 1;
					}


					else {
						B[l][k] = A[k][l];
					}
				}

				if (flag) {

					B[temp_y][temp_x] = A[temp_x][temp_y];

				}

			}

			x_index = i; 
			y_index = j + (N/16); 

			for(k = x_index ; k < x_index + block_size ; k++) {

				flag = 0;
				for(l = y_index ; l < y_index + block_size ; l ++) {


					if (((k-x_index)==(l-y_index))) {
						temp_x = k;
                                                temp_y = l;
                                                flag = 1;
					}


					else {
						B[l][k] = A[k][l];
					}
				}

				if (flag) {

					B[temp_y][temp_x] = A[temp_x][temp_y];

				}

			}


			x_index = i + (N/16); 
			y_index = j + (N/16); 

			for(k = x_index ; k < x_index + block_size ; k++) {

				flag = 0;
				for(l = y_index ; l < y_index + block_size ; l ++) {

					if (((k-x_index)==(l-y_index))) {
					        temp_x = k;
                                                temp_y = l;
                                            	flag = 1;
					}

					else {
						B[l][k] = A[k][l];
					}
				}

				if (flag) {

					B[temp_y][temp_x] = A[temp_x][temp_y];

				}

			}


		}


	}


}


else {
	block_size = 16;

	for( i = 0 ; i < N ;i += block_size) {

		for( j = 0 ; j < M ; j += block_size ) {



			for(k = i ; (k < (i + block_size)) && (k<N) ; k++) {


				for(l = j ; (l < (j + block_size)) && (l<M) ; l ++) {

					B[l][k] = A[k][l];
				
				}



			}

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






////////////////////////////////////////////////////////////////////////

char transpose_1_desc[] = "Part (b) Submit Try";
void transpose_1(int M, int N, int A[N][M], int B[M][N]){
	REQUIRES(M > 0);
	REQUIRES(N > 0);
	int i,j,k,l,x_index,y_index;
	int temp;
	int diagonal_case;
	int diagonal_case_index_x = 0;
	int diagonal_case_index_y = 0;
	int block_size = 4;   
	int big_block_size = 8;
	int flag = 0;


	if(M==32) {

		block_size = 8;

		for( i = 0 ; i < M ;i += block_size) {

			for( j = 0 ; j < N; j += block_size ) {


				for(k = i ; k < i + block_size ; k++) {

					flag = 0;
					for(l = j ; l < j + block_size ; l ++) {

						if ((k==l)) {
							diagonal_case = A[k][l]; 
							diagonal_case_index_x = k;
							flag = 1;
						}

						else {
							temp = A[k][l];
							B[l][k] = temp;
						}
					}

					if (flag) {

						B[diagonal_case_index_x][diagonal_case_index_x] = diagonal_case;

					}


				}

			}

		}

	}

	else if (M == 64) {

		block_size = 4;
		big_block_size = 8;

		for( i = 0 ; i < M ;i += big_block_size) {

			for( j = 0 ; j < N ; j += big_block_size ) {


				x_index = i + (M/16); 
				y_index = j; 


				for(k = x_index ; k < x_index + block_size ; k++) {

					flag = 0;
					for(l = y_index ; l < y_index + block_size ; l ++) {

						if ((k%4 == 0) && (l%4==0)) {
							//			diagonal_case = A[k][l]; 
							diagonal_case_index_x = k;
							diagonal_case_index_y = l;
							flag = 1;
						}

						else {
							temp = A[k][l];
							B[l][k] = temp;
						}
					}

					if (flag) {

						B[diagonal_case_index_y][diagonal_case_index_x] = A[diagonal_case_index_x][diagonal_case_index_y];

					}

				}


				x_index = i; 
				y_index = j; 

				for(k = x_index ; k < x_index + block_size ; k++) {

					flag = 0;
					for(l = y_index ; l < y_index + block_size ; l ++) {

						if ((k%4 == 0) && (l%4==0)) {
							//		diagonal_case = A[k][l]; 
							diagonal_case_index_x = k;
							diagonal_case_index_y = l;
							flag = 1;
						}

						else {
							temp = A[k][l];
							B[l][k] = temp;
						}
					}

					if (flag) {

						B[diagonal_case_index_y][diagonal_case_index_x] = A[diagonal_case_index_x][diagonal_case_index_y];

						//B[diagonal_case_index_y][diagonal_case_index_x] = diagonal_case;

					}

				}

				x_index = i; 
				y_index = j + (N/16); 

				for(k = x_index ; k < x_index + block_size ; k++) {

					flag = 0;
					for(l = y_index ; l < y_index + block_size ; l ++) {


						if ((k%4 == 0) && (l%4==0)) {
							//	diagonal_case = A[k][l]; 
							diagonal_case_index_x = k;
							diagonal_case_index_y = l;
							flag = 1;
						}

						else {
							temp = A[k][l];
							B[l][k] = temp;
						}
					}

					if (flag) {


						B[diagonal_case_index_y][diagonal_case_index_x] = A[diagonal_case_index_x][diagonal_case_index_y];
						//B[diagonal_case_index_y][diagonal_case_index_x] = diagonal_case;

					}

				}


				x_index = i + (N/16); 
				y_index = j + (N/16); 

				for(k = x_index ; k < x_index + block_size ; k++) {

					flag = 0;
					for(l = y_index ; l < y_index + block_size ; l ++) {



						if ((k%4 == 0) && (l%4==0)) {
							//			diagonal_case = A[k][l]; 
							diagonal_case_index_x = k;
							diagonal_case_index_y = l;
							flag = 1;
						}

						else {
							temp = A[k][l];
							B[l][k] = temp;
						}
					}

					if (flag) {


						B[diagonal_case_index_y][diagonal_case_index_x] = A[diagonal_case_index_x][diagonal_case_index_y];
						//				B[diagonal_case_index_y][diagonal_case_index_x] = diagonal_case;

					}

				}


			}


		}


	}


	else {
		block_size = 16;

		for( i = 0 ; i < N ;i += block_size) {

			for( j = 0 ; j < M ; j += block_size ) {



				for(k = i ; (k < (i + block_size)) && (k<N) ; k++) {


					for(l = j ; (l < (j + block_size)) && (l<M) ; l ++) {

						temp = A[k][l];
						B[l][k] = temp;
					}



				}

			}

		}


	}

	ENSURES(is_transpose(M, N, A, B));


}

// This function registers all the transpose functions you define
// with the driver
void registerFunctions(){
	// Format:
	// registerTransFunction( function_name, function_description );
	registerTransFunction( doNothing, doNothing_desc);
	registerTransFunction( trans, trans_desc);
	registerTransFunction( transpose_submit, transpose_submit_desc);
	registerTransFunction( transpose_1, transpose_1_desc);

	//Following the above two examples, please register the transpose functions
	//you you want to test.
}


