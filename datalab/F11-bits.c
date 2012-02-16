/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* 
 * bitOr - x|y using only ~ and & 
 *   Example: bitOr(6, 5) = 7
 *   Legal ops: ~ &
 *   Max ops: 8
 *   Rating: 1
 */
int bitOr(int x, int y) {
  /*Using Demorgan's Law (x+y)' = x'y'*/

	return ~((~x)&(~y));
}

/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
/*Getting all FF's and left shifting to keep MSB */

  return ((~0)<<31);

}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
/*2's complement = 1's complement + 1 */

  return ((~x)+1);
}
/* 
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) {
/*Getting desired byte in 0th byte positon then using bit-mask */ 

  return ((x>>(n<<3))&0xff) ;
}
/* 
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
*   Legal ops: !  & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int divpwr2(int x, int n) {
/*Add bias if x < 0 and perform right shift */    
    int offset = (1<<n)+ (~0);
    int msb = (x>>31)&0x01;
    int result1 = (x>>n);
    int result2 = (x+offset)>>n;
    int checkmsb = msb + (~0);
    return ((checkmsb&result1)|(~checkmsb&result2)); 
}
/* 
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3 
 */
int logicalShift(int x, int n) {
/* Right shift and zero out 32-n bits on the left and account for special case of zero*/

  int checkn = !n + (~0);
  int result1 = (x>>n);
  int mask = (32 + (~n+1));
  int result2 =  result1 & (~((~0)<<mask));
  int finalresult = (checkn & result2)|(~checkn & result1);
  return finalresult;
}
/* 
 * isPositive - return 1 if x > 0, return 0 otherwise 
 *   Example: isPositive(-1) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 3
 */
int isPositive(int x) {
 /*Retrieve MSB and account for special case of Zero*/
 
  int check1 = ((x>>31)&0x01);
  int check2 = !x ;
  return !(check1 + check2); 
  
}
/* 
 * isLess - if x < y  then return 1, else return 0 
 *   Example: isLess(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLess(int x, int y) {
/* Retrieve sign of both numbers - subtract if of same sign otherwise base decision on msb itself */

   int msbx = ((x>>31)&0x01);
   int msby = ((y>>31)&0x01); 
   int samesign = !(msbx^msby);
   int resultmask = !samesign + (~0);
   int result1 = ((x+(~y)+1)>>31)&0x01;
   int result2 = msbx; 
   return ((resultmask&result1)|(~resultmask&result2));
}
/* 
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int bang(int x) {
/* Using - 2's complement of zero is zero itself and setting bits in 2's complement and then seeing value from MSB */ 
 
  int complement = ~x + 1;
  return ((~(complement | x)>>31)&0x01);
}
/*
 * isPower2 - returns 1 if x is a power of 2, and 0 otherwise
 *   Examples: isPower2(5) = 0, isPower2(8) = 1, isPower2(0) = 0
 *   Note that no negative number is a power of 2.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 4
 */
int isPower2(int x) {
/* All powers of 2 have only one set digit and negative numbers and zero aren't considered */
 int check1 = x&(x+(~0));
 int check2 = !x;
 int check3 = (x>>31)&0x01;
 return !(check1 + check2 + check3);
  
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
   int arg = x;   
   int rightshift;
   int finalresult;
   int msb = (x>>31)&0x01;
   int mask1 = (~0)<<16;
   int mask2 = 0xff<<8;
   int mask3 = 0xf0;
   int mask4 = 0x0c;
   int mask5 = 0x02;
   int signcheck = msb + (~0);
   arg = (signcheck&x)|(~signcheck&((~x+1)));
   int argmsb = (arg>>31)&0x01;
   int checkmsb = argmsb + (~0);
//   printf("arg %d rgmsb %d checkmsb %d",arg,argmsb,checkmsb);
   int result = !!arg;
   rightshift = !!(arg&mask1)<<4;
   arg>>=rightshift;
   result |= rightshift;
   rightshift = !!(arg&mask2)<<3;
   arg>>=rightshift;
   result |= rightshift;
   rightshift = !!(arg&mask3)<<2;
   arg>>=rightshift;
   result |= rightshift;
   rightshift = !!(arg&mask4)<<1;
   arg>>=rightshift;
   result |= rightshift;
   result += !!(arg&mask5);
    finalresult = (checkmsb & (result+1))|(~checkmsb & result);
   return result;
}
/* 
 * float_neg - Return bit-level equivalent of expression -f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_neg(unsigned uf) {
 int result = uf;
 int msb = (uf>>31)&0x01;
 int exponent = (uf>>23)&0xff;
 int fraction = uf&0x7fffff;
 result |= 0x80000000;

 if(msb)
   result &= 0x7fffffff;
 
 if(exponent == 0xff && fraction!=0)
   result = uf;
 
 return result;
}
/* 
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_i2f(int x) {
   unsigned int result = 0;
   int arg = x;
   int msb = (x>>31)&0x01;
   int power = 0;
   int quotient = 2;
   int radix =2;
   int bias = 127;
   int exponent = 0;
   int remainder=0;
   int bitposition = 0;
   int mask=0;
    if(arg<0)
     arg = -arg;

   while(quotient>1)
   { remainder = arg;
     while(remainder > 1)
     { remainder = remainder - radix;
          }     
     if(arg > 1)
{  //  {printf("\nArg %d Remainder %d Bitposition %d ",arg, remainder , bitposition); 
     mask |= (remainder<<(bitposition++));
     power ++;
      }
     quotient = arg / radix;
     arg = arg / radix ;
   
    }
  
   if(x) 
  	 exponent = power + bias;      
   
   if(bitposition>23)
     {
       mask = (mask >> (bitposition-23))&0x7fffff;
       bitposition = 23;
      } 

   result |=  mask<<(23-bitposition);
   result |= (msb<<31);
   result |= (exponent<<23); 
   
   if(x==0x80000000)
     result = 0xcf000000;
//   printf("\n Exponent %x",exponent);
   return result;
}
