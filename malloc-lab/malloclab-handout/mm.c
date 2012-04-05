/*
 * mm.c
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mm.h"
#include "memlib.h"

/* If you want debugging output, use the following macro.  When you hand
 * in, remove the #define DEBUG line. */
//#define DEBUG
#ifdef DEBUG
# define dbg_printf(...) printf(__VA_ARGS__)
#else
# define dbg_printf(...)
#endif

#define HEAP_DEBUG
#ifdef HEAP_DEBUG
# define heap_printf(...) printf(__VA_ARGS__)
#else
# define heap_printf(...)
#endif


/* do not change the following! */
#ifdef DRIVER
/* create aliases for driver tests */
#define malloc mm_malloc
#define free mm_free
#define realloc mm_realloc
#define calloc mm_calloc
#endif /* def DRIVER */

/* Basic macros and constants */
#define WSIZE 4            /* Word and Header / Footer size */
#define DSIZE 8            /* Double word size */
#define CHUNKSIZE (1<<10) /* Extend heap by this amount */
#define MIN_BLOCK_SIZE 16 /* Minimum block size */

#define MAX(x,y) (( (x) > (y) ) ? (x) : (y))

/* Pack a size and allocated bits in a word */
#define PACK(size,alloc)   ((size)|(alloc))

/* Read and write word at address p */
#define GET(p)    (*(unsigned int *)(p))
#define PUT(p,val)    (*(unsigned int *)(p) = (val))

/* Read size and allocated fields from address p */
#define GET_SIZE(p)  (GET(p) & (~0x7)) 
#define GET_ALLOC(p)  (GET(p) & 0x1)

/* Given block pointer bp,compute address of its header and footer */ 
#define HDRP(bp)  ((char *)(bp) - WSIZE)
#define FTRP(bp)  ((char *)(bp) + GET_SIZE(HDRP(bp))- DSIZE)

/* Given block pointer bp,comute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE((char *)(bp) -WSIZE))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE((char *)(bp) -DSIZE))

/* Given free block pointer bp,compute address of 
 * next and previous free blocks 
 * Free blocks are designed like normal blocks , the
 * pointer points to one word after the Header. 
 * Header + WSIZE = Next Free Block Address
 * Header + 2*WSIZE = Previous Free Block Address */
/* #define NEXT_FREE_BLKP(bp) GET(bp)
#define PREV_FREE_BLKP(bp) GET((char *)(bp) + WSIZE) */

/* Given free block pointer bp,
 * SET address of its next and previous free
 * block pointers */
/* #define SET_NEXT_FREE_BLKP(bp,val) PUT(((char *)(bp)),val)
#define SET_PREV_FREE_BLKP(bp.val) PUT(((char *)(bp) + WSIZE),val) */


/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(p) (((size_t)(p) + (ALIGNMENT-1)) & ~0x7)

/* Returns true if p is EPILOGUE block */
#define IS_EPILOGUE(p) ((!GET_SIZE(HDRP(p))) && (GET_ALLOC(HDRP(p))))

/* Returns true if *p1 = *p2 */
#define ARE_EQUAL(p1,p2) ((GET(p1)) == (GET(p2)))

/* To enable heap checker */
#define VERBOSE 1

/* Function prototypes for static functions defined below */
static void *extend_heap(size_t words);
static void *coalesce(void *bp);
static void *find_fit(size_t asize);
static void place(void *bp,size_t asize);
static void printblock(void *bp);
static void checkblock(void *bp);
static void *NEXT_FREE_BLKP(void *bp);
static void *PREV_FREE_BLKP(void *bp);
static void SET_NEXT_FREE_BLKP(void *bp,unsigned long val);
static void SET_PREV_FREE_BLKP(void *bp,unsigned long val);
static void insert_in_list(void *bp);
static void remove_from_list(void *bp);

/* Points to start of heap */
static char * start_of_heap;

/* Points to prologue block */
static char * heap_listp;

/* Points to first free block in list */
static char * free_root;
/*
 * Initialize: return -1 on error, 0 on success.
 */
