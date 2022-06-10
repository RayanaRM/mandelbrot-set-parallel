#include "includes/fractalCreator.h"
#include <iostream>
#include <pthread.h>
#include "includes/bitmap.h"

int const width = 100;
int const height = 100;
int const blocoX = 10;
int const blocoY = 10;
typedef unsigned char ColorRGB[3];

struct FractalParams {
	int startX;
	int endX;
	int startY;
	int endY;
	ColorRGB *colors;
};

void *buildFractal(void *fractalParams) {

	FractalParams *params = static_cast<FractalParams *>(fractalParams);

	FractalCreator frac(width, height);
	frac.addRange(0.0, RGB(0, 0, 0));
	frac.addRange(1.0, RGB(0, 0, 255));
	
	std::vector<RGBValues> sr = frac.run(params->startX, params->endX, params->startY, params->endY);
	for (int j = params->startY; j < params->endY; j++)	{
		for (int i = params->startX; i < params->endX; i++) {
			params->colors[(j * width) + i][0] = sr[(j * width) + i].red;
			params->colors[(j * width) + i][1] = sr[(j * width) + i].green;
			params->colors[(j * width) + i][2] = sr[(j * width) + i].blue;
		}
	}
}

int main() {
	int tmpCont = 0;
	int qtdThreads = blocoY * blocoX;

	// cria o buffer de trabalho
	int(*bufferTrab)[4] = new int[qtdThreads][4];
	int Y = 0;
	while (Y < height)	{
		int X = 0;
		while (X < width) {
			bufferTrab[tmpCont][0] = X;
			bufferTrab[tmpCont][1] = Y;
			bufferTrab[tmpCont][2] = X + (width / blocoY);
			bufferTrab[tmpCont][3] = Y + (height / blocoX);

			tmpCont++;
			X += (width / blocoY);
		}

		Y += (height / blocoX);
	}
	
	pthread_t *threads = new pthread_t[qtdThreads];
	FractalParams *fractalParams = new FractalParams[qtdThreads];
	ColorRGB *colors = new ColorRGB[height * width];

	for (int i = 0; i < qtdThreads; i++)	{

		FractalParams currentFractalParams = {
			.startX = bufferTrab[i][0],
			.endX = bufferTrab[i][2],
			.startY = bufferTrab[i][1],
			.endY = bufferTrab[i][3],
			.colors = colors
		};

		fractalParams[i] = currentFractalParams;

		pthread_create(&threads[i], NULL, buildFractal, &fractalParams[i]);
	}

	for (int i = 0; i < qtdThreads; i++)	{
		pthread_join(threads[i], NULL);
	}
	
	bmp::Bitmap _bitmap(width, height);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			_bitmap.setPixel(i, j, colors[(j * width) + i][0], colors[(j * width) + i][1], colors[(j * width) + i][2]);
		}
	}

	_bitmap.write("outputs/test.bmp");

	std::cout << "Finished." << std::endl;
	return 0;
}
