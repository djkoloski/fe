#pragma once

#include <Fe/Core/FeType.h>

class feMetaField
{
private:
	const feUWord _size;
	const feUWord _offset;

public:
	feMetaField(feUWord size, feUWord offset);

	feUWord getSize() const;
	feUWord getOffset() const;
};

class feMetaObject
{
private:
	const feUWord _metaFieldCount;
	const feMetaField *_metaFields;

public:
	feMetaObject(feUWord metaFieldCount, const feMetaField *metaFields);

	feUWord getMetaFieldCount() const;
	const feMetaField *getMetaFields() const;
};