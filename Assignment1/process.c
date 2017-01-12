#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<mqueue.h>
#include<errno.h>
#include<sys/stat.h>
#include<unistd.h>
#include<time.h>
#include<sys/time.h>
#include<sys/wait.h>
#include<unistd.h>

struct timeval time_1,time_2,time_3;
double time1,time2,time3;
void producer(int N,const int B)
{
int randomNumber;
pid_t child_pid;
mqd_t qdes;
mode_t mode=S_IRUSR|S_IWUSR;
char qname[]="/Gursharan'sMessageQueue";
struct mq_attr attr;//Setting the attributes of the Message Queue
attr.mq_flags=0;
attr.mq_maxmsg=B;
attr.mq_msgsize=sizeof(randomNumber);
gettimeofday(&time_1,NULL);//FUNCTION TO GET THE TIME OF THE PROCESS 
time1=time_1.tv_sec+time_1.tv_usec/1000000.0;
child_pid=fork();	//Child id after forking the process where Parent Process is given by getpid() and child process by child_pid
//int ChildStatus;
if(child_pid!=0)
{
	qdes=mq_open(qname,O_RDWR|O_CREAT,mode,&attr);		//Producer-mq_open result after creating and initializing qdes
	if(qdes==-1)
	{
		perror("Producer-mq_open() has failed to open");
		exit(1);
	}
int i=0;
	gettimeofday(&time_2,NULL);
	time2=time_2.tv_sec+time_2.tv_usec/1000000.0;
	printf("Initialization Time=%.61f seconds\n",(time2-time1));//Time taken to Initialize
while(i<N)
	{
		randomNumber=(rand()%9);		//Random Number generator for generating a random number
	if(mq_send(qdes,(char*)&randomNumber,sizeof(randomNumber),0)==-1)
	{
		perror("Producer has Failed to deliever the message to the  Queue");//Failure to send a message to the Queue.
	}
	fflush(stdout);		////to write whatever written  in the file is written on the console.
	i++;
}
gettimeofday(&time_3,NULL);
time3=time_3.tv_sec+time_3.tv_usec/1000000.0;
printf("Transmission Time=%.61f seconds\n",(time3-time2));
if(mq_close(qdes)==-1)
{
	perror("Producer-mq_close() has failed to closed");//When the message queue has failed to close.
	exit(2);
}
if(mq_unlink(qname)!=0)
{
	perror("mq_unlink() failed");
	exit(3);
	}
int childStatus;
wait(&childStatus);
if(WIFEXITED(childStatus))
{
	printf("child Process exited normally\n");
}
}
else
{
	sleep(1);						//The child process,here it would be consumer

if(B<N)
{
	int i,receivedInteger;
	receivedInteger=0;
	i=0;
	int ret_open=mq_open(qname,O_RDONLY,mode,&attr);
while(i<N)
{
if(mq_receive(ret_open,(char*)&receivedInteger,sizeof(int),0)==-1)
{
perror("Consumer-Error");
}
fflush(stdout);
i++;
}
if(mq_close(ret_open)==-1)
{
perror("Consumer-mq_close() has failed to close");
exit(2);
}
}
}
}
int main(int argc,char*argv[])
{
	int N,B;
N=atoi(argv[1]);//Input of the N and B values while runnnig the Program.
B=atoi(argv[2]);
printf("Values of N is %d and B is%d\n",N,B);
if(N<B){
printf("Program quit");
exit(1);
}
else{
fflush(stdout);
producer(N,B);
return 0;
}
}
