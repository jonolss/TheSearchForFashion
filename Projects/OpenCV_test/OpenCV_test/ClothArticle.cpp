#include "ClothArticle.h"



ImageFeatures::ImageFeatures()
{
	;
}

ImageFeatures::ImageFeatures(Mat image)
{
	Mat hsvImg;
	cvtColor(image, hsvImg, COLOR_BGR2HSV);

	for (int i = 0; i < 3; i++)
	{
		Mat ch, hs;
		ch = getChannel(hsvImg, i);
		hs = getHsvHist(ch, i, 32);
		hsvHists[i] = normalizeHist(hs);
	}

	for (int i = 0; i < 3; i++)
	{
		Mat ch, hs;
		ch = getChannel(image, i);
		hs = get8bitHist(ch, 32);
		rgbHists[i] = normalizeHist(hs);
	}

	Mat imgGray;
	cvtColor(image, imgGray, COLOR_BGR2GRAY);
	
	Mat binary;
	threshold(imgGray, binary, 248, THRESH_BINARY_INV, THRESH_BINARY);
	binary = binary * 255;

	Mat imgBlur = preformGaussianBlur(imgGray);
	Mat edges = preformCanny(imgBlur);

	edgeHists[0] = createlocalEdgeImageHist(edges, 30);

	imgBlur = preformGaussianBlur(binary);
	edges = preformCanny(imgBlur);

	edgeHists[1] = createlocalEdgeImageHist(/*binary*/edges, 30);

	//fVec.at<float>(0, nhs.rows * j + k) = (float)nhs.at<float>(k, 0);
}

ImageFeatures::~ImageFeatures(){}

Mat ImageFeatures::getRGBHist(int ch) { return rgbHists[ch]; }
Mat ImageFeatures::getHSVHist(int ch) { return hsvHists[ch]; }
Mat ImageFeatures::getEdgeHist(int n) { return edgeHists[n]; }


ClothArticle::ClothArticle(string id, string path, string color, string clType, int sleeveType)
{
	this->id = id;
	this->color = checkColor(color);
	this->clType = checkClType(clType);
	this->sleeveType = checkSleeveType(sleeveType);
	this->path = path;
	Mat tmp = imread(path, IMREAD_UNCHANGED);
	if (path.find(".png") != string::npos)
	{
		filterAlphaArtifacts(&tmp);
	}
	Mat tmp2 = resizeImg(tmp);

	this->imgFeats = ImageFeatures(tmp2);

}

Mat ClothArticle::resizeImg(Mat input)
{
	Mat out(Size(300, 300), CV_8UC3);

	int inpHt = input.size().height;
	int inpWh = input.size().width;
	if (inpHt > 300 || inpWh > 300)
	{
		resize(input, out, Size(300, 300), 0.0, 0.0, INTER_AREA);
	}
	else if (inpHt < 300 || inpWh < 300)
	{
		resize(input, out, Size(300, 300), 0.0, 0.0, INTER_LINEAR);
	}

	return out;
}

ClothArticle::~ClothArticle() {}

string ClothArticle::getId() { return id; }

Mat ClothArticle::getImage() 
{ 
	Mat tmp = imread(path, IMREAD_UNCHANGED);
	if (path.find(".png") != string::npos)
	{
		filterAlphaArtifacts(&tmp);
	}
	Mat tmp2 = resizeImg(tmp);
	return tmp2;
}

art_color ClothArticle::getColor()           { return color; }
art_clType ClothArticle::getClType()         { return clType; }
art_sleeveType ClothArticle::getSleeveType() { return sleeveType; }

void ClothArticle::setColor(art_color color) { this->color = color; }
void ClothArticle::setClType(art_clType clType) { this->clType = clType; }
void ClothArticle::setSleeveType(art_sleeveType sleeveType) { this->sleeveType = sleeveType; }

ImageFeatures ClothArticle::getImgFeats() { return imgFeats; }

vector<int> ClothArticle::getClasses()
{
	vector<int> output;
	output.push_back(color);
	output.push_back(clType);
	output.push_back(sleeveType);
	return output;
}


art_color checkColor(string input)
{
	
	if (input == "Aqua")
		return Aqua;
	if (input == "Corall")
		return Corall;
	if (input == "Lime")
		return Lime;
	if (input == "Metall")
		return Metall;
	if (input == "Beige")
		return Beige;
	if (input == "Bla")
		return Bla;
	if (input == "Blank")
		return Blank;
	if (input == "Brun")
		return Brun;
	if (input == "Gra")
		return Gra;
	if (input == "Gron")
		return Gron;
	if (input == "Gul")
		return Gul;
	if (input == "Lila")
		return Lila;
	if (input == "Orange")
		return Orange;
	if (input == "Rod")
		return Rod;
	if (input == "Rosa")
		return Rosa;
	if (input == "Svart")
		return Svart;
	if (input == "Turkos")
		return Turkos;
	if (input == "Vit")
		return Vit;
	return Vit;
}

