/*
 * axhash - Much fast. Very hashing. Such independence. Wow.
 * Copyright (C) 2025 Tommy Ettinger
 *
 * Based on 'mx3', by Jon Maiga, which is public domain under the Unlicense.
 * Uses C defines and macros from 'rapidhash', which is licensed under the
 * BSD 2-clause license. This does not use the rapidhash algorithm.
 *
 * This work is released to the public as free and unencumbered software under the
 * CC0 1.0 Universal license. See the file LICENSE in this directory.
 *
 * You can contact the author at:
 *   - axhash source repository: https://github.com/tommyettinger/axhash
 */

/*
 *  Includes.
 */
#include <stdint.h>
#include <limits.h>   // for CHAR_BIT
#include <string.h>

/*
 *  C++ macros.
 *
 *  AXHASH_INLINE can be overridden to be stronger than a hint, i.e. by adding __attribute__((always_inline)).
 */
#ifdef __cplusplus
  #define AXHASH_NOEXCEPT noexcept
  #define AXHASH_CONSTEXPR constexpr
  #ifndef AXHASH_INLINE
    #define AXHASH_INLINE inline
  #endif
#else
  #define AXHASH_NOEXCEPT
  #define AXHASH_CONSTEXPR static const
  #ifndef AXHASH_INLINE
    #define AXHASH_INLINE static inline
  #endif
#endif

/*
 *  Likely and unlikely macros.
 */
#if defined(__GNUC__) || defined(__INTEL_COMPILER) || defined(__clang__)
  #define _likely_(x)  __builtin_expect(x,1)
  #define _unlikely_(x)  __builtin_expect(x,0)
#else
  #define _likely_(x) (x)
  #define _unlikely_(x) (x)
#endif

/*
 *  Endianness macros.
 */
#ifndef AXHASH_LITTLE_ENDIAN
  #if defined(_WIN32) || defined(__LITTLE_ENDIAN__) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    #define AXHASH_LITTLE_ENDIAN
  #elif defined(__BIG_ENDIAN__) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    #define AXHASH_BIG_ENDIAN
  #else
    #warning "could not determine endianness! Falling back to little endian."
    #define AXHASH_LITTLE_ENDIAN
  #endif
#endif

/*
 * From https://stackoverflow.com/a/776523 by Peter Cordes.
 */
AXHASH_INLINE uint32_t ax_rotl64 (uint64_t n, unsigned int c)
{
  const unsigned int mask = (CHAR_BIT*sizeof(n) - 1);  // assumes width is a power of 2.
  c &= mask;
  return (n<<c) | (n>>( (-c)&mask ));
}

/*
 *  Default seed. This is one of Moremur's multipliers, which is a mixing function by Pelle Evensen.
 * It could be pretty much anything.
 */
#define AX_SEED (0x3C79AC492BA7B653ull)

/*
 *  Simple mixing function. Note that if the input is 0, this returns 0.
 *
 *  @param x  unsigned 64-bit number.
 */
AXHASH_INLINE uint64_t mix(uint64_t x) AXHASH_NOEXCEPT {
  AXHASH_CONSTEXPR unsigned int R0 = 23u;
  AXHASH_CONSTEXPR unsigned int R1 = 43u;
  AXHASH_CONSTEXPR unsigned int R2 = 11u;
  AXHASH_CONSTEXPR unsigned int R3 = 50u;
  x = (x ^ ax_rotl64(x, R0) ^ ax_rotl64(x, R1));
  x *= C;
  x = (x ^ ax_rotl64(x, R2) ^ ax_rotl64(x, R3));
  return x;
}

/*
 *  Simple two-argument mixing function that combines both arguments. Note that if both inputs are 0, this returns 0.
 *
 *  @param h  unsigned 64-bit number; typically a value being accumulated onto.
 *  @param x  unsigned 64-bit number; typically a datum that should be incorporated with mixing into @h .
 */
AXHASH_INLINE uint64_t mix_stream(uint64_t h, uint64_t x) AXHASH_NOEXCEPT {
  AXHASH_CONSTEXPR unsigned int R1 = 39u;
  x *= C;
  x ^= (x >> R1);
  h += x * C;
  h *= C;
  return h;
}

/*
 *  A fast five-argument mixing function that only mixes the last four arguments, and sums them with the first.
 *  If the last four arguments are all 0, this returns @h without changes.
 *
 *  @param h  unsigned 64-bit number; typically a value being accumulated onto.
 *  @param a  unsigned 64-bit number; will be mixed with c and d.
 *  @param b  unsigned 64-bit number; will be mixed with c and d.
 *  @param c  unsigned 64-bit number; will be mixed with a and b.
 *  @param d  unsigned 64-bit number; will be mixed with a and b.
 */
AXHASH_INLINE uint64_t mix_stream_bulk(uint64_t h, uint64_t a, uint64_t b, uint64_t c, uint64_t d) AXHASH_NOEXCEPT {
  AXHASH_CONSTEXPR unsigned int R2 = 29u;
  return h
         + (ax_rotl64(a, R2) - c) * Q
         + (ax_rotl64(b, R2) - d) * R
         + (ax_rotl64(c, R2) - b) * S
         + (ax_rotl64(d, R2) - a) * T;
}


