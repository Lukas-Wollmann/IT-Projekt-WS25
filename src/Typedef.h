#pragma once
#include <cstddef>
#include <cstdint>
#include <expected>
#include <functional>
#include <memory>
#include <optional>

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float    f32;
typedef double   f64;

template <typename T>
using Ref = std::reference_wrapper<T>;

template <typename T>
using Opt = std::optional<T>;

template <typename R, typename E>
using Result = std::expected<R, E>;

template <typename T>
using Ptr = std::shared_ptr<T>;

template <typename T>
using Box = std::unique_ptr<T>;

template <typename T>
using Vec = std::vector<T>;

template <typename U, typename V>
using Pair = std::pair<U, V>;
