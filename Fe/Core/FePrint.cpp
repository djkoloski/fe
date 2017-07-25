#include <Fe/Core/FePrint.h>

// Enabled
#if FE_USING(FE_FEATURE_PRINT)

#include <cstdio>
#include <cstdarg>

void fePrint(feRawString fmt, ...)
{
	auto args = va_list();
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
}

#endif