#line 228 "/home/apoorve/splash2_benchmark-1.0/codes/null_macros/c.m4.null.POSIX"

#line 1 "fft.C"
/*************************************************************************/
/*                                                                       */
/*  Copyright (c) 1994 Stanford University                               */
/*                                                                       */
/*  All rights reserved.                                                 */
/*                                                                       */
/*  Permission is given to use, copy, and modify this software for any   */
/*  non-commercial purpose as long as this copyright notice is not       */
/*  removed.  All other uses, including redistribution in whole or in    */
/*  part, are forbidden without prior written permission.                */
/*                                                                       */
/*  This software is provided with absolutely no warranty and no         */
/*  support.                                                             */
/*                                                                       */
/*************************************************************************/

/*************************************************************************/
/*                                                                       */
/*  Perform 1D fast Fourier transform using six-step FFT method          */
/*                                                                       */
/*  1) Performs staggered, blocked transposes for cache-line reuse       */
/*  2) Roots of unity rearranged and distributed for only local          */
/*     accesses during application of roots of unity                     */
/*  3) Small set of roots of unity elements replicated locally for       */
/*     1D FFTs (less than root N elements replicated at each node)       */
/*  4) Matrix data structures are padded to reduce cache mapping         */
/*     conflicts                                                         */
/*                                                                       */
/*  Command line options:                                                */
/*                                                                       */
/*  -mM : M = even integer; 2**M total complex data points transformed.  */
/*  -pP : P = number of processors; Must be a power of 2.                */
/*  -nN : N = number of cache lines.                                     */
/*  -lL : L = Log base 2 of cache line length in bytes.                  */
/*  -s  : Print individual processor timing statistics.                  */
/*  -t  : Perform FFT and inverse FFT.  Test output by comparing the     */
/*        integral of the original data to the integral of the data      */
/*        that results from performing the FFT and inverse FFT.          */
/*  -o  : Print out complex data points.                                 */
/*  -h  : Print out command line options.                                */
/*                                                                       */
/*  Note: This version works under both the FORK and SPROC models        */
/*                                                                       */
/*************************************************************************/

#include <stdio.h>
#include <math.h>
#define PAGE_SIZE               4096
#define NUM_CACHE_LINES        65536 
#define LOG2_LINE_SIZE             4
#define PI                         3.1416
#define DEFAULT_M                 10
#define DEFAULT_P                  1


#line 55
#include <pthread.h>
#line 55
#include <sys/time.h>
#line 55
#include <unistd.h>
#line 55
#include <stdlib.h>
#line 55
#define MAX_THREADS 32
#line 55
pthread_t PThreadTable[MAX_THREADS];
#line 55


#define SWAP_VALS(a,b) {double tmp; tmp=a; a=b; b=tmp;}

struct GlobalMemory {
  long id;
  pthread_mutex_t (idlock);
  
#line 62
struct {
#line 62
	pthread_mutex_t	mutex;
#line 62
	pthread_cond_t	cv;
#line 62
	unsigned long	counter;
#line 62
	unsigned long	cycle;
#line 62
} (start);
#line 62

  long *transtimes;
  long *totaltimes;
  unsigned long starttime;
  unsigned long finishtime;
  unsigned long initdonetime;
} *Global;


long P = DEFAULT_P;
long M = DEFAULT_M;
long N;                  /* N = 2^M                                */
long rootN;              /* rootN = N^1/2                          */
double *x;              /* x is the original time-domain data     */
double *trans;          /* trans is used as scratch space         */
double *umain;          /* umain is roots of unity for 1D FFTs    */
double *umain2;         /* umain2 is entire roots of unity matrix */
long test_result = 0;
long doprint = 0;
long dostats = 0;
long transtime = 0;
long transtime2 = 0;
long avgtranstime = 0;
long avgcomptime = 0;
unsigned long transstart = 0;
unsigned long transend = 0;
long maxtotal=0;
long mintotal=0;
double maxfrac=0;
double minfrac=0;
double avgfractime=0;
long orig_num_lines = NUM_CACHE_LINES;     /* number of cache lines */
long num_cache_lines = NUM_CACHE_LINES;    /* number of cache lines */
long log2_line_size = LOG2_LINE_SIZE;
long line_size;
long rowsperproc;
double ck1;
double ck3;                        /* checksums for testing answer */
long pad_length;

void SlaveStart(void);
double TouchArray(double *x, double *scratch, double *u, double *upriv, long MyFirst, long MyLast);
double CheckSum(double *x);
void InitX(double *x);
void InitU(long N, double *u);
void InitU2(long N, double *u, long n1);
long BitReverse(long M, long k);
void FFT1D(long direction, long M, long N, double *x, double *scratch, double *upriv, double *umain2,
	   long MyNum, long *l_transtime, long MyFirst, long MyLast, long pad_length, long test_result, long dostats);
void TwiddleOneCol(long direction, long n1, long j, double *u, double *x, long pad_length);
void Scale(long n1, long N, double *x);
void Transpose(long n1, double *src, double *dest, long MyNum, long MyFirst, long MyLast, long pad_length);
void CopyColumn(long n1, double *src, double *dest);
void Reverse(long N, long M, double *x);
void FFT1DOnce(long direction, long M, long N, double *u, double *x);
void PrintArray(long N, double *x);
void printerr(char *s);
long log_2(long number);

