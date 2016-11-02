/*

time calculation may be a nightware! 
Beware of float, int, unsigned int conversion.
you could use gettimeofday(...) to get down to microseconds!

*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct _flow
{
	float arrivalTime ;
	float transTime ;
	int priority ;
	int id ;
} flow;

#define MAXFLOW 255


flow flowList[MAXFLOW];   // parse input in an array of flow
flow *queueList[MAXFLOW];  // store waiting flows while transmission pipe is occupied.
pthread_t thrList[MAXFLOW]; // each thread executes one flow
pthread_mutex_t trans_mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t trans_cvar = PTHREAD_COND_INITIALIZER;

void requestPipe(flow *item) {
	/*pthread_mutex_lock(&trans_mtx);*/

	if (queueList[0] == NULL) {
		printf("%dthe queue is empty && pipe available TRANSMIT\n",item->id);
		queueList[0] = item;
		/*...do some stuff..*/
			/*unlock mutex;*/
		/*return ;*/
	}else{
		printf("%dthe queue is not empty SORT SHIT\n",item->id);

		int k = 1;
		while(queueList[k] != NULL){
			queueList[k]; /*Add to the end of the list*/
			k++;
		}
		queueList[k] = item;
	}
	/*add item in queue, sort the queue according rules*/

	/*printf(Waiting...);*/
	// while(){wait;}
	// ^^from lab
	// key point here..
	// wait till pipe to be available and be at the top of the queue

	// update queue

	/*pthread_mutex_unlock(&trans_mtx);*/
	printf("***********\n");
}

/*void releasePipe() {*/
	/*// I believe you genuis will figure this out!*/
/*}*/


// entry point for each thread created
void *thrFunction(void *flowItem) {

	flow *item = (flow *)flowItem ;

	// wait for arrival
	usleep((int)(100000*item->arrivalTime));
	printf("Flow %2d arrives: arrival time (%.2f), transmission time (%.1f), priority (%2d).\n",item->id,.1*item->arrivalTime,.1*item->transTime,item->priority);

	requestPipe(item);
	printf("start\n");

	// sleep for transmission time
	sleep(1);

	/*releasePipe(item);*/
	printf("Finish\n");
}

int main(int argc, char **argv) {


	// file handling

	FILE *fp = fopen(argv[1],"r"); // read fild
	if(fp == NULL) {
		perror("Error opening file");
		return(-1);
	}

	// read number of flows
	char *numFlows = (char*)malloc(sizeof(1024));
	fgets(numFlows,1024,fp);

	int x;
	for(x = 0; x<atoi(numFlows);x++) {
		/*implement strtok using past examples*/
		char* line = (char*)malloc(sizeof(1024));
		fgets(line,1024,fp);
		char* str;
		flow *nwflow = (flow*)malloc(sizeof(flow));
		/*id*/
		str = strtok(line,":");
		(*nwflow).id = atoi(str);
		/*arrival time*/
		str = strtok(NULL,",");
		(*nwflow).arrivalTime = atof(str);
		/*transmission time*/
		str = strtok(NULL,",");
		(*nwflow).transTime = atof(str);
		/*priority*/
		str = strtok(NULL,",");
		(*nwflow).priority = atoi(str);
		flowList[x] = *nwflow;
		/*parse line, using strtok() & atoi(), store them in flowList[i];*/
	}

	fclose(fp); // release file descriptor

	for(x = 0; x<atoi(numFlows);x++){
		// create a thread for each flow 
		pthread_create(&thrList[x], NULL, thrFunction, (void *)&flowList[x]);
	}
	// wait for all threads to terminate
	sleep(10);
	pthread_join(thrList[0],NULL);

	// destroy mutex & condition variable

	return 0;
}
