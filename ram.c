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

/* node in a partition */
struct node
{
	int vertex;
	int degree;
	int weight;
};

typedef struct node Node;

void print_matrix(int **sq);                                          // print matrix
int faults();                                                         // compute the number of faults from the given percentage
void srand( unsigned seed );
void construct_partitions(Node *left, Node *right, int **sq);         // construct partitions
void print_partitions(Node *l, Node *r, int lsize, int rsize);        // print partitions
int minimum_degree(Node *part, int size);                             // finds the minimum degree
void max_weight(Node *part, int size);                                // finds the node with maximum weight
void swap(Node *p, int from, int to);                                 // swap 2 nodes
int nodes_zerodegree(Node *part, int *size);                          // finds the nodes with zero degree
int find_k(Node *left, Node *right, int **sq, int lsize, int rsize);  // finds the maximum k (kxk defect-free crossbar) from the 
                                                                      // nxn partially-defective crossbar
int gettimeofday(struct timeval *tv, struct timezone *tz);

int main(int argc, char *argv[])
{
	int i, j, l;
	int **matrix;               // N x N matrix
	Node *lpart, *rpart;        // left and right partition
	int p, r, c;
	int k = 0, k_temp;
	float res;
	int res1;
	int *consistency;
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
	
	lpart = (Node *)malloc(N*sizeof(Node));
	rpart = (Node *)malloc(N*sizeof(Node));
	
	consistency = (int *)calloc(N+1, sizeof(int));
   
	srand( time(NULL) );
	p = faults();
      
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
		
		construct_partitions(lpart, rpart, matrix);
  
		k_temp = find_k(lpart, rpart, matrix, N, N);
		consistency[k_temp]++;
		k += k_temp;
	}
	gettimeofday(&tv2, &tzone);
	elapsed_seconds  = tv2.tv_sec  - tv1.tv_sec;
    elapsed_useconds = tv2.tv_usec - tv1.tv_usec;

    //printf("Elapsed time = %f seconds + %f microseconds\n",
    //        (double)elapsed_seconds/(double)T, (double)elapsed_useconds/(double)T);

    elapsed_utime = (elapsed_seconds) * 1000000 + elapsed_useconds;
    //elapsed_mtime = ((elapsed_seconds) * 1000 + elapsed_useconds/1000.0) + 0.5;

    //printf("Elapsed time = %f microseconds\n", (double)elapsed_utime/(double)T);
    //printf("Elapsed time = %f milliseconds\n", (double)elapsed_mtime/(double)T);
	printf("%f ",(double)elapsed_utime/(double)T);
	
	res = (float)k/(float)T;
	
	res1 = (int)res;
		if((res - (float)res1 >= 0.5))
			res1++;
         
	//printf("K = %d (%.2f %%), #crosspoints = %d (%.2f %%)\n", res1, (double)res1*100/(double)N, res1*res1, (double)(res1*res1*100)/(double)(N*N) );
	printf("%d",res1);

	for(i = 0; i < N; i++)
	      if(consistency[i])
              printf(" %d %d",i, consistency[i]);
             
	free(consistency);
	free(lpart);
	free(rpart);
	for(i = 0; i < N; i++)
		free(matrix[i]);
	free(matrix);
	//system("PAUSE");
	
	return 0;
}

/* print matrix */
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

/* compute the number of faults from the given percentage */
int faults()
{
	int intpart;
	double num_faults = (double)N*N*P/100.0;
	
	intpart = (int)num_faults;
	if((num_faults - intpart) > 0.0) // eg p = 5.6 -> return p = 6 
		return num_faults + 1;
	else
		return num_faults;
}

/* construct partitions */
void construct_partitions(Node *left, Node *right, int **sq)
{
	int i, j;
	
	for(i = 0; i < N; i++)
	{
		left[i].degree = 0;
		right[i].degree = 0;
		left[i].vertex = i;
		right[i].vertex = i;
	}
	
	for(i = 0; i < N; i++)
	{
		for(j = 0; j < N; j++)
		{
			if(sq[i][j] == 1) // compute the degree of each node
			{
				left[i].degree++;
				right[j].degree++;
			}
		}
	}
}

/* print partitions */
void print_partitions(Node *l, Node *r, int lsize, int rsize)
{
	int i;
	
	printf("left size: %d\t right size: %d\n\n",lsize,rsize);
	for(i = 0; i < N; i++){
		printf("vertex=%d  degree=%d  weight=%d\n",l[i].vertex, l[i].degree, l[i].weight);
	}
	printf("\n");
	for(i = 0; i < N; i++){
		printf("vertex=%d  degree=%d  weight=%d\n",r[i].vertex, r[i].degree, r[i].weight);
	}
	printf("\n\n");
}