int mm_init(void) {

	dbg_printf("In Init\n");
        free_root = NULL;
	if((heap_listp = mem_sbrk((4*WSIZE))) == (void *)-1) {
		return -1;
	}

	start_of_heap = heap_listp;
	PUT(heap_listp,0);   /* Alignment Padding */
	PUT(heap_listp + (1*WSIZE),PACK(DSIZE,1)); /* Prologue Header */
	PUT(heap_listp + (2*WSIZE),PACK(DSIZE,1)); /* Prologue Footer */
	PUT(heap_listp + (3*WSIZE),PACK(0,1)); /* Epilogue Footer */
	heap_listp += (2*WSIZE);

	dbg_printf("Heap list is %p\n",heap_listp);

	/* Extend empty heap with a free block of CHUNKZISE bytes */
	if (extend_heap(CHUNKSIZE/WSIZE) == NULL ) {
		return -1;
	}

	mm_checkheap(VERBOSE);

	return 0;

}


/* extend_heap: Extends the heap with a new free block */
static void *extend_heap(size_t words) {

	char *bp;
	size_t size;

	/* Allocate an even no. of words to maintain alignment */
	size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
	if ((long)(bp = mem_sbrk(size)) == -1 ) {
		dbg_printf("OUCH\n");
		return NULL;
	}

	/* Initialize free block header/footer and the epilogue */
	PUT(HDRP(bp),PACK(size,0)); /* Free block header */
	PUT(FTRP(bp),PACK(size,0)); /* Free block footer */
	PUT(HDRP(NEXT_BLKP(bp)),PACK(0,1)); /* New epilogue header */

	/* Put newly created free block in list */
	//insert_in_list(bp);
	dbg_printf("Extend Heap after insert\n");
        mm_checkheap(VERBOSE);

	/* Coalesce if previous block was free */
	return	coalesce(bp);

}

/*
 * malloc
 */
void *malloc (size_t size) {

	size_t asize; /* Adjusted block size */
	size_t extendsize; /* Amount to extend heap if no fit */
	char *bp;

	dbg_printf("[Trying] MALLOC size = (%u)\n",(unsigned int)(size));
	/* Ignore spurious requests */
	if (size == 0) {
		return NULL;
	}

	/* Adjust block size to include overhead and alignment reqs. */
	//asize = ALIGN(size);
	if (size <= DSIZE)
		asize = 2*DSIZE;
	else
		asize = DSIZE*((size + (DSIZE) + (DSIZE-1)) / DSIZE) ;


	/* Search the free list for a fit */
	if ((bp = find_fit(asize)) != NULL) {
		dbg_printf("Returned from find fit\n");
                place(bp,asize);
		dbg_printf("[Success] MALLOC addr = (%p) size = (%u)\n",bp,(unsigned int)(asize));
		mm_checkheap(VERBOSE);
		return bp;
	}
	/* No fit found. Get more memory and place the block */
	extendsize = MAX(asize,CHUNKSIZE);
	if ((bp = extend_heap(extendsize/WSIZE)) == NULL) {
		dbg_printf("[Failed To Extend Heap] MALLOC asize = (%u)\n",(unsigned int)(asize));
		return NULL;
	}

	dbg_printf("[Heap Extended] MALLOC\n");
	place(bp,asize);
	dbg_printf("[Success] MALLOC addr = (%p) size = (%u)\n",bp,(unsigned int)(asize));
	mm_checkheap(VERBOSE);
	return bp;

}

/*
 * free
 */
void free (void *ptr) {

	size_t size;

	if(!ptr) { 
		return;
	}

	/* Deallocate block and coalesce if possible */
	size = GET_SIZE(HDRP(ptr));
	dbg_printf("[Trying] FREE addr = (%p) size = (%u)\n",ptr,(unsigned int)(size));
	PUT(HDRP(ptr),PACK(size,0));
	PUT(FTRP(ptr),PACK(size,0));

	/* Addition of freed pointer
	 * in free list is done along
	 * with coalescing in coalesce
	 * function itself */
	coalesce(ptr);
	dbg_printf("[After Coalesce] FREE\n");
	mm_checkheap(VERBOSE);

}

