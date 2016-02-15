
#ifndef CLOTHARTICLE_H
#define CLOTHARTICLE_H

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>

#include "ImageUtilities.h"



using namespace std;
using namespace cv;

enum art_color
{
	Aqua, Corall, Lime, Metall,
	Beige, Bla, Blank, Brun, Gra,
	Gron, Gul, Lila, Orange, Rod,
	Rosa, Svart, Turkos, Vit
};

enum art_clType
{
	Bangle, Dress, Jumpsuit, Playsuit,
	Sweatshirt, Tanktop, Tshirt,
	Blazer, Blouse, Camisole, Jacket,
	Kimono, Shirt, Top, Tunic
};

enum art_sleeveType
{
	Unknown = -1,
	None = 0,
	Short = 1,
	Long = 2
};


class ImageFeatures
{
private:
	Mat hsvHists[3];
	Mat rgbHists[3];
	Mat edgeHists[2];
public:
	ImageFeatures();
	ImageFeatures(Mat image);
	~ImageFeatures();

	Mat getRGBHist(int ch);
	Mat getHSVHist(int ch);
	Mat getEdgeHist(int n);


};

class ClothArticle
{
private:
	string path;
	ImageFeatures imgFeats;
	string id;
	art_color color;
	art_clType clType;
	art_sleeveType sleeveType;

	Mat resizeImg(Mat input);
public:
	ClothArticle(string id, string path, string color, string clType, int sleeveType);
	~ClothArticle();

	void setColor(art_color color);
	void setClType(art_clType clType);
	void setSleeveType(art_sleeveType sleeveType);

	string         getId();
	Mat            getImage();
	art_color      getColor();
	art_clType     getClType();
	art_sleeveType getSleeveType();
	ImageFeatures  getImgFeats();

	vector<int> getClasses();
};

art_color checkColor(string input);
art_clType checkClType(string input);
art_sleeveType checkSleeveType(int input);

string to_string(art_color val);
string to_string(art_clType val);
string to_string(art_sleeveType val);

vector<ClothArticle *> readCatalogeFromFile(string path);
ClothArticle *inputParser(string input);

#endif