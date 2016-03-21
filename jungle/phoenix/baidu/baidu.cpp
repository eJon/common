//@version 1.0.0.1 
//@author AndrewPD
//@date 2014-12-18
//@brief 

#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "runner.h"
#include "baidu_runner.h"

using namespace std;

//typedef void *(*TaskFunc) (void *);
struct thread_param_t {
  char conf_name[100];
  thread_param_t(){
    memset(this,0,sizeof(thread_param_t));
  }
};
static void *thread_routine(void *arg) {
  jungle::Runner *runner = new jungle::BaiduRunner();
  thread_param_t *param = (thread_param_t*)arg;
  if (runner->Init(param->conf_name)) {
    return NULL;
  }
  //  while(!runner->terminate()) {
    runner->Start();
    //  }
  delete runner;
  runner = NULL;
  return NULL;
}
int main(int argc, char** argv) {
  if (argc != 2) {
    cout<<"param missed"<<endl;
    return 0;
  }
  thread_param_t param;
  memcpy(param.conf_name,argv[1],strlen(argv[1]));
  cout<<"[DEBUG]conf name: "<<param.conf_name<<endl;
  pthread_t thread;
  int ret = pthread_create(&thread, NULL, thread_routine, (void*)&param);
  pthread_join(thread, NULL);
  return 0;
}