/* Coalesces contiguous free blocks.
 * Also responsible for adding final
 * coalesced block to free list */
static void *coalesce(void *bp) {

	size_t prev_alloc,next_alloc,size;
	void *next_blkp;
	void *prev_blkp;

	prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
	next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
	size = GET_SIZE(HDRP(bp));

	/* Case : Both previous and next are allocated
	 * No Coalescing just insert free block in list */
	if (prev_alloc && next_alloc) {
		insert_in_list(bp);	
		return bp;
	}


	/* Case : Previous allocated but Next is free
	 * Coalescing current and next into one free block. */
	else if (prev_alloc && !next_alloc) {

		next_blkp = NEXT_BLKP(bp);
		size +=  GET_SIZE(HDRP(NEXT_BLKP(bp)));
		PUT(HDRP(bp),PACK(size,0));
		PUT(FTRP(bp),PACK(size,0));

		/* Remove next block from list
		 * and add newly coalesced block
		 * to list */
		remove_from_list(next_blkp);
		insert_in_list(bp);

	}


	/* Case : Next allocated but Previous is free
	 * Coalescing current and previous into one free block. */
	else if (!prev_alloc && next_alloc) {

                dbg_printf("In colaesce with previous\n");
		prev_blkp = PREV_BLKP(bp);	

		size +=  GET_SIZE(FTRP(PREV_BLKP(bp)));
		PUT(FTRP(bp),PACK(size,0));
		PUT(HDRP(PREV_BLKP(bp)),PACK(size,0));
		bp = PREV_BLKP(bp);

		/* Remove previous block from list
		 * and add newly coalesced block
		 * to list */
		remove_from_list(prev_blkp);
                insert_in_list(bp);
                dbg_printf("Heap after coalesce\n");
                mm_checkheap(VERBOSE); 

	}

	/* Case : Previous  and Next both free
	 * Coalescing previous,current and next into one
	 * big free block */
	else {

		prev_blkp = PREV_BLKP(bp);	
		next_blkp = NEXT_BLKP(bp);	


		size +=  GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
		PUT(HDRP(PREV_BLKP(bp)),PACK(size,0));
		PUT(FTRP(NEXT_BLKP(bp)),PACK(size,0));
		bp = PREV_BLKP(bp);

		/* Remove previous and next block from 
		 * list and add newly coalesced block
		 * to list */
		remove_from_list(prev_blkp);
		remove_from_list(next_blkp);
		insert_in_list(bp);

	}

	return bp;

}

/* find_fit - Returns first free block
 * equal to or larger than parameter 
 * asize. Returns NULL if no fit found  */ 
static void *find_fit(size_t asize) {

	void *bp;
	/* Start iteration from first free block*/
	bp = free_root;

        dbg_printf("In find fit for adjusted size = %lu\n",(unsigned long)asize);
	while (bp != NULL) {
		/* GET_ALLOC is just put for safety , will remove
		 * this once correctness verified */ 
		if (!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp)))) {
			dbg_printf("Find Fit : found match\n");
			return bp;
		}

		bp = NEXT_FREE_BLKP(bp);
	}

	dbg_printf("Find Fit : no match\n");
	return NULL; /* No fit */
}

/* place - Place asize in block bp. Split bp if
 * remainder is greater than or equal to minimum
 * block size */
static void place(void *bp,size_t asize) {

	size_t free_blk_size;
	size_t temp;
	void *original_blkp;
	free_blk_size = GET_SIZE(HDRP(bp));
	temp = free_blk_size - asize;

	/* Condition to split block bp */
	if ( temp  >= MIN_BLOCK_SIZE) {

		original_blkp = bp;
		dbg_printf("[Splitting] Place\n");
		/* For splitting put remainder size in footer of block
		 * bp , then place adjusted size in header and new footer
		 * of allocated block bp. Finally , put remainder in header
		 * of remaining block after splitting */
		PUT(HDRP(bp),PACK(asize,1)); 
		PUT(FTRP(bp),PACK(asize,1));
		bp = NEXT_BLKP(bp);
		PUT(HDRP(bp),PACK(temp,0)); 
		PUT(FTRP(bp),PACK(temp,0)); 

		/* After splitting we need to remove
		 * original free block from free list
		 * and add new free block to free list*/ 
		remove_from_list(original_blkp);	
		insert_in_list(bp);	
		return;

	}

	/* No splitting, remove newly
	 * allocated block from free list */  
	dbg_printf("[No Splitting] Place\n");
	PUT(HDRP(bp),PACK(free_blk_size,1)); 
	PUT(FTRP(bp),PACK(free_blk_size,1)); 
	remove_from_list(bp);	
}

