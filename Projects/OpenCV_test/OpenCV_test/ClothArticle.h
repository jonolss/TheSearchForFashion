
#ifndef CLOTHARTICLE_H
#define CLOTHARTICLE_H

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>

#include "ImageUtilities.h"
#include "Config.h"



using namespace std;
//using namespace cv;

enum art_color
{
	Aqua, Corall, Lime, Metall,
	Beige, Bla, Blank, Brun, Gra,
	Gron, Gul, Lila, Orange, Rod,
	Rosa, Svart, Turkos, Vit,
	Khaki, Skin
};

enum art_clType
{
	Bangle, Dress, Jumpsuit, Playsuit,
	Sweatshirt, Tanktop, Tshirt,
	Blazer, Blouse, Camisole, Jacket,
	Kimono, Shirt, Top, Tunic,
	Chinos, Jeans, Leggings, Shorts,
	Singlet, Skirt, Tights, Trousers,
	ClTypeError
};

enum art_sleeveType
{
	NotKnown = -1,
	None = 0,
	Short = 1,
	Long = 2
};


class ImageFeatures
{
private:
	vector<cv::Mat> hsvHists;
	vector<cv::Mat> rgbHists;
	vector<cv::Mat> edgeVect;
	vector<cv::Mat> binVect;
	int maxHorizontal;
public:
	ImageFeatures();
	ImageFeatures(cv::Mat image, bool png);
	ImageFeatures::ImageFeatures(vector<cv::Mat> rgb, vector<cv::Mat> hsv, vector<cv::Mat> edge, vector<cv::Mat> bin);
	~ImageFeatures();

	vector<cv::Mat> getRGBHists();
	vector<cv::Mat> getHSVHists();
	vector<cv::Mat> getEdgeVect();
	vector<cv::Mat> getBinVect();

	int getMaxHorizontal();

	cv::Mat getRGBHist(int ch);
	cv::Mat getHSVHist(int ch);
	cv::Mat getEdgeVect(int n);
	cv::Mat getBinVect(int n);

};

class ClothArticle
{
private:
	string id;
	string path;
	art_color color;
	art_clType clType;
	art_sleeveType sleeveType;
	ImageFeatures *imgFeats;
	int clusterId;
	int clusterColor;
	int clusterClType;

	
public:
	ClothArticle(string id, string path, string color, string clType, int sleeveType);
	ClothArticle(string id, string path, art_color color, art_clType clType, art_sleeveType sleeveType, ImageFeatures *imgFeats);
	ClothArticle(string id, string path);
	~ClothArticle();

	void setColor(art_color color);
	void setClType(art_clType clType);
	void setSleeveType(art_sleeveType sleeveType);
	void setClusterId(int id);
	void setClusterColor(int id);
	void setClusterClType(int id);

	int            getClusterId();
	int            getClusterColor();
	int            getClusterClType();
	string         getId();
	string         getPath();
	cv::Mat        getImage();
	art_color      getColor();
	art_clType     getClType();
	art_sleeveType getSleeveType();
	ImageFeatures* getImgFeats();

	vector<int> getClasses();
};

art_color checkColor(string input);
art_clType checkClType(string input);
art_sleeveType checkSleeveType(int input);

string to_string(art_color val);
string to_string(art_clType val);
string to_string(art_sleeveType val);

vector<ClothArticle *> *readCatalogeFromFile(string path, bool partial);
ClothArticle *inputParser(string input, bool partial);

void saveCataloge(vector<ClothArticle*> *input, string path);
void saveCataloge(vector<ClothArticle*> *input, ofstream *outFile);

vector<ClothArticle*> *loadCataloge(string path);
vector<ClothArticle*> *loadCataloge(ifstream *inFile);

void saveClArticle(ClothArticle *input, string path);
void saveClArticle(ClothArticle *input, ofstream *outFile);

ClothArticle *loadClArticle(string path);
ClothArticle *loadClArticle(ifstream *inFile);


void saveImgFeats(ImageFeatures *input, ofstream *outFile);
ImageFeatures *loadImgFeats(ifstream *inFile);

#endif