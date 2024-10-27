#include <stdint.h>
#include "util.h"

/**
 * Reverse the bits of a byte. 
 */
uint8_t reverse_bits(uint8_t x) {
  x = ((x & 0xAA) >> 1) | ((x & 0x55) << 1);
  x = ((x & 0xCC) >> 2) | ((x & 0x33) << 2);
  x = ((x & 0xF0) >> 4) | ((x & 0x0F) << 4);
  return x;
}
