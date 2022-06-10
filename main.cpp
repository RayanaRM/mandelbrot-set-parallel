#include "includes/fractalCreator.h"
#include <iostream>
#include <pthread.h>
#include "includes/bitmap.h"

int const WIDTH = 100;
int const HEIGHT = 100;
int const DIV_HEIGHT = HEIGHT / 10;
int const DIV_WIDTH = WIDTH / 10;
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

void *buildFractal(void *threadArguments) {

	FractalParams *args = static_cast<FractalParams *>(threadArguments);

	FractalCreator frac(WIDTH, HEIGHT);
	frac.addRange(0.0, RGB(0, 0, 0));
	frac.addRange(1.0, RGB(0, 0, 255));
	
	std::vector<RGBValues> sr = frac.run(args->startX, args->endX, args->startY, args->endY);
	for (int j = args->startY; j < args->endY; j++)	{
		for (int i = args->startX; i < args->endX; i++) {
			args->colors[(j * WIDTH) + i][0] = sr[(j * WIDTH) + i].red;
			args->colors[(j * WIDTH) + i][1] = sr[(j * WIDTH) + i].green;
			args->colors[(j * WIDTH) + i][2] = sr[(j * WIDTH) + i].blue;
		}
	}
}

int main() {
	int tmpCont = 0;
	int threadCount = DIV_WIDTH * DIV_HEIGHT;
	int	qtdX = (WIDTH / DIV_WIDTH);
	int	qtdY = (HEIGHT / DIV_HEIGHT);

	int(*numThread)[4] = new int[threadCount][4];

	// cria o buffer de trabalho
	int imgY = 0;
	while (imgY < HEIGHT)	{
		int imgX = 0;
		while (imgX < WIDTH) {
			numThread[tmpCont][0] = imgX;
			numThread[tmpCont][1] = imgY;
			numThread[tmpCont][2] = imgX + qtdX;
			numThread[tmpCont][3] = imgY + qtdY;

			tmpCont++;
			imgX += qtdX;
		}

		imgY += qtdY;
	}
	bmp::Bitmap _bitmap(WIDTH, HEIGHT);
	
	pthread_t *threads = new pthread_t[threadCount];
	FractalParams *threadArguments = new FractalParams[threadCount];
	ColorRGB *colors = new ColorRGB[HEIGHT * WIDTH];

	for (int i = 0; i < threadCount; i++)	{

		FractalParams currentThreadArguments = {
			.startX = numThread[i][0],
			.endX = numThread[i][2],
			.startY = numThread[i][1],
			.endY = numThread[i][3],
			.colors = colors
		};

		threadArguments[i] = currentThreadArguments;

		pthread_create(&threads[i], NULL, buildFractal, &threadArguments[i]);
	}

	for (int i = 0; i < threadCount; i++)	{
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
