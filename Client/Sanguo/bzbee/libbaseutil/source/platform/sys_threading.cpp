/*!
 * \file sys_threading.cpp
 * \adopted from Doom3 Engine
 * \author kechen
 * \date 2019/4/13
 *
 *
 */

#include "sys_threading.h"
#include <thread>

NS_BEGIN

/*
================================================================================================
================================================================================================
*/

/*
========================
Sys_Createthread
========================
*/
ThreadHandle Sys_CreateThread(xthread_t function, void *parms, xthreadPriority priority, const char *name, core_t core, int stackSize, bool suspended) {
	std::thread t(function, parms);
	t.detach();
	return 1;
}


/*
========================
Sys_GetCurrentThreadID
========================
*/
ThreadHandle Sys_GetCurrentThreadID() {
	return 1;
}

/*
========================
Sys_WaitForThread
========================
*/
void Sys_WaitForThread(ThreadHandle threadHandle) {
	
}

/*
========================
Sys_DestroyThread
========================
*/
void Sys_DestroyThread(ThreadHandle threadHandle) {

}

/*
========================
Sys_Yield
========================
*/
void Sys_Yield() {
	std::this_thread::yield();
}

NS_END
