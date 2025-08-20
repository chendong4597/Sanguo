#pragma once

#ifndef _MACROUTILITY_H_
#define _MACROUTILITY_H_

#if defined(WIN32) || defined(__ANDROID__) || defined(__APPLE__)
#define USE_WWISE
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)\
if (p)\
{\
    delete (p);\
    (p) = nullptr;\
}
#endif

//#ifndef DELETE
//#define DELETE(p)\
//if (p)\
//{\
//    delete (p);\
//}
//#endif

#ifndef NULL_RETURN
#define NULL_RETURN(p)\
if ((p) == nullptr)\
{\
    return;\
}
#endif

#ifndef NULL_RETURN_V
#define NULL_RETURN_V(p, v)\
if ((p) == nullptr)\
{\
    return (v);\
}
#endif

#ifndef NULL_CONTINUE
#define NULL_CONTINUE(p)\
if ((p) == nullptr)\
{\
    continue;\
}
#endif

#ifndef V_RETURN
#define V_RETURN(x)\
{\
	AKRESULT ret = (x);\
	if (ret != AK_Success)\
	{\
		LOGE("{0} V_RETURN {1}", #x, ret);\
		return false;\
	}\
}
#endif

#ifndef DECLARE_PROPERTY
#define DECLARE_PROPERTY(type, name)\
public:\
	const type& get##name() const { return m_##name; };\
	void set##name(const type& value) { m_##name = value; };\
private:\
	type m_##name{};
#endif

#endif