#pragma once

#include <Fe/System/Path.h>

template <typename ...Rest>
feString Path::join(feStringView first, const Rest &...rest)
{
#if FE_IS_TARGET(WINDOWS)
	return feStringUtil::joinWrapped("", "", "\\", first, rest...);
#else
	return feStringUtil::joinWrapped("", "", "/", first, rest...);
#endif
}