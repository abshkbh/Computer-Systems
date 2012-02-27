#define DEBUG
#ifdef DEBUG
#define dbg_printf(...) printf(__VA_ARGS__)
#else
#define dbg_printf(...)
#endif


#include <getopt.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
static int verbose ;   //For verbose argument

int main(int argc,char **argv) {


int b,s,B,S,E,optchar ;
int total_cache_size = 0;
char parse_line[100]; //line to parse
char instruction;
unsigned long int address;  // address parsed 
int size;  //size ofoperation
FILE *fp;
char *t = NULL;
char str[1];

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

S = (1<<s);  // S = 2^s
B = (1<<b); //  B = 2^b
total_cache_size = S*E*B ;

////////////////////////////////////////////////////////

while(fgets(parse_line,100,fp)!= NULL) {

  dbg_printf("%s\n",parse_line);
  instruction = parse_line[1];
  sscanf(parse_line,"%s%lx%c%d",str,&address,str,&size);
  dbg_printf("%c %lu,%d\n",instruction,address,size);
}

fclose(fp);

}
