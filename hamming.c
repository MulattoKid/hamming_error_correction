#include "hamming.h"

#include <stdio.h>

/**
 * We are encoding input data into 1-byte blocks. We can arrange the bits as below
 * 
 *     r0  r1
 * c0 | 0 | 1 |
 *    ---------
 * c1 | 2 | 3 |
 *    ---------
 * c2 | 4 | 5 |
 *    ---------
 * c3 | 6 | 7 |
 * 
 * To determine where something is we use binary search, so we ask
 * 1) Is it in r0?
 *     - if yes, it's in r0
 *     - if not, it's in r1
 * 2) Is it in c0 or c1?
 *     - if yes, it's in c0 or c1
 *     - if no, it's in c2 or c3
 * 3) Is it in c0 or c2?
 *     - if yes, it's in c0 or c2
 *     - if no, it's in c1 or c3
 * Asking these three questions will give us a row and a column, and thus a cell/index.
 * 
 * Hamming encoding works by ensuring each group we ask about has an even number of
 * bits set, it has parity. To ensure each group has parity we need one parity bit
 * per group, i.e. 3 bits. Given that our output is 8 bits, and we need 3 for parity
 * we can only encode 4 bits of data. That leaves us with 1 extra bit.
 * 
 * - The parity bit for 1) will be at index 6
 * - The parity bit for 2) will be at index 2
 * - The parity bit for 3) will be at index 4
 * 
 * To handle the case where there are no errors (we end up at bit 7) when asking our
 * questions we simply ignore this bit when computing the parity bits. This means
 * our output is essentially 7-bit, and not 8-bit. This works fine as we had 1 bit
 * left over that's not used.
*/
bool hamming_encode(uint8_t in, uint8_t* out)
{
    printf("hamming_encode:\n");
    printf("\tInput: 0x%02X\n", in);

    if ((in & 0b11110000) > 0)
    {
        printf("Only the first 4 bits can be used\n");
        return false;
    }

    // Modify input to have input bits in locations 0, 1, 3 and 5
    // Move bit 3 to 5
    in |= (in & 0b1000) << 2;
    // Clear bit 3
    in &= 0b11110111;
    // Move bit 2 to 3
    in |= (in & 0b100) << 1;
    // Clear bit 2
    in &= 0b11111011;

    // r0
    uint8_t r0_bits_set = 0;
    if ((in & 0b010000000) != 0)
    {
        r0_bits_set++;
    }
    if ((in & 0b00010000) != 0)
    {
        r0_bits_set++;
    }
    if ((in & 0b00000100) != 0)
    {
        r0_bits_set++;
    }
    if ((in & 0b00000001) != 0)
    {
        r0_bits_set++;
    }
    uint8_t r0_parity_bit = (r0_bits_set % 2) == 0 ? 0 : 1;
    printf("\tr0 parity bit: %u\n", r0_parity_bit);

    // c0c1
    uint8_t c0c1_bits_set = 0;
    if ((in & 0b00001000) != 0)
    {
        c0c1_bits_set++;
    }
    if ((in & 0b00000100) != 0)
    {
        c0c1_bits_set++;
    }
    if ((in & 0b00000010) != 0)
    {
        c0c1_bits_set++;
    }
    if ((in & 0b00000001) != 0)
    {
        c0c1_bits_set++;
    }
    uint8_t c0c1_parity_bit = (c0c1_bits_set % 2) == 0 ? 0 : 1;
    printf("\tc0c1 parity bit: %u\n", c0c1_parity_bit);

    // c0c2
    uint8_t c0c2_bits_set = 0;
    if ((in & 0b00000010) != 0)
    {
        c0c2_bits_set++;
    }
    if ((in & 0b00000001) != 0)
    {
        c0c2_bits_set++;
    }
    if ((in & 0b00100000) != 0)
    {
        c0c2_bits_set++;
    }
    if ((in & 0b00010000) != 0)
    {
        c0c2_bits_set++;
    }
    uint8_t c0c2_parity_bit = (c0c2_bits_set % 2) == 0 ? 0 : 1;
    printf("\tc0c2 parity bit: %u\n", c0c2_parity_bit);
    
    // Move parity bits to correct locations
    r0_parity_bit = r0_parity_bit << 5;
    c0c1_parity_bit = c0c1_parity_bit << 1;
    c0c2_parity_bit = c0c2_parity_bit << 3;
    uint8_t parity_mask = r0_parity_bit | c0c1_parity_bit | c0c2_parity_bit;
    printf("\tParity mask: 0x%02X\n", parity_mask);

    *out = in | parity_mask;

    return true;
}

/**
 * 
*/
bool hamming_verify(uint8_t in, uint8_t* error_index)
{
    printf("hamming_verify:\n");
    printf("\tInput: 0x%02X\n", in);

    // r0
    uint8_t r0_bits_set = 0;
    if ((in & 0b010000000) != 0)
    {
        r0_bits_set++;
    }
    if ((in & 0b00010000) != 0)
    {
        r0_bits_set++;
    }
    if ((in & 0b00000100) != 0)
    {
        r0_bits_set++;
    }
    if ((in & 0b00000001) != 0)
    {
        r0_bits_set++;
    }
    uint8_t r0_even_bit_sets = (r0_bits_set % 2) == 0 ? 1 : 0;
    printf("\tr0 even bits set: %u\n", r0_even_bit_sets);

    // c0c1
    uint8_t c0c1_bits_set = 0;
    if ((in & 0b00001000) != 0)
    {
        c0c1_bits_set++;
    }
    if ((in & 0b00000100) != 0)
    {
        c0c1_bits_set++;
    }
    if ((in & 0b00000010) != 0)
    {
        c0c1_bits_set++;
    }
    if ((in & 0b00000001) != 0)
    {
        c0c1_bits_set++;
    }
    uint8_t c0c1_even_bits_set = (c0c1_bits_set % 2) == 0 ? 1 : 0;
    printf("\tc0c1 even bits set: %u\n", c0c1_even_bits_set);

    // c0c2
    uint8_t c0c2_bits_set = 0;
    if ((in & 0b00000010) != 0)
    {
        c0c2_bits_set++;
    }
    if ((in & 0b00000001) != 0)
    {
        c0c2_bits_set++;
    }
    if ((in & 0b00100000) != 0)
    {
        c0c2_bits_set++;
    }
    if ((in & 0b00010000) != 0)
    {
        c0c2_bits_set++;
    }
    uint8_t c0c2_even_bits_set = (c0c2_bits_set % 2) == 0 ? 1 : 0;
    printf("\tc0c2 even bits set: %u\n", c0c2_even_bits_set);

    // Determine flipped bit location
    if (r0_even_bit_sets == 0)
    {
        if (c0c1_even_bits_set == 0)
        {
            if (c0c2_even_bits_set == 0)
            {
                *error_index = 0;
            }
            else
            {
                *error_index = 2;
            }
        }
        else
        {
            if (c0c2_even_bits_set == 0)
            {
                *error_index = 4;
            }
            else
            {
                *error_index = 6;
            }
        }
    }
    else
    {
        if (c0c1_even_bits_set == 0)
        {
            if (c0c2_even_bits_set == 0)
            {
                *error_index = 1;
            }
            else
            {
                *error_index = 3;
            }
        }
        else
        {
            if (c0c2_even_bits_set == 0)
            {
                *error_index = 5;
            }
            else
            {
                *error_index = UINT8_MAX;
            }
        }
    }
    printf("\tError index: %u\n", *error_index);

    return true;
}
