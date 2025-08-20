/*!
 * \file Thread.h
 * \adopted from Doom3 Engine
 * \author kechen
 * \date 2019/4/13
 *
 *
 */

#ifndef __THREAD_H__
#define __THREAD_H__

#include <mutex>
#include <atomic>
#include <condition_variable>
#include "../BaseType.h"
#include "../platform/sys_threading.h"

NS_BEGIN

/*
================================================
idSysMutex provides a C++ wrapper to the low level system mutex functions.  A mutex is an
object that can only be locked by one thread at a time.  It's used to prevent two threads
from accessing the same piece of data simultaneously.
================================================
*/
class idSysMutex {
public:
	idSysMutex() { };
	~idSysMutex() { };

	bool			Lock(bool blocking = true);
	void			Unlock() { handle.unlock(); }

private:
	std::mutex 	handle;

					idSysMutex( const idSysMutex & s ) {}
	void			operator=( const idSysMutex & s ) {}
};

/*
================================================
idScopedCriticalSection is a helper class that automagically locks a mutex when it's created
and unlocks it when it goes out of scope.
================================================
*/
class idScopedCriticalSection {
public:
	idScopedCriticalSection( idSysMutex & m ) : mutex(&m) { mutex->Lock(); }
	~idScopedCriticalSection() { mutex->Unlock(); }

private:
	idSysMutex *	mutex;	// NOTE: making this a reference causes a TypeInfo crash
};

/*
================================================
idSysSignal is a C++ wrapper for the low level system signal functions.  A signal is an object
that a thread can wait on for it to be raised.  It's used to indicate data is available or that
a thread has reached a specific point.
================================================
*/
class idSysSignal {
public:
	static const int	WAIT_INFINITE = -1;

	idSysSignal() { };
	~idSysSignal() { };

	void	Raise();
	void	Clear();

	// Wait returns true if the object is in a signalled state and
	// returns false if the wait timed out. Wait also clears the signalled
	// state when the signalled state is reached within the time out period.
	bool	Wait(int timeout = WAIT_INFINITE);

private:
	std::condition_variable		m_cv;
	std::mutex 					m_mutex;
	bool						m_flag{ false };
						idSysSignal( const idSysSignal & s ) {}
	void				operator=( const idSysSignal & s ) {}
};

/*
================================================
idSysInterlockedInteger is a C++ wrapper for the low level system interlocked integer
routines to atomically increment or decrement an integer.
================================================
*/
class idSysInterlockedInteger {
public:
	idSysInterlockedInteger() {}
	~idSysInterlockedInteger() {}
	idSysInterlockedInteger(const idSysInterlockedInteger& other) : value(other.value.load()) {}
	idSysInterlockedInteger& operator=(const idSysInterlockedInteger& other) { value = other.value.load(); return *this; }

	// atomically increments the integer and returns the new value
	int					Increment() { return ++value ; }

	// atomically decrements the integer and returns the new value
	int					Decrement() { return --value; }

	// atomically adds a value to the integer and returns the new value
	int					Add( int v ) { return value += v; }

	// atomically subtracts a value from the integer and returns the new value
	int					Sub( int v ) { return value -= v; }

	// returns the current value of the integer
	int					GetValue() const { return value.load(); }

	// sets a new value, Note: this operation is not atomic
	void				SetValue( int v ) { value.store(v); }

private:
	std::atomic<int>	value{ 0 };
};

/*
================================================
idSysThread is an abstract base class, to be extended by classes implementing the
idSysThread::Run() method. 

	class idMyThread : public idSysThread {
	public:
		virtual int Run() {
			// run thread code here
			return 0;
		}
		// specify thread data here
	};

	idMyThread thread;
	thread.Start( "myThread" );

A worker thread is a thread that waits in place (without consuming CPU)
until work is available. A worker thread is implemented as normal, except that, instead of
calling the Start() method, the StartWorker() method is called to start the thread.
Note that the Sys_CreateThread function does not support the concept of worker threads.

	class idMyWorkerThread : public idSysThread {
	public:
		virtual int Run() {
			// run thread code here
			return 0;
		}
		// specify thread data here
	};

	idMyWorkerThread thread;
	thread.StartThread( "myWorkerThread" );
 
	// main thread loop
	for ( ; ; ) {
		// setup work for the thread here (by modifying class data on the thread)
		thread.SignalWork();           // kick in the worker thread
		// run other code in the main thread here (in parallel with the worker thread)
		thread.WaitForThread();        // wait for the worker thread to finish
		// use results from worker thread here
	}

In the above example, the thread does not continuously run in parallel with the main Thread,
but only for a certain period of time in a very controlled manner. Work is set up for the
Thread and then the thread is signalled to process that work while the main thread continues.
After doing other work, the main thread can wait for the worker thread to finish, if it has not
finished already. When the worker thread is done, the main thread can safely use the results
from the worker thread.

Note that worker threads are useful on all platforms but they do not map to the SPUs on the PS3.
================================================
*/

class idSysThread {
public:
					idSysThread();
	virtual			~idSysThread();

	const char *	GetName() const { return name.c_str(); }
	ThreadHandle	GetThreadHandle() const { return threadHandle; }
	bool			IsRunning() const { return isRunning; }
	bool			IsTerminating() const { return isTerminating; }

	//------------------------
	// Thread Start/Stop/Wait
	//------------------------

	bool			StartThread( const char * name, core_t core, 
								 xthreadPriority priority = THREAD_NORMAL,
								 int stackSize = DEFAULT_THREAD_STACK_SIZE );

	bool			StartWorkerThread( const char * name, core_t core, 
									   xthreadPriority priority = THREAD_NORMAL,
									   int stackSize = DEFAULT_THREAD_STACK_SIZE );

	void			StopThread( bool wait = true );

	// This can be called from multiple other threads. However, in the case
	// of a worker thread, the work being "done" has little meaning if other
	// threads are continuously signalling more work.
	void			WaitForThread();

	//------------------------
	// Worker Thread
	//------------------------

	// Signals the thread to notify work is available.
	// This can be called from multiple other threads.
	void			SignalWork();

	// Returns true if the work is done without waiting.
	// This can be called from multiple other threads. However, the work
	// being "done" has little meaning if other threads are continuously
	// signalling more work.
	bool			IsWorkDone();

protected:
	// The routine that performs the work.
	virtual int		Run();

private:
	tstring			name;
	ThreadHandle	threadHandle;
	bool			isWorker;
	bool			isRunning;
	volatile bool	isTerminating;
	volatile bool	moreWorkToDo;
	idSysSignal		signalWorkerDone;
	idSysSignal		signalMoreWorkToDo;
	idSysMutex		signalMutex;

	static int		ThreadProc( idSysThread * thread );

					idSysThread( const idSysThread & s ) {}
	void			operator=( const idSysThread & s ) {}
};

NS_END

#endif // !__THREAD_H__
