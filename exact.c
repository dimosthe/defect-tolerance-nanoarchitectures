#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
 
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

int N, P, T;

struct timezone 
{
  int  tz_minuteswest; /* minutes W of Greenwich */
  int  tz_dsttime;     /* type of dst correction */
};

struct biclique
{
	int k1;
	int k2;
};

typedef struct biclique Bic;

void print_matrix(int **sq);
int faults();
Bic exact(int *l,int *r, int lsize, int rsize,int **sq);
int gettimeofday(struct timeval *tv, struct timezone *tz);

int main(int argc, char *argv[])
{
	int i, j;
	int **matrix;               // N x N matrix
	int *lpart, *rpart;         // left and right partition
    int p, r, c, l;
	int k = 0;
	Bic final;
	float res;
	int res1;
	struct timeval tv1,tv2; 
    struct timezone tzone;
    long elapsed_utime;    /* elapsed time in microseconds */
    long elapsed_mtime;    /* elapsed time in milliseconds */
    long elapsed_seconds;  /* diff between seconds counter */
    long elapsed_useconds; /* diff between microseconds counter */
	
    if( argc != 4 ) /* checks the inputs from the command line */
	{
		printf("Invalid input!! Please try again.\n");
		return 0;
	}
		
	N = atoi(argv[1]); // get N (NxN partially-defective crossbar)
	P = atoi(argv[2]); // get P (defect rate)
	T = atoi(argv[3]); // get number of samples
  
	matrix = (int **)malloc(N*sizeof(int*));
	for(i = 0; i < N; i++)
		matrix[i]=(int *)calloc(N, sizeof(int)); // all values are set to zero
	
	lpart = (int *)malloc(N*sizeof(int));
	rpart = (int *)malloc(N*sizeof(int));
		
	p = faults();
	
	srand( time(NULL) );
	
    gettimeofday(&tv1, NULL); 
    for(l = 0; l < T; l++) 
	{
        for(i = 0; i < N; i++)
		{
			for(j = 0; j < N; j++)
				matrix[i][j] = 0;
		}
        
        /* random faults */
        for(i = 0; i < p; i++)
	    {
		      r = rand()%N;
		      c = rand()%N;
	
		      if(matrix[r][c] == 0)
                    matrix[r][c] = 1;
              else
			      i--;
        }
	
        for(i = 0; i < N; i++) // initialize partitions
        {
             lpart[i] = i; 
             rpart[i] = i;        
        }
	
        final = exact(lpart, rpart, N, N, matrix);
        
        k += (final.k1 + final.k2)/2;
    } 
	
    gettimeofday(&tv2, &tzone);
	elapsed_seconds  = tv2.tv_sec  - tv1.tv_sec;
    elapsed_useconds = tv2.tv_usec - tv1.tv_usec;
    elapsed_utime = (elapsed_seconds) * 1000000 + elapsed_useconds;
    printf("%f ",(double)elapsed_utime/(double)T);
    	
	res = (float)k/(float)T;
	
	res1 = (int)res;
		if((res - (float)res1 >= 0.5))
			res1++;
         
	printf("%d",res1);
	free(lpart);
	free(rpart);
	for(i = 0; i < N; i++)
		free(matrix[i]);
	free(matrix);
	
    system("PAUSE");
 }


void print_matrix(int **sq)
{
	int i, j;
	printf("%d x %d matrix:\n",N,N);
	for(i = 0; i < N; i++)
	{
		for(j = 0; j < N; j++)
			printf("%d ",sq[i][j]);
		printf("\n");
	}
	printf("\n");
}

int faults()
{
	int intpart;
	double num_faults = (double)N*N*P/100.0;
	
	intpart = (int)num_faults;
	if((num_faults - intpart) > 0.0)
		return num_faults + 1;
	else
		return num_faults;
}

/* Exact Algorithm*/
Bic exact(int *l, int *r, int lsize, int rsize, int **sq)
{
	int u, i, j;
	int _V[rsize];
 
	Bic res, res1;
	
	if(lsize == 0) /* |U|=0, return 0x|V|*/
	{
		res.k1 = 0;
		res.k2 = rsize;
		return res;
	}
	
	if(rsize == 0) /* |V|=0, return 0x|U|*/
	{
		res.k1 = lsize;
		res.k2 = 0;
		return res;
	}
	
	u = l[lsize-1]; /* take the last node in U */

    j = 0;
	for(i = 0; i < rsize; i++)
		if(!sq[u][r[i]])
		{
			_V[j] = r[i]; /* nodes connected to u in V */
            j++;
		}
			
	res = exact(l,r,lsize-1,rsize, sq);
	res1 = exact(l,_V,lsize-1,j, sq);
	
	res1.k1++;
    if((res.k1+res.k2)>=(res1.k1+res1.k2)) /* return the maximum biclique */
           return res;
    else
          return res1;
    
 }



int gettimeofday(struct timeval *tv, struct timezone *tz)
{
  FILETIME ft;
  unsigned __int64 tmpres = 0;
  static int tzflag = 0;

  if (NULL != tv)
  {
    GetSystemTimeAsFileTime(&ft);

    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;

    tmpres /= 10;  /*convert into microseconds*/
    /*converting file time to unix epoch*/
    tmpres -= DELTA_EPOCH_IN_MICROSECS; 
    tv->tv_sec = (long)(tmpres / 1000000UL);
    tv->tv_usec = (long)(tmpres % 1000000UL);
  }

  if (NULL != tz)
  {
    if (!tzflag)
    {
      _tzset();
      tzflag++;
    }
    tz->tz_minuteswest = _timezone / 60;
    tz->tz_dsttime = _daylight;
  }

  return 0;
}
