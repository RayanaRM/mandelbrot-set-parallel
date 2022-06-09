#include <iostream>
#include "includes/fractalCreator.h"
#include <pthread.h>
#include "includes/bitmap.h"
#include <map>
#include "includes/bitmap.h"

int const WIDTH = 100;
int const HEIGHT = 100;
typedef unsigned char Pixel[5];
static const int MAX_ITERATIONS = 10000;

static const unsigned char MAX_RGB_VALUE = 255;
struct Px
{
	int
			x,
			y,
			red,
			green,
			blue;
};

struct ThreadArguments
{
	int
			threadId,
			startX,
			endX,
			startY,
			endY;
	double
			minBoundsReal,
			minBoundsImaginary,
			scaleReal,
			scaleImaginary;
	Pixel
			*colors,
			*pixels;
};

void *threadFunction(void *threadArguments)
{

	ThreadArguments *args = static_cast<ThreadArguments *>(threadArguments);

	
	FractalCreator frac(WIDTH, HEIGHT);
	frac.addRange(0.0, RGB(0, 0, 0));
	frac.addRange(1.0, RGB(0, 0, 255));
	std::vector<Some> sr = frac.run(args->startX, args->endX,	args->startY,	args->endY);
	// std::cout << sr.size() << std::endl;
	// std::cout << args->threadId << std::endl;
	// std::cout << sr.size() << std::endl;
	// std::cout << args->threadId * 10 + args->startX << std::endl;
	int opa =0;
	for (int j = args->startY; j < args->endY; j++) {
		for (int i = args->startX; i < args->endX; i++) {
			args->pixels[(j*100)+i][0] = sr[(j*100)+i].x;
			args->pixels[(j*100)+i][1] = sr[(j*100)+i].y;
			args->pixels[(j*100)+i][2] = sr[(j*100)+i].red;
			args->pixels[(j*100)+i][3] = sr[(j*100)+i].green;
			args->pixels[(j*100)+i][4] = sr[(j*100)+i].blue;
			// std::cout << sr[i].x << " | " << sr[(j*100)+i].y << " | " << sr[i+(j*10)].red << " | " << sr[i+(j*10)].green << " | " << sr[i+(j*10)].blue << std::endl;
			// std::cout << args->startX << " | " << args->endX << " | " << args->startY << " | " << args->endY << std::endl;
			opa++;
		}
	}
	// std::cout << opa << std::endl;
	// std::cout << "-------" << std::ezndl;
}

int main()
{
	static const int
			DIV_HEIGHT = HEIGHT/10,		// Vertical divisions
			DIV_WIDTH = WIDTH/10,			// Horizontal divisions
			IMG_HEIGHT = WIDTH, // PPM image height
			IMG_WIDTH = HEIGHT; // PPM image width
		// MAX_ITERATIONS = 100;	// Maximum number of iterations

	int
			qtdX = (IMG_WIDTH / DIV_WIDTH),		// Width of each sector
			qtdY = (IMG_HEIGHT / DIV_HEIGHT), // Height of each sector
			tmpCont = 0,
			// threadCount = 1; // Sector count
			threadCount = DIV_WIDTH * DIV_HEIGHT; // Sector count

	int(*numThread)[4] = new int[threadCount][4];

	// cria o buffer de trabalho um matriz com dados de onde está localizado cada pedaço
	int imgY = 0;
	while (imgY < IMG_HEIGHT)
	{
		int imgX = 0;
		while (imgX < IMG_WIDTH)
		{
			numThread[tmpCont][0] = imgX;				 // startX
			numThread[tmpCont][1] = imgY;				 // startY
			numThread[tmpCont][2] = imgX + qtdX; // endX
			numThread[tmpCont][3] = imgY + qtdY; // endY

			tmpCont++;
			imgX += qtdX;
		}

		imgY += qtdY;
	}
	bmp::Bitmap _bitmap(WIDTH, HEIGHT);
	// Parallel from here!
	pthread_t *threads = new pthread_t[threadCount]; // Thread list
	ThreadArguments *threadArguments = new ThreadArguments[threadCount];
	Pixel *pixels = new Pixel[IMG_HEIGHT * IMG_WIDTH];

	for (int i = 0; i < threadCount; i++)
	{

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

	// std::vector<Some> sr = frac.run();
	// std::cout << sizeof(pixels) << std::endl;
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			_bitmap.setPixel(pixels[(j*100)+i][0], pixels[(j*100)+i][1], pixels[(j*100)+i][2], pixels[(j*100)+i][3], pixels[(j*100)+i][4]);
		}
	}

	_bitmap.write("outputs/test.bmp");

	std::cout << "Finished." << std::endl;
	return 0;
}
