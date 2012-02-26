#include <getopt.h>
#include <stdio.h>
#include <unistd.h>

static int verbose ;   //For verbose argument

int main(int argc,char **argv) {


int b,s,E ;
char *t = NULL;

if (argc < 5) {
  printf("Too few arguments \n");
  exit(0);
}


while ((optchar = getopt (argc,argv,"b:s:E:tv::"))!= -1) {

  switch (optchar) {


    case 'b' : b = atoi(optarg);
               break;
    case 't' : t = (char *)(optarg);
               break;
    case 'E' : E = atoi(optarg);
               break;
    case 's' : s = atoi(optarg);
               break;
    case 'v' : verbose = 1;
               break;
    default :  printf("Wrong argument usage\n")
               printf("%s [-b] block-bits [-s] set-bits [-t] input-file [-E] lines per set [[-v] optional] verbose\n",argv[0]);
               exit(0);

  }


printf("\n Arguments were %d %d %d %s %d",b,s,E,t,verbose);

}


}