/*
 * realloc - you may want to look at mm-naive.c
 */
void *realloc(void *oldptr, size_t size) {

	size_t oldsize;
	void *newptr;

	dbg_printf("In realloc\n");

	/* If size == 0 then this is just free, and we return NULL. */
	if(size == 0) {
		mm_free(oldptr);
		return 0;
	}

	/* If oldptr is NULL, then this is just malloc. */
	if(oldptr == NULL) {
		return mm_malloc(size);
	}

	newptr = mm_malloc(size);

	/* If realloc() fails the original block is left untouched  */
	if(!newptr) {
		return 0;
	}

	/* Copy the old data. */
	oldsize = GET_SIZE(HDRP(oldptr));
	if(size < oldsize){
		oldsize = size;
	}
	memcpy(newptr, oldptr, oldsize);

	/* Free the old block. */
	mm_free(oldptr);

	return newptr;
}

/*
 * calloc - you may want to look at mm-naive.c
 * This function is not tested by mdriver, but it is
 * needed to run the traces.
 */
void *calloc (size_t nmemb, size_t size) {

	size_t bytes = nmemb * size;
	void *newptr;
	newptr = malloc(bytes);
	if(newptr != NULL) {
		memset(newptr, 0, bytes);
	}
	return newptr;

}


/*
 * Return whether the pointer is in the heap.
 * May be useful for debugging.
 */
/* static int in_heap(const void *p) {
   return p <= mem_heap_hi() && p >= mem_heap_lo();
   } */

/*
 * Return whether the pointer is aligned.
 * May be useful for debugging.
 */
/*   static int aligned(const void *p) {
     return (size_t)ALIGN(p) == (size_t)p;
     } */ 

/* Get address of next free block pointer
 * of free block bp. All addresses are relative
 * to start of heap  */
static void * NEXT_FREE_BLKP(void *bp) {

        unsigned int next = GET(bp);

	if (next == 0){
		return NULL;
	} 
	return (start_of_heap + next);
}

/* Get address of previous free block pointer
 * of free block bp. All addresses are relative
 * to start of heap  */
static void * PREV_FREE_BLKP(void *bp) {

        unsigned int prev =  GET((char *)(bp) + WSIZE);  

	if (prev == 0) {
		return NULL;
	} 

	return (start_of_heap + prev);
}

/* Set address of next free block pointer
 * of free block bp */
static void  SET_NEXT_FREE_BLKP(void *bp,unsigned long val) {
	PUT(bp,(val & (~0)));
}

/* Set address of previous free block pointer
 * of free block bp */
static void SET_PREV_FREE_BLKP(void *bp,unsigned long val) {
	PUT(((char *)(bp) + WSIZE),(val & (~0)));
}

/* Insert a free block in free block list */
static void insert_in_list(void *bp) {

	void *temp;
	/* If no nodes in free list
	 * set block as root and both
	 * its pointers to zero */ 
	if(free_root == NULL) {
		free_root = bp;
		SET_NEXT_FREE_BLKP(free_root,0);
		SET_PREV_FREE_BLKP(free_root,0);
	}
	/* If free list is not empty 
	 * set block as root and both
	 * its pointers to zero. Also
	 * set old free root's prev 
	 * pointer to new free root  */ 
	else {
		temp = free_root;
		free_root = bp;
		SET_NEXT_FREE_BLKP(free_root,(unsigned long)temp);
		SET_PREV_FREE_BLKP(free_root,0);
		SET_PREV_FREE_BLKP(temp,(unsigned long)free_root);
	}


}

