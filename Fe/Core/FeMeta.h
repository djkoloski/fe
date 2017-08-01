#pragma once

#if defined(CODEGEN)
#define Meta(...) __attribute__((annotate(#__VA_ARGS__)))
#else
#define Meta(...)
#endif

class feMetaAttribute
{
public:
	virtual ~feMetaAttribute() = default;
};

class Get : public feMetaAttribute
{};
class Set : public feMetaAttribute
{};