#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <limits.h>

#include "clz.h"

#define MAX 1000000

uint8_t clz_iter( uint32_t x){
	int n=32, c=16;
	do{
		uint32_t y=x>>c;
		if(y){ n-=c; x=y; }
		c >>= 1;
	}while( c );
	return ( n-x );
}

uint8_t clz_recur( uint32_t x ){
	return x ? clz_recur( x >> 1) - 1 : 32;
}

uint8_t clz_binary_search( uint32_t x ){
    if (x == 0) return 32;
    int n = 0;
    if (x <= 0x0000FFFF) { n += 16; x <<= 16; }
    if (x <= 0x00FFFFFF) { n += 8; x <<= 8; }
    if (x <= 0x0FFFFFFF) { n += 4; x <<= 4; }
    if (x <= 0x3FFFFFFF) { n += 2; x <<= 2; }
    if (x <= 0x7FFFFFFF) { n += 1; x <<= 1; }
    return n;
}

uint8_t clz_byte_shift( uint32_t x ){
   	if (x == 0) return 32;
    int n = 1;
    if ((x >> 16) == 0) { n += 16; x <<= 16; }
    if ((x >> 24) == 0) { n += 8; x <<= 8; }
    if ((x >> 28) == 0) { n += 4; x <<= 4; }
    if ((x >> 30) == 0) { n += 2; x <<= 2; }
    n = n - (x >> 31);
    return n;
}

/*uint8_t clz_harley( uint32_t x ){
	static prog_uint8_t const Table[] = {
      0xFF, 0, 0xFF, 15, 0xFF, 1, 28, 0xFF,
      16, 0xFF, 0xFF, 0xFF, 2, 21, 29, 0xFF,
      0xFF, 0xFF, 19, 17, 10, 0xFF, 12, 0xFF,
      0xFF, 3, 0xFF, 6, 0xFF, 22, 30, 0xFF,
      14, 0xFF, 27, 0xFF, 0xFF, 0xFF, 20, 0xFF,
      18, 9, 11, 0xFF, 5, 0xFF, 0xFF, 13,
      26, 0xFF, 0xFF, 8, 0xFF, 4, 0xFF, 25,
      0xFF, 7, 24, 0xFF, 23, 0xFF, 31, 0xFF,
    };

    // Propagate leftmost 1-bit to the right 
    x = x | (x >> 1);
    x = x | (x >> 2);
    x = x | (x >> 4);
    x = x | (x >> 8);
    x = x | (x >> 16);
 
    // x = x * 0x6EB14F9 
    x = (x << 3) - x;   // Multiply by 7.
    x = (x << 8) - x;   // Multiply by 255. 
    x = (x << 8) - x;   // Again. 
    x = (x << 8) - x;   // Again. 

    return pgm_read_byte(&Table[x >> 26]);
}*/

static double diff_in_second( struct timespec t1, struct timespec t2 ){
	struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } 
	else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec + diff.tv_nsec / 1000000000.0);
};


int main(){
	//printf("%d\n", INT_MAX);
	int i;
	uint32_t k=1024;
	struct timespec start, end;
	double cpu_time;

	clock_gettime( CLOCK_REALTIME, &start );
	for(i=1; i<=MAX; i++){
		clz_iter(k);
	}
	clock_gettime( CLOCK_REALTIME, &end);
	cpu_time = diff_in_second( start, end );
	printf("cost of clz_iter(): %lf (sec)\n", cpu_time);
///////////////////////

    clock_gettime( CLOCK_REALTIME, &start );
    for(i=1; i<=MAX; i++){
		clz_binary_search(k);
    }

    clock_gettime( CLOCK_REALTIME, &end);
    cpu_time = diff_in_second( start, end );
    printf("cost of clz_binary_search(): %lf (sec)\n", cpu_time);
//////////////////

    clock_gettime( CLOCK_REALTIME, &start );
    for(i=1; i<=MAX; i++){
       clz_byte_shift(k);
    }

    clock_gettime( CLOCK_REALTIME, &end);
    cpu_time = diff_in_second( start, end );
    printf("cost of clz_byte_shift(): %lf (sec)\n", cpu_time);
	///////////////

    clock_gettime( CLOCK_REALTIME, &start );
    for(i=1; i<=MAX; i++){
       clz_recur(k);
    }
    clock_gettime( CLOCK_REALTIME, &end);
    cpu_time = diff_in_second( start, end );
    printf("cost of clz_recur(): %lf (sec)\n", cpu_time);

	return 0;
}