void srand48(long int seedval);
double drand48(void);

int main(int argc, char *argv[])
{
  long i; 
  long c;
  extern char *optarg;
  long m1;
  long factor;
  long pages;
  unsigned long start;

  {
#line 134
	struct timeval	FullTime;
#line 134

#line 134
	gettimeofday(&FullTime, NULL);
#line 134
	(start) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 134
};

  while ((c = getopt(argc, argv, "p:m:n:l:stoh")) != -1) {
    switch(c) {
      case 'p': P = atoi(optarg); 
                if (P < 1) {
                  printerr("P must be >= 1\n");
                  exit(-1);
                }
                if (log_2(P) == -1) {
                  printerr("P must be a power of 2\n");
                  exit(-1);
                }
	        break;  
      case 'm': M = atoi(optarg); 
                m1 = M/2;
                if (2*m1 != M) {
                  printerr("M must be even\n");
                  exit(-1);
                }
	        break;  
      case 'n': num_cache_lines = atoi(optarg); 
                orig_num_lines = num_cache_lines;
                if (num_cache_lines < 1) {
                  printerr("Number of cache lines must be >= 1\n");
                  exit(-1);
                }
	        break;  
      case 'l': log2_line_size = atoi(optarg); 
                if (log2_line_size < 0) {
                  printerr("Log base 2 of cache line length in bytes must be >= 0\n");
                  exit(-1);
                }
	        break;  
      case 's': dostats = !dostats; 
	        break;
      case 't': test_result = !test_result; 
	        break;
      case 'o': doprint = !doprint; 
	        break;
      case 'h': printf("Usage: FFT <options>\n\n");
                printf("options:\n");
                printf("  -mM : M = even integer; 2**M total complex data points transformed.\n");
                printf("  -pP : P = number of processors; Must be a power of 2.\n");
                printf("  -nN : N = number of cache lines.\n");
                printf("  -lL : L = Log base 2 of cache line length in bytes.\n");
                printf("  -s  : Print individual processor timing statistics.\n");
                printf("  -t  : Perform FFT and inverse FFT.  Test output by comparing the\n");
                printf("        integral of the original data to the integral of the data that\n");
                printf("        results from performing the FFT and inverse FFT.\n");
                printf("  -o  : Print out complex data points.\n");
                printf("  -h  : Print out command line options.\n\n");
                printf("Default: FFT -m%1d -p%1d -n%1d -l%1d\n",
                       DEFAULT_M,DEFAULT_P,NUM_CACHE_LINES,LOG2_LINE_SIZE);
		exit(0);
	        break;
    }
  }

  {;};

  N = 1<<M;
  rootN = 1<<(M/2);
  rowsperproc = rootN/P;
  if (rowsperproc == 0) {
    printerr("Matrix not large enough. 2**(M/2) must be >= P\n");
    exit(-1);
  }

  line_size = 1 << log2_line_size;
  if (line_size < 2*sizeof(double)) {
    printf("WARNING: Each element is a complex double (%ld bytes)\n",2*sizeof(double));
    printf("  => Less than one element per cache line\n");
    printf("     Computing transpose blocking factor\n");
    factor = (2*sizeof(double)) / line_size;
    num_cache_lines = orig_num_lines / factor;
  }  
  if (line_size <= 2*sizeof(double)) {
    pad_length = 1;
  } else {
    pad_length = line_size / (2*sizeof(double));
  }

  if (rowsperproc * rootN * 2 * sizeof(double) >= PAGE_SIZE) {
    pages = (2 * pad_length * sizeof(double) * rowsperproc) / PAGE_SIZE;
    if (pages * PAGE_SIZE != 2 * pad_length * sizeof(double) * rowsperproc) {
      pages ++;
    }
    pad_length = (pages * PAGE_SIZE) / (2 * sizeof(double) * rowsperproc);
  } else {
    pad_length = (PAGE_SIZE - (rowsperproc * rootN * 2 * sizeof(double))) /

                 (2 * sizeof(double) * rowsperproc);
    if (pad_length * (2 * sizeof(double) * rowsperproc) !=
        (PAGE_SIZE - (rowsperproc * rootN * 2 * sizeof(double)))) {
      printerr("Padding algorithm unsuccessful\n");
      exit(-1);
    }
  }

/*  Global = (struct GlobalMemory *) valloc(sizeof(struct GlobalMemory));;
  x = (double *) valloc(2*(N+rootN*pad_length)*sizeof(double)+PAGE_SIZE);;
  trans = (double *) valloc(2*(N+rootN*pad_length)*sizeof(double)+PAGE_SIZE);;
  umain = (double *) valloc(2*rootN*sizeof(double));;  
  umain2 = (double *) valloc(2*(N+rootN*pad_length)*sizeof(double)+PAGE_SIZE);;

  Global->transtimes = (long *) valloc(P*sizeof(long));;  
  Global->totaltimes = (long *) valloc(P*sizeof(long));;  
*/


posix_memalign(&Global ,sizeof(struct GlobalMemory *) ,(sizeof(struct GlobalMemory)));
posix_memalign(&x ,sizeof(double *) ,(2*(N+rootN*pad_length)*sizeof(double)+PAGE_SIZE));
posix_memalign(&trans ,sizeof(double *) ,(2*(N+rootN*pad_length)*sizeof(double)+PAGE_SIZE));
posix_memalign(&umain ,sizeof(double *) ,(2*rootN*sizeof(double)));
posix_memalign(&umain2 ,sizeof(double *) ,(2*(N+rootN*pad_length)*sizeof(double)+PAGE_SIZE));
posix_memalign(&Global->transtimes ,sizeof(long *) ,(P*sizeof(long)));
posix_memalign(&Global->totaltimes ,sizeof(long *) ,(P*sizeof(long)));


  if (Global == NULL) {
    printerr("Could not malloc memory for Global\n");
    exit(-1);
  } else if (x == NULL) {
    printerr("Could not malloc memory for x\n");
    exit(-1);
  } else if (trans == NULL) {
    printerr("Could not malloc memory for trans\n");
    exit(-1);
  } else if (umain == NULL) {
    printerr("Could not malloc memory for umain\n");
    exit(-1);
  } else if (umain2 == NULL) {
    printerr("Could not malloc memory for umain2\n");
    exit(-1);
  }

  x = (double *) (((unsigned long) x) + PAGE_SIZE - ((unsigned long) x) % PAGE_SIZE);
  trans = (double *) (((unsigned long) trans) + PAGE_SIZE - ((unsigned long) trans) % PAGE_SIZE);
  umain2 = (double *) (((unsigned long) umain2) + PAGE_SIZE - ((unsigned long) umain2) % PAGE_SIZE);

/* In order to optimize data distribution, the data structures x, trans, 
   and umain2 have been aligned so that each begins on a page boundary. 
   This ensures that the amount of padding calculated by the program is 
   such that each processor's partition ends on a page boundary, thus 
   ensuring that all data from these structures that are needed by a 
   processor can be allocated to its local memory */

/* POSSIBLE ENHANCEMENT:  Here is where one might distribute the x,
   trans, and umain2 data structures across physically distributed 
   memories as desired.
   
   One way to place data is as follows:

   double *base;
   long i;

   i = ((N/P)+(rootN/P)*pad_length)*2;
   base = &(x[0]);
   for (j=0;j<P;j++) {
    Place all addresses x such that (base <= x < base+i) on node j
    base += i;
   }

   The trans and umain2 data structures can be placed in a similar manner.

   */

  printf("\n");
  printf("FFT with Blocking Transpose\n");
  printf("   %ld Complex Doubles\n",N);
  printf("   %ld Processors\n",P);
  if (num_cache_lines != orig_num_lines) {
    printf("   %ld Cache lines\n",orig_num_lines);
    printf("   %ld Cache lines for blocking transpose\n",num_cache_lines);
  } else {
    printf("   %ld Cache lines\n",num_cache_lines);
  }
  printf("   %d Byte line size\n",(1 << log2_line_size));
  printf("   %d Bytes per page\n",PAGE_SIZE);
  printf("\n");

  {
#line 316
	unsigned long	Error;
#line 316

#line 316
	Error = pthread_mutex_init(&(Global->start).mutex, NULL);
#line 316
	if (Error != 0) {
#line 316
		printf("Error while initializing barrier.\n");
#line 316
		exit(-1);
#line 316
	}
#line 316

#line 316
	Error = pthread_cond_init(&(Global->start).cv, NULL);
#line 316
	if (Error != 0) {
#line 316
		printf("Error while initializing barrier.\n");
#line 316
		pthread_mutex_destroy(&(Global->start).mutex);
#line 316
		exit(-1);
#line 316
	}
#line 316

#line 316
	(Global->start).counter = 0;
#line 316
	(Global->start).cycle = 0;
#line 316
};
  {pthread_mutex_init(&(Global->idlock), NULL);};
  Global->id = 0;
  InitX(x);                  /* place random values in x */

  if (test_result) {
    ck1 = CheckSum(x);
  }
  if (doprint) {
    printf("Original data values:\n");
    PrintArray(N, x);
  }

  InitU(N,umain);               /* initialize u arrays*/
  InitU2(N,umain2,rootN);

  /* fire off P processes */

  {
#line 334
	long	i, Error;
#line 334

#line 334
	for (i = 0; i < (P) - 1; i++) {
#line 334
		Error = pthread_create(&PThreadTable[i], NULL, (void * (*)(void *))(SlaveStart), NULL);
#line 334
		if (Error != 0) {
#line 334
			printf("Error in pthread_create().\n");
#line 334
			exit(-1);
#line 334
		}
#line 334
	}
#line 334

#line 334
	SlaveStart();
#line 334
};
  {
#line 335
	unsigned long	i, Error;
#line 335
	for (i = 0; i < (P) - 1; i++) {
#line 335
		Error = pthread_join(PThreadTable[i], NULL);
#line 335
		if (Error != 0) {
#line 335
			printf("Error in pthread_join().\n");
#line 335
			exit(-1);
#line 335
		}
#line 335
	}
#line 335
};

  if (doprint) {
    if (test_result) {
      printf("Data values after inverse FFT:\n");
    } else {
      printf("Data values after FFT:\n");
    }
    PrintArray(N, x);
  }

  transtime = Global->transtimes[0];
  printf("\n");
  printf("                 PROCESS STATISTICS\n");
  printf("            Computation      Transpose     Transpose\n");
  printf(" Proc          Time            Time        Fraction\n");
  printf("    0        %10ld     %10ld      %8.5f\n",
         Global->totaltimes[0],Global->transtimes[0],
         ((double)Global->transtimes[0])/Global->totaltimes[0]);
  if (dostats) {
    transtime2 = Global->transtimes[0];
    avgtranstime = Global->transtimes[0];
    avgcomptime = Global->totaltimes[0];
    maxtotal = Global->totaltimes[0];
    mintotal = Global->totaltimes[0];
    maxfrac = ((double)Global->transtimes[0])/Global->totaltimes[0];
    minfrac = ((double)Global->transtimes[0])/Global->totaltimes[0];
    avgfractime = ((double)Global->transtimes[0])/Global->totaltimes[0];
    for (i=1;i<P;i++) {
      if (Global->transtimes[i] > transtime) {
        transtime = Global->transtimes[i];
      }
      if (Global->transtimes[i] < transtime2) {
        transtime2 = Global->transtimes[i];
      }
      if (Global->totaltimes[i] > maxtotal) {
        maxtotal = Global->totaltimes[i];
      }
      if (Global->totaltimes[i] < mintotal) {
        mintotal = Global->totaltimes[i];
      }
      if (((double)Global->transtimes[i])/Global->totaltimes[i] > maxfrac) {
        maxfrac = ((double)Global->transtimes[i])/Global->totaltimes[i];
      }
      if (((double)Global->transtimes[i])/Global->totaltimes[i] < minfrac) {
        minfrac = ((double)Global->transtimes[i])/Global->totaltimes[i];
      }
      printf("  %3ld        %10ld     %10ld      %8.5f\n",
             i,Global->totaltimes[i],Global->transtimes[i],
             ((double)Global->transtimes[i])/Global->totaltimes[i]);
      avgtranstime += Global->transtimes[i];
      avgcomptime += Global->totaltimes[i];
      avgfractime += ((double)Global->transtimes[i])/Global->totaltimes[i];
    }
    printf("  Avg        %10.0f     %10.0f      %8.5f\n",
           ((double) avgcomptime)/P,((double) avgtranstime)/P,avgfractime/P);
    printf("  Max        %10ld     %10ld      %8.5f\n",
	   maxtotal,transtime,maxfrac);
    printf("  Min        %10ld     %10ld      %8.5f\n",
	   mintotal,transtime2,minfrac);
  }
  Global->starttime = start;
  printf("\n");
  printf("                 TIMING INFORMATION\n");
  printf("Start time                        : %16lu\n",
	  Global->starttime);
  printf("Initialization finish time        : %16lu\n",
	  Global->initdonetime);
  printf("Overall finish time               : %16lu\n",
	  Global->finishtime);
  printf("Total time with initialization    : %16lu\n",
	  Global->finishtime-Global->starttime);
  printf("Total time without initialization : %16lu\n",
	  Global->finishtime-Global->initdonetime);
  printf("Overall transpose time            : %16ld\n",
         transtime);
  printf("Overall transpose fraction        : %16.5f\n",
         ((double) transtime)/(Global->finishtime-Global->initdonetime));
  printf("\n");

  if (test_result) {
    ck3 = CheckSum(x);
    printf("              INVERSE FFT TEST RESULTS\n");
    printf("Checksum difference is %.3f (%.3f, %.3f)\n",
	   ck1-ck3, ck1, ck3);
    if (fabs(ck1-ck3) < 0.001) {
      printf("TEST PASSED\n");
    } else {
      printf("TEST FAILED\n");
    }
  }

  {exit(0);};
}


