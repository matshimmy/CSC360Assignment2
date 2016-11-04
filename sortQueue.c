#include <string.h>
#include "flowStruct.h"

void sortQueue(flow *item, flow **queueList){
	int k = 0;
	while(queueList[k] != NULL){
		k++;
	}
	queueList[k] = item;
	int origink = k;
	if(k != 0){/*first in queue*/
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
	}
}
