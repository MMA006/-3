#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include "list.h"
#define WIN32_MEAN_AND_LEAN

void write(BITMAPFILEHEADER file_header, RGBTRIPLE pixel, FILE* input, FILE* output, BITMAPINFOHEADER info_header, RGBTRIPLE** img, int Width, int Height) {
	/*запись нового файла*/
	info_header.biHeight = Height;
	info_header.biWidth = Width;
	fwrite(&file_header, sizeof(BITMAPFILEHEADER), 1, output);
	fwrite(&info_header, sizeof(BITMAPINFOHEADER), 1, output);

	//расчет нового значения padding
	int padding = 0;
	if ((Height * 3) % 4)
		padding = 4 - (Height * 3) % 4;

	/*---------запись картинки--------------*/
	for (int i = 0; i < Height; i++) {
		for (int j = 0; j < Width; j++) {
			fwrite(&img[i][j], sizeof(RGBTRIPLE), 1, output);
		}
		if (padding != 0) {
			BYTE filler = 0;
			fwrite(&filler, sizeof(BYTE), padding, output);
		}
	}
}

void read(const char* file_in, const char* file_out) {
	BITMAPFILEHEADER file_header;
	BITMAPINFOHEADER info_header;
	RGBTRIPLE pixel;
	FILE* input, * output;
	input = fopen(file_in, "rb");
	output = fopen(file_out, "wb");
	if (!input) {
		printf("File bmp not found\n");
		system("pause");
		return;
	}
	/*чтение BMPFILEHEADER и BMPINFOHEADER*/
	fread(&file_header, sizeof(BITMAPFILEHEADER), 1, input);
	fread(&info_header, sizeof(BITMAPINFOHEADER), 1, input);

	if (info_header.biCompression != 0 || info_header.biBitCount != 24 || info_header.biPlanes != 1) {
		printf("File not a BMP24\n");
		system("pause");
		return;
	}

	int Width = info_header.biWidth;
	int Height = info_header.biHeight;

	/*длина строки должны быть кратна 4, иначе файл будет поврежден.
	Будем проверять это условие.
	padding - количество байт, недостающих до того чтобы кол-во байты было кратно 4*/
	unsigned char padding = 0;
	if ((Width * 3) % 4)
		padding = 4 - (Width * 3) % 4;

	/*выделяем память для массива данных о пикселях*/
	RGBTRIPLE** img = malloc(Height * sizeof(RGBTRIPLE));
	for (size_t i = 0; i < Height; i++)
		img[i] = malloc(Width * sizeof(RGBTRIPLE));

	/*чтение пикселей*/
	for (int i = 0; i < Height; i++) {
		for (int j = 0; j < Width; j++) {
			fread(&pixel, sizeof(RGBTRIPLE), 1, input);
			//считывание r g b
			byte red = pixel.rgbtRed;
			byte green = pixel.rgbtGreen;
			byte blue = pixel.rgbtBlue;
			//перевод в y cb cr
			double y = 0.299 * (double)red + 0.587 * (double)green + 0.114 * (double)blue;
			double cb = 128 - 0.168736 * (double)red - 0.331264 * (double)green + 0.5 * (double)blue;
			double cr = 128 + 0.5 * (double)red - 0.418688 * (double)green - 0.081312 * (double)blue;
			//уменьшение яркости
			y -= 100;
			if (y < 0)
				y = 0;
			else if (y > 255)
				y = 255;
			//обратный перевод в r g b
			pixel.rgbtRed = (double)y + 1.402 * (double)(cr - 128);
			pixel.rgbtGreen = (double)y - 0.34414 * (double)(cb - 128) - 0.71414 * (double)(cr - 128);
			if (pixel.rgbtGreen < 0)
				pixel.rgbtGreen = 0;
			else if (pixel.rgbtGreen > 255)
				pixel.rgbtGreen = 255;
			pixel.rgbtBlue = (double)y + 1.772 * (double)(cb - 128);
			img[i][j] = pixel;
		}
		fseek(input, padding, SEEK_CUR); // пропуск "выравнивающих" байтов
	}
	fclose(input);
	write(file_header, pixel, input, output, info_header, img, Width, Height);
}