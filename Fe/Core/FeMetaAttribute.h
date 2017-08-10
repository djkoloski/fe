#pragma once

class feMetaAttribute
{
public:
	virtual ~feMetaAttribute() = default;
};

class Get : public feMetaAttribute
{};
class Set : public feMetaAttribute
{};