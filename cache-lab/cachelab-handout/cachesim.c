//#define DEBUG
#ifdef DEBUG
#define dbg_printf(...) printf(__VA_ARGS__)
#else
#define dbg_printf(...)
#endif

#define COLD_CACHE_VALUE ~0LU
#include <getopt.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "cachelab-tools.h"

static int verbose ;   //For verbose argument
static int B,S,E ;
static unsigned int global_reference_counter;
const unsigned long int mask = (~0);
static int  misses = 0;
static int hits = 0;
static int evictions = 0;
static unsigned long int **cache;                         //Implement cache in form of unsigned int array of S sets and E lines 

static unsigned long int **reference_counter;   	//To keep track of cache references and implement LRU 

int check_cache(unsigned int set,unsigned int tag);        //Check if input set has given tag

int is_in_cache(unsigned int set,unsigned int tag);       //1 if in cache increments its ref counter  0 if not given set tag

int check_cold_cache(unsigned int set);        		 //Check if any line in particular set is cold, return line  else return -1

void insert_in_cache(unsigned int set,unsigned int line,unsigned int tag);       //1 if in cache 0 if not

void initialize_cache(); 					 //Initialized cache of integers with S sets and E lines

void initialize_reference_counter();  				//Initializes reference counter to zero  for cache elements

void increment_reference_counter(unsigned int set,unsigned int line); //Increments LRU count value for given set/line

unsigned int line_to_evict(unsigned int set) ;			     //Returns line to evict from set

void call_load_store_case(unsigned int set , unsigned int tag);     //Checks if hit , miss , miss+evict on load or store

int main(int argc,char **argv) {


        int b,s,tag_bits;
	unsigned int set,tag,byte;
	int optchar ;
	int total_cache_size = 0;
	char instruction;
	unsigned long int address;  // address parsed 
	int size;  //size ofoperation
	FILE *fp;
	char *t = NULL;
	int i;
	if (argc < 5) {
		printf("Too few arguments \n");
		exit(0);
	} 



	while ((optchar = getopt (argc,argv,"b:s:E:t:v::"))!= -1) {

		switch (optchar) {


			case 'b' : b = atoi(optarg);
				   break;
			case 't' : t = (char *)(optarg);
				   fp = fopen(t,"r");
				   break;
			case 'E' : E = atoi(optarg);
				   break;
			case 's' : s = atoi(optarg);
				   break;
			case 'v' : verbose = 1;
				   break;
			default :  printf("Wrong argument usage\n");
				   printf("%s [-b] block-bits [-s] set-bits [-t] input-file [-E] lines per set [[-v] optional] verbose\n",argv[0]);
				   exit(0);

		}


	}


	////////////////////////////////////////////////////////
	// Initialize cache and cache parameters

	tag_bits = s+b;
	S = (1<<s);  // S = 2^s
	B = (1<<b); //  B = 2^b
	total_cache_size = S*E*B ;

	cache = (unsigned long int **)malloc(S *sizeof(unsigned long int *));

	for(i = 0 ; i < S ; i++ ) {

		cache[i] = (unsigned long int *)malloc(E* sizeof(unsigned long int)); //Should be B*E but B doesnt matter in lab ,
										      // so B = 8 bytes

	}

	initialize_cache();

	reference_counter = (unsigned long int **)malloc(S *sizeof(unsigned long int *));

	for(i = 0 ; i < S ; i++ ) {

		reference_counter[i] = (unsigned long int *)malloc(E* sizeof(unsigned long int)); //Should be B*E but B doesnt matter in lab ,
											         // so B = 8 bytes

	}

	initialize_reference_counter();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





	while(fscanf(fp," %c %lx, %d",&instruction,&address,&size)!= EOF) {

		byte = address & (mask>>(64-b));                               //Getting byte offset
		set = (address>>b) & (mask>>(64-s));                          // Getting set
		tag = (address>>(s+b)) & (mask>>(64-tag_bits));              // Getting tag


		if(instruction == 'I') {                   //Disregard I - Instructions

			continue;


		}
		if(instruction == 'L') {                  //For Load - check for hit , cold cache and miss evicition in serial order

		    if(verbose)
                       printf("%c %lx, %d ",instruction,address,size);	
                    
                     call_load_store_case(set,tag);

		}

		if(instruction == 'S') {                   //For Store - check for hit , cold cache and miss evicition in serial order

			if(verbose)
				printf("%c %lx, %d ",instruction,address,size);	

			call_load_store_case(set,tag);
		}

		if(instruction == 'M') {                   // Call checking function twice , one for Load and one for Store


			if(verbose)
				printf("%c %lx, %d ",instruction,address,size);	

			call_load_store_case(set,tag);

			call_load_store_case(set,tag);
		}

	}  

	printCachesimResults(hits,misses,evictions);         // Print final result
	fclose(fp);

}

