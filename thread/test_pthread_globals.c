#include <stdio.h>
#include <pthread.h>
#include <setjmp.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include "arb_threadstatic_macros.h"


THREADSAFE_MVAR( int, global_int_var);
#define global_int_var (*_global_int_var_p())

THREADSAFE_MARR( char, global_str_var, 10);
#define global_str_var (*_global_str_var_p())

void envaluate(int i, char *str)
{
        global_int_var = i;
        strcpy(global_str_var, str);
}

void * child1(void *arg)
{
        envaluate(1, "1");
        sleep(2);
        printf("In child1, global_int_var = %d, global_str_var = %s\n", global_int_var, global_str_var);
}

void * child2(void *arg)
{
        sleep(1);
        envaluate(2, "2");
        printf("In child2, global_int_var = %d, global_str_var = %s\n", global_int_var, global_str_var);
}

int main(void)
{
        int tid1,tid2;
        pthread_key_create( &global_int_var_p_key      , free_global_int_var_p_key);
        pthread_key_create( &global_str_var_p_key      , free_global_str_var_p_key);
        
        pthread_create(&tid1,NULL,child1,NULL);
        pthread_create(&tid2,NULL,child2,NULL);
        sleep(10);
        pthread_key_delete( global_int_var_p_key);
        pthread_key_delete( global_str_var_p_key);
        printf("main thread exit\n");
        return 0;
}
