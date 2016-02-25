#include "ClothArticle.h"


/**Standard constructor.
*
*/
ImageFeatures::ImageFeatures()
{
	;
}

/**Constructor for making ImageFeatures.
* \param iamge The image who's features is going to be extracted.
*/
ImageFeatures::ImageFeatures(cv::Mat image, bool png)
{
	cv::Mat hsvImg;
	cv::cvtColor(image, hsvImg, cv::COLOR_BGR2HSV);

	for (int i = 0; i < 3; i++)
	{
		cv::Mat ch, hs;
		ch = getChannel(hsvImg, i);
		hs = getHsvHist(ch, i, 32);
		hsvHists[i] = normalizeHist(hs);
	}

	for (int i = 0; i < 3; i++)
	{
		cv::Mat ch, hs;
		ch = getChannel(image, i);
		hs = get8bitHist(ch, 32);
		rgbHists[i] = normalizeHist(hs);
	}

	cv::Mat imgGray;
	cv::cvtColor(image, imgGray, cv::COLOR_BGR2GRAY);
	
	cv::Mat binary;
	if (png)
	{
		cv::Mat *ch = (cv::Mat*)calloc(4, sizeof(cv::Mat));

		cv::split(image, ch);
		cv::Mat alpha = ch[3];
		cv::threshold(alpha, binary, 254, cv::THRESH_BINARY_INV, cv::THRESH_BINARY);

		for (int i = 0; i < 4; i++)
		{

			ch[i].release();
		}
		free(ch);
		binary = binary * 255;

		//imshow("Alpa", alpha);
		
	}
	else
	{
		cv::threshold(imgGray, binary, 248, cv::THRESH_BINARY_INV, cv::THRESH_BINARY);
		binary = binary * 255;
	}
	//imshow("Binär", binary);
	//waitKey(0);
	
	
	cv::Mat imgBlur = preformGaussianBlur(imgGray);
	cv::Mat edges = preformCanny(imgBlur);

	edgeHists[0] = createlocalEdgeImageHist(edges, 30);

	imgBlur = preformGaussianBlur(binary);
	edges = preformCanny(imgBlur);

	edgeHists[1] = createlocalEdgeImageHist(/*binary*/edges, 30);

	//fVec.at<float>(0, nhs.rows * j + k) = (float)nhs.at<float>(k, 0);
}

/**Standard desturctor.
*
*/
ImageFeatures::~ImageFeatures(){}

/**Returns the n'th rgb histogram of the article.
*
* \param n Channel of rgb histograms.
* \return The histogram of chosen rgb channel.
*/
cv::Mat ImageFeatures::getRGBHist(int ch) { return rgbHists[ch]; }
/**Returns the n'th hsv histogram of the article.
*
* \param n Channel of hsv histograms.
* \return The histogram of chosen hsv channel.
*/
cv::Mat ImageFeatures::getHSVHist(int ch) { return hsvHists[ch]; }
/**Returns the n'th edge histogram of the article.
*
* \param n Index of edge histograms.
* \return The chosen edge histogram.
*/
cv::Mat ImageFeatures::getEdgeHist(int n) { return edgeHists[n]; }

/**Constructor for making a partial ClothArticle.
*
* \param id Id of the article.
* \param path Path to the image of the article.
*/
ClothArticle::ClothArticle(string id, string path)
{
	this->id = id;
	this->path = path;
}

/**Constructor for making a ClothArticle.
*
* \param id Id of the article.
* \param path Path to the image of the article.
* \param color Color of the article.
* \param clType Clothing type of the article.
* \param sleeveType Sleeve type of the article.
*/
ClothArticle::ClothArticle(string id, string path, string color, string clType, int sleeveType)
{
	this->id = id;
	this->color = checkColor(color);
	this->clType = checkClType(clType);
	this->sleeveType = checkSleeveType(sleeveType);
	this->path = path;
	cv::Mat tmp = cv::imread(path, cv::IMREAD_UNCHANGED);
	bool png = path.find(".png") != string::npos;
	if (png)
		filterAlphaArtifacts(&tmp);
	cv::Mat tmp2 = resizeImg(tmp,300,300);

	this->imgFeats = ImageFeatures(tmp2,png);

}

/**Standard desturctor.
*
*/
ClothArticle::~ClothArticle() {}


/**Returns the id of the article.
*
* \return Article's id.
*/
string ClothArticle::getId() { return id; }

/**Returns the path to the article's image.
*
* \return Path to article's image.
*/
string ClothArticle::getPath() { return path; }

