/*
 * axhash - Much fast. Very hashing. Such platform. Wow.
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
#include <limits.h>
#include <string.h>

/*
 *  C++ macros.
 *
 *  AX_INLINE can be overridden to be stronger than a hint, i.e. by adding __attribute__((always_inline)).
 */
#ifdef __cplusplus
  #define AX_NOEXCEPT noexcept
  #ifndef AX_INLINE
    #define AX_INLINE inline
  #endif
#else
  #define AX_NOEXCEPT
  #ifndef AX_INLINE
    #define AX_INLINE static inline
  #endif
#endif

/*
 *  Endianness macros.
 */
#ifndef AX_LITTLE_ENDIAN
  #if defined(_WIN32) || defined(__LITTLE_ENDIAN__) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    #define AX_LITTLE_ENDIAN
  #elif defined(__BIG_ENDIAN__) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    #define AX_BIG_ENDIAN
  #else
    #warning "could not determine endianness! Falling back to little endian."
    #define AX_LITTLE_ENDIAN
  #endif
#endif

//    MX3 multiplier.
static const uint64_t AX_C = UINT64_C(0xBEA225F9EB34556D);
//    Random 64-bit probable primes, as given by Java's BigInteger class.
static const uint64_t AX_Q = UINT64_C(0xD1B92B09B92266DD);
static const uint64_t AX_R = UINT64_C(0x9995988B72E0D285);
static const uint64_t AX_S = UINT64_C(0x8FADF5E286E31587);
static const uint64_t AX_T = UINT64_C(0xFCF8B405D3D0783B);


/*
 *  From https://stackoverflow.com/a/776523 by Peter Cordes.
 *  @param n  the unsigned 64-bit number to bitwise-rotate left.
 *  @param c  the shift count for how many steps to rotate; should be unsigned.
 */
AX_INLINE uint64_t ax_rotl64 (uint64_t n, unsigned int c)
{
  const unsigned int mask = (CHAR_BIT*sizeof(n) - 1);  // assumes width is a power of 2.
  c &= mask;
  return (n<<c) | (n>>( (-c)&mask ));
}

/*
 *  Default seed. This is one of Moremur's multipliers, which is a mixing function by Pelle Evensen.
 *  It could be pretty much anything.
 */
#define AX_SEED UINT64_C(0x3C79AC492BA7B653)

/*
 *  Simple mixing function. Note that if the input is 0, this returns 0.
 *
 *  @param x  unsigned 64-bit number.
 */
AX_INLINE uint64_t ax_mix(uint64_t x) AX_NOEXCEPT {
  static const unsigned int R0 = 23u;
  static const unsigned int R1 = 43u;
  static const unsigned int R2 = 11u;
  static const unsigned int R3 = 50u;
  x = (x ^ ax_rotl64(x, R0) ^ ax_rotl64(x, R1));
  x *= AX_C;
  x = (x ^ ax_rotl64(x, R2) ^ ax_rotl64(x, R3));
  return x;
}

  /*
 *  Simple two-argument mixing function that combines both arguments. Note that if both inputs are 0, this returns 0.
 *
 *  @param h  unsigned 64-bit number; typically a value being accumulated onto.
 *  @param x  unsigned 64-bit number; typically a datum that should be incorporated with mixing into @h .
 */
AX_INLINE uint64_t ax_mix_stream(uint64_t h, uint64_t x) AX_NOEXCEPT {
  static const unsigned int R1 = 39u;
  x *= AX_C;
  x ^= (x >> R1);
  h += x * AX_C;
  h *= AX_C;
  return h;
}

/*
 *  A fast five-argument mixing function that only mixes the last four arguments, and sums them with the first.
 *  If the last four arguments are all 0, this returns @h without changes.
 *
 *  @param h  unsigned 64-bit number; typically a value being accumulated onto.
 *  @param a  unsigned 64-bit number; will be mixed with b and d.
 *  @param b  unsigned 64-bit number; will be mixed with c and a.
 *  @param c  unsigned 64-bit number; will be mixed with d and b.
 *  @param d  unsigned 64-bit number; will be mixed with a and c.
 */
AX_INLINE uint64_t ax_mix_stream_bulk(const uint64_t h, const uint64_t a, const uint64_t b, const uint64_t c, const uint64_t d) AX_NOEXCEPT {
  static const unsigned int Q2 = 28u;
  static const unsigned int R2 = 29u;
  static const unsigned int S2 = 27u;
  static const unsigned int T2 = 25u;
  return h
         + (ax_rotl64(a, Q2) + b) * AX_Q
         + (ax_rotl64(b, R2) + c) * AX_R
         + (ax_rotl64(c, S2) + d) * AX_S
         + (ax_rotl64(d, T2) + a) * AX_T;
}


/*
 *  Read functions.
 */
