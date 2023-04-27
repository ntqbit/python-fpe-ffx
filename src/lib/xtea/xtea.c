#include "xtea.h"

#define DELTA 0x9E3779B9

void xtea_encrypt(const uint32_t key[4], uint32_t data[2], int num_rounds)
{
    uint32_t sum = 0;

    for (int i = 0; i < num_rounds; i++)
    {
        data[0] += (((data[1] << 4) ^ (data[1] >> 5)) + data[1]) ^ (sum + key[sum & 3]);
        sum += DELTA;
        data[1] += (((data[0] << 4) ^ (data[0] >> 5)) + data[0]) ^ (sum + key[(sum >> 11) & 3]);
    }
}

void xtea_decrypt(const uint32_t key[4], uint32_t data[2], int num_rounds)
{
    uint32_t sum = DELTA * num_rounds;

    for (int i = 0; i < num_rounds; i++)
    {
        data[1] -= (((data[0] << 4) ^ (data[0] >> 5)) + data[0]) ^ (sum + key[(sum >> 11) & 3]);
        sum -= DELTA;
        data[0] -= (((data[1] << 4) ^ (data[1] >> 5)) + data[1]) ^ (sum + key[sum & 3]);
    }
}