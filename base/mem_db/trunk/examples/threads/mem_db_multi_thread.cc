#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <sys/timeb.h>
#include <inttypes.h>
#include <stdint.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "mem_db.h"
using namespace mem_db;

#define MAX_NUM_OF_THREAD 1024

// mem_db data information
static const std::string sTableName = "ad_info";
static const std::string sTableKey = "keykey";
static const std::string sTableColomnSeed = "columncolumn";

static std::vector<std::string> sTableColumns[MAX_NUM_OF_THREAD];
static char *sTableItemData = NULL;

// thread information
// 0: quit; 1; coninue; 2: sleep;
static char sRunFlag;
static int64_t sRunCount[MAX_NUM_OF_THREAD];
static int64_t sRunErrorCount[MAX_NUM_OF_THREAD];

// global information
static int sThreadNum;
static int sValuelength;
static int sRunTime;
static int sNumOfWorkItemPerThread;

// each thread has its own instance
struct MDBSession {
  MemDB mdb;
  int thread_id;
};

// mem_db session for each thread
static MDBSession sSessions[MAX_NUM_OF_THREAD];

static void create_worker (void * (*func) (void *), void *arg) {
  pthread_t       thread;
  pthread_attr_t  attr;
  int             ret;

  pthread_attr_init (&attr);

  if ((ret = pthread_create (&thread, &attr, func, arg)) != 0) {
    fprintf (stderr, "Can't create thread: %s\n",
             strerror (ret));
    exit (1);
  }
}

static void *WorkerFuncForGet (void *arg) {
  int thread_id = * ((int *)arg);
  int ret = 0;
  std::vector<std::string> values;

  while (sRunFlag) {
    sRunCount[thread_id]++;
    ret = sSessions[thread_id].mdb.GetRowWithColumns (sTableName, sTableKey,
          sTableColumns[thread_id],
          values);

    if (MEM_DB_SUCCESS != ret) {
      sRunErrorCount[thread_id]++;
      continue;
    }

    // do value checking
    for (std::vector<std::string>::iterator iter = values.begin ();
         iter != values.end (); iter++) {
      if (0 != iter->compare (sTableItemData)) {
        sRunErrorCount[thread_id]++;
        continue;
      }
    }

  }
}

int main (int argc, char **argv) {
  sigset_t signal_mask;
  sigemptyset (&signal_mask);
  sigaddset (&signal_mask, SIGPIPE);
  int rc = pthread_sigmask (SIG_BLOCK, &signal_mask, NULL);

  if (rc != 0) {
    printf ("block sigpipe error\n");
  }

  if (argc  < 5) {
    printf ("usage:\n");
    printf ("  $: <cmd> <number_of_work_item_per_thread> <thread_num> <value_length(Bytes)> <run_time<s>>\n");
    return 1;
  }

  sNumOfWorkItemPerThread =  atoi (argv[1]);
  sThreadNum = atoi (argv[2]);
  sValuelength = atoi (argv[3]);
  sRunTime = atoi (argv[4]);

  if (sThreadNum > MAX_NUM_OF_THREAD) {
    printf ("Number of threads is too big!!!\n");
  }

  if (sNumOfWorkItemPerThread <= 0) {
    printf ("Number of work items per thread is too small!!!\n");
  }

  // table columns
  char temp[16] = {0};

  for (int i = 0; i < sThreadNum; ++i) {
    for (int j = 0; j < sNumOfWorkItemPerThread; ++j) {
      snprintf (temp, 15, "%d", j + i * sNumOfWorkItemPerThread);
      sTableColumns[i].push_back (sTableColomnSeed + temp);
    }
  }

  // columns
  sTableItemData = (char *) calloc (sValuelength + 1, sizeof (char *));
  memset (sTableItemData, 'y', sValuelength);
  sTableItemData[sValuelength] = 0;
  // vaiues(all the same)
  std::vector<std::string> value_vector;

  for (int i = 0; i < sNumOfWorkItemPerThread; ++i) {
    value_vector.push_back (sTableItemData);
  }

  int ret = 0;

  // do mem_db initialization
  for (int i = 0; i < sThreadNum; ++i) {
    ret = sSessions[i].mdb.Init ("/home/w/conf/mem_db/mem_db.conf");

    if (MEM_DB_SUCCESS != ret) {
      printf ("mem_db initialization fails!\n");
      return 0;
    }

    sSessions[i].thread_id = i;
  }

  // single thread to insert value in mem_db
  // set flag to 1
  for (int i = 0; i < sThreadNum; ++i) {
    ret = sSessions[i].mdb.PutRowWithColumns (sTableName, sTableKey,
          sTableColumns[i], value_vector);

    if (MEM_DB_SUCCESS != ret) {
      printf ("mem_db initialization fails!\n");
      return 0;
    }
  }

  for (int i = 0; i < sThreadNum; ++i) {
    sRunCount[i] = 0;
    sRunErrorCount[i] = 0;
  }

  printf ("begin test for Gets\n");
  sRunFlag = 1;

  for (int i = 0; i < sThreadNum; ++i) {
    create_worker (WorkerFuncForGet, & (sSessions[i].thread_id));
  }

  ::sleep (sRunTime);
  sRunFlag = 0;

  // do statistics
  int64_t total_count = 0;
  int64_t error_count = 0;

  for (int i = 0; i < sThreadNum; ++i) {
    total_count += sRunCount[i];
    error_count += sRunErrorCount[i];
  }

  printf ("--------------------------------------\n");
  printf ("Total number of GETs:    %ld\n",  total_count);
  printf ("Failed number of GETs:   %ld\n",  error_count);
  printf ("Time cost:               %ld seconds\n",  sRunTime);
  printf ("Value length:            %d Bytes\n",	  sValuelength);
  printf ("Thread Number:           %d\n",	  sThreadNum);
  printf ("Columns per Operation:   %d\n\n", sThreadNum);
  printf ("--------------------------------------\n");
  printf ("QPS:                     %d\n",   total_count / sRunTime);

  for (int i = 0; i < sThreadNum; ++i) {
    ret = sSessions[i].mdb.Free ();

    if (MEM_DB_SUCCESS != ret) {
      printf ("mem_db free() fails!\n");
      return 0;
    }
  }

  usleep (5);

  return 0;
}