int check_cache(unsigned int set,unsigned int tag) {  //Returns 1 if cache set has tag and 0 otherwise

	int j;
	unsigned long int *temp;


	temp = (unsigned long int *)cache[set];

	for(j = 0 ; j < E ; j++) {

		if(*(temp +j) == tag) {
			return 1;                      
		}

	}

	return 0;

}


void initialize_cache() {            //Initializes cache with values of ~0

	int i,j;
	unsigned long int *temp;
	for(i = 0 ; i < S ; i++) {
		temp = (unsigned long int *)cache[i];
		for(j = 0 ; j < E ; j++) {

			*(temp + j) = (~0);    //Writing E bytes to cold cache value ~0
		}

	}

}

void initialize_reference_counter() {       //Initialize LRU reference count with value 0 for each element
	int i,j;
	unsigned long int *temp;
	for(i = 0 ; i < S ; i++) {

		temp = (unsigned long int *)reference_counter[i];

		for(j = 0 ; j < E ; j++) {

			*(temp + j) = 0;    //Writing E bytes to cold cache value 0

		}

	}

}


int check_cold_cache(unsigned int set) {         //Check if any line in particular set is cold, return line  else return -1
	int j;
	unsigned long int *temp;
	temp = (unsigned long int *)cache[set];
	for(j = 0 ; j < E ; j++) {

		if(*(temp +j) == COLD_CACHE_VALUE) {
			dbg_printf("Found cold entry in line : %d of set = %u",j,set);
			return j;
		}

	}
	return -1;

}


int is_in_cache(unsigned int set,unsigned int tag) {         //1 if in cache increments its ref counter  0 if not given set tag

	int j;
	unsigned long int *temp;
	temp = (unsigned long int *)cache[set];
	for(j = 0 ; j < E ; j++) {

		if(*(temp + j) == tag) {
			dbg_printf("HIT Found entry in line : %d of set = %u",j,set);
			increment_reference_counter(set,j);
			return 1;
		}

	}
	return 0;

}

void insert_in_cache(unsigned int set,unsigned int line,unsigned int tag) {  //Inserts in cache given set,line,tag       

	unsigned long int *temp;
	temp = (unsigned long int *)cache[set];
	*(temp +line) = tag;

}

void increment_reference_counter(unsigned int set,unsigned int line) {      //Increment reference counter of line in set   

	unsigned long int *temp;
	temp = (unsigned long int *)reference_counter[set];
	*(temp + line) = ++global_reference_counter;
	dbg_printf("\nIncrement line : %u of set = %u to value = %lu",line,set,*(temp+line));
}


unsigned int line_to_evict(unsigned int set) {    //Returns line to evict from set 

	unsigned int line = 0 ;        //This will set line to evict to zero
	unsigned int min = 0;
	int j;
	unsigned long int *temp;
	temp = (unsigned long int *)reference_counter[set];
	min = *(temp + 0 ); //Set min value to line 0
	line = 0 ;
	for(j = 0 ; j < E ; j++) {
		if(*(temp + j) < min)
			line = j;
	}
	return line;

}



void call_load_store_case(unsigned int set , unsigned int tag) {          //Decides if its a miss or a hit

	unsigned int evicted_line;
	int status;
	if (is_in_cache(set,tag)) {           // First check if entry is in cache
		hits++;
		if(verbose)
			printf("HIT\n");
	} 

	else if((status = check_cold_cache(set))!= -1) {   //Check if cache is cold, if yes then enter element in cache
		misses ++;
		insert_in_cache(set,status,tag);
		increment_reference_counter(set,status); 
		if(verbose)
			printf("MISS\n");

	} 

	else if  (!is_in_cache(set,tag)) {              //If not in cache and cache not cold , evict a line 
		//from given set and enter given line
		evicted_line = line_to_evict(set);
		evictions ++;
		misses++;
		insert_in_cache(set,evicted_line,tag);
		increment_reference_counter(set,evicted_line);
		if(verbose)	
			printf("MISS EVICTION\n");
	}


}

/* EOF */
