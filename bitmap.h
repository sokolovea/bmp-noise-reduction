#pragma once
#include <stdint.h>

#pragma pack(push, 1)
struct bitmap_file_header
{
    uint16_t bf_type;
    uint32_t bf_size;
    uint16_t bf_reserved_1;
    uint16_t bf_reserved_2;
    uint32_t bf_off_bits;
};
#pragma pack(pop)

struct bitmap_info_header
{
    uint32_t bi_size;
    int32_t bi_width;
    int32_t bi_height;
    uint16_t bi_planes;
    uint16_t bi_bit_count;
    uint8_t payload[24];
};

#pragma pack(push, 1)
struct bitmap_24_pixel
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};
#pragma pack(pop)
