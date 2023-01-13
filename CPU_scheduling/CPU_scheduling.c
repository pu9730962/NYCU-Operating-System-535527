#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sched.h>
#include <pthread.h>
#include <time.h>

pthread_barrier_t barrier;
typedef struct{
    int thread_num;
    double busy_wait;
}thread_info_t;

void *thread_func(void *arg)
{   
    /* 1. Wait until all threads are ready */
    pthread_barrier_wait(&barrier);
    /*承接傳入的struct*/
    thread_info_t *thread_information;
    thread_information=(thread_info_t*)arg; 
    int thread_num=thread_information->thread_num;
    double busy_wait=thread_information->busy_wait;
    /*set CPU affinity*/
    int cpu_id =0;
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_id,&cpuset);
    sched_setaffinity(0,sizeof(cpuset),&cpuset);
    clock_t start_time , end_time; 
    /* 2. Do the task */
    for (int i = 0; i < 3; i++) {      
        printf("Thread %d is running\n", thread_num);
        start_time=clock();
        /* Busy for <time_wait> seconds */
        while(1){
            end_time=clock();
            if(((double)(end_time-start_time)/CLOCKS_PER_SEC)>busy_wait){
                break;
            }
        }
        sched_yield();
    }
     /* 3. Exit the function  */
    pthread_exit(NULL);                 
}


int main(int argc,char *argv[]){
    int ch;
    int num_threads=atoi(argv[2]);
    int thread_id[num_threads];
    thread_info_t thread_information[num_threads];
    pthread_attr_t attr[num_threads];
    struct sched_param param[num_threads];
    pthread_barrier_init(&barrier,NULL,(num_threads+1));
    char *policies[num_threads];
    int priorities[num_threads];
    pthread_t thread[num_threads];
    char *d=",";
    char *s_p;
    char *p_p;
    double busy_time;
    char str1[]="NORMAL";
    char str2[]="FIFO";
    int policy_int;

    int cpu_id =0;
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_id,&cpuset);
    sched_setaffinity(0,sizeof(cpuset),&cpuset);

    while((ch=getopt(argc,argv,"n:t:s:p:")) !=-1){
        switch(ch){
            case 'n':
                for(int i=0;i<num_threads;i++){
                    thread_id[i]=i;
                }
                break;
            case 't':
                busy_time=atof(optarg);
                break;
            case 's':
                char *s_string=optarg;
                s_p=strtok(s_string,d);
                for(int i=0;i<num_threads;i++){
                    policies[i]=s_p;
                    s_p=strtok(NULL,d);
                }
                break;
            case 'p':
                char *p_string=optarg;
                p_p=strtok(p_string,d);
                for(int i=0;i<num_threads;i++){
                    priorities[i]=atoi(p_p);
                    p_p=strtok(NULL,d);
                }
                break;
                  }
    }
    for (int i = 0; i<num_threads; i++){
        thread_information[i].thread_num=thread_id[i];
        thread_information[i].busy_wait=busy_time;
        
        if(strcmp(policies[i],str1)==0){
            policy_int=0;
        }
        else if(strcmp(policies[i],str2)==0){
            policy_int=1;
        }
 
        if(policy_int==1){
            pthread_attr_init(&attr[i]);
            pthread_attr_setinheritsched(&attr[i],PTHREAD_EXPLICIT_SCHED);
            pthread_attr_setschedpolicy(&attr[i],policy_int);
            param[i].sched_priority=priorities[i];
            pthread_attr_setschedparam(&attr[i],&param[i]);
            pthread_create(&thread[i],&attr[i],thread_func,&thread_information[i]);
            // pthread_attr_destroy(&attr[i]);
        }else if(policy_int==0){
            pthread_create(&thread[i],NULL,thread_func,&thread_information[i]);
        }
    }
    pthread_barrier_wait(&barrier);
    pthread_barrier_destroy(&barrier);

    for (int i = 0; i<num_threads; i++){
        pthread_join(thread[i],NULL);
    }
    return 0;
}