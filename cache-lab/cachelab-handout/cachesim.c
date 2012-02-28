#define MY_DEBUG
#ifdef MY_DEBUG
#define dbg_printf(...) printf(__VA_ARGS__)
#else
#define dbg_printf(...)
#endif

#define COLD_CACHE_VALUE ~0U
#include <getopt.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "cachelab-tools.h"
static int verbose ;   //For verbose argument
static int b,s,tag_bits,B,S,E ;
static unsigned long int global_reference_counter;
unsigned int set,tag,byte;
const unsigned long int mask = (~0);
static unsigned int  misses = 0;
static unsigned int hits = 0;
static unsigned int evictions = 0;
unsigned long int **cache;
unsigned long int **reference_counter;   //To keep track of cache references and implement LRU 

int check_cache(unsigned int set,unsigned int tag);
void insert_in_cache(unsigned int set,unsigned int line,unsigned int tag);       //1 if in cache 0 if not
void initialize_cache();
void show_cache();
void initialize_reference_counter();
void show_reference_counter();
void increment_reference_counter(unsigned int set,unsigned int line);
unsigned int line_to_evict(unsigned int set) ; //Returns line to evict from set
void load_action(unsigned int tag,unsigned int set);
void store_action(unsigned int tag,unsigned int set);

int main(int argc,char **argv) {


	int optchar ;
	int total_cache_size = 0;
	char parse_line[100]; //line to parse
	char instruction;
	unsigned long int address;  // address parsed 
	int size;  //size ofoperation
	FILE *fp;
	char *t = NULL;
	char str[1];  //Dummy char for parsing
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

	printf("Arguments were %d %d %d %s %d\n",b,s,E,t,verbose);

	////////////////////////////////////////////////////////
	// Initialize cache and cache parameters

	tag_bits = s+b;
	S = (1<<s);  // S = 2^s
	B = (1<<b); //  B = 2^b
	total_cache_size = S*E*B ;

	cache = (unsigned long int **)malloc(S *sizeof(unsigned long int *));

	for(i = 0 ; i < S ; i++ ) {

		cache[i] = (unsigned long int *)malloc(E* sizeof(unsigned long int)); //Should be B*E but B doesnt matter in lab , so B = 8 bytes

	}

        initialize_cache();

	reference_counter = (unsigned long int **)malloc(S *sizeof(unsigned long int *));

	for(i = 0 ; i < S ; i++ ) {

	reference_counter[i] = (unsigned long int *)malloc(E* sizeof(unsigned long int)); //Should be B*E but B doesnt matter in lab , so B = 8 bytes

	}

       initialize_reference_counter();


	////////////////////////////////////////////////////////

	while(fgets(parse_line,100,fp)!= NULL) {

		// dbg_printf("%s\n",parse_line);
		instruction = parse_line[1];
		sscanf(parse_line,"%s%lx%c%d",str,&address,str,&size);
		dbg_printf("%c %lu,%d\n",instruction,address,size);

		byte = address & (mask>>(64-b));                               //Getting byte offset
		set = (address>>b) & (mask>>(64-s));                          // Getting set
		tag = (address>>(s+b)) & (mask>>(64-tag_bits));              // Getting tag


		if(instruction == 'I') {
			dbg_printf("I = %lx %x %x %x\n",address,tag,set,byte); 

		}

		if(instruction == 'L') {

			dbg_printf("L = %lx %x %x %x\n",address,tag,set,byte); 
                        load_action(tag,set);

		}

		if(instruction == 'S') {

			dbg_printf("S = %lx %x %x %x\n",address,tag,set,byte); 
                        store_action(tag,set);

		}

		if(instruction == 'M') {
			dbg_printf("M = %lx %x %x %x\n",address,tag,set,byte); 
                        load_action(tag,set);
                        store_action(tag,set);

		}

		show_cache();
		show_reference_counter();
                dbg_printf("\nHits = %u Misses = %u Evicitions = %u\n",hits,misses,evictions);

	}

        printCachesimResults(hits,misses,evictions);
	fclose(fp);

}

int check_cache(unsigned int set,unsigned int tag) {  //Returns 1 if cache set has tag and 0 otherwise

	int j;
	unsigned long int *temp;


	temp = (unsigned long int *)cache[set];

	for(j = 0 ; j < E ; j++) {

		if(*(temp +j) == tag) {
			dbg_printf("HIT Found entry in line : %d of set = %u",j,set);   
			return 1;                      
		}

	}

	return 0;

}


void initialize_cache() {

	int i,j;
	unsigned long int *temp;

	for(i = 0 ; i < S ; i++) {

		temp = (unsigned long int *)cache[i];

		for(j = 0 ; j < E ; j++) {

			*(temp + j) = (~0);    //Writing E bytes to cold cache value ~0

		}

	}

}

void initialize_reference_counter() {

	int i,j;
	unsigned long int *temp;

	for(i = 0 ; i < S ; i++) {

		temp = (unsigned long int *)reference_counter[i];

		for(j = 0 ; j < E ; j++) {

			*(temp + j) = 0;    //Writing E bytes to cold cache value 0

		}

	}

}




void show_cache() {

	int i,j;
	unsigned long int *temp;

        dbg_printf("\n\\\\\\\\\\Cache\\\\\\\\\\\\\\");
	
         for(i = 0 ; i < S ; i++) {

		temp = (unsigned long int *)cache[i];
		dbg_printf("\n");

		for(j = 0 ; j < E ; j++) {

			dbg_printf("%lx ",temp[j]);    //Writing E bytes to cold cache value ~0

		}

	}

}

void show_reference_counter() {

	int i,j;
	unsigned long int *temp;

        dbg_printf("\n\\\\\\\\\\Reference Counter\\\\\\\\\\\\\\");
	for(i = 0 ; i < S ; i++) {

		temp = (unsigned long int *)reference_counter[i];
		dbg_printf("\n");

		for(j = 0 ; j < E ; j++) {

			dbg_printf("%lu ",temp[j]);    //Writing E bytes to cold cache value ~0

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



void load_action(unsigned int tag,unsigned int set) {

int status;
unsigned int evicted_line;

	if((status = check_cold_cache(set))!= -1) {   //If cache is cold
		misses ++;
		insert_in_cache(set,status,tag);
		increment_reference_counter(set,status); 
		dbg_printf("\nMISS");

	} 

	else if (is_in_cache(set,tag)) {
		hits++;
		dbg_printf("\nHIT");
	} 

	else if (!is_in_cache(set,tag)) {
		evicted_line = line_to_evict(set);
		evictions ++;
		misses++;
		insert_in_cache(set,evicted_line,tag);
		increment_reference_counter(set,evicted_line);
		dbg_printf("\nMISS + EVICTION");
	}

}



void store_action(unsigned int tag,unsigned int set) {

int status;
unsigned int evicted_line;

	if((status = check_cold_cache(set))!= -1) {   //If cache is cold
		misses ++;
		insert_in_cache(set,status,tag);
		increment_reference_counter(set,status); 
		dbg_printf("\nMISS");

	} 

	else if (is_in_cache(set,tag)) {
		hits++;
		dbg_printf("\nHIT");
	} 

	else if (!is_in_cache(set,tag)) {
		evicted_line = line_to_evict(set);
		evictions ++;
		misses++;
		insert_in_cache(set,evicted_line,tag);
		increment_reference_counter(set,evicted_line);
		dbg_printf("\nMISS + EVICTION");
	}

}
