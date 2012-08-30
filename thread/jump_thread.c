#include <stdio.h>
#include <pthread.h>
#include <setjmp.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

pthread_key_t   key;

void call_longjmp(void)
{
        longjmp(pthread_getspecific(key), 1);
}

void * child1(void *arg)
{
        jmp_buf g_sJumpEnv;
        
          pthread_detach( pthread_self() ); 

        if (setjmp(g_sJumpEnv) == 1)
        {
          printf("jump thread 11111\n");
          return;
        }
        
        pthread_setspecific(key,(void *)g_sJumpEnv);
       
        printf("thread1 setjmp\n");
        sleep(2);
        
        call_longjmp();
}

void * child2(void *arg)
{
        jmp_buf g_sJumpEnv;
         
           pthread_detach( pthread_self() ); 
       
        sleep(1);
        if (setjmp(g_sJumpEnv) == 1)
        {
          printf("jump thread 22222\n");
          return;
        }
        pthread_setspecific(key,(void *)g_sJumpEnv);


        printf("thread2 setjmp\n");
        
        call_longjmp();
}

int main(void)
{
        int tid1,tid2;
        int cycle = 100;
        pthread_key_create(&key,NULL);
        
        while(cycle--)
        {
          pthread_create(&tid1,NULL,child1,NULL);
          pthread_create(&tid2,NULL,child2,NULL);
        }
        sleep(10);
        pthread_key_delete(key);

        printf("main thread exit\n");
        return 0;
}
