#pragma once
#include <stdint.h>
#include <stdio.h>
#include "bitmap.h"

// important: inputRowBytesCount, outputRowBytesCount - row sizes without padding 
static void increaseResolution(struct bitmap_24_pixel* inputRow, struct bitmap_24_pixel* outputRow, uint64_t inputRowPixelsCount, uint64_t outputRowPixelsCount, uint64_t coeff) {
    for (uint64_t i = 0; i < inputRowPixelsCount; i++) {
        for (uint64_t j = 0; j < coeff; j++) {
            outputRow[i * coeff + j] = inputRow[i];
            if (i * coeff + j >= outputRowPixelsCount || i >= inputRowPixelsCount) {
                perror("BUG increaseResolution\n");
            }
        }
    }
}

// important: inputRowBytesCount, outputRowBytesCount - row sizes without padding
static void decreaseResolution(struct bitmap_24_pixel* inputRow, struct bitmap_24_pixel* outputRow, uint64_t inputRowPixelsCount, uint64_t outputRowPixelsCount, uint64_t coeff) {
    for (uint64_t i = 0, j = 0; i < outputRowPixelsCount; i += 1, j += coeff) {
        outputRow[i] = inputRow[j];
        if (i >= outputRowPixelsCount || j >= inputRowPixelsCount) {
            perror("BUG decreaseResolution\n");
        }
    }
}

uint64_t getRowSizeWithPadding(uint64_t rowSizeWithoutPadding) {
    return ((rowSizeWithoutPadding + 3) &~ 3);
}

uint64_t divide_with_ceil(uint64_t a, uint64_t b) {
    return (a + b - 1) / b;
}
