#ifndef HAMMING_H
#define HAMMING_H

#include <stdbool.h>
#include <stdint.h>

bool hamming_encode(uint8_t in, uint8_t* out);
bool hamming_verify(uint8_t in, uint8_t* error_index);

#endif
