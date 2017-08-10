#pragma once

template <typename T>
T *feMetaField::getMember(feObject *object) const
{
	return reinterpret_cast<T *>((feUByte *)object + _offset);
}