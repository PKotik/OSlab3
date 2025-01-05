/*
  File for 'smokers' task implementation.
*/

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "devices/timer.h"

enum component
{
   paper = 0,
   tobacco,
   matches
};


static struct semaphore smoker1;
static struct semaphore smoker2;
static struct semaphore smoker3;
static struct semaphore Mediator;

static void init(void)
{
    sema_init (&smoker1, 0);
    sema_init (&smoker2, 0);
    sema_init (&smoker3, 0);
    sema_init (&Mediator, 0);
}

static void mediator(void* arg UNUSED)
{
    msg("Mediator created." );
    sema_down(&Mediator);

    sema_up(&smoker1);
    sema_down(&Mediator);

    //return;

    sema_up(&smoker2);
    sema_down(&Mediator);

    //return;

    sema_up(&smoker3);
    sema_down(&Mediator);

    //return;

}

static void smoker(enum component what_have)
{
    char* item_names[] = { "paper", "tobacco", "matches" };
    msg("Smoker with %s created.", item_names[ (int) what_have] );
    sema_up(&Mediator);
    switch (what_have)
    {
      case 0:
        sema_down(&smoker1);
        break;
      case 1:
        sema_down(&smoker2);
        break;
      case 2:
        sema_down(&smoker3);
        break;
    }

    timer_sleep(10);

    // Not implemented    
}

static void smokerw(void* arg)
{
    smoker( (enum component) arg );
}

void test_smokers(void)
{
  init();

  thread_create("mediator", PRI_DEFAULT, &mediator, 0);
  thread_create("smoker1", PRI_DEFAULT, &smokerw, (void*) paper );
  thread_create("smoker2", PRI_DEFAULT, &smokerw, (void*) tobacco );
  thread_create("smoker3", PRI_DEFAULT, &smokerw, (void*) matches );  

  timer_msleep(5000);
  pass();
}