/*
 *  Read functions.
 */
#ifdef AXHASH_LITTLE_ENDIAN
AXHASH_INLINE uint64_t ax_read64(const uint8_t *p) AXHASH_NOEXCEPT { uint64_t v; memcpy(&v, p, sizeof(uint64_t)); return v;}
AXHASH_INLINE uint64_t ax_read32(const uint8_t *p) AXHASH_NOEXCEPT { uint32_t v; memcpy(&v, p, sizeof(uint32_t)); return v;}
AXHASH_INLINE uint64_t ax_read16(const uint8_t *p) AXHASH_NOEXCEPT { uint16_t v; memcpy(&v, p, sizeof(uint16_t)); return v;}
#elif defined(__GNUC__) || defined(__INTEL_COMPILER) || defined(__clang__)
AXHASH_INLINE uint64_t ax_read64(const uint8_t *p) AXHASH_NOEXCEPT { uint64_t v; memcpy(&v, p, sizeof(uint64_t)); return __builtin_bswap64(v);}
AXHASH_INLINE uint64_t ax_read32(const uint8_t *p) AXHASH_NOEXCEPT { uint32_t v; memcpy(&v, p, sizeof(uint32_t)); return __builtin_bswap32(v);}
AXHASH_INLINE uint64_t ax_read16(const uint8_t *p) AXHASH_NOEXCEPT { uint16_t v; memcpy(&v, p, sizeof(uint16_t)); return __builtin_bswap16(v);}
#elif defined(_MSC_VER)
AXHASH_INLINE uint64_t ax_read64(const uint8_t *p) AXHASH_NOEXCEPT { uint64_t v; memcpy(&v, p, sizeof(uint64_t)); return _byteswap_uint64(v);}
AXHASH_INLINE uint64_t ax_read32(const uint8_t *p) AXHASH_NOEXCEPT { uint32_t v; memcpy(&v, p, sizeof(uint32_t)); return _byteswap_ulong(v);}
AXHASH_INLINE uint64_t ax_read16(const uint8_t *p) AXHASH_NOEXCEPT { uint16_t v; memcpy(&v, p, sizeof(uint16_t)); return _byteswap_ushort(v);}
#else
AXHASH_INLINE uint64_t ax_read64(const uint8_t *p) AXHASH_NOEXCEPT {
  uint64_t v; memcpy(&v, p, 8);
  return (((v >> 56) & 0xff)| ((v >> 40) & 0xff00)| ((v >> 24) & 0xff0000)| ((v >>  8) & 0xff000000)| ((v <<  8) & 0xff00000000)| ((v << 24) & 0xff0000000000)| ((v << 40) & 0xff000000000000)| ((v << 56) & 0xff00000000000000));
}
AXHASH_INLINE uint64_t ax_read32(const uint8_t *p) AXHASH_NOEXCEPT {
  uint32_t v; memcpy(&v, p, 4);
  return (((v >> 24) & 0xff)| ((v >>  8) & 0xff00)| ((v <<  8) & 0xff0000)| ((v << 24) & 0xff000000));
}
AXHASH_INLINE uint64_t ax_read16(const uint8_t *p) AXHASH_NOEXCEPT {
  uint16_t v; memcpy(&v, p, 2);
  return (((v >>  8) & 0xff)| ((v <<  8) & 0xff00));
}
#endif

/*
 *  axhash main function.
 *
 *  @param key     Buffer to be hashed.
 *  @param len     @key length, in bytes.
 *  @param seed    64-bit seed used to alter the hash result predictably.
 *
 *  Returns a 64-bit hash.
 */
AXHASH_INLINE uint64_t axhash_internal(const void *key, size_t len, uint64_t seed) AXHASH_NOEXCEPT {
  // TODO: Fill this in with the actual code!
  return seed;
}

/*
 *  axhash seeded hash function.
 *
 *  @param key     Buffer to be hashed.
 *  @param len     @key length, in bytes.
 *  @param seed    64-bit seed used to alter the hash result predictably.
 *
 *  Calls axhash_internal using provided parameters, but does some mixing to seed.
 *
 *  Returns a 64-bit hash.
 */
AXHASH_INLINE uint64_t axhash_seeded(const void *key, size_t len, uint64_t seed) AXHASH_NOEXCEPT {
  return axhash_internal(key, len, seed ^ ax_rotl64(seed, 29u) ^ ax_rotl64(seed, 47u));
}

/*
 *  axhash default hash function.
 *
 *  @param key     Buffer to be hashed.
 *  @param len     @key length, in bytes.
 *
 *  Calls axhash_internal using provided parameters and the default seed, without mixing.
 *
 *  Returns a 64-bit hash.
 */
AXHASH_INLINE uint64_t axhash(const void *key, size_t len) AXHASH_NOEXCEPT {
  return axhash_internal(key, len, AX_SEED);
}
