/* 15-213 Fall 2010 
 * Provides useful tools for this lab. 
 *
 *
 * Yi Zhuang (yzhuang@cs.cmu.edu)
 * Date: 08/30/2010
 * Version: 0.1
 */

#ifndef CACHELAB_TOOLS_H
#define CACHELAB_TOOLS_H

#define MAX_TRANS_FUNCS 100

typedef struct trans_func{
  void (*func_ptr)(int M,int N,int[N][M],int[M][N]);
  char* description;
  char correct;
  unsigned int num_hits;
  unsigned int num_misses;
  unsigned int num_evictions;
} trans_func_t;

/* This function is used by the cachesim to print results
 * These results are examined by the driver. 
 * 
 * numHit: number of cache hits
 * numMiss: number of cache misses
 * numEviction: number of evictions
 *
 */
void printCachesimResults(unsigned int numHit, unsigned int numMiss, 
                          unsigned int numEviction);

/* Fill the matrix with data */
void initMatrix(int M, int N, int A[N][M], int B[M][N]);

/* The trans function that produces correct result. */
void correctTrans(int M, int N, int A[N][M], int B[M][N]);

/* Add the given function to the function list */
void registerTransFunction(
    void (*trans)(int M,int N,int[N][M],int[M][N]), char* desc);

#endif
