/*

time calculation may be a nightware! 
Beware of float, int, unsigned int conversion.
you could use gettimeofday(...) to get down to microseconds!

*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include "sortQueue.h"
#include "flowStruct.h"

#define MAXFLOW 255


flow flowList[MAXFLOW];   // parse input in an array of flow
flow *queueList[MAXFLOW];  // store waiting flows while transmission pipe is occupied.
pthread_t thrList[MAXFLOW]; // each thread executes one flow
pthread_mutex_t trans_mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t trans_cvar = PTHREAD_COND_INITIALIZER;
int iTrans = 0; /*inTransmission 0 == in transmission*/

void requestPipe(flow *item) {
	pthread_mutex_lock(&trans_mtx);

	if (queueList[0] == NULL && iTrans == 0) {
		iTrans = item->id;/*in transmission*/
		pthread_mutex_unlock(&trans_mtx);
		return;
	}

	sortQueue(item, queueList); /*Sorts that item in the queue*/
	
	do{
		usleep(100000); /*wait a little to make sure iTrans has been successful*/
		printf("Flow %2d waits for the finish of flow %2d. \n",item->id,iTrans);
		pthread_cond_wait(&trans_cvar, &trans_mtx);
	}while((item->id != queueList[0]->id) && (iTrans == 0)); /*checks if it is in top of the queue*/
	iTrans = item->id;
	printf("itemid: %d\n",item->id);
	/*Moves everything over one*/
	int i = 0;
	while(queueList[i+1] != NULL){
		queueList[i] = queueList[i+1];
		i++;
	}
	queueList[i] = NULL;

	pthread_mutex_unlock(&trans_mtx);
}

void releasePipe() {
	iTrans = 0; /*reset transmission id*/
	pthread_cond_broadcast(&trans_cvar);
}


/*entry point for each thread created*/
void *thrFunction(void *flowItem) {

	flow *item = (flow *)flowItem ;
	struct timeval start, end;
	float atime;

	/*wait for arrival*/
	usleep((int)(100000*item->arrivalTime)); /*Multiply by 100000 because input file is gives in seconds one decimal place to the right and usleep is amount of microseconds*/
	printf("Flow %2d arrives: arrival time (%.2f), transmission time (%.1f), priority (%2d).\n",item->id,.1*item->arrivalTime,.1*item->transTime,item->priority);

	requestPipe(item);
	printf("Flow %2d starts its transmission at time %.2f.\n",item->id,.1*item->transTime);

	gettimeofday(&start,NULL);
	/*sleep for transmission time*/
	usleep((int)(100000*item->transTime));
	gettimeofday(&end,NULL);

	atime = (float)(((end.tv_sec - start.tv_sec)*1000000L +end.tv_usec) - start.tv_usec)/1000000;
	releasePipe();
	printf("Flow %2d finishes its transmission at time %.2f.\n",item->id,atime);

	return 0;
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
	}

	fclose(fp); // release file descriptor

	for(x = 0; x<atoi(numFlows);x++){
		// create a thread for each flow 
		pthread_create(&thrList[x], NULL, thrFunction, (void *)&flowList[x]);
	}
	for(x = 0; x<atoi(numFlows);x++){
		pthread_join(thrList[x],NULL); /*waits for all flows to terminate*/
	}

	pthread_mutex_destroy(&trans_mtx);
	pthread_cond_destroy(&trans_cvar);

	return 0;
}
