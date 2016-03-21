#include <gtest/gtest.h>
#include <iostream>
#include <string>

#include <matchserver/test/test.h>
#include <gtest/gtest.h>

#include <mscom/base/event_mgr.h>
#include <mscom/base/thread.h>
#include <mscom/base/refsync.h>

using namespace std;

RefSync wg;
CEventMgr snd_wg;
CEventMgr rcv_wg;

TEST(event_mgr, Init) 
{
	int i;
	int thread_idxs[5] = {0,1,2,3,4};
	int filter[5] = {1,1,1,1,1};

	snd_wg.Init(5);
	rcv_wg.Init(5);
	rcv_wg.SetEvent(0);
	rcv_wg.SetEvent(1);
	rcv_wg.SetEvent(2);
	rcv_wg.SetEvent(3);
	rcv_wg.SetEvent(4);
	rcv_wg.WaitMultipleEvent(5, filter, 10);
	for (i = 0; i < 5; i++) {
		printf("done %d\n", filter[i]);
	}
}
