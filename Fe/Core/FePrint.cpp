#include <Fe/Core/FePrint.h>

// Enabled
#if FE_USING(FE_FEATURE_PRINT)

#include <cstdio>
#include <cstdarg>

void fePrint(feRawString fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
}

#endif