#ifdef _WIN32
#include "cimg\CImg.h"
#endif
#ifdef linux
#include "cimg/CImg.h"
#endif
#include <iostream>
#include <cmath>

using namespace cimg_library;

double **getGaussian(int height, int width, double sigma)
{
    double **filter;
    double sum = 0.0;
    int i, j;

    filter = new double *[height];
    for (int i = 0; i < height; i++)
        filter[i] = new double[width];

    for (i = 0; i < height; i++)
        for (j = 0; j < width; j++)
        {
            filter[i][j] = exp(-(i * i + j * j) / (2 * sigma * sigma)) / (2 * M_PI * sigma * sigma);
            sum += filter[i][j];
        }

    for (i = 0; i < height; i++)
        for (j = 0; j < width; j++)
            filter[i][j] /= sum;

    return filter;
}

double ***applyFilter(double ***image, double **filter, int width, int height, int filterWidth, int filterHeight)
{

    int newImageHeight = height - filterHeight + 1;
    int newImageWidth = width - filterWidth + 1;
    int d, i, j, h, w;

    double ***newImage;
    newImage = new double **[3];
    for (int i = 0; i < 3; i++)
    {
        newImage[i] = new double *[width];
        for (int j = 0; j < width; j++)
            newImage[i][j] = new double[height];
    }

    for (d = 0; d < 3; d++)
        for (i = 0; i < newImageWidth; i++)
            for (j = 0; j < newImageHeight; j++)
                for (h = i; h < i + filterWidth; h++)
                    for (w = j; w < j + filterHeight; w++)
                        newImage[d][i][j] += filter[h - i][w - j] * image[d][h][w];

    return newImage;
}

int main(int argc, char const *argv[])
{
    //Read input image
    CImg<unsigned char> originalImage("img/input.jpg");
    /*
      For store RGB image data int  3d double array
      kernel[k][x][y]
      Dimension 1 (K): for Red Blue Green (RGB) spectrum
      Dimension 2 (x): pixels of image in width
      Dimension 3 (y): pixels of image in height
    */
    double ***kernel;

    int width = originalImage.width();
    int height = originalImage.height();

    //Memory Allocation for kernel
    kernel = new double **[3];
    for (int i = 0; i < 3; i++)
    {
        kernel[i] = new double *[width];
        for (int j = 0; j < width; j++)
            kernel[i][j] = new double[height];
    }

    //Copy input image to kernel
    for (int k = 0; k < 3; k++)
        for (int x = 0; x < width; x++)
            for (int y = 0; y < height; y++)
                kernel[k][x][y] = originalImage(x, y, 0, k);

    //Showing orginal input image
    CImgDisplay Original_image(originalImage, "Original image");

    /*---*/
    int filterHeight = 25;
    int filterWidth = 25;
    //Apply Gaussian blur filter on input image
    kernel = applyFilter(kernel, getGaussian(filterHeight, filterWidth, 10.0), width, height, filterWidth, filterHeight);
    /*---*/

    int bluredImageHeight = height - filterHeight + 1;
    int bluredImageWidth = width - filterWidth + 1;
    //Create CImg object for convert double kernel to RGB image
    CImg<unsigned char> image(bluredImageWidth, bluredImageHeight, 1, 3);

    //Copy blured image to image object
    for (int k = 0; k < 3; k++)
        for (int x = 0; x < bluredImageWidth; x++)
            for (int y = 0; y < bluredImageHeight; y++)
                image(x, y, 0, k) = kernel[k][x][y];

    image.normalize(0, 255);
    //Showing blured image
    CImgDisplay bluredImage(image, "Blured Image");
    while (!bluredImage.is_closed())
        bluredImage.wait();
    return 0;
}
