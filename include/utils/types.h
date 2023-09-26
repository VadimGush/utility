#ifndef UTILITIES_TYPES_H
#define UTILITIES_TYPES_H

#include <stdint.h>
#include <stdlib.h>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using f32 = float;
using f64 = double;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using fd_t = int;

constexpr static u32 KB = 1024;
constexpr static u32 MB = 1024 * KB;
constexpr static u32 GB = 1024 * MB;

#endif //UTILITIES_TYPES_H
