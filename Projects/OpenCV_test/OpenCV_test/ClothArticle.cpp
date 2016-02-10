#include "ClothArticle.h"

ClothArticle::ClothArticle(char inputType, string id, string path, string color, string clType, int sleeveType)
{
	this->inputType = inputType;
	this->id = id;
	this->color = checkColor(color);
	this->clType = checkClType(clType);
	this->sleeveType = checkSleeveType(sleeveType);
	Mat tmp = imread(path, IMREAD_UNCHANGED);
	if (path.find(".png") != string::npos)
	{
		filterAlphaArtifacts(&tmp);
	}
	image = resizeImg(tmp);
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

void ClothArticle::filterAlphaArtifacts(Mat *img)
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

ClothArticle::~ClothArticle() {}

void ClothArticle::showImage() { imshow("Article", image); }

char ClothArticle::getInputType() { return inputType; }

string ClothArticle::getId() { return id; }

Mat ClothArticle::getImage() { return image; }

art_color ClothArticle::getColor() { return color; }
art_clType ClothArticle::getClType() { return clType; }
art_sleeveType ClothArticle::getSleeveType() { return sleeveType; }

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

ClothArticle *inputParser(string input)
{

	//#V7;20;3;6;2#6567570./images/6567570.jpgVitBlouse-1
	string inp = "#V7;20;3;6;2#6567570./images/6567570.jpgVitBlouse-1";

	string::iterator it = input.begin();
	if (*it++ != '#')
	{
		cout << "Corrupt start of header.";
		return NULL;
	}
	//#|V7;20;3;6;2#6567570./images/6567570.jpgVitBlouse-1
	char inputType = *it++;
	//#V|7;20;3;6;2#6567570./images/6567570.jpgVitBlouse-1

	string buff = "";
	for (; *it != ';'; it++)
	{
		buff += *it;
	}
	int idLength = stoi(buff);
	it++;
	//#V7;|20;3;6;2#6567570./images/6567570.jpgVitBlouse-1

	buff = "";
	for (; *it != ';'; it++)
	{
		buff += *it;
	}
	int pathLength = stoi(buff);
	it++;
	//#V7;20;|3;6;2#6567570./images/6567570.jpgVitBlouse-1

	buff = "";
	for (; *it != ';'; it++)
	{
		buff += *it;
	}
	int colorLength = stoi(buff);
	it++;
	//#V7;20;3;|6;2#6567570./images/6567570.jpgVitBlouse-1

	buff = "";
	for (; *it != ';'; it++)
	{
		buff += *it;
	}
	int clTypeLength = stoi(buff);
	it++;
	//#V7;20;3;6;|2#6567570./images/6567570.jpgVitBlouse-1

	buff = "";
	for (; *it != '#'; it++)
	{
		buff += *it;
	}
	int sleeveTypeLength = stoi(buff);
	it++;
	//#V7;20;3;6;2#|6567570./images/6567570.jpgVitBlouse-1

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

	return new ClothArticle(inputType, id, path, color, clType, sleeveType);
}