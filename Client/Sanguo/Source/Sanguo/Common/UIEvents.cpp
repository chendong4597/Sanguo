
#include "UIEvents.h"




static HandlerUIEvents* g_HandlerUIEvents = nullptr;

HandlerUIEvents::HandlerUIEvents()
{
	//g_HandlerUIEvents = this;
}

HandlerUIEvents::~HandlerUIEvents()
{
	g_HandlerUIEvents = nullptr;
}

HandlerUIEvents& HandlerUIEvents::getInstance()
{
	//assert(g_HandlerUIEvents);
	if (g_HandlerUIEvents == nullptr)
	{
		g_HandlerUIEvents = new HandlerUIEvents();
	}
	return *g_HandlerUIEvents;
}
