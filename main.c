#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <malloc.h>
#include "bitmap.h"
#include "bitmap_util.h"

enum work_mode {
    INCREASE,
    DECREASE
};

int main(int argc, char** argv) {
    if (argc != 5) {
        perror("Wrong parameters count!");
        return 1;
    }

    FILE* input_file = fopen(argv[3], "r");
    FILE* output_file = fopen(argv[4], "w");

    if (!input_file || !output_file) {
        perror("Can't open files!");
        return 2;
    }

    struct bitmap_file_header bitmap_input_file_header;
    struct bitmap_info_header bitmap_input_info_header;

    size_t count_elems = fread(&bitmap_input_file_header, sizeof(struct bitmap_file_header), 1, input_file);
    if (count_elems != 1) {
        perror("Error reading source image at start!");
        return 2;
    }
    if (bitmap_input_file_header.bf_type != 0x4d42) {
        perror("Not BMP file!");
        return 3;
    }

    count_elems = fread(&bitmap_input_info_header, sizeof(struct bitmap_info_header), 1, input_file);
    if (count_elems != 1) {
        perror("Error reading source image!");
        return 2;
    }

    if (bitmap_input_info_header.bi_bit_count != 24) {
        perror("Not 24 bit BMP file!");
        return 4;
    }

    enum work_mode work_mode;
    if (!strcmp("-inc", argv[1])) {
        work_mode = INCREASE;
    }
    else if (!strcmp("-dec", argv[1])) {
        work_mode = DECREASE;
    } else {
        perror("Unknown work mode !");
        return 5;
    }

    int32_t coeff = atoi(argv[2]);
    if (coeff <= 0) {
        perror("Coefficient is not correct!");
        return 6;
    }

    int32_t input_width_px = bitmap_input_info_header.bi_width;
    int32_t input_height_px = bitmap_input_info_header.bi_height;

    int32_t output_width_px = work_mode == INCREASE? input_width_px * coeff : divide_with_ceil(input_width_px, coeff);
    int32_t output_height = work_mode == INCREASE? input_height_px * coeff : divide_with_ceil(input_height_px, coeff);

    // if (outputWidth <= 0 || inputWidth <= 0) {
    //     cerr << "Coefficient is too large!" << endl;
    //     return 5;
    // }

    struct bitmap_file_header bitmap_output_file_header = bitmap_input_file_header;
    struct bitmap_info_header bitmap_output_info_header = bitmap_input_info_header;

    uint64_t input_row_bytes_count = input_width_px * 3;
    uint64_t output_row_bytes_count = output_width_px * 3;

    uint64_t input_row_bytes_count_with_padding = getRowSizeWithPadding(input_row_bytes_count);
    uint64_t output_row_bytes_count_with_padding = getRowSizeWithPadding(output_row_bytes_count);

    uint64_t input_row_padding_bytes = input_row_bytes_count_with_padding - input_row_bytes_count;
    uint64_t output_row_padding_bytes = output_row_bytes_count_with_padding - output_row_bytes_count;

    uint8_t* input_row = malloc(input_row_bytes_count_with_padding * sizeof(uint8_t));
    uint8_t* output_row = malloc(output_row_bytes_count_with_padding * sizeof(uint8_t));

    if (!input_row || !output_row) {
        perror("Malloc error!");
        return 5;
    }

    bitmap_output_file_header.bf_size = sizeof(struct bitmap_file_header) + sizeof(struct bitmap_info_header)
                                                                    + output_row_bytes_count * output_height;
    bitmap_output_file_header.bf_off_bits = sizeof(struct bitmap_file_header) + sizeof(struct bitmap_info_header);
    count_elems = fwrite(&bitmap_output_file_header, sizeof(struct bitmap_file_header), 1, output_file);

    bitmap_output_info_header.bi_width = output_width_px;
    bitmap_output_info_header.bi_height = output_height;
    count_elems = fwrite(&bitmap_output_info_header, sizeof(struct bitmap_info_header), 1, output_file);
    uint64_t fseek_code;

    if (work_mode == INCREASE) {
        for (int32_t i = 0; i < input_height_px; i++) {
            count_elems = fread(input_row, sizeof(struct bitmap_24_pixel), input_width_px, input_file);
            fseek_code = fseek(input_file, input_row_padding_bytes, SEEK_CUR);
            if (count_elems != input_width_px || fseek_code != 0) {
                perror("Error reading source image!");
                return 6;
            }
            increaseResolution((struct bitmap_24_pixel*)input_row, (struct bitmap_24_pixel*)output_row, input_width_px, output_width_px, coeff);
            for (int32_t duplication = 0; duplication < coeff; duplication++) {
                count_elems = fwrite(output_row, sizeof(uint8_t), output_row_bytes_count_with_padding, output_file);
                if (count_elems != output_row_bytes_count_with_padding) {
                    perror("Error writing to file!");
                    return 7;
                }
            }
        }
    } else if (work_mode == DECREASE) {
        uint32_t iterations_count = divide_with_ceil(input_height_px, coeff);
        for (uint32_t i = 0; i < iterations_count; i++) {
            count_elems = fread(input_row, sizeof(struct bitmap_24_pixel), input_width_px, input_file);
            if (count_elems != input_width_px) {
                perror("Error reading source image!");
                return 6;
            }
            fseek_code = fseek(input_file, input_row_padding_bytes + input_row_bytes_count_with_padding * (coeff - 1), SEEK_CUR);
            decreaseResolution((struct bitmap_24_pixel*)input_row, (struct bitmap_24_pixel*)output_row, input_width_px, output_width_px, coeff);
            count_elems = fwrite(output_row, sizeof(uint8_t), output_row_bytes_count_with_padding, output_file);
            if (count_elems != output_row_bytes_count_with_padding) {
                perror("Error writing to file!");
                return 7;
            }
        }
    }
    fclose(output_file);
    fclose(input_file);
    free(input_row);
    free(output_row);
    return 0;
}
