#include "ImageUtilities.h"

/**Downsamples an image and put the result in a one dimensional matrix.
*
* \param edges Image that is going to be downsampled.
* \param size Number of pixels on one side used for the downsampling, e.g. size=10 uses 100 pixels.
* \return The resulting matrix.
*/
cv::Mat createlocalEdgeImageHist(cv::Mat edges, int size)
{
	int nVerBoxs = edges.size().height / size;
	int nHorBoxs = edges.size().width / size;
	int leiHistSize = nVerBoxs * nHorBoxs;
	//vector<float> leiHist(leiHistSize);
	cv::Mat leiHist(leiHistSize, 1, CV_32FC1);

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
* \param img Reference to the image with artifacts in the alpha region.
*/
void filterAlphaArtifacts(cv::Mat *img)
{
	cv::Mat *ch = (cv::Mat*)calloc(4, sizeof(cv::Mat));

	split(*img, ch);
	cv::Mat alpha = ch[3];

	//namedWindow("omg", 1);
	//imshow("omg", *img);

	for (int i = 0; i < alpha.rows; i++)
	{
		for (int j = 0; j < alpha.cols; j++)
		{
			if (alpha.at<unsigned char>(i, j) == 0)
			{
				img->at<cv::Vec4b>(i, j) = cv::Vec4b(255, 255, 255, 0);
			}
		}
	}

	for (int i = 0; i < 4; i++)
	{

		ch[i].release();
	}

	free(ch);

	/*
	namedWindow("Hello", 1);
	imshow("Hello", alpha);
	namedWindow("Query edges", 1);
	imshow("Query edges", *img);
	waitKey(0);
	*/
}

/**Preformce a canny edge detection inputed image.
*
* \param src Image that is going to be edge detected.
* \return The resulting edge image.
*/
cv::Mat preformCanny(cv::Mat src)
{
	cv::Mat out;

	double threshold1 = 60.0;
	double threshold2 = 120.0;

	int apertureSize = 3;
	bool L2gradient = false;

	Canny(src, out, threshold1, threshold2, apertureSize, L2gradient);

	return out;
}

/**Preformce a gaussian blur on inputed image.
*
* \param src Image that needs blurring.
* \return The resulting blurred image.
*/
cv::Mat preformGaussianBlur(cv::Mat src)
{
	cv::Mat out;

	cv::Size ksize = cv::Size(3, 3);
	double sigmaX = 4.00;
	double sigmaY = 4.00;

	GaussianBlur(src, out, ksize, sigmaX, sigmaY);

	return out;
}

/**Makes a normalized version of inputed histogram.
*
* \param hist Histogram that needs normalization.
* \return The resulting normalized histogram.
*/
cv::Mat normalizeHist(cv::Mat hist)
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
* \param img1D Image whose channel is going to be extracted.
* \param numLevels Channel that is going to be extracted. Red 0, Green 1, Blue 2.
* \param ...
* \return The requested channel of the input image.
*/
cv::Mat get8bitHist(cv::Mat img1D, int numLevels, int minRange, int maxRange)
{
	cv::MatND hist, hist2;
	int histSize[] = { numLevels };
	float hranges[] = { 0, 256 };
	float hranges2[] = { 0, 255 }; //Takes away the background which always are completly white in test images. May also take away other bright pixels that aren't entierly white, but this shouldn't be many pixels.
	const float* ranges[] = { hranges };
	const float* ranges2[] = { hranges2 };
	int channels[] = { 0 };

	calcHist(&img1D, 1, channels, cv::Mat(), hist, 1, histSize, ranges, true, false);
	calcHist(&img1D, 1, channels, cv::Mat(), hist2, 1, histSize, ranges2, true, false);

	return hist2;
}


/**Makes a histogram of chosen channel of a HSV image.
*
* \param img1D Image whose channel is going to be extracted.
* \param type Type of channel that is going to be extracted. Hue 0, Saturation 1, Value 2.
* \param numLevels Channel that is going to be extracted. Hue 0, Saturation 1, Value 2.
* \param ...
* \return The requested channel of the input image.
*/
cv::Mat getHsvHist(cv::Mat img1D, int type, int numLevels, int minRange, int maxRange)
{
	cv::MatND hist;
	int histSize[] = { numLevels };
	int channels[] = { 0 };
	float hranges[] = { 0, 180 };
	float sranges[] = { 0, 256 };
	float vranges[] = { 0, 360 };
	const float* ranges[] = { hranges };
	if (type == 0)
	{
		const float* ranges[] = { hranges };
		calcHist(&img1D, 1, channels, cv::Mat(), hist, 1, histSize, ranges, true, false);
	}

	else if (type == 1)
	{
		const float* ranges[] = { sranges };
		calcHist(&img1D, 1, channels, cv::Mat(), hist, 1, histSize, ranges, true, false);
	}

	else
	{
		const float* ranges[] = { vranges };
		calcHist(&img1D, 1, channels, cv::Mat(), hist, 1, histSize, ranges, true, false);
	}
	return hist;
}

/**Recovers the requsted channel from an image.
*
* \param src Image whose channel is going to be extracted.
* \param channel Index of the channel that is going to be extracted.
* \return The requested channel of the input image.
*/
cv::Mat getChannel(cv::Mat src, int channel)
{
	cv::Mat *ch = (cv::Mat*)calloc(4, sizeof(cv::Mat));

	cv::Mat tmpCh;
	cv::split(src, ch);
	ch[channel].copyTo(tmpCh);

	for (int i = 0; i < 4; i++)
	{

		ch[i].release();
	}

	free(ch);

	return tmpCh;
}

/**Resize images, default = 300x300.
*
* \param input Image that is going to be resized.
* \return The resulting resized image.
*/
/*cv::Mat resizeImg(cv::Mat input, int sizeX, int sizeY)
{
	cv::Mat out(cv::Size(sizeX, sizeY), CV_8UC3);

	int inpHt = input.size().height;
	int inpWh = input.size().width;
	if (inpWh > sizeX || inpHt > sizeY)
	{
		cv::resize(input, out, cv::Size(sizeX, sizeY), 0.0, 0.0, cv::INTER_AREA);
	}
	else if (inpWh < sizeX || inpHt < sizeY)
	{
		cv::resize(input, out, cv::Size(sizeX, sizeY), 0.0, 0.0, cv::INTER_LINEAR);
	}

	return out;
}*/

cv::Mat resizeImg(cv::Mat input, int sizeX, int sizeY) //int sides)
{
	int sides = sizeX;

	cv::Size inSize = input.size();
	float inRatio = (float)inSize.width / (float)inSize.height;
	cv::Size newSize;
	
	if (inRatio < 0.9)
		newSize = cv::Size((int)((float)sides * inRatio), sides);
	else if (inRatio > 1.1)
		newSize = cv::Size(sides, (int)((float)sides * (1/inRatio)));
	else
		newSize = cv::Size(sides, sides);

	cv::Mat mid(newSize, CV_8UC3);

	if (inSize.width > sides || inSize.height > sides)
	{
		cv::resize(input, mid, newSize, 0.0, 0.0, cv::INTER_AREA);
	}
	else if (inSize.width < sides || inSize.height < sides)
	{
		cv::resize(input, mid, newSize, 0.0, 0.0, cv::INTER_LINEAR);
	}

	if (mid.size() == cv::Size(sides, sides))
	{
		return mid;
	}

	cv::Mat out(cv::Size(sides, sides), CV_8UC3, cv::Scalar(255, 255, 255));
	if (mid.size().width == sides)
	{
		int delta = (sides - mid.size().height) / 2;

		mid.copyTo(out(cv::Rect(0, delta, mid.cols, mid.rows)));
	}
	else
	{
		int delta = (sides - mid.size().width) / 2;

		mid.copyTo(out(cv::Rect(delta, 0, mid.cols, mid.rows)));
	}

	return out;
}