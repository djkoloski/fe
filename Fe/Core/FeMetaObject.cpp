#include <Fe/Core/FeMetaObject.h>

feMetaField::feMetaField(feUWord size, feUWord offset) :
	_size(size),
	_offset(offset)
{};
feUWord feMetaField::getSize() const
{
	return _size;
}
feUWord feMetaField::getOffset() const
{
	return _offset;
}

feMetaObject::feMetaObject(feUWord metaFieldCount, const feMetaField *metaFields) :
	_metaFieldCount(metaFieldCount),
	_metaFields(metaFields)
{}
feUWord feMetaObject::getMetaFieldCount() const
{
	return _metaFieldCount;
}
const feMetaField *feMetaObject::getMetaFields() const
{
	return _metaFields;
}