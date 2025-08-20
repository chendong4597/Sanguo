#include "profiler.h"

int g_profileMask = kProfileAll;
void enableProfileCategory(int masks)
{
	g_profileMask |= masks;
}
void disableProfileCategory(int masks)
{
	g_profileMask &= ~masks;
}