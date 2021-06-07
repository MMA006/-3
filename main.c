#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include "list.h"
#define WIN32_MEAN_AND_LEAN

int main(int argc, char* argv[]) {
	if (argc > 1)
		read(argv[1], argv[2]);
	else
		printf("Not arguments");
	return 0;
}