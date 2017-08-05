#include <Fe/Core/FeMetaObject.h>

feMetaField::feMetaField(feUWord size, feUWord offset, const feMetaObject *type) :
	_size(size),
	_offset(offset),
	_type(type)
{};
feUWord feMetaField::getSize() const
{
	return _size;
}
feUWord feMetaField::getOffset() const
{
	return _offset;
}
const feMetaObject *feMetaField::getType() const
{
	return _type;
}

feMetaObject::feMetaObject(IntrinsicType intrinsicType, feUWord metaFieldCount, const feMetaField *metaFields) :
	_intrinsicType(intrinsicType),
	_metaFieldCount(metaFieldCount),
	_metaFields(metaFields)
{}
feMetaObject::IntrinsicType feMetaObject::getIntrinsicType() const
{
	return _intrinsicType;
}
feUWord feMetaObject::getMetaFieldCount() const
{
	return _metaFieldCount;
}
const feMetaField *feMetaObject::getMetaFields() const
{
	return _metaFields;
}

static const auto k_feBoolMetaObject = feMetaObject(
	feMetaObject::IntrinsicType::feBool,
	0,
	nullptr);
static const auto k_feCharMetaObject = feMetaObject(
	feMetaObject::IntrinsicType::feChar,
	0,
	nullptr);
static const auto k_feFloatMetaObject = feMetaObject(
	feMetaObject::IntrinsicType::feFloat,
	0,
	nullptr);
static const auto k_feDoubleMetaObject = feMetaObject(
	feMetaObject::IntrinsicType::feDouble,
	0,
	nullptr);
static const auto k_feByteMetaObject = feMetaObject(
	feMetaObject::IntrinsicType::feByte,
	0,
	nullptr);
static const auto k_feUByteMetaObject = feMetaObject(
	feMetaObject::IntrinsicType::feUByte,
	0,
	nullptr);
static const auto k_feShortMetaObject = feMetaObject(
	feMetaObject::IntrinsicType::feShort,
	0,
	nullptr);
static const auto k_feUShortMetaObject = feMetaObject(
	feMetaObject::IntrinsicType::feUShort,
	0,
	nullptr);
static const auto k_feIntMetaObject = feMetaObject(
	feMetaObject::IntrinsicType::feInt,
	0,
	nullptr);
static const auto k_feUIntMetaObject = feMetaObject(
	feMetaObject::IntrinsicType::feUInt,
	0,
	nullptr);
static const auto k_feLongMetaObject = feMetaObject(
	feMetaObject::IntrinsicType::feLong,
	0,
	nullptr);
static const auto k_feULongMetaObject = feMetaObject(
	feMetaObject::IntrinsicType::feULong,
	0,
	nullptr);

const feMetaObject *feMetaObject::k_feBoolMetaObject = &::k_feBoolMetaObject;
const feMetaObject *feMetaObject::k_feCharMetaObject = &::k_feCharMetaObject;
const feMetaObject *feMetaObject::k_feFloatMetaObject = &::k_feFloatMetaObject;
const feMetaObject *feMetaObject::k_feDoubleMetaObject = &::k_feDoubleMetaObject;
const feMetaObject *feMetaObject::k_feByteMetaObject = &::k_feByteMetaObject;
const feMetaObject *feMetaObject::k_feUByteMetaObject = &::k_feUByteMetaObject;
const feMetaObject *feMetaObject::k_feShortMetaObject = &::k_feShortMetaObject;
const feMetaObject *feMetaObject::k_feUShortMetaObject = &::k_feUShortMetaObject;
const feMetaObject *feMetaObject::k_feIntMetaObject = &::k_feIntMetaObject;
const feMetaObject *feMetaObject::k_feUIntMetaObject = &::k_feUIntMetaObject;
const feMetaObject *feMetaObject::k_feLongMetaObject = &::k_feLongMetaObject;
const feMetaObject *feMetaObject::k_feULongMetaObject = &::k_feULongMetaObject;