#include "includes/fractalCreator.h"
#include <iostream>
#include <pthread.h>
#include "includes/bitmap.h"

int const WIDTH = 100;
int const HEIGHT = 100;
typedef unsigned char Pixel[5];

struct ThreadArguments {
	int
			threadId,
			startX,
			endX,
			startY,
			endY;
	Pixel
			*colors,
			*pixels;
};

void *threadFunction(void *threadArguments) {

	ThreadArguments *args = static_cast<ThreadArguments *>(threadArguments);

	FractalCreator frac(WIDTH, HEIGHT);
	frac.addRange(0.0, RGB(0, 0, 0));
	frac.addRange(1.0, RGB(0, 0, 255));
	std::vector<Some> sr = frac.run(args->startX, args->endX, args->startY, args->endY);
	// std::cout << args->startX << " | " << args->endX << " | " << args->startY << " | " << args->endY << " | " << std::endl;
	for (int j = args->startY; j < args->endY; j++)	{
		for (int i = args->startX; i < args->endX; i++) {
			args->pixels[(j * WIDTH) + i][0] = sr[(j * WIDTH) + i].x;
			args->pixels[(j * WIDTH) + i][1] = sr[(j * WIDTH) + i].y;
			args->pixels[(j * WIDTH) + i][2] = sr[(j * WIDTH) + i].red;
			args->pixels[(j * WIDTH) + i][3] = sr[(j * WIDTH) + i].green;
			args->pixels[(j * WIDTH) + i][4] = sr[(j * WIDTH) + i].blue;

			// std::cout << (j * WIDTH) + i << std::endl;
			
		}
	}
}

int main() {
	
	static const int
			DIV_HEIGHT = HEIGHT / 10,
			DIV_WIDTH = WIDTH / 10,
			IMG_HEIGHT = WIDTH,
			IMG_WIDTH = HEIGHT;
	int
			qtdX = (IMG_WIDTH / DIV_WIDTH),
			qtdY = (IMG_HEIGHT / DIV_HEIGHT),
			tmpCont = 0,
			// threadCount = 1;
			threadCount = DIV_WIDTH * DIV_HEIGHT;

	int(*numThread)[4] = new int[threadCount][4];

	// cria o buffer de trabalho
	int imgY = 0;
	while (imgY < IMG_HEIGHT)	{
		int imgX = 0;
		while (imgX < IMG_WIDTH) {
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
	ThreadArguments *threadArguments = new ThreadArguments[threadCount];
	Pixel *pixels = new Pixel[IMG_HEIGHT * IMG_WIDTH];

	for (int i = 0; i < threadCount; i++)	{

		ThreadArguments currentThreadArguments = {
			.threadId = i,
			.startX = numThread[i][0],
			.endX = numThread[i][2],
			.startY = numThread[i][1],
			.endY = numThread[i][3],
			.pixels = pixels
		};

		threadArguments[i] = currentThreadArguments;

		pthread_create(&threads[i], NULL, threadFunction, &threadArguments[i]);
	}

	for (int i = 0; i < threadCount; i++)	{
		pthread_join(threads[i], NULL);
	}
	
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			_bitmap.setPixel(pixels[(j * WIDTH) + i][0], pixels[(j * WIDTH) + i][1], pixels[(j * WIDTH) + i][2], pixels[(j * WIDTH) + i][3], pixels[(j * WIDTH) + i][4]);
		}
	}

	_bitmap.write("outputs/test.bmp");

	std::cout << "Finished." << std::endl;
	return 0;
}
