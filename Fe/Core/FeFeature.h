#pragma once

#define FE_USING(x)				(1 x)
#define FE_IN_USE				== 1
#define FE_NOT_IN_USE			== 0

#define FE_ENUM_TYPE			== 1
#define FE_ENUM_VALUE			1
#define FE_IS_ENUM(e, x)		((1 e##_TYPE_##x) && (e##_##x))