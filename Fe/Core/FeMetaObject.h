#pragma once

#include <Fe/Core/FeType.h>

class feObject;
class feMetaObject;

class feMetaField
{
private:
	const feUWord _size;
	const feUWord _offset;
	const feMetaObject *_type;

public:
	feMetaField(feUWord size, feUWord offset, const feMetaObject *_type);

	feUWord getSize() const;
	feUWord getOffset() const;
	const feMetaObject *getType() const;
	template <typename T = feObject>
	T *getMember(feObject *object) const;
};

class feMetaObject
{
public:
	enum class IntrinsicType
	{
		feBool,
		feChar,
		feFloat,
		feDouble,
		feByte,
		feUByte,
		feShort,
		feUShort,
		feInt,
		feUInt,
		feLong,
		feULong,
		feObject
	};

	static const feMetaObject *k_feBoolMetaObject;
	static const feMetaObject *k_feCharMetaObject;
	static const feMetaObject *k_feFloatMetaObject;
	static const feMetaObject *k_feDoubleMetaObject;
	static const feMetaObject *k_feByteMetaObject;
	static const feMetaObject *k_feUByteMetaObject;
	static const feMetaObject *k_feShortMetaObject;
	static const feMetaObject *k_feUShortMetaObject;
	static const feMetaObject *k_feIntMetaObject;
	static const feMetaObject *k_feUIntMetaObject;
	static const feMetaObject *k_feLongMetaObject;
	static const feMetaObject *k_feULongMetaObject;

private:
	const IntrinsicType _intrinsicType;
	const feUWord _metaFieldCount;
	const feMetaField *_metaFields;

public:
	feMetaObject(IntrinsicType intrinsicType, feUWord metaFieldCount, const feMetaField *metaFields);

	IntrinsicType getIntrinsicType() const;
	feUWord getMetaFieldCount() const;
	const feMetaField *getMetaFields() const;
};

#include <Fe/Core/FeMetaObject.inl>