void SlaveStart()
{
  long i;
  long MyNum;
  double *upriv;
  long initdone; 
  long finish; 
  long l_transtime=0;
  long MyFirst; 
  long MyLast;

  {pthread_mutex_lock(&(Global->idlock));};
    MyNum = Global->id;
    Global->id++;
  {pthread_mutex_unlock(&(Global->idlock));}; 

  {;};

/* POSSIBLE ENHANCEMENT:  Here is where one might pin processes to
   processors to avoid migration */

  {
#line 452
	unsigned long	Error, Cycle;
#line 452
	long		Cancel, Temp;
#line 452

#line 452
	Error = pthread_mutex_lock(&(Global->start).mutex);
#line 452
	if (Error != 0) {
#line 452
		printf("Error while trying to get lock in barrier.\n");
#line 452
		exit(-1);
#line 452
	}
#line 452

#line 452
	Cycle = (Global->start).cycle;
#line 452
	if (++(Global->start).counter != (P)) {
#line 452
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 452
		while (Cycle == (Global->start).cycle) {
#line 452
			Error = pthread_cond_wait(&(Global->start).cv, &(Global->start).mutex);
#line 452
			if (Error != 0) {
#line 452
				break;
#line 452
			}
#line 452
		}
#line 452
		pthread_setcancelstate(Cancel, &Temp);
#line 452
	} else {
#line 452
		(Global->start).cycle = !(Global->start).cycle;
#line 452
		(Global->start).counter = 0;
#line 452
		Error = pthread_cond_broadcast(&(Global->start).cv);
#line 452
	}
#line 452
	pthread_mutex_unlock(&(Global->start).mutex);
#line 452
};

  upriv = (double *) malloc(2*(rootN-1)*sizeof(double));  
  if (upriv == NULL) {
    fprintf(stderr,"Proc %ld could not malloc memory for upriv\n",MyNum);
    exit(-1);
  }
  for (i=0;i<2*(rootN-1);i++) {
    upriv[i] = umain[i];
  }   

  MyFirst = rootN*MyNum/P;
  MyLast = rootN*(MyNum+1)/P;

  TouchArray(x, trans, umain2, upriv, MyFirst, MyLast);

  {
#line 468
	unsigned long	Error, Cycle;
#line 468
	long		Cancel, Temp;
#line 468

#line 468
	Error = pthread_mutex_lock(&(Global->start).mutex);
#line 468
	if (Error != 0) {
#line 468
		printf("Error while trying to get lock in barrier.\n");
#line 468
		exit(-1);
#line 468
	}
#line 468

#line 468
	Cycle = (Global->start).cycle;
#line 468
	if (++(Global->start).counter != (P)) {
#line 468
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 468
		while (Cycle == (Global->start).cycle) {
#line 468
			Error = pthread_cond_wait(&(Global->start).cv, &(Global->start).mutex);
#line 468
			if (Error != 0) {
#line 468
				break;
#line 468
			}
#line 468
		}
#line 468
		pthread_setcancelstate(Cancel, &Temp);
#line 468
	} else {
#line 468
		(Global->start).cycle = !(Global->start).cycle;
#line 468
		(Global->start).counter = 0;
#line 468
		Error = pthread_cond_broadcast(&(Global->start).cv);
#line 468
	}
#line 468
	pthread_mutex_unlock(&(Global->start).mutex);
#line 468
};

/* POSSIBLE ENHANCEMENT:  Here is where one might reset the
   statistics that one is measuring about the parallel execution */

  if ((MyNum == 0) || (dostats)) {
    {
#line 474
	struct timeval	FullTime;
#line 474

#line 474
	gettimeofday(&FullTime, NULL);
#line 474
	(initdone) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 474
};
  }

  /* perform forward FFT */
  FFT1D(1, M, N, x, trans, upriv, umain2, MyNum, &l_transtime, MyFirst, 
	MyLast, pad_length, test_result, dostats);

  /* perform backward FFT */
  if (test_result) {
    FFT1D(-1, M, N, x, trans, upriv, umain2, MyNum, &l_transtime, MyFirst, 
	  MyLast, pad_length, test_result, dostats);
  }  

  if ((MyNum == 0) || (dostats)) {
    {
#line 488
	struct timeval	FullTime;
#line 488

#line 488
	gettimeofday(&FullTime, NULL);
#line 488
	(finish) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 488
};
    Global->transtimes[MyNum] = l_transtime;
    Global->totaltimes[MyNum] = finish-initdone;
  }
  if (MyNum == 0) {
    Global->finishtime = finish;
    Global->initdonetime = initdone;
  }
}


