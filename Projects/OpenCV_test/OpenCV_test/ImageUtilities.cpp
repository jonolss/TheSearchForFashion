#include "ImageUtilities.h"

/**Downsamples an image and put the result in a one dimensional matrix.
*
* Input1: The image that is going to be downsampled.
* Input1: The number of pixels on one side used for the downsampling, e.g. size=10 uses 100 pixels.
* Output: The resulting matrix.
*/
Mat createlocalEdgeImageHist(Mat edges, int size)
{
	int nVerBoxs = edges.size().height / size;
	int nHorBoxs = edges.size().width / size;
	int leiHistSize = nVerBoxs * nHorBoxs;
	//vector<float> leiHist(leiHistSize);
	Mat leiHist(leiHistSize, 1, CV_32FC1);

	for (int s = 0; s < nHorBoxs; s++)
	{
		for (int t = 0; t < nVerBoxs; t++)
		{
			int tot = 0;
			for (int i = 0; i < size; i++)
			{
				for (int j = 0; j < size; j++)
				{
					if ((int)edges.at<unsigned char>(s * size + i, t * size + j) != 0)
						tot++;
				}
			}
			leiHist.at<float>(s * nVerBoxs + t, 0) = (float)tot / (float)(size*size);
		}
	}
	return leiHist;
}

/**Sets all pixels with a alpha value greater than 0 to the color white(255,255,255).
*
* Input1: The image with artifacts in the alpha region.
*/
void filterAlphaArtifacts(Mat *img)
{
	Mat *ch = (Mat*)calloc(4, sizeof(Mat));

	split(*img, ch);
	Mat ch1 = ch[3];

	for (int i = 0; i < ch1.rows; i++)
	{
		for (int j = 0; j < ch1.cols; j++)
		{
			if (ch1.at<unsigned char>(i, j) == 0)
			{
				img->at<Vec4b>(i, j) = Vec4b(255, 255, 255, 0);
			}
		}
	}
	free(ch);
}

/**Preformce a canny edge detection inputed image.
*
* Input1: The image that is going to be edge detected.
* Output: The resulting edge image.
*/
Mat preformCanny(Mat src)
{
	Mat out;

	double threshold1 = 60.0;
	double threshold2 = 120.0;

	int apertureSize = 3;
	bool L2gradient = false;

	Canny(src, out, threshold1, threshold2, apertureSize, L2gradient);

	return out;
}

/**Preformce a gaussian blur on inputed image.
*
* Input1: The image that needs blurring.
* Output: The resulting blurred image.
*/
Mat preformGaussianBlur(Mat src)
{
	Mat out;

	Size ksize = Size(3, 3);
	double sigmaX = 4.00;
	double sigmaY = 4.00;

	GaussianBlur(src, out, ksize, sigmaX, sigmaY);

	return out;
}

/**Makes a normalized version of inputed histogram.
*
* Input1: The histogram that needs normalization.
* Output: The resulting normalized histogram.
*/
Mat normalizeHist(Mat hist)
{
	float totEnt = 0;
	for (int k = 0; k < 32; k++)
	{
		totEnt += hist.at<float>(k, 0);
	}
	for (int k = 0; k < 32; k++)
	{
		hist.at<float>(k, 0) /= totEnt;
	}

	return hist;
}

/**Makes a histogram of chosen channel of a RGB image.
*
* Input1: The image whose channel is going to be extracted.
* Input2: Number of the channel that is going to be extracted.
* InputX: ...
* Output: The requested channel of the input image.
*/
Mat get8bitHist(Mat img1D, int numLevels, int minRange, int maxRange)
{
	MatND hist, hist2;
	int histSize[] = { numLevels };
	float hranges[] = { 0, 256 };
	float hranges2[] = { 0, 255 }; //Takes away the background which always are completly white in test images. May also take away other bright pixels that aren't entierly white, but this shouldn't be many pixels.
	const float* ranges[] = { hranges };
	const float* ranges2[] = { hranges2 };
	int channels[] = { 0 };

	calcHist(&img1D, 1, channels, Mat(), hist, 1, histSize, ranges, true, false);
	calcHist(&img1D, 1, channels, Mat(), hist2, 1, histSize, ranges2, true, false);

	return hist2;
}


/**Makes a histogram of chosen channel of a HSV image.
*
* Input1: The image whose channel is going to be extracted.
* Input2: Number of the channel that is going to be extracted.
* InputX: ...
* Output: The requested channel of the input image.
*/
Mat getHsvHist(Mat img1D, int type, int numLevels, int minRange, int maxRange)
{
	MatND hist;
	int histSize[] = { numLevels };
	int channels[] = { 0 };
	float hranges[] = { 0, 180 };
	float sranges[] = { 0, 256 };
	float vranges[] = { 0, 360 };
	const float* ranges[] = { hranges };
	if (type == 0)
	{
		const float* ranges[] = { hranges };
		calcHist(&img1D, 1, channels, Mat(), hist, 1, histSize, ranges, true, false);
	}

	else if (type == 1)
	{
		const float* ranges[] = { sranges };
		calcHist(&img1D, 1, channels, Mat(), hist, 1, histSize, ranges, true, false);
	}

	else
	{
		const float* ranges[] = { vranges };
		calcHist(&img1D, 1, channels, Mat(), hist, 1, histSize, ranges, true, false);
	}
	return hist;
}

/**Recovers the requsted channel from an image.
*
* Input1: The image whose channel is going to be extracted.
* Input2: Number of the channel that is going to be extracted.
* Output: The requested channel of the input image.
*/
Mat getChannel(Mat src, int channel)
{
	Mat *ch = (Mat*)calloc(4, sizeof(Mat));

	Mat tmpCh;
	split(src, ch);
	ch[channel].copyTo(tmpCh);

	for (int i = 0; i < 4; i++)
	{

		ch[i].release();
	}

	free(ch);

	return tmpCh;
}