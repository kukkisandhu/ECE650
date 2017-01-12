#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
//#include <mqueue.h>
#include <sys/stat.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h> 
#include <string.h> 
#include<semaphore.h>
#include<sys/time.h>
#define QUEUE_SIZE  6
#define _XOPEN_SOURCE 600

sem_t ConsumeSem;//unnamed Semaphore
sem_t ProduceSem;//Unnamed
sem_t lock;
int *Buffer = NULL;
int count = 0; 
pthread_mutex_t lock_mutex = PTHREAD_MUTEX_INITIALIZER;
int P=1;
int C=1;
int N;
int B;
int countC =0; 
void* produce(void *thread_ptr);//definition to the structure producer
void* consume(void *thread_ptr);//definition to the struct consumer;


struct timeval time_a,time_b,time_c;
double timea;
double timeb;
double timec;

struct producer{
	int Parent_ID;
	int MaxNum;
	int NumberOfProducer;
	int Buffersize;
	int NumberOfConsumer;

}producer;

struct consumer{
	int CID;
	int size;
	int Buffersize;

}consumer;

int main (int argc, char *argv[])
{
	FILE *fp;	
	int i;	
	char *Bn; 
	B = strtol(argv[2], &Bn, 10);
	char *Nn;
	N = strtol(argv[1], &Nn, 10);
	
	sem_init(&ProduceSem, 0, B);
	sem_init(&ConsumeSem, 0, 0);
	
	pthread_t threadid[P+C];
	Buffer = malloc(sizeof(int)*B);
	
	for(i = 0; i < B; i ++){
		Buffer[i] = -1; 
	}
	
	int a; 
	struct producer data[P]; 
	//FILE *fp;
	//fp=fopen("mydata2.txt","a");	
	
	for(a = 0; a < P; a++){
		data[a].Parent_ID = a; 
		data[a].Buffersize = B; 
		data[a].MaxNum = N; 
		data[a].NumberOfProducer = P; 
		data[a].NumberOfConsumer = C;
		int thread_Number;
		gettimeofday(&time_a,NULL);
	        timea=time_a.tv_sec+time_a.tv_usec/1000000.0;
		thread_Number = pthread_create(&threadid[a], NULL, &produce,(void *) &data[a]);
		gettimeofday(&time_b,NULL);
	    timeb=time_b.tv_sec+time_b.tv_usec/1000000.0;
		
		printf("Initialization Time=%.61f seconds\n",(timeb-timea));
		printf("Thread = %d\n",thread_Number);
	//FILE *fp;
	//fp=fopen("mydata2.txt","a");	

	}
	struct consumer data1[C];
	int b;
	//FILE *fp;
	//fp=fopen("mydata2.txt","a");	
	for(b = 0; b < C; b++){
		
		data1[b].CID =b;
		data1[b].size = N; 
		data1[b].Buffersize = B; 
	
		int thread_Number;
			
		thread_Number = pthread_create(&threadid[b+P], NULL,  &consume,(void *) &data1[b]);
		gettimeofday(&time_c,NULL);
        timec=time_c.tv_sec+time_c.tv_usec/1000000.0;
        printf("Transmission Time=%.61f seconds\n",timec-timeb);
	//	fprintf(fp,"\n%.61f",(time2-time1));
	//fprintf(fp,"\n");
	}
	
	int c;
	for(c =0 ; c <C+P; c++ ){
		pthread_join (threadid[c], NULL);

	}

	return 0; 
}
void* produce(void* ptr){ 
	struct producer *data;            
    data = (struct producer*) ptr;
	int i; 
	int n = data->Parent_ID;
	int semValue;
	while(n < data->MaxNum){		
		sem_getvalue(&ProduceSem, &semValue);
		sem_wait(&ProduceSem);  		
		pthread_mutex_lock (&lock_mutex);
		
		for(i = 0; i <data->Buffersize; i++ ){
			if(Buffer[i] == -1){
				
				
				Buffer[i] = n; 
				break; 	
			}
		}
		pthread_mutex_unlock (&lock_mutex);

		
		
		sem_post(&ConsumeSem);
		
		n = n + data->NumberOfProducer; 
		}
	return NULL;
} 
void* consume(void* ptr){
	struct consumer *data;            
    data = (struct consumer *) ptr;
	countC++; 
	int semValue;
	while(count < data->size){
			int i;
			int flag = 0;
			sem_getvalue(&ConsumeSem, &semValue);
			sem_wait(&ConsumeSem);
		for(i = 0; i <data->Buffersize; i++ ){
			pthread_mutex_lock (&lock_mutex);
				if(Buffer[i] != -1){
				
					Buffer[i] = -1;
					count++;
					sem_post(&ProduceSem);
					
				
				}
				pthread_mutex_unlock (&lock_mutex);					
			}
	}
		if(count >= data->size-1)
			{
				sem_post(&ConsumeSem); 
			}	
		

return NULL;
} 
