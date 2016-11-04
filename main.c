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
int iTrans = 0; /*inTransmission 0 == in transmission*/

void requestPipe(flow *item) {
	pthread_mutex_lock(&trans_mtx);

	if (queueList[0] == NULL && iTrans == 0) {
		iTrans = 1;/*in transmission*/
		pthread_mutex_unlock(&trans_mtx);
		return;
		printf("THE THREAD SHOULDN'T get here\n");
	}

	int k = 1;
	while(queueList[k] != NULL){
		k++;
	}
	queueList[k] = item;

	int origink = k;
	/*(a) The one with the highest priority start its transmission first.*/
	while(queueList[k]->priority<queueList[k-1]->priority){
		/*bubble sort up*/
		flow *temp = queueList[k-1];
		queueList[k-1] = queueList[k];
		queueList[k] = temp;

		k--;
		if(k == 0){ /*The array would be out of bounds if k is 0*/
			break;
		}
	}
	/*(b) If there is a tie at the highest priority, the one whose arrival time is the earliest start its transmission first.*/
	if(k != 0 ){ /*already at the top of the list*/
		while(queueList[k]->priority==queueList[k-1]->priority){
			/*bubble sort up*/
			if(queueList[k]->arrivalTime<queueList[k-1]->arrivalTime){
				flow *temp = queueList[k-1];
				queueList[k-1] = queueList[k];
				queueList[k] = temp;
			}

			k--;
			if(k == 0){ /*The array would be out of bounds if k is 0*/
				break;
			}
		}
	}
	/*(c) If there is still a tie, the one that has the smallest transmission time starts its transmission first.*/
	if(k != 0 ){ /*already at the top of the list*/
		while(queueList[k]->priority==queueList[k-1]->priority && queueList[k]->arrivalTime==queueList[k-1]->arrivalTime){
			/*bubble sort up*/
			if(queueList[k]->transTime<queueList[k-1]->transTime){
				flow *temp = queueList[k-1];
				queueList[k-1] = queueList[k];
				queueList[k] = temp;
			}

			k--;
			if(k == 0){ /*The array would be out of bounds if k is 0*/
				break;
			}
		}
	}
	/*(d) If there is still a tie, the one that appears first in the input file starts its transmission first.*/
	if(k != 0 ){ /*already at the top of the list*/
		while(queueList[k]->priority==queueList[k-1]->priority && queueList[k]->arrivalTime==queueList[k-1]->arrivalTime && queueList[k]->transTime==queueList[k-1]->transTime){
			/*bubble sort up*/
			if(queueList[k]->id<queueList[k-1]->id){
				flow *temp = queueList[k-1];
				queueList[k-1] = queueList[k];
				queueList[k] = temp;
			}

			k--;
			if(k == 0){ /*The array would be out of bounds if k is 0*/
				break;
			}
		}
	}
	if(k != 0){
		printf("Flow %2d waits for the finish of flow %2d. \n",queueList[k]->id,queueList[k-1]->id);
	}
	/*wait till pipe to be available and be at the top of the queue*/
	while(item->id != queueList[0]->id && iTrans == 0){ /*while the item is not at the front of the list*/
		pthread_cond_wait(&trans_cvar, &trans_mtx);
	}

	/*Moves everything over one*/
	int i = 0;
	while(queueList[i+1] != NULL){
		queueList[i] = queueList[i+1];
	}
	queueList[i] = NULL;

	pthread_mutex_unlock(&trans_mtx);
}

void releasePipe() {
	pthread_cond_signal(&trans_cvar);
}


/*entry point for each thread created*/
void *thrFunction(void *flowItem) {

	flow *item = (flow *)flowItem ;

	/*wait for arrival*/
	usleep((int)(100000*item->arrivalTime)); /*Multiply by 100000 because input file is gives in seconds one decimal place to the right and usleep is amount of microseconds*/
	printf("Flow %2d arrives: arrival time (%.2f), transmission time (%.1f), priority (%2d).\n",item->id,.1*item->arrivalTime,.1*item->transTime,item->priority);

	requestPipe(item);
	printf("Flow %2d starts its transmission at time %.2f.\n",item->id,item->transTime);

	/*sleep for transmission time*/
	usleep((int)(100000*item->transTime));

	iTrans = 0;
	releasePipe();
	printf("Flow %2d finishes its transmission at time %d.\n",item->id,item->transTime); /*TODO: calculate actual transmission time*/

	return 0; /*TODO: check this warning*/
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
