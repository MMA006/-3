#pragma once
#include <stdio.h>
void write(BITMAPFILEHEADER file_header, RGBTRIPLE pixel, FILE* input, FILE* output, BITMAPINFOHEADER info_header, RGBTRIPLE** img, int Width, int Height);
void read(const char* file_in, const char* file_out);