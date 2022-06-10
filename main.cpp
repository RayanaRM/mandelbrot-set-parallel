#include "includes/fractalCreator.h"
#include <iostream>
#include <pthread.h>
#include "includes/bitmap.h"

int const WIDTH = 100;
int const HEIGHT = 100;
int const BLOCO_X = HEIGHT / 10;
int const BLOCO_Y = WIDTH / 10;
typedef unsigned char ColorRGB[3];

struct FractalParams {
	int
			startX,
			endX,
			startY,
			endY;
	ColorRGB
			*colors;
};

void *buildFractal(void *fractalParams) {

	FractalParams *params = static_cast<FractalParams *>(fractalParams);

	FractalCreator frac(WIDTH, HEIGHT);
	frac.addRange(0.0, RGB(0, 0, 0));
	frac.addRange(1.0, RGB(0, 0, 255));
	
	std::vector<RGBValues> sr = frac.run(params->startX, params->endX, params->startY, params->endY);
	for (int j = params->startY; j < params->endY; j++)	{
		for (int i = params->startX; i < params->endX; i++) {
			params->colors[(j * WIDTH) + i][0] = sr[(j * WIDTH) + i].red;
			params->colors[(j * WIDTH) + i][1] = sr[(j * WIDTH) + i].green;
			params->colors[(j * WIDTH) + i][2] = sr[(j * WIDTH) + i].blue;
		}
	}
}

int main() {
	int tmpCont = 0;
	int qtdThreads = BLOCO_Y * BLOCO_X;
	int	qtdX = (WIDTH / BLOCO_Y);
	int	qtdY = (HEIGHT / BLOCO_X);

	int(*bufferTrab)[4] = new int[qtdThreads][4];

	// cria o buffer de trabalho
	int Y = 0;
	while (Y < HEIGHT)	{
		int X = 0;
		while (X < WIDTH) {
			bufferTrab[tmpCont][0] = X;
			bufferTrab[tmpCont][1] = Y;
			bufferTrab[tmpCont][2] = X + qtdX;
			bufferTrab[tmpCont][3] = Y + qtdY;

			tmpCont++;
			X += qtdX;
		}

		Y += qtdY;
	}
	bmp::Bitmap _bitmap(WIDTH, HEIGHT);
	
	pthread_t *threads = new pthread_t[qtdThreads];
	FractalParams *fractalParams = new FractalParams[qtdThreads];
	ColorRGB *colors = new ColorRGB[HEIGHT * WIDTH];

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
	
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			_bitmap.setPixel(i, j, colors[(j * WIDTH) + i][0], colors[(j * WIDTH) + i][1], colors[(j * WIDTH) + i][2]);
		}
	}

	_bitmap.write("outputs/test.bmp");

	std::cout << "Finished." << std::endl;
	return 0;
}
