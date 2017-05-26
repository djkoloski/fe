#pragma once

#include <memory>

template <typename T>
using feUnique = std::unique_ptr<T>;
template <typename T>
using feShared = std::shared_ptr<T>;
template <typename T>
using feWeak = std::weak_ptr<T>;

#define feMakeUnique std::make_unique
#define feMakeShared std::make_shared