double TouchArray(double *x, double *scratch, double *u, double *upriv, long MyFirst, long MyLast)
{
  long i,j,k;
  double tot = 0.0;

  /* touch my data */
  for (j=0;j<2*(rootN-1);j++) {
    tot += upriv[j];
  }   
  for (j=MyFirst; j<MyLast; j++) {
    k = j * (rootN + pad_length);
    for (i=0;i<rootN;i++) {
      tot += x[2*(k+i)] + x[2*(k+i)+1] + 
             scratch[2*(k+i)] + scratch[2*(k+i)+1] +
	     u[2*(k+i)] + u[2*(k+i)+1];
    }
  }  
  return tot;
}


double CheckSum(double *x)
{
  long i,j,k;
  double cks;

  cks = 0.0;
  for (j=0; j<rootN; j++) {
    k = j * (rootN + pad_length);
    for (i=0;i<rootN;i++) {
      cks += x[2*(k+i)] + x[2*(k+i)+1];
    }
  }

  return(cks);
}


void InitX(double *x)
{
  long i,j,k;

  srand48(0);
  for (j=0; j<rootN; j++) {
    k = j * (rootN + pad_length);
    for (i=0;i<rootN;i++) {
      x[2*(k+i)] = drand48();
      x[2*(k+i)+1] = drand48();
    }
  }
}


