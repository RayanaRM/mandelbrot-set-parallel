#ifndef FRACTALCREATOR_H_
#define FRACTALCREATOR_H_

#include "bitmap.h"
#include "mandelbrot.h"
#include "zoomList.h"
#include "zoom.h"
#include "rgb.h"
#include <cstdint>
#include <vector>
#include <memory>
#include <math.h>
#include <assert.h>
struct Some
{
	int 
		x, y, red, green, blue;
};
class FractalCreator{
private:
	int _width;
	int _height;
	int _total{0};
	bmp::Bitmap _bitmap;
	std::unique_ptr<int[]> _histogram;
	std::unique_ptr<int[]> _fractal;
	ZoomList _zoomList;
	
	std::vector<int> _ranges;
	std::vector<RGB> _colors;
	std::vector<int> _rangeTotals;

	bool _bGotFirstRange{false};

private:
	void writeBitmap(std::string fileName);
	void calculateRangeTotals();
	void calculateIterations(int startX, int endX, int startY, int endY);
	std::vector<Some> drawFractal();
	void calculateTotalIterations();

public:
	FractalCreator(int width, int height);
	void addZoom(const Zoom& zoom);
	std::vector<Some> run(int startX, int endX, int startY, int endY);
	void addRange(double rangeEnd, const RGB& rgb);
	int getRange(int iterations) const;
};

#endif
