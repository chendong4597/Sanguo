/*!
* \file S3Referenced.h
* \adopted from S3 engine
* \author kechen
* \date 2018/11/22
*
*
*/
#pragma once

#include "s3_ref_ptr.h"
#include "../AssertionMacros.h"

// namespace tq
// {
/** @script{interface} */
class  CReferenced
{
public:
	CReferenced():_refCount(0){}
	CReferenced(const CReferenced&):_refCount(0){}

	virtual ~CReferenced(){ 
		if (_refCount>0)
		{
			ASSERT(false);
		}
	}

    inline CReferenced& operator = (const CReferenced&) { return *this; }

    /** Increment the reference count by one, indicating that 
        this object has another pointer which is referencing it.*/
	virtual int ref() const{return ++_refCount;}
    
    /** Decrement the reference count by one, indicating that 
        a pointer to this object is referencing it.  If the
        reference count goes to zero, it is assumed that this object
        is no longer referenced and is automatically deleted.*/
	virtual int unref() const{
		int newRef = --_refCount;
		bool needDelete = (newRef == 0);
		if (needDelete)
		{
			delete this;
		}

		return newRef;
	}
    
	inline int GetRefCount() const{return _refCount;}
protected:
	mutable int	_refCount;
};

//}