void InitU(long N, double *u)
{
  long q; 
  long j; 
  long base; 
  long n1;

  for (q=0; 1<<q<N; q++) {  
    n1 = 1<<q;
    base = n1-1;
    for (j=0; j<n1; j++) {
      if (base+j > rootN-1) { 
	return;
      }
      u[2*(base+j)] = cos(2.0*PI*j/(2*n1));
      u[2*(base+j)+1] = -sin(2.0*PI*j/(2*n1));
    }
  }
}


void InitU2(long N, double *u, long n1)
{
  long i,j,k; 

  for (j=0; j<n1; j++) {  
    k = j*(rootN+pad_length);
    for (i=0; i<n1; i++) {  
      u[2*(k+i)] = cos(2.0*PI*i*j/(N));
      u[2*(k+i)+1] = -sin(2.0*PI*i*j/(N));
    }
  }
}


long BitReverse(long M, long k)
{
  long i; 
  long j; 
  long tmp;

  j = 0;
  tmp = k;
  for (i=0; i<M; i++) {
    j = 2*j + (tmp&0x1);
    tmp = tmp>>1;
  }
  return(j);
}


void FFT1D(long direction, long M, long N, double *x, double *scratch, double *upriv, double *umain2,
           long MyNum, long *l_transtime, long MyFirst, long MyLast, long pad_length, long test_result, long dostats)
{
  long j;
  long m1; 
  long n1;
  unsigned long clocktime1;
  unsigned long clocktime2;

  m1 = M/2;
  n1 = 1<<m1;

  {
#line 615
	unsigned long	Error, Cycle;
#line 615
	long		Cancel, Temp;
#line 615

#line 615
	Error = pthread_mutex_lock(&(Global->start).mutex);
#line 615
	if (Error != 0) {
#line 615
		printf("Error while trying to get lock in barrier.\n");
#line 615
		exit(-1);
#line 615
	}
#line 615

#line 615
	Cycle = (Global->start).cycle;
#line 615
	if (++(Global->start).counter != (P)) {
#line 615
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 615
		while (Cycle == (Global->start).cycle) {
#line 615
			Error = pthread_cond_wait(&(Global->start).cv, &(Global->start).mutex);
#line 615
			if (Error != 0) {
#line 615
				break;
#line 615
			}
#line 615
		}
#line 615
		pthread_setcancelstate(Cancel, &Temp);
#line 615
	} else {
#line 615
		(Global->start).cycle = !(Global->start).cycle;
#line 615
		(Global->start).counter = 0;
#line 615
		Error = pthread_cond_broadcast(&(Global->start).cv);
#line 615
	}
#line 615
	pthread_mutex_unlock(&(Global->start).mutex);
#line 615
};

  if ((MyNum == 0) || (dostats)) {
    {
#line 618
	struct timeval	FullTime;
#line 618

#line 618
	gettimeofday(&FullTime, NULL);
#line 618
	(clocktime1) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 618
};
  }

  /* transpose from x into scratch */
  Transpose(n1, x, scratch, MyNum, MyFirst, MyLast, pad_length);
  
  if ((MyNum == 0) || (dostats)) {
    {
#line 625
	struct timeval	FullTime;
#line 625

#line 625
	gettimeofday(&FullTime, NULL);
#line 625
	(clocktime2) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 625
};
    *l_transtime += (clocktime2-clocktime1);
  }

  /* do n1 1D FFTs on columns */
  for (j=MyFirst; j<MyLast; j++) {
    FFT1DOnce(direction, m1, n1, upriv, &scratch[2*j*(n1+pad_length)]);
    TwiddleOneCol(direction, n1, j, umain2, &scratch[2*j*(n1+pad_length)], pad_length);
  }  

  {
#line 635
	unsigned long	Error, Cycle;
#line 635
	long		Cancel, Temp;
#line 635

#line 635
	Error = pthread_mutex_lock(&(Global->start).mutex);
#line 635
	if (Error != 0) {
#line 635
		printf("Error while trying to get lock in barrier.\n");
#line 635
		exit(-1);
#line 635
	}
#line 635

#line 635
	Cycle = (Global->start).cycle;
#line 635
	if (++(Global->start).counter != (P)) {
#line 635
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 635
		while (Cycle == (Global->start).cycle) {
#line 635
			Error = pthread_cond_wait(&(Global->start).cv, &(Global->start).mutex);
#line 635
			if (Error != 0) {
#line 635
				break;
#line 635
			}
#line 635
		}
#line 635
		pthread_setcancelstate(Cancel, &Temp);
#line 635
	} else {
#line 635
		(Global->start).cycle = !(Global->start).cycle;
#line 635
		(Global->start).counter = 0;
#line 635
		Error = pthread_cond_broadcast(&(Global->start).cv);
#line 635
	}
#line 635
	pthread_mutex_unlock(&(Global->start).mutex);
#line 635
};

  if ((MyNum == 0) || (dostats)) {
    {
#line 638
	struct timeval	FullTime;
#line 638

#line 638
	gettimeofday(&FullTime, NULL);
#line 638
	(clocktime1) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 638
};
  }
  /* transpose */
  Transpose(n1, scratch, x, MyNum, MyFirst, MyLast, pad_length);

  if ((MyNum == 0) || (dostats)) {
    {
#line 644
	struct timeval	FullTime;
#line 644

#line 644
	gettimeofday(&FullTime, NULL);
#line 644
	(clocktime2) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 644
};
    *l_transtime += (clocktime2-clocktime1);
  }

  /* do n1 1D FFTs on columns again */
  for (j=MyFirst; j<MyLast; j++) {
    FFT1DOnce(direction, m1, n1, upriv, &x[2*j*(n1+pad_length)]);
    if (direction == -1)
      Scale(n1, N, &x[2*j*(n1+pad_length)]);
  }

  {
#line 655
	unsigned long	Error, Cycle;
#line 655
	long		Cancel, Temp;
#line 655

#line 655
	Error = pthread_mutex_lock(&(Global->start).mutex);
#line 655
	if (Error != 0) {
#line 655
		printf("Error while trying to get lock in barrier.\n");
#line 655
		exit(-1);
#line 655
	}
#line 655

#line 655
	Cycle = (Global->start).cycle;
#line 655
	if (++(Global->start).counter != (P)) {
#line 655
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 655
		while (Cycle == (Global->start).cycle) {
#line 655
			Error = pthread_cond_wait(&(Global->start).cv, &(Global->start).mutex);
#line 655
			if (Error != 0) {
#line 655
				break;
#line 655
			}
#line 655
		}
#line 655
		pthread_setcancelstate(Cancel, &Temp);
#line 655
	} else {
#line 655
		(Global->start).cycle = !(Global->start).cycle;
#line 655
		(Global->start).counter = 0;
#line 655
		Error = pthread_cond_broadcast(&(Global->start).cv);
#line 655
	}
#line 655
	pthread_mutex_unlock(&(Global->start).mutex);
#line 655
};

  if ((MyNum == 0) || (dostats)) {
    {
#line 658
	struct timeval	FullTime;
#line 658

#line 658
	gettimeofday(&FullTime, NULL);
#line 658
	(clocktime1) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 658
};
  }

  /* transpose back */
  Transpose(n1, x, scratch, MyNum, MyFirst, MyLast, pad_length);

  if ((MyNum == 0) || (dostats)) {
    {
#line 665
	struct timeval	FullTime;
#line 665

#line 665
	gettimeofday(&FullTime, NULL);
#line 665
	(clocktime2) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 665
};
    *l_transtime += (clocktime2-clocktime1);
  }

  {
#line 669
	unsigned long	Error, Cycle;
#line 669
	long		Cancel, Temp;
#line 669

#line 669
	Error = pthread_mutex_lock(&(Global->start).mutex);
#line 669
	if (Error != 0) {
#line 669
		printf("Error while trying to get lock in barrier.\n");
#line 669
		exit(-1);
#line 669
	}
#line 669

#line 669
	Cycle = (Global->start).cycle;
#line 669
	if (++(Global->start).counter != (P)) {
#line 669
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 669
		while (Cycle == (Global->start).cycle) {
#line 669
			Error = pthread_cond_wait(&(Global->start).cv, &(Global->start).mutex);
#line 669
			if (Error != 0) {
#line 669
				break;
#line 669
			}
#line 669
		}
#line 669
		pthread_setcancelstate(Cancel, &Temp);
#line 669
	} else {
#line 669
		(Global->start).cycle = !(Global->start).cycle;
#line 669
		(Global->start).counter = 0;
#line 669
		Error = pthread_cond_broadcast(&(Global->start).cv);
#line 669
	}
#line 669
	pthread_mutex_unlock(&(Global->start).mutex);
#line 669
};

  /* copy columns from scratch to x */
  if ((test_result) || (doprint)) {  
    for (j=MyFirst; j<MyLast; j++) {
      CopyColumn(n1, &scratch[2*j*(n1+pad_length)], &x[2*j*(n1+pad_length)]); 
    }  
  }  

  {
#line 678
	unsigned long	Error, Cycle;
#line 678
	long		Cancel, Temp;
#line 678

#line 678
	Error = pthread_mutex_lock(&(Global->start).mutex);
#line 678
	if (Error != 0) {
#line 678
		printf("Error while trying to get lock in barrier.\n");
#line 678
		exit(-1);
#line 678
	}
#line 678

#line 678
	Cycle = (Global->start).cycle;
#line 678
	if (++(Global->start).counter != (P)) {
#line 678
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 678
		while (Cycle == (Global->start).cycle) {
#line 678
			Error = pthread_cond_wait(&(Global->start).cv, &(Global->start).mutex);
#line 678
			if (Error != 0) {
#line 678
				break;
#line 678
			}
#line 678
		}
#line 678
		pthread_setcancelstate(Cancel, &Temp);
#line 678
	} else {
#line 678
		(Global->start).cycle = !(Global->start).cycle;
#line 678
		(Global->start).counter = 0;
#line 678
		Error = pthread_cond_broadcast(&(Global->start).cv);
#line 678
	}
#line 678
	pthread_mutex_unlock(&(Global->start).mutex);
#line 678
};
}


