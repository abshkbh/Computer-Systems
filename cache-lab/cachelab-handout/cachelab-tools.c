#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cachelab-tools.h"

trans_func_t func_list[MAX_TRANS_FUNCS];
int func_counter = 0; 


void printCachesimResults(unsigned int num_hit, 
                          unsigned int num_miss, unsigned int num_eviction){
  printf("Cachesim Results:   Hits: %d  Misses: %d  Evictions: %d\n", 
          num_hit, num_miss, num_eviction);
  FILE* output_fp = fopen(".cachesim_results", "w");
  assert(output_fp);
  fprintf(output_fp, "%u %u %u\n", num_hit, num_miss, num_eviction);
  fclose(output_fp);
}

/* No longer used
 * because this method does not allocate continous block of memory
 *
//memory allocation for A and B
void allocMatrix(int M, int N, int*** A){
  int i;
  assert(*A = (int**) malloc( sizeof(int*) * M ));
  
  for(i=0; i<M; i++){
    assert((*A)[i]=(int*) malloc( sizeof(int) * N));
  }
}

void freeMatrix(int M, int N, int** A){
  int i;
  for(i=0; i<M; i++)
    free(A[i]);
  
  free(A);
}
*/

// Initialize the given matrix
void initMatrix(int M, int N, int A[N][M], int B[M][N])
{
  int i, j;

  for (i = 0; i < N; i++){
	  for (j = 0; j < M; j++){
	    // A[i][j] = i+j;  //The matrix created this way is symmetric
      A[i][j]=i*M + j;
      B[j][i]=0;
	  }
  }
}

// Correct transpose function used to evaluate correctness
void correctTrans(int M, int N, int A[N][M], int B[M][N])
{
  int i, j, tmp;
  for (i = 0; i < N; i++){
	  for (j = 0; j < M; j++){
	    tmp = A[i][j];
	    B[j][i] = tmp;
  	}
  }    
}


// Add the given trans function into your list
void registerTransFunction(
    void (*trans)(int M,int N,int[N][M],int[M][N]) , char* desc){
  func_list[func_counter].func_ptr = trans;
  func_list[func_counter].description = desc;
  func_list[func_counter].correct = 0;
  func_list[func_counter].num_hits = 0;
  func_list[func_counter].num_misses = 0;
  func_list[func_counter].num_evictions =0;
  func_counter++;
}
