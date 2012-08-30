#include <stdio.h>
#include <pthread.h>
#include <setjmp.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

sigjmp_buf jbuf;

void call_longjmp(void)
{
        siglongjmp(jbuf, 1);
}

void * child1(void *arg)
{
        if (sigsetjmp(jbuf, 0) == 1)
        {
          printf("jump thread 11111\n");
          return;
        }
        
        printf("thread1 setjmp\n");
        sleep(2);
        
        call_longjmp();
}

void * child2(void *arg)
{                 
        sleep(1);
        if (sigsetjmp(jbuf, 0) == 1)
        {
          printf("jump thread 22222\n");
          return;
        }

        printf("thread2 setjmp\n");
        
        call_longjmp();
}

int main(void)
{
        int tid1,tid2;

        pthread_create(&tid1,NULL,child1,NULL);
        pthread_create(&tid2,NULL,child2,NULL);
        sleep(10);

        printf("main thread exit\n");
        return 0;
}
