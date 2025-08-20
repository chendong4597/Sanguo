/*!
* \file GarbageCollection.cpp
*
* \author zhichenxia
* \date 2017/11/22
*
*
*/
#include "GarbageCollection.h"

GarbageCollection::GarbageCollection(void)
{

}

GarbageCollection& GarbageCollection::getInstance(void)
{
    static GarbageCollection s_instance;
    return s_instance;
}

void GarbageCollection::operator+= (const GarbageCollection::Func& func)
{
	if (m_terminated)
		func();
	else
		m_queue.push_back(func);
}

void GarbageCollection::run(void)
{
	if (m_terminated) return;

	std::list<Func> q;
	q.swap(m_queue);
	for(const auto& v : q) v();
}

void GarbageCollection::terminate(void)
{
	run();
	m_terminated = true;
}
