#pragma once

#include <stdint.h>

#define XTEA_BLOCKLEN 8

void xtea_encrypt(const uint32_t key[4], uint32_t data[2], int num_rounds);

void xtea_decrypt(const uint32_t key[4], uint32_t data[2], int num_rounds);