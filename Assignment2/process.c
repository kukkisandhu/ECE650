#include <sys/msg.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include<time.h>
#include <math.h>
#include <limits.h>
#include <string.h>

#define QUEUE_NAME "/Gursharan'sQueue"
#define MAX_SIZE 10
#define MSG_STOP "exit"
#define GOODRAND random()
#define GOODRANDMAX INT_MAX
#define RANDTYPE long
struct request{
int req_num;
int req_size;
};

int ct_parms_without_io = 0;
int ct_parms_ = 0;
int ct_parms_io = 0;
float probability_consumer = 0;


//Poisson Distribution to generate t1
RANDTYPE poisson(double lambda){
  RANDTYPE k=0;
  double L=exp(-lambda), p=1;
  do {
    ++k;
    p *= GOODRAND/(double)GOODRANDMAX;
  } while (p > L);
  return --k;
}
//File *fp;

int rs=0;

#define CHECKONTHEQUEUE(x) \
    do { \
        if (!(x)) { \
            fprintf(stderr, "%s:%d: ", __func__, __LINE__); \
            perror(#x); \
            exit(-1); \
        } \
    } while (0) \

	int main(int argc, char *argv[])
	{
		int countError=0;
		int var1=0,var2=0;
		mqd_t mq;           /* queue descriptor */
   		struct mq_attr attr; /* output attr struct for getattr */
		char buffer[MAX_SIZE + 1];
		int requestsTransmitted;
		int must_stop = 0;
		int N = 0;
		struct timeval t0;
		struct timeval t1;
		struct timeval t2;
		struct timeval t3;
		int shmid;
		int *received;
		key_t key;

		double elapsed = 0;
		float probability = 0;
		int numberOfConsumers = 0;
		int numberOfProducers = 0;
		int i = 0;
		int status = 0 ;
		int t = 0;

		if (argc < 10)
		{
				printf("Usage : %s <T> <B> <P> <C> <P_t parms> <R_s parms> <C_t1 parms> <C_t2 parms> <p_i>\n", argv[0]);
				return 1;
		}
		
		/*
	     * Create the segment.
	     */
		shmid = shmget(IPC_PRIVATE, sizeof(int)* 2, S_IRUSR | S_IWUSR); 

		/*
	     * Attaching the segment to our data space.
	     */
	    received = (int* ) shmat(shmid, NULL, 0);
	    received[0] = 0;
	    received[1] = 0;
		srand(time(NULL));
		numberOfProducers = atoi(argv[3]);
		numberOfConsumers = atoi(argv[4]);
		ct_parms_ = atoi (argv[5]);
		ct_parms_=poisson(ct_parms_);
		printf("ct_parms_ %d\n",ct_parms_);	
		//rs=atoi(argv[2]);	
		ct_parms_without_io = atoi (argv[7]);
		//ct_parms_without_io=poisson(ct_parms_without_io);
		printf("ct_parms_without_io %d\n",ct_parms_without_io);	
		
		ct_parms_io = atoi(argv[8]);
		//ct_parms_io=poisson(ct_parms_io);
		printf("ct_parms_io %d\n",ct_parms_io);	

		probability_consumer = atof(argv[9]);
//		var1=probability_consumer*ct_parms_io;
//		ct_parms_io=poisson(var1);			
//		var2=(1.0-probability_consumer)*ct_parms_without_io;
//		ct_parms_without_io=poisson(var2);
//		printf("ct_parms_io=%d\n",ct_parms_io);
//		printf("ct_parms_without_io=%d\n",ct_parms_without_io);			
		var1=probability_consumer*ct_parms_io;
		ct_parms_io=poisson(var1);
	 /* Errorcheck if one of the arguments is negative.*/
    if ( atoi(argv[1]) < 0 || atoi(argv[2]) < 0 || atoi(argv[3]) < 0 || atoi(argv[4]) < 0  || 
    	 atoi(argv[5]) < 0 || atoi(argv[6]) < 0 || atoi(argv[7]) < 0 || atoi(argv[8]) < 0  || 
    	 atoi(argv[9]) < 0 || atoi(argv[10]) < 0 || atoi(argv[11]) < 0 || atoi(argv[12]) < 0 || atoi(argv[13]) < 0) 
    { 
        fprintf(stderr, "Every argument has to be a positive integer.\n");
        return -1;
}
		//Probability distribution on the parameters.
		//Random delays 
		printf("ct_parms_without_io with Io %d\n",ct_parms_io);
		var2=(1-probability_consumer)*ct_parms_without_io;
		//Consumer sleep time that would be accessing the IO and database.
 
		ct_parms_without_io=poisson(var2);
		printf("ct_parms_without_io without Io %d\n",ct_parms_without_io);
		//Conusmer time that would not be accessing the IO or database,thus time would be relatively faster than the other.
		srand(time(NULL));
		gettimeofday(&t0, 0);
		
		/* initialize the queue attributes */
		attr.mq_flags = 0;
		attr.mq_maxmsg = atoi(argv[2]);
		attr.mq_msgsize = MAX_SIZE;
		attr.mq_curmsgs = 0;
		//Where the maximum size of the Queue is 256, as set by the hard setting the linux
		N = atoi(argv[1]);

		mq_unlink(QUEUE_NAME);
		/* create the message queue */
		mq = mq_open(QUEUE_NAME, O_EXCL | O_CREAT | O_RDWR, 0644, &attr);
		CHECKONTHEQUEUE((mqd_t)-1 != mq);

		while(i < numberOfConsumers)
		{
			int pid = fork();//Parent process forking the child Process.

			if (pid == 0)//child process
			{
				printf("Number of Consumers %d has been created\n",i);
				//Consumer Count that has been created
				int consumed = 0;
				ssize_t bytes_read;

				do{
					probability = ((rand() % 100) / 100.0);
					if(probability <= probability_consumer)
						sleep (ct_parms_without_io);
					else
					{
						sleep (ct_parms_io);
					}

					/* receive the message */
	        		bytes_read = mq_receive(mq, buffer, MAX_SIZE, NULL);
	        		//CHECKONTHEQUEUE(bytes_read >= 0);
				int mq_Error=mq_receive(mq,buffer, MAX_SIZE, NULL);
	        		if(mq_Error !=0){
				
				printf("Consumer is not Consuming\n");
				countError++;
				printf("Total number of times Consumer is not Consuming= %d\n",countError);
				}

				buffer[bytes_read] = '\0';
	        		if (! strncmp(buffer, MSG_STOP, strlen(MSG_STOP)))
			        {
			            must_stop = 1;
				//printf("Queue not consumed");
			        }
			        else
			        {
			        	printf("Consumed item number %d\n",received[1]); 
			        	received[1]++; 
			        }
				}
				while(!must_stop);

				printf("All done consuming.\n");
				/* cleanup */
	    		CHECKONTHEQUEUE((mqd_t)-1 != mq_close(mq));
	    		exit(1);
    		
			}
			i++;
		}

		i = 0;
		while(i < numberOfProducers)
		{
			int pid = fork();

			if (pid == 0)
			{
				printf("Producer %d created\n",i);
				//Producer
				gettimeofday(&t1, 0);
				
				int itemsProduced = 0;
				int timeT = 0; 
				while (t < N){
					sleep(ct_parms_);

					strcpy(buffer,"A");//Passing any String to the Buffer.
					 /* send the message */
	        			//CHECKONTHEQUEUE(0 <= mq_send(mq, buffer, MAX_SIZE, 0));
					int mq_Error=mq_send(mq, buffer, MAX_SIZE, 0);
					if(mq_Error != 0){
						printf("Negative Result %d\n",mq_Error);
					        }
					//to find the number of generated after every 10 seconds.
					else if (mq_Error == 0 && t > timeT ) {
						itemsProduced++;
						timeT += 10;
						printf( "Items produced : %d \n", itemsProduced );
						itemsProduced = 0;
					}
					printf("Produced item number %d\n", received[0]);
					
					received[0]++;
					gettimeofday(&t3, 0);
					t = (t3.tv_sec-t0.tv_sec) + (t3.tv_usec-t0.tv_usec)/10000000.0;
				}

				/* send the terminate message to consumer */
				strcpy(buffer,MSG_STOP);
	        	CHECKONTHEQUEUE(0 <= mq_send(mq, buffer, MAX_SIZE, 0));

				printf("Producer finished.\n");

				wait(NULL); // wait for child process
				/* cleanup */
	    		CHECKONTHEQUEUE((mqd_t)-1 != mq_close(mq));
	    		exit(1);	
			}
			i++;
		}

		gettimeofday(&t1, 0);

		for(i = 0 ; i < numberOfProducers; i++)
		{
			wait(&status);
			strcpy(buffer,MSG_STOP);
	        CHECKONTHEQUEUE(0 <= mq_send(mq, buffer, MAX_SIZE, 0));
		}

		for(i = 0; i < numberOfConsumers;i++)
			wait(&status);
		gettimeofday(&t2, 0);
//Extracting the timmings from the process
		CHECKONTHEQUEUE((mqd_t)-1 != mq_unlink(QUEUE_NAME));
	    elapsed = (t1.tv_sec-t0.tv_sec) + (t1.tv_usec-t0.tv_usec)/1000000.0;
		printf("Time to initialize system: %lf seconds\n",elapsed);
		elapsed = (t2.tv_sec-t1.tv_sec) + (t2.tv_usec-t1.tv_usec)/1000000.0;
		printf("Time to transmit data: %lf seconds\n",elapsed);		
		printf("Total transmitted: %d\n",received[0]);
//		printf("Total number of times Consumer is not Consuming %d\n",countError);
		/* detach from the segment: */
		//fflush(stdout);
		//fp=fopen("mydata1.txt","a");

	    if (shmdt(received) == -1) {
	        perror("shmdt");
	        exit(1);
	    }
	}
