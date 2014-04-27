#ifndef _CCLASSDESCFACTOR_H_
#define _CCLASSDESCFACTOR_H_
#include "plugapi.h"

class ClassDescFactor : public ClassDesc
{
public:
	ClassDescFactor();
	~ClassDescFactor();

	virtual int				IsPublic();
	virtual void*			Create(BOOL loading = FALSE);
	virtual const MCHAR*	ClassName();
	virtual SClass_ID		SuperClassID();
	virtual Class_ID		ClassID();
	virtual const MCHAR*	Category();
	virtual MCHAR* GetRsrcString(INT_PTR id);
};

#endif