art_clType checkClType(string input)
{

	if (input == "Bangle")
		return Bangle;
	if (input == "Dress")
		return Dress;
	if (input == "Jumpsuit")
		return Jumpsuit;
	if (input == "Playsuit")
		return Playsuit;
	if (input == "Sweatshirt")
		return Sweatshirt;
	if (input == "Tanktop")
		return Tanktop;
	if (input == "Tshirt")
		return Tshirt;
	if (input == "Blazer")
		return Blazer;
	if (input == "Blouse")
		return Blouse;
	if (input == "Camisole")
		return Camisole;
	if (input == "Jacket")
		return Jacket;
	if (input == "Kimono")
		return Kimono;
	if (input == "Shirt")
		return Shirt;
	if (input == "Top")
		return Top;
	if (input == "Tunic")
		return Tunic;
	return Tunic;
}

art_sleeveType checkSleeveType(int input)
{
	if (input == 0)
		return None;
	if (input == 1)
		return Short;
	if (input == 2)
		return Long;
	return Unknown;
}

string to_string(art_color val)
{
	switch (val)
	{
		case Aqua:
			return "Aqua";
		case Corall:
			return "Corall";
		case Lime:
			return "Lime";
		case Metall:
			return "Metall";
		case Beige:
			return "Beige";
		case Bla:
			return "Bla";
		case Blank:
			return "Blank";
		case Brun:
			return "Brun";
		case Gra:
			return"Gra";
		case Gron:
			return "Gron";
		case Gul:
			return "Gul";
		case Lila:
			return "Lila";
		case Orange:
			return "Orange";
		case Rod:
			return "Rod";
		case Rosa:
			return "Rosa";
		case Svart:
			return "Svart";
		case Turkos:
			return "Turkos";
		case Vit:
			return "Vit";
		default:
			return "Invalid color value";
	}
}
string to_string(art_clType val)
{
	switch (val)
	{
		case Bangle:
			return "Bangle";
		case Dress:
			return "Dress";
		case Jumpsuit:
			return "Jumpsuit";
		case Playsuit:
			return "Playsuit";
		case Sweatshirt:
			return "Sweatshirt";
		case Tanktop:
			return "Tanktop";
		case Tshirt:
			return "Tshirt";
		case Blazer:
			return "Blazer";
		case Blouse:
			return "Blouse";
		case Camisole:
			return "Camisole";
		case Jacket:
			return "Jacket";
		case Kimono:
			return "Kimono";
		case Shirt:
			return "Shirt";
		case Top:
			return "Top";
		case Tunic:
			return "Tunic";
		defualt:
			return "Invalid clothing type";
	}
}
string to_string(art_sleeveType val)
{
	switch (val)
	{
		case Unknown:
			return "Unknown";
		case None:
			return "None";
		case Short:
			return "Short";
		case Long:
			return "Long";
		default:
			return "Invalid sleeve type";
	}
}

vector<ClothArticle *> readCatalogeFromFile(string path)
{
	ifstream inputFile(path, ios::in);

	vector<ClothArticle*> allArticles;

	string line;
	while (getline(inputFile, line))
	{
		allArticles.push_back(inputParser(line));
	}
	inputFile.close();

	return allArticles;
}

ClothArticle *inputParser(string input)
{

	//#7;20;3;6;2#6567570./images/6567570.jpgVitBlouse-1
	string inp = "#V7;20;3;6;2#6567570./images/6567570.jpgVitBlouse-1";

	string::iterator it = input.begin();
	if (*it++ != '#')
	{
		cout << "Corrupt start of header.";
		return NULL;
	}
	//#|7;20;3;6;2#6567570./images/6567570.jpgVitBlouse-1

	string buff = "";
	for (; *it != ';'; it++)
	{
		buff += *it;
	}
	int idLength = stoi(buff);
	it++;
	//#7;|20;3;6;2#6567570./images/6567570.jpgVitBlouse-1

	buff = "";
	for (; *it != ';'; it++)
	{
		buff += *it;
	}
	int pathLength = stoi(buff);
	it++;
	//#7;20;|3;6;2#6567570./images/6567570.jpgVitBlouse-1

	buff = "";
	for (; *it != ';'; it++)
	{
		buff += *it;
	}
	int colorLength = stoi(buff);
	it++;
	//#7;20;3;|6;2#6567570./images/6567570.jpgVitBlouse-1

	buff = "";
	for (; *it != ';'; it++)
	{
		buff += *it;
	}
	int clTypeLength = stoi(buff);
	it++;
	//#7;20;3;6;|2#6567570./images/6567570.jpgVitBlouse-1

	buff = "";
	for (; *it != '#'; it++)
	{
		buff += *it;
	}
	int sleeveTypeLength = stoi(buff);
	it++;
	//#7;20;3;6;2#|6567570./images/6567570.jpgVitBlouse-1

	buff = "";
	for (int i = 0; i < idLength; i++, it++)
	{
		buff += *it;
	}
	string id = buff;

	buff = "";
	for (int i = 0; i < pathLength; i++, it++)
	{
		buff += *it;
	}
	string path = buff;

	buff = "";
	for (int i = 0; i < colorLength; i++, it++)
	{
		buff += *it;
	}
	string color = buff;

	buff = "";
	for (int i = 0; i < clTypeLength; i++, it++)
	{
		buff += *it;
	}
	string clType = buff;

	buff = "";
	for (int i = 0; i < sleeveTypeLength; i++, it++)
	{
		buff += *it;
	}
	int sleeveType = stoi(buff);

	return new ClothArticle(id, path, color, clType, sleeveType);
}