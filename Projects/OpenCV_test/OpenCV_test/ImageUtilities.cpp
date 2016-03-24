/**
* @file
* @author Jonathan Olsson <jonolss92@gmail.com>
* @version 1.0
*
* @section LICENSE
*
* Here I will have information about License.
*
* @section DESCRIPTION
*
* Some utillity functions that is helping with images.
*
*/

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


	cv::Mat tmp = resizeImg(edges, nHorBoxs, nVerBoxs);

	tmp = tmp.reshape(1, 1);
	tmp = tmp.t();

	cv::Mat tmp2(tmp.size(), CV_32FC1);
	for (int i = 0; i < tmp.size().height; i++)
		tmp2.at<float>(i, 0) = ((float)tmp.at<uchar>(i, 0)) / (float)255;

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

	return tmp2;
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
cv::Mat preformCanny(cv::Mat src, double lowThresh, double highThresh)
{
	cv::Mat out;

	int apertureSize = 3;
	bool L2gradient = false;

	Canny(src, out, lowThresh, highThresh, apertureSize, L2gradient);

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
	double sigmaX = 4.0;
	double sigmaY = 4.0;

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
* \param type Channel that is going to be extracted. Red 0, Green 1, Blue 2.
* \param numBins Number of bins that the histogram will have.
* \return The requested channel of the input image.
*/
cv::Mat get8bitHist(cv::Mat img1D, int numBins)
{
	cv::MatND hist, hist2;
	int histSize[] = { numBins };
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
* \param numBins Number of bins that the histogram will have.
* \return The requested channel of the input image.
*/
cv::Mat getHsvHist(cv::Mat img1D, int type, int numBins)
{
	cv::MatND hist;
	int histSize[] = { numBins };
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
cv::Mat resizeImg(cv::Mat input, int sizeX, int sizeY)
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

/**Saves an openCV matrix to an open file.
* 
* \param input The image that is going to be stored.
* \param outFile A filestream for an output file.
*/
void saveMat(cv::Mat input, ofstream *outFile)
{
	int width    = input.size().width;
	int height   = input.size().height;
	int type     = input.type();
	int elemSize = input.elemSize();
	char* data   = (char*)input.data;
	
	int *head = (int*)calloc(4, sizeof(int));
	*head = width;
	*(head + 1) = height;
	*(head + 2) = type;
	*(head + 3) = elemSize;
	outFile->write((char*)head, 4 * sizeof(int));
	free(head);

	int bytes = width * height * elemSize;
	outFile->write((char*)data, bytes);
}

/**Loades an openCV matrix from an open file.
*
* \param inFile A filestream for an input file.
* \return The stored matrix.
*/
cv::Mat loadMat(ifstream *inFile)
{
	int width;
	int height;
	int type;
	int elemSize;
	char* data;

	int bytes = 4 * sizeof(int);
	int *head = (int*)calloc(4, sizeof(int));
	inFile->read((char*)head, bytes);
	width    = *head;
	height   = *(head + 1);
	type     = *(head + 2);
	elemSize = *(head + 3);
	free(head);

	bytes = width * height * elemSize;
	data = (char*)calloc(bytes, sizeof(char));
	inFile->read((char*)data, bytes);
	
	//cout << width << endl;
	//cout << height << endl;
	//cout << type << endl;
	//cout << elemSize << endl;

	cv::Mat res = cv::Mat(cv::Size(width, height), type, (uchar*)data);
	return res;
}

/**Makes a topological skeleton by using morphological operators.
*
* \param binaryImg An binary image that is going to be skeletonised.
* \return The topological skeleton of the input image.
*/
cv::Mat skeletonizeMorph(cv::Mat *binaryImg)
{
	cv::Mat skel(binaryImg->size(), CV_8UC1, cv::Scalar(0));
	cv::Mat temp(binaryImg->size(), CV_8UC1);

	cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));

	bool done;
	do
	{
		cv::morphologyEx(*binaryImg, temp, cv::MORPH_OPEN, element);
		cv::bitwise_not(temp, temp);
		cv::bitwise_and(*binaryImg, temp, temp);
		cv::bitwise_or(skel, temp, skel);
		cv::erode(*binaryImg, *binaryImg, element);

		double max;
		cv::minMaxLoc(*binaryImg, 0, &max);
		done = (max == 0);
	} while (!done);

	return skel;
}


void thinningIteration(cv::Mat& img, int iter)
{
	CV_Assert(img.channels() == 1);
	CV_Assert(img.depth() != sizeof(uchar));
	CV_Assert(img.rows > 3 && img.cols > 3);

	cv::Mat marker = cv::Mat::zeros(img.size(), CV_8UC1);

	int nRows = img.rows;
	int nCols = img.cols;

	if (img.isContinuous()) {
		nCols *= nRows;
		nRows = 1;
	}

	int x, y;
	uchar *pAbove;
	uchar *pCurr;
	uchar *pBelow;
	uchar *nw, *no, *ne;    // north (pAbove)
	uchar *we, *me, *ea;
	uchar *sw, *so, *se;    // south (pBelow)

	uchar *pDst;

	// initialize row pointers
	pAbove = NULL;
	pCurr = img.ptr<uchar>(0);
	pBelow = img.ptr<uchar>(1);

	for (y = 1; y < img.rows - 1; ++y) {
		// shift the rows up by one
		pAbove = pCurr;
		pCurr = pBelow;
		pBelow = img.ptr<uchar>(y + 1);

		pDst = marker.ptr<uchar>(y);

		// initialize col pointers
		no = &(pAbove[0]);
		ne = &(pAbove[1]);
		me = &(pCurr[0]);
		ea = &(pCurr[1]);
		so = &(pBelow[0]);
		se = &(pBelow[1]);

		for (x = 1; x < img.cols - 1; ++x) {
			// shift col pointers left by one (scan left to right)
			nw = no;
			no = ne;
			ne = &(pAbove[x + 1]);
			we = me;
			me = ea;
			ea = &(pCurr[x + 1]);
			sw = so;
			so = se;
			se = &(pBelow[x + 1]);

			int A = (*no == 0 && *ne == 1) + (*ne == 0 && *ea == 1) +
				(*ea == 0 && *se == 1) + (*se == 0 && *so == 1) +
				(*so == 0 && *sw == 1) + (*sw == 0 && *we == 1) +
				(*we == 0 && *nw == 1) + (*nw == 0 && *no == 1);
			int B = *no + *ne + *ea + *se + *so + *sw + *we + *nw;
			int m1 = iter == 0 ? (*no * *ea * *so) : (*no * *ea * *we);
			int m2 = iter == 0 ? (*ea * *so * *we) : (*no * *so * *we);

			if (A == 1 && (B >= 2 && B <= 6) && m1 == 0 && m2 == 0)
				pDst[x] = 1;
		}
	}

	img &= ~marker;
}

/**
* Function for thinning the given binary image
*
* Parameters:
* 		src  The source image, binary with range = [0,255]
* 		dst  The destination image
*/
void thinning(const cv::Mat& src, cv::Mat& dst)
{
	dst = src.clone();
	dst /= 255;         // convert to binary image

	cv::Mat prev = cv::Mat::zeros(dst.size(), CV_8UC1);
	cv::Mat diff;

	do {
		thinningIteration(dst, 0);
		thinningIteration(dst, 1);
		cv::absdiff(dst, prev, diff);
		dst.copyTo(prev);
	} while (cv::countNonZero(diff) > 0);

	dst *= 255;
}


void onlyBackground(cv::Mat &src, cv::Mat &dst)
{
	dst = cv::Mat::ones(src.size(), CV_8U);
	cv::Mat checked = cv::Mat::zeros(src.size(), CV_8U);

	stack<cv::Point> next;
	next.push(cv::Point(0, 0));
	checked.at<uchar>(cv::Point(0, 0)) = 1;

	next.push(cv::Point(0, src.rows-1));
	checked.at<uchar>(cv::Point(0, src.rows - 1)) = 1;

	next.push(cv::Point(src.cols-1, 0));
	checked.at<uchar>(cv::Point(src.cols - 1, 0)) = 1;

	next.push(cv::Point(src.cols-1, src.rows-1));
	checked.at<uchar>(cv::Point(src.cols - 1, src.rows - 1)) = 1;
	

	while (!next.empty())
	{
		cv::Point curr = next.top();
		int x = curr.x;
		int y = curr.y;
		next.pop();
		dst.at<uchar>(curr) = 0;

		if (x != 0 && !checked.at<uchar>(cv::Point(x - 1, y)) && src.at<uchar>(cv::Point(x - 1, y)) == 0)
		{
			next.push(cv::Point(x - 1, y));
			checked.at<uchar>(cv::Point(x - 1, y)) = 1;
		}

		if (x != (src.size().width - 1) && !checked.at<uchar>(cv::Point(x + 1, y)) && src.at<uchar>(cv::Point(x + 1, y)) == 0)
		{
			next.push(cv::Point(x + 1, y));
			checked.at<uchar>(cv::Point(x + 1, y)) = 1;
		}

		if (y != 0 && !checked.at<uchar>(cv::Point(x, y - 1)) && src.at<uchar>(cv::Point(x, y - 1)) == 0)
		{
			next.push(cv::Point(x, y - 1));
			checked.at<uchar>(cv::Point(x, y - 1)) = 1;
		}
			
		if (y != (src.size().height - 1) && !checked.at<uchar>(cv::Point(x, y + 1)) && src.at<uchar>(cv::Point(x, y + 1)) == 0)
		{
			next.push(cv::Point(x, y + 1));
			checked.at<uchar>(cv::Point(x, y + 1)) = 1;
		}
			
	}
}

int maxHorizontalEdges(cv::Mat edgeImg, int minDist, int minHeight)
{
	int max[] = { 0,0,0,0,0,0,0,0,0,0 };

	for (int y = 0; y < edgeImg.rows; y++)
	{
		int edges = 0;
		int step = minDist;
		for (int x = 0; x < edgeImg.cols; x++)
		{
			uchar current = edgeImg.at<uchar>(cv::Point(x, y));
			if (++step >= minDist)
			{
				if (current)
				{
					edges++;
				}
			}
			
			if (current)
			{
				step = 0;
			}
		}

		switch (edges)
		{
			case 0:
				max[0]++;
				break;
			case 1:
				max[1]++;
				break;
			case 2:
				max[2]++;
				break;
			case 3:
				max[3]++;
				break;
			case 4:
				max[4]++;
				break;
			case 5:
				max[5]++;
				break;
			case 6:
				max[6]++;
				break;
			case 7:
				max[7]++;
				break;
			case 8:
				max[8]++;
				break;
			case 9:
				max[9]++;
				break;
			default:
				break;
		}
	}

	int biggest = 0;
	for (int i = 0; i < 10; i++)
	{
		if(max[i] >= minHeight)
			biggest = i;
	}
	return biggest;
}


void createGradiantHistogram(cv::Mat &src, cv::Mat &dst, int num)
{

	float maxVal = (float)(src.rows * src.cols)*255.;

	cv::Mat tmp = cv::Mat(num * 2, 1, CV_32F);
	for (int i = 0; i < num; i++)
	{
		cv::Mat gradX, gradY;
		double ang = ((double)i / (double)num) * 90.;

		rotateAndGradiant(src, gradX, gradY, ang);

		//cv::threshold()

		//gradX = preformCanny(gradX, 80, 180);
		//gradY = preformCanny(gradY, 80, 180);

		//cv::threshold(gradX, gradX, 100, 1, 0);// cv::THRESH_TRIANGLE);
		//cv::threshold(gradY, gradY, 100, 1, 0);//cv::THRESH_TRIANGLE);

		//gradX *= 255;
		//gradY *= 255;

		//cv::imshow("gradX", gradX);
		//cv::imshow("gradY", gradY);
		//cv::waitKey();

		tmp.at<float>(i, 0)       = (float)cv::sum(gradX)[0] / maxVal;
		tmp.at<float>(i + num, 0) = (float)cv::sum(gradY)[0] / maxVal;
	}
	tmp.copyTo(dst);
}

void rotateAndGradiant(cv::Mat &src, cv::Mat &dstX, cv::Mat &dstY, double ang)
{
	cv::Mat tmp0;
	cv::cvtColor(src, tmp0, CV_RGB2GRAY);
	//cv::equalizeHist(tmp0, tmp0);

	int xLen = src.cols;
	int yLen = src.rows;
	double angRad = ang*PI/180;
	int bx = xLen * cos(angRad) + yLen * sin(angRad);
	int by = xLen * sin(angRad) + yLen * cos(angRad);
	cv::Mat tmp1(cv::Size(bx, by), CV_8U, cv::Scalar(255, 255, 255));

	int deltaX = (bx - xLen) / 2;
	int deltaY = (by - yLen) / 2;
	tmp0.copyTo(tmp1(cv::Rect(deltaX, deltaY, xLen, yLen)));

	cv::Point2f pt(bx / 2., by / 2.);
	cv::Mat r = cv::getRotationMatrix2D(pt, ang, 1.0);

	cv::Mat tmp2;
	cv::warpAffine(tmp1, tmp2, r, cv::Size(bx, by), 1, 0, cv::Scalar(255, 255, 255));

	cv::Mat grad_x1, grad_y1;

	cv::Sobel(tmp2, grad_x1, CV_16S, 2, 0, 3);
	cv::Sobel(tmp2, grad_y1, CV_16S, 0, 2, 3);

	cv::convertScaleAbs(grad_x1, dstX);
	cv::convertScaleAbs(grad_y1, dstY);
}