/**Returns the image of the article, by reading the image file. 
*
* \return The article's image.
*/
cv::Mat ClothArticle::getImage()
{ 
	cv::Mat tmp = cv::imread(path, cv::IMREAD_UNCHANGED);
	if (path.find(".png") != string::npos)
	{
		filterAlphaArtifacts(&tmp);
	}
	cv::Mat tmp2 = resizeImg(tmp,300,300);
	return tmp2;
}


/**Returns the article's type of color.
*
* \return The article's type of color.
*/
art_color ClothArticle::getColor()           { return color; }
/**Returns the article's type of clothing.
*
* \return The article's type of clothing.
*/
art_clType ClothArticle::getClType()         { return clType; }
/**Returns the article's type of sleve.
*
* \return The article's type of sleeve.
*/
art_sleeveType ClothArticle::getSleeveType() { return sleeveType; }

/**Sets the article's type of color.
*
* \param color Type of color that is being set.
*/
void ClothArticle::setColor(art_color color) { this->color = color; }
/**Sets the article's type of clothing.
*
* \param clType Type of clothing that is being set.
*/
void ClothArticle::setClType(art_clType clType) { this->clType = clType; }
/**Sets the article's type of sleeve.
*
* \param sleeveType Type of sleeve that is being set.
*/
void ClothArticle::setSleeveType(art_sleeveType sleeveType) { this->sleeveType = sleeveType; }

/**Returns the article's image features.
*
* \return ImageFeatures of the article.
*/
ImageFeatures ClothArticle::getImgFeats() { return imgFeats; }

/**Returns the article's classes.
*
* \return A vector with the classes of the article.
*/
vector<int> ClothArticle::getClasses()
{
	vector<int> output;
	output.push_back(color);
	output.push_back(clType);
	output.push_back(sleeveType);
	return output;
}


/**Convert string to correspondant enumaral.
*
* \param input Inputed string.
* \return The correspondant enumarale.
*/
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

/**Convert string to correspondant enumaral.
*
* \param input Inputed string.
* \return The correspondant enumarale.
*/
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
	if (input == "Chinos")
		return Chinos;
	if (input == "Jeans")
		return Jeans;
	if (input == "Leggings")
		return Leggings;
	if (input == "Shorts")
		return Shorts;
	if (input == "Singlet")
		return Singlet;
	if (input == "Skirt")
		return Skirt;
	if (input == "Tights")
		return Tights;
	if (input == "Trousers")
		return Trousers;

	return ClTypeError;
}


/**Convert numerical value to correspondant enumaral.
*
* \param input Numerical value.
* \return The correspondant enumarale.
*/
art_sleeveType checkSleeveType(int input)
{
	if (input == 0)
		return None;
	if (input == 1)
		return Short;
	if (input == 2)
		return Long;
	return NotKnown;
}

/**Convert enumaral value to string.
*
* \param val Enumaral value.
* \return A string object containing the representation of val as a sequence of characters.
*/
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
			return "Gra";
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
		case Khaki:
			return "Khaki";
		case Skin:
			return "Skin";
		default:
			return "Invalid color value";
	}
}
/**Convert enumaral value to string.
*
* \param val Enumaral value.
* \return A string object containing the representation of val as a sequence of characters.
*/
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
		case Chinos:
			return "Chinos";
		case Jeans:
			return "Jeans";
		case Leggings:
			return "Leggings";
		case Shorts:
			return "Shorts";
		case Singlet:
			return "Singlet";
		case Skirt:
			return "Skirt";
		case Tights:
			return "Tights";
		case Trousers:
			return "Trousers";
		defualt:
			return "Invalid clothing type";
	}
}
/**Convert enumaral value to string.
*
* \param val Enumaral value.
* \return A string object containing the representation of val as a sequence of characters.
*/
string to_string(art_sleeveType val)
{
	switch (val)
	{
		case NotKnown:
			return "NotKnown";
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

/**Reads and parses a cataloge file into a vector of ClothArticles.
*
* \param path Path to a cataloge file.
* \param partial When true makes ClothArticle with only id and path.
* \return A vector of all ClothArticle in the cataloge.
*/
vector<ClothArticle *> readCatalogeFromFile(string path, bool partial)
{
	ifstream inputFile(path, ios::in);

	vector<ClothArticle*> allArticles;

	string line;
	while (getline(inputFile, line))
	{
		allArticles.push_back(inputParser(line, partial));
	}
	inputFile.close();

	return allArticles;
}

/**Parse lines from a cataloge file into ClothArticles.
*
* \param input Line that is going to be parsed.
* \param partial When true makes ClothArticle with only id and path.
* \return Resulting ClothArticle using parsed data.
*/
ClothArticle *inputParser(string input, bool partial)
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

	if (partial)
	{
		return new ClothArticle(id, path);
	}
	return new ClothArticle(id, path, color, clType, sleeveType);
}