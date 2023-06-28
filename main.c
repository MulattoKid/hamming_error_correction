#include "hamming.h"

#include <stdio.h>

int main(int argc, char** argv)
{
    /* Encode */
    uint8_t in = 0b00001111;
    uint8_t out = 0;
    if (hamming_encode(in, &out) != true)
    {
        return -1;
    }

    /* Verify */
    uint8_t corrupted = 0b00000111;
    uint8_t error_index = 0;
    if (hamming_verify(corrupted, &error_index) != true)
    {
        return -1;
    }

    return 0;
}
