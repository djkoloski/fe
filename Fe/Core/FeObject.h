#pragma once

#include <Fe/Core/FeMetaObject.h>

class feObject
{
public:
	virtual ~feObject() = default;
	virtual const feMetaObject *getMetaObject() const;
};