/* Remove a free block from free block list */
static void remove_from_list(void *bp) {

	void *prev_blkp;
	void *next_blkp;
	/* If no nodes in free list
	 * can't remove anything */ 
	if(free_root == NULL) {
		dbg_printf("[Error]Trying to remove from empty list");
		return;
	}

	/* If node given to remove is root 
	 * itself , then next free block (if exists)
	 * becomes root */ 
	else if (free_root == bp) {

		if (NEXT_FREE_BLKP(free_root) != NULL) {
			free_root = NEXT_FREE_BLKP(free_root);
			SET_PREV_FREE_BLKP(free_root,0);
		}
		else {
			free_root = NULL;
		}

	}

	/* If free list is not empty 
	 * remove from list by manipulating
	 * pointers of previous and next blocks  */ 
	else {
		prev_blkp = PREV_FREE_BLKP(bp);
		next_blkp = NEXT_FREE_BLKP(bp);

		/* If block to remove is last element in list */
		if(next_blkp == NULL) {
			SET_NEXT_FREE_BLKP(prev_blkp,0);
		}

		/* If block to remove is in 
		 * middle of list */
		else {
			SET_NEXT_FREE_BLKP(prev_blkp,(unsigned long)next_blkp);
			SET_PREV_FREE_BLKP(next_blkp,(unsigned long)prev_blkp);
		}
	}

}





static void printblock(void *bp) {
	size_t hsize, halloc, fsize, falloc;

	hsize = GET_SIZE(HDRP(bp));
	halloc = GET_ALLOC(HDRP(bp));  
	fsize = GET_SIZE(FTRP(bp));
	falloc = GET_ALLOC(FTRP(bp));  

	if (hsize == 0) {
		heap_printf("%p: EOL\n", bp);
		return;
	}

	if (halloc) {

		heap_printf("%p: header: [%u:%c] footer: [%u:%c]\n", bp, 
				(unsigned int)hsize, (halloc ? 'a' : 'f'), 
				(unsigned int)fsize, (falloc ? 'a' : 'f')); 
	}

	else {
		heap_printf("%p: header: [%u:%c] next: [%lx] prev: [%lx] footer: [%u:%c]\n", bp, 
				(unsigned int)hsize, (halloc ? 'a' : 'f'),(unsigned long)NEXT_FREE_BLKP(bp),
				(unsigned long) PREV_FREE_BLKP(bp),(unsigned int)fsize, (falloc ? 'a' : 'f')); 

	}
}

static void checkblock(void *bp) {
	if ((size_t)bp % 8){
		heap_printf("Error: %p is not doubleword aligned\n", bp);
	}
	if (GET(HDRP(bp)) != GET(FTRP(bp))) {
		heap_printf("Error: header does not match footer\n");
	}
}


/* 
 * checkheap - Minimal check of the heap for consistency 
 */
void mm_checkheap(int verbose) {

        return;
	char *bp = heap_listp;
	int next_count = 0;	
	if (verbose) {
		heap_printf("Heap (%p):\n", heap_listp);
	}

	if ((GET_SIZE(HDRP(heap_listp)) != DSIZE) || !GET_ALLOC(HDRP(heap_listp))) {
		heap_printf("Bad prologue header\n");
	}
	checkblock(heap_listp);

	for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
		if (verbose) {
			printblock(bp);
		}
		checkblock(bp);
	}

	for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
		if (!GET_ALLOC(HDRP(bp))) {
			if (NEXT_FREE_BLKP(bp) == NULL) {
				next_count++;
			}

		}
	}

	if (next_count > 1 ) {
		heap_printf("Free list error : has more than one endpoint\n");
	}
	if (next_count == 0 ) {
		heap_printf("Free list error : has no endpoint\n");
	}



	if (verbose)
		printblock(bp);
	if ((GET_SIZE(HDRP(bp)) != 0) || !(GET_ALLOC(HDRP(bp)))) {
		heap_printf("Bad epilogue header\n");
	}

}