#ifdef AX_LITTLE_ENDIAN
AX_INLINE uint64_t ax_read64(const uint8_t * p, const uint32_t i) AX_NOEXCEPT { uint64_t v; memcpy(&v, &p[i], 8); return v;}
AX_INLINE uint64_t ax_read32(const uint8_t * p, const uint32_t i) AX_NOEXCEPT { uint32_t v; memcpy(&v, &p[i], 4); return v;}
AX_INLINE uint64_t ax_read16(const uint8_t * p, const uint32_t i) AX_NOEXCEPT { uint16_t v; memcpy(&v, &p[i], 2); return v;}
#elif defined(__GNUC__) || defined(__INTEL_COMPILER) || defined(__clang__)
AX_INLINE uint64_t ax_read64(const uint8_t *p, const uint32_t i) AX_NOEXCEPT { uint64_t v; memcpy(&v, &p[i], sizeof(uint64_t)); return __builtin_bswap64(v);}
AX_INLINE uint64_t ax_read32(const uint8_t *p, const uint32_t i) AX_NOEXCEPT { uint32_t v; memcpy(&v, &p[i], sizeof(uint32_t)); return __builtin_bswap32(v);}
AX_INLINE uint64_t ax_read16(const uint8_t *p, const uint32_t i) AX_NOEXCEPT { uint16_t v; memcpy(&v, &p[i], sizeof(uint16_t)); return __builtin_bswap16(v);}
#elif defined(_MSC_VER)
AX_INLINE uint64_t ax_read64(const uint8_t *p, const uint32_t i) AX_NOEXCEPT { uint64_t v; memcpy(&v, &p[i], sizeof(uint64_t)); return _byteswap_uint64(v);}
AX_INLINE uint64_t ax_read32(const uint8_t *p, const uint32_t i) AX_NOEXCEPT { uint32_t v; memcpy(&v, &p[i], sizeof(uint32_t)); return _byteswap_ulong(v);}
AX_INLINE uint64_t ax_read16(const uint8_t *p, const uint32_t i) AX_NOEXCEPT { uint16_t v; memcpy(&v, &p[i], sizeof(uint16_t)); return _byteswap_ushort(v);}
#else
AX_INLINE uint64_t ax_read64(const uint8_t *p) AX_NOEXCEPT {
  uint64_t v; memcpy(&v, p, 8);
  return (((v >> 56) & 0xff)| ((v >> 40) & 0xff00)| ((v >> 24) & 0xff0000)| ((v >>  8) & 0xff000000)| ((v <<  8) & 0xff00000000)| ((v << 24) & 0xff0000000000)| ((v << 40) & 0xff000000000000)| ((v << 56) & 0xff00000000000000));
}
AX_INLINE uint64_t ax_read32(const uint8_t *p) AX_NOEXCEPT {
  uint32_t v; memcpy(&v, p, 4);
  return (((v >> 24) & 0xff)| ((v >>  8) & 0xff00)| ((v <<  8) & 0xff0000)| ((v << 24) & 0xff000000));
}
AX_INLINE uint64_t ax_read16(const uint8_t *p) AX_NOEXCEPT {
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
AX_INLINE uint64_t ax_hash_internal(const void *key, size_t len, uint64_t seed) AX_NOEXCEPT {
  static const unsigned int R1 = 37u;
  const uint8_t *buf=(const uint8_t *)key; 
  uint64_t h = len ^ seed;

  while (len >= 64) {
    len -= 64;
    h = ax_mix_stream_bulk (h * AX_C,          ax_read64 (buf,  0), ax_read64 (buf,  8), ax_read64(buf, 16), ax_read64(buf, 24));
    h = ax_mix_stream_bulk (ax_rotl64 (h, R1), ax_read64 (buf, 32), ax_read64 (buf, 40), ax_read64(buf, 48), ax_read64(buf, 56));
    buf += 64;
  }

  while (len >= 8) {
    len -= 8;
    h = ax_mix_stream(h, ax_read64(buf, 0));
    buf += 8;
  }
    
  const uint8_t* const tail8 = buf;
  switch (len) {
  case 1: h = (ax_mix_stream(h, tail8[0]));                                                                     break;
  case 2: h = (ax_mix_stream(h, ax_read16(tail8, 0)));                                                          break;
  case 3: h = (ax_mix_stream(h, ax_read16(tail8, 0) | (uint64_t)(tail8[2]) << 16));                             break;
  case 4: h = (ax_mix_stream(h, ax_read32(tail8, 0)));                                                          break;
  case 5: h = (ax_mix_stream(h, ax_read32(tail8, 0) | (uint64_t)(tail8[4]) << 32));                             break;
  case 6: h = (ax_mix_stream(h, ax_read32(tail8, 0) | ax_read16(tail8, 4) << 32));                              break;
  case 7: h = (ax_mix_stream(h, ax_read32(tail8, 0) | ax_read16(tail8, 4) << 32 | (uint64_t)(tail8[6]) << 48)); break;
  default:;
  }
  return ax_mix(h);
}

/*
 *  axhash seeded hash function.
 *
 *  @param key     Buffer to be hashed.
 *  @param len     @key length, in bytes.
 *  @param seed    64-bit seed used to alter the hash result predictably; will be mixed a little.
 *
 *  Calls ax_hash_internal using provided parameters, but does some mixing to seed.
 *  This is, in fact, nearly identical to ax_hash_internal, except that this
 *  function mixes seed with a XOR-Rotate-XOR-Rotate step. If you are confident that your seed is not going
 *  to be used in conjunction with other extremely similar seeds (such as seed, seed+1, seed+2, etc.), then
 *  you can call ax_hash_internal directly with your unmixed seed to save a few cycles.
 *
 *  Returns a 64-bit hash.
 */
AX_INLINE uint64_t ax_hash_seeded(const void *key, size_t len, uint64_t seed) AX_NOEXCEPT {
  return ax_hash_internal (key, len, (seed ^ ax_rotl64 (seed, 29u) ^ ax_rotl64 (seed, 47u)));
}

/*
 *  axhash default hash function.
 *
 *  @param key     Buffer to be hashed.
 *  @param len     @key length, in bytes.
 *
 *  Calls ax_hash_internal using provided parameters and the default seed, without mixing.
 *
 *  Returns a 64-bit hash.
 */
AX_INLINE uint64_t ax_hash(const void *key, size_t len) AX_NOEXCEPT {
  return ax_hash_internal(key, len, AX_SEED);
}