void TwiddleOneCol(long direction, long n1, long j, double *u, double *x, long pad_length)
{
  long i;
  double omega_r; 
  double omega_c; 
  double x_r; 
  double x_c;

  for (i=0; i<n1; i++) {
    omega_r = u[2*(j*(n1+pad_length)+i)];
    omega_c = direction*u[2*(j*(n1+pad_length)+i)+1];  
    x_r = x[2*i]; 
    x_c = x[2*i+1];
    x[2*i] = omega_r*x_r - omega_c*x_c;
    x[2*i+1] = omega_r*x_c + omega_c*x_r;
  }
}


void Scale(long n1, long N, double *x)
{
  long i;

  for (i=0; i<n1; i++) {
    x[2*i] /= N;
    x[2*i+1] /= N;
  }
}


void Transpose(long n1, double *src, double *dest, long MyNum, long MyFirst, long MyLast, long pad_length)
{
  long i; 
  long j; 
  long k; 
  long l; 
  long m;
  long blksize;
  long numblks;
  long firstfirst;
  long h_off;
  long v_off;
  long v;
  long h;
  long n1p;
  long row_count;

  blksize = MyLast-MyFirst;
  numblks = (2*blksize)/num_cache_lines;
  if (numblks * num_cache_lines != 2 * blksize) {
    numblks ++;
  }
  blksize = blksize / numblks;
  firstfirst = MyFirst;
  row_count = n1/P;
  n1p = n1+pad_length;
  for (l=MyNum+1;l<P;l++) {
    v_off = l*row_count;
    for (k=0; k<numblks; k++) {
      h_off = firstfirst;
      for (m=0; m<numblks; m++) {
        for (i=0; i<blksize; i++) {
	  v = v_off + i;
          for (j=0; j<blksize; j++) {
	    h = h_off + j;
            dest[2*(h*n1p+v)] = src[2*(v*n1p+h)];
            dest[2*(h*n1p+v)+1] = src[2*(v*n1p+h)+1];
          }
        }
	h_off += blksize;
      }
      v_off+=blksize;
    }
  }

  for (l=0;l<MyNum;l++) {
    v_off = l*row_count;
    for (k=0; k<numblks; k++) {
      h_off = firstfirst;
      for (m=0; m<numblks; m++) {
        for (i=0; i<blksize; i++) {
	  v = v_off + i;
          for (j=0; j<blksize; j++) {
            h = h_off + j;
            dest[2*(h*n1p+v)] = src[2*(v*n1p+h)];
            dest[2*(h*n1p+v)+1] = src[2*(v*n1p+h)+1];
          }
        }
	h_off += blksize;
      }
      v_off+=blksize;
    }
  }

  v_off = MyNum*row_count;
  for (k=0; k<numblks; k++) {
    h_off = firstfirst;
    for (m=0; m<numblks; m++) {
      for (i=0; i<blksize; i++) {
        v = v_off + i;
        for (j=0; j<blksize; j++) {
          h = h_off + j;
          dest[2*(h*n1p+v)] = src[2*(v*n1p+h)];
          dest[2*(h*n1p+v)+1] = src[2*(v*n1p+h)+1];
	}
      }
      h_off += blksize;
    }
    v_off+=blksize;
  }
}