/* finds the minimum degree */
int minimum_degree(Node *part, int size)
{
	int i, mindegree;
	
	mindegree = part[0].degree;
	for(i = 0; i < size; i++)
	{
		if(mindegree > part[i].degree)
			mindegree = part[i].degree;
	}
	
	return mindegree;
}

/* finds the node with maximum weight */
void max_weight(Node *part, int size)
{
	int i, max_weight, max_node = 0;
	
	max_weight = part[0].weight;
	for(i = 0; i < size; i++)
	{
		if(max_weight < part[i].weight)
		{
			max_weight = part[i].weight;
			max_node = i;
		}
	}
	
	swap(part, max_node, size - 1); // put the node with the maximum weight to the end of the partition
}

/* finds the maximum k (kxk defect-free crossbar) from the nxn partially-defective crossbar */
int find_k(Node *left, Node *right, int **sq, int lsize, int rsize)
{
	int choose_partition = 0; // 0 : choose left partition, 1 : choose right partition
	int i, j, k, m;
	int leftnum = 0;  // the number of nodes with zero degree in the left partition
	int rightnum = 0; // the number of nodes with zero degree in the right partition
	int mindegree;
	
	leftnum = nodes_zerodegree(left, &lsize);    // finds the nodes with zero degree in the left partition
	rightnum = nodes_zerodegree(right, &rsize);  // finds the nodes with zero degree in the right partition
	
	while(lsize != 0 && rsize != 0)        // U and V != 0
	{
		if(!choose_partition) // left partition
		{
			for(i = 0; i < lsize; i++)
				left[i].weight = 0;   // set the weight to zero
				
			mindegree = minimum_degree(right, rsize);  // node in right partition with minimum degree
			
			for(i = 0; i < rsize; i++)
			{
				if(mindegree == right[i].degree) // the nodes with minimum degree in the right partition
				{
					for(j = 0; j < lsize; j++)
					{
						if(sq[left[j].vertex][right[i].vertex]) // the nodes in the left partition which are connected to the minimum degree nodes in the rigth partition
							left[j].weight++;                // compute the weight of each node in the left partition
					}
				}
			}
			
			max_weight(left, lsize); // node in the left partition with maximum degree
			
			for(i = 0; i < rsize; i++)
				if(sq[left[lsize-1].vertex][right[i].vertex])
					right[i].degree--; // decreases the degree of the u neighbours
					
			rightnum += nodes_zerodegree(right, &rsize);
				
			lsize -= 1; // decreases the size of left partition
			left[lsize].degree = -1;
		}
		
		else // right partition
		{
			for(i = 0; i < rsize; i++)
				right[i].weight = 0; // set the weight to zero
				
			mindegree = minimum_degree(left, lsize); // node in left partition with minimum degree
			
			for(i = 0; i < lsize; i++)
			{
				if(mindegree == left[i].degree) // the nodes with minimum degree in the left partition
				{
					for(j = 0; j < rsize; j++)
					{
						if(sq[left[i].vertex][right[j].vertex]) // the nodes in the right partition which are connected to the minimum degree nodes in the left partition
							right[j].weight++;                // compute the weight of each node in the right partition
					}
				}
			}
			
			max_weight(right, rsize); // node in the right partition with maximum degree
			
			for(i = 0; i < lsize; i++)
				if(sq[left[i].vertex][right[rsize-1].vertex])
					left[i].degree--; // decreases the degree of the v neighbours
			
			leftnum += nodes_zerodegree(left, &lsize);
			
			rsize -= 1; // decreases the size of left partition
			right[rsize].degree = -1;
		}	
		choose_partition = !choose_partition;
	}
	
	if(leftnum > rightnum ) 
		return rightnum;
	else
		return leftnum;
}

/* swap 2 nodes */
void swap(Node *p, int from, int to)
{
	Node temp;
	
	temp.vertex = p[to].vertex;
	temp.degree = p[to].degree;
	temp.weight = p[to].weight;
	
	p[to].vertex = p[from].vertex;
	p[to].degree = p[from].degree;
	p[to].weight = p[from].weight;
	
	p[from].vertex = temp.vertex;
	p[from].degree = temp.degree;
	p[from].weight = temp.weight;
}

/* finds the nodes with zero degree */
int nodes_zerodegree(Node *part, int *size)
{
	int i, k, j = 0;
	int nodes_zero = 0;
	int temp[*size];
	int result;
	
	for(i = 0; i < *size; i++)
		if(part[i].degree == 0)  // if d(u)=0
		{
			nodes_zero++;
			temp[j] = i;
			j++;
		}
	result = nodes_zero; 
	k = *size - 1;
	*size -= nodes_zero; // the size of the partition is reduced
		
	j = 0;
	while(nodes_zero != 0)
	{
		if(part[k].degree)
		{
			swap(part, temp[j], k); // put the nodes with zero degree to the end of the partition
			j++;  
		}
		k--;
		nodes_zero--;
	}

	return result;
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
