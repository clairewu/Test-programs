#include <stdio.h>
#include<stdlib.h>
#include <pthread.h>
#include <setjmp.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;//in fact 1 mutex is ok
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
const char * function3(int *test){
    static char strBuff[16];
    sprintf(strBuff, "%d", *test);
    printf("%s", strBuff);
    return strBuff;
}
void *function1(void *arg){
    pthread_mutex_lock(&mutex1); //dead lock
    pthread_mutex_lock(&mutex2);
    function3((int *)arg);
    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex1);
}
void *function2(void *arg){
pthread_mutex_lock(&mutex2);
pthread_mutex_lock(&mutex1);
function3((int *)arg);
pthread_mutex_unlock(&mutex1);
pthread_mutex_unlock(&mutex2);
}
int main(int argc, char *argv[]){
    int i=0,childpid=0;
    pthread_t a_thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    for(i=0;i<10;){
      
        pthread_create(&a_thread, &attr, function1, (void *)&i);
           pthread_mutex_lock(&mutex1); //dead lock
    pthread_mutex_lock(&mutex2);
i++;
    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex1);

    }
    for(i=10;i<20;i++){
        pthread_create(&a_thread, &attr, function2, (void *)&i);
    }
    if((childpid = fork()) < 0)
        exit(-1);
    else if(childpid == 0){
        pthread_mutex_lock(&mutex1);
        pthread_mutex_lock(&mutex2);
        printf("%s", "abcdef");    
        pthread_mutex_unlock(&mutex1); //free mutex2 first
        pthread_mutex_unlock(&mutex2);
        sleep(10);
        exit(0);
    }
    else{
        waitpid(childpid, NULL, 0);
        //sleep(36);
    return 0;
    }
}
int proc_wrap_up(){}