void CopyColumn(long n1, double *src, double *dest)
{
  long i;

  for (i=0; i<n1; i++) {
    dest[2*i] = src[2*i];
    dest[2*i+1] = src[2*i+1];
  }
}


void Reverse(long N, long M, double *x)
{
  long j, k;

  for (k=0; k<N; k++) {
    j = BitReverse(M, k);
    if (j > k) {
      SWAP_VALS(x[2*j], x[2*k]);
      SWAP_VALS(x[2*j+1], x[2*k+1]);
    }
  }
}


void FFT1DOnce(long direction, long M, long N, double *u, double *x)
{
  long j; 
  long k; 
  long q; 
  long L; 
  long r; 
  long Lstar;
  double *u1; 
  double *x1; 
  double *x2;
  double omega_r; 
  double omega_c; 
  double tau_r; 
  double tau_c; 
  double x_r; 
  double x_c;

  Reverse(N, M, x);

  for (q=1; q<=M; q++) {
    L = 1<<q; r = N/L; Lstar = L/2;
    u1 = &u[2*(Lstar-1)];
    for (k=0; k<r; k++) {
      x1 = &x[2*(k*L)];
      x2 = &x[2*(k*L+Lstar)];
      for (j=0; j<Lstar; j++) {
	omega_r = u1[2*j]; 
        omega_c = direction*u1[2*j+1];
	x_r = x2[2*j]; 
        x_c = x2[2*j+1];
	tau_r = omega_r*x_r - omega_c*x_c;
	tau_c = omega_r*x_c + omega_c*x_r;
	x_r = x1[2*j]; 
        x_c = x1[2*j+1];
	x2[2*j] = x_r - tau_r;
	x2[2*j+1] = x_c - tau_c;
	x1[2*j] = x_r + tau_r;
	x1[2*j+1] = x_c + tau_c;
      }
    }
  }
}


void PrintArray(long N, double *x)
{
  long i, j, k;

  for (i=0; i<rootN; i++) {
    k = i*(rootN+pad_length);
    for (j=0; j<rootN; j++) {
      printf(" %4.2f %4.2f", x[2*(k+j)], x[2*(k+j)+1]);
      if (i*rootN+j != N-1) {
        printf(",");
      }
      if ((i*rootN+j+1) % 8 == 0) {
        printf("\n");
      }
    }
  }
  printf("\n");
  printf("\n");
}


void printerr(char *s)
{
  fprintf(stderr,"ERROR: %s\n",s);
}


long log_2(long number)
{
  long cumulative = 1, out = 0, done = 0;

  while ((cumulative < number) && (!done) && (out < 50)) {
    if (cumulative == number) {
      done = 1;
    } else {
      cumulative = cumulative * 2;
      out ++;
    }
  }

  if (cumulative == number) {
    return(out);
  } else {
    return(-1);
  }
}

