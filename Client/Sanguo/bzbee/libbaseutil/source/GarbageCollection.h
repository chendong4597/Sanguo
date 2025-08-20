/*!
* \file GarbageCollection.h
*
* \author zhichenxia
* \date 2017/11/22
*
*
*/
#pragma once
#include <list>
#include <functional>


class GarbageCollection
{
	GarbageCollection(void);

public:
	static GarbageCollection& getInstance(void);

	typedef std::function<void(void)> Func;
	void operator+= (const Func& func);

	void run(void);
	void terminate(void);

private:
	std::list<Func> m_queue;
	bool m_terminated{ false };
};


#define _GC GarbageCollection::getInstance() +=
