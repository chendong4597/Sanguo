/*!
 * \file Thread.cpp
 * \adopted from Doom3 Engine
 * \author kechen
 * \date 2019/4/13
 *
 *
 */
#include "Thread.h"

NS_BEGIN

bool idSysMutex::Lock(bool blocking /*= true*/)
{
	if (handle.try_lock() == 0) {
		if (!blocking) {
			return false;
		}
		handle.lock();
	}
	return true;
}

void idSysSignal::Raise()
{
	std::unique_lock<std::mutex> lck(m_mutex);
	m_flag = true;
	m_cv.notify_all();
}

void idSysSignal::Clear()
{
	std::unique_lock<std::mutex> lck(m_mutex);
	m_flag = false;
}

bool idSysSignal::Wait(int timeout /*= WAIT_INFINITE*/)
{
	std::unique_lock<std::mutex> lck(m_mutex);
	m_cv.wait(lck, [this] { return m_flag; });
	return true;
}

/*
================================================================================================
Contains the vartious ThreadingClass implementations.
================================================================================================
*/

/*
================================================================================================

	idSysThread

================================================================================================
*/

/*
========================
idSysThread::idSysThread
========================
*/
idSysThread::idSysThread() :
		threadHandle(0),
		isWorker( false ),
		isRunning( false ),
		isTerminating( false ),
		moreWorkToDo( false ) {
}

/*
========================
idSysThread::~idSysThread
========================
*/
idSysThread::~idSysThread() {
	StopThread( true );
	if ( threadHandle ) {
		Sys_DestroyThread( threadHandle );
	}
}

/*
========================
idSysThread::StartThread
========================
*/
bool idSysThread::StartThread( const char * name_, core_t core, xthreadPriority priority, int stackSize ) {
	if ( isRunning ) {
		return false;
	}

	name = name_;

	isTerminating = false;

	if ( threadHandle ) {
		Sys_DestroyThread( threadHandle );
	}

	threadHandle = Sys_CreateThread( (xthread_t)ThreadProc, this, priority, name_, core, stackSize, false );

	isRunning = true;
	return true;
}

/*
========================
idSysThread::StartWorkerThread
========================
*/
bool idSysThread::StartWorkerThread( const char * name_, core_t core, xthreadPriority priority, int stackSize ) {
	if ( isRunning ) {
		return false;
	}

	isWorker = true;

	bool result = StartThread( name_, core, priority, stackSize );

	signalWorkerDone.Wait( idSysSignal::WAIT_INFINITE );

	return result;
}

/*
========================
idSysThread::StopThread
========================
*/
void idSysThread::StopThread( bool wait ) {
	if ( !isRunning ) {
		return;
	}
	if ( isWorker ) {
		signalMutex.Lock();
		moreWorkToDo = true;
		signalWorkerDone.Clear();
		isTerminating = true;
		signalMoreWorkToDo.Raise();
		signalMutex.Unlock();
	} else {
		isTerminating = true;
	}
	if ( wait ) {
		WaitForThread();
	}
}

/*
========================
idSysThread::WaitForThread
========================
*/
void idSysThread::WaitForThread() {
	if ( isWorker ) {
		signalWorkerDone.Wait( idSysSignal::WAIT_INFINITE );
	} else if ( isRunning ) {
		Sys_DestroyThread( threadHandle );
		threadHandle = 0;
	}
}

/*
========================
idSysThread::SignalWork
========================
*/
void idSysThread::SignalWork() {
	if ( isWorker ) {
		signalMutex.Lock();
		moreWorkToDo = true;
		signalWorkerDone.Clear();
		signalMoreWorkToDo.Raise();
		signalMutex.Unlock();
	}
}

/*
========================
idSysThread::IsWorkDone
========================
*/
bool idSysThread::IsWorkDone() {
	if ( isWorker ) {
		// a timeout of 0 will return immediately with true if signaled
		if ( signalWorkerDone.Wait( 0 ) ) {
			return true;
		}
	}
	return false;
}

/*
========================
idSysThread::ThreadProc
========================
*/
int idSysThread::ThreadProc( idSysThread * thread ) {
	int retVal = 0;

	if (thread->isWorker) {
		for (; ; ) {
			thread->signalMutex.Lock();
			if (thread->moreWorkToDo) {
				thread->moreWorkToDo = false;
				thread->signalMoreWorkToDo.Clear();
				thread->signalMutex.Unlock();
			}
			else {
				thread->signalWorkerDone.Raise();
				thread->signalMutex.Unlock();
				thread->signalMoreWorkToDo.Wait(idSysSignal::WAIT_INFINITE);
				continue;
			}

			if (thread->isTerminating) {
				break;
			}

			retVal = thread->Run();
		}
		thread->signalWorkerDone.Raise();
	}
	else {
		retVal = thread->Run();
	}

	thread->isRunning = false;

	return retVal;
}

/*
========================
idSysThread::Run
========================
*/
int idSysThread::Run() {
	// The Run() is not pure virtual because on destruction of a derived class
	// the virtual function pointer will be set to NULL before the idSysThread
	// destructor actually stops the thread.
	return 0;
}

NS_END