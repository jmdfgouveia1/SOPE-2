#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>


typedef unsigned long QueueElem;
unsigned int listSize;
int* primes;
unsigned int nth;
unsigned int inc;
sem_t ter;
pthread_mutex_t nthmu =PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lismu =PTHREAD_MUTEX_INITIALIZER;

typedef struct{
	QueueElem *v; // pointer to the queue buffer
	unsigned int capacity; // queue capacity
	unsigned int first; // head of the queue
	unsigned int last;// tail of the queue
	sem_t empty; // semaphores and mutex for implementing the
	sem_t full; // producer-consumer paradigm
	pthread_mutex_t mutex;
} CircularQueue;
//------------------------------------------------------------------------------------------
// Allocates space for circular queue 'q' having 'capacity' number of elements
// Initializes semaphores & mutex needed to implement the producer-consumer paradigm
// Initializes indexes of the head and tail of the queue
// TO DO BY STUDENTS: ADD ERROR TESTS TO THE CALLS & RETURN a value INDICATING (UN)SUCESS
void queue_init(CircularQueue **q, unsigned int capacity) // TO DO: change return value
{
	*q = (CircularQueue *) malloc(sizeof(CircularQueue));
	sem_init(&((*q)->empty), 0, capacity);
	sem_init(&((*q)->full), 0, 0);
	pthread_mutex_init(&((*q)->mutex), NULL);
	(*q)->v = (QueueElem *) malloc(capacity * sizeof(QueueElem));
	(*q)->capacity = capacity;
	(*q)->first = 0;
	(*q)->last = 0;
}
//------------------------------------------------------------------------------------------
// Inserts 'value' at the tail of queue 'q'
void queue_put(CircularQueue *q, QueueElem value)
{
// TO DO BY STUDENTS
	sem_wait(&q->empty);
	pthread_mutex_lock(&q->mutex);
	q->v[q->last]=value;
	q->last++;
	if(q->last==10){
		q->last=0;
	}
	pthread_mutex_unlock(&q->mutex);
	sem_post(&q->full);

}
//------------------------------------------------------------------------------------------
// Removes element at the head of queue 'q' and returns its 'value'
QueueElem queue_get(CircularQueue *q)
{
// TO DO BY STUDENTS
	sem_wait(&q->full);
	pthread_mutex_lock(&q->mutex);
	QueueElem qtemp = q->v[q->first];
	q->first++;
	if(q->first==10){
		q->first=0;
	}
	
	pthread_mutex_unlock(&q->mutex);
	sem_post(&q->empty);
	return qtemp;
}
//------------------------------------------------------------------------------------------
// Frees space allocated for the queue elements and auxiliary management data
// Must be called when the queue is no more needed
void queue_destroy(CircularQueue *q){
// TO DO BY STUDENTS
	free(q->v);
	free(q);
}



void* filter(void* q){
	QueueElem prm=queue_get(q);
	QueueElem c1= queue_get(q);
	//printf(" ler %d \n",c1);
	if(prm>ceil(sqrt(listSize))){
		while(c1!=0){
	//		printf(" a ler finais : %d \n",c1);
			if(c1!=0){
				pthread_mutex_lock(&lismu);
			primes[inc]=c1;
			inc++;
			pthread_mutex_unlock(&lismu);
		}
			c1=queue_get(q);

		}
		pthread_mutex_lock(&lismu);
		primes[inc]=prm;
		inc++;
		pthread_mutex_unlock(&lismu);
		while(nth>1){

		}
		sem_post(&ter);
	}
	else{

		CircularQueue* q2;
		queue_init(&q2,10);
		pthread_t tid2;
		pthread_create(&tid2,NULL,filter,q2);
	//	printf("thread created \n");
		pthread_mutex_lock(&nthmu);
		nth++;
		pthread_mutex_unlock(&nthmu);
		while(c1!=0){
			if(c1%prm!=0){
				queue_put(q2,c1);
	//			printf(" por %d \n",c1);
			}	
			
			c1=queue_get(q);
	//		printf(" ler %d \n",c1);

		}
	

	queue_put(q2,0);
	pthread_mutex_lock(&lismu);
	primes[inc]=prm;
	//printf("acabada thread com o primo inicial %d \n",prm);
	inc++;
pthread_mutex_unlock(&lismu);
	}
	pthread_mutex_lock(&nthmu);
	nth--;
	pthread_mutex_unlock(&nthmu);
	return NULL;
}


void * first(void* q){
	int i;

	CircularQueue* q2;
	queue_init(&q2,10);
	pthread_t tid2;
	pthread_create(&tid2,NULL,filter,q2);
	//printf("thread created \n");
	pthread_mutex_lock(&nthmu);
	nth++;
	pthread_mutex_unlock(&nthmu);
	for(i=3;i<=listSize;i+=2){
		queue_put(q2,i);
		//printf(" a thr 1 vai por %d \n",i);

	}
	queue_put(q2,0);
	pthread_mutex_lock(&lismu);
	primes[inc]=2;
	inc++;
	pthread_mutex_unlock(&lismu);
	//	printf("acabada thread com o primo inicial %d \n",2);
	pthread_mutex_lock(&nthmu);
	nth--;
	pthread_mutex_unlock(&nthmu);
	return NULL;
}

int cmpfunc (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}
int main(int argc, char *argv[]){
	listSize= strtol(argv[1], NULL, 0);
	nth=0;
	inc=0;
	sem_init(&ter, 0, 0);
	primes = (int*) malloc(ceil(1.2*listSize/log(listSize)*sizeof(int)));

	pthread_t tid;
	pthread_create(&tid,NULL,first,NULL);
	nth++;
	sem_wait(&ter);
	int l;
	//printf("last element %d \n",primes[inc]);
	qsort(primes,inc,sizeof(int),cmpfunc);
	for(l=0;l<inc;l++)
		printf("primo : %d \n",primes[l]);
	return 0;
}