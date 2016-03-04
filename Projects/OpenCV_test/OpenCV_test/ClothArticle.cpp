#include "ClothArticle.h"

/** Private declarations.
*
*/
void saveImgFeats(ImageFeatures *input, ofstream *outFile);
ImageFeatures *loadImgFeats(ifstream *inFile);

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
		hsvHists.push_back(normalizeHist(hs));
	}

	for (int i = 0; i < 3; i++)
	{
		cv::Mat ch, hs;
		ch = getChannel(image, i);
		hs = get8bitHist(ch, 32);
		rgbHists.push_back(normalizeHist(hs));
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
	cv::Mat edges = preformCanny(imgBlur, CANNY_THRESH_LOW, CANNY_THRESH_HIGH);
	cv::Mat edgesBlur = preformGaussianBlur(edges);


	edgeHists.push_back(createlocalEdgeImageHist(edgesBlur, IMAGE_SIZE_XY / EDGE_IMAGE_SIZE_XY));

	imgBlur = preformGaussianBlur(binary);
	edges = preformCanny(imgBlur, CANNY_THRESH_LOW, CANNY_THRESH_HIGH);
	edgesBlur = preformGaussianBlur(edges);

	edgeHists.push_back(createlocalEdgeImageHist(/*binary*/edgesBlur, IMAGE_SIZE_XY / EDGE_IMAGE_SIZE_XY));

	//fVec.at<float>(0, nhs.rows * j + k) = (float)nhs.at<float>(k, 0);
}

/**Constructor for making ImageFeatures.
*
* \param rgb Histogram over the images rgb channels.
* \param hsv Histogram over the images hsv channels.
* \param edge Histogram over the images edges.
*/
ImageFeatures::ImageFeatures(vector<cv::Mat> rgb, vector<cv::Mat> hsv, vector<cv::Mat> edge)
{
	this->rgbHists  = rgb;
	this->hsvHists  = hsv;
	this->edgeHists = edge;
}

/**Standard desturctor.
*
*/
ImageFeatures::~ImageFeatures()
{
	while (!hsvHists.empty())
	{
		delete hsvHists.back().data;
		hsvHists.back().release();
		hsvHists.pop_back();
	}
	hsvHists.clear();
	while (!rgbHists.empty())
	{
		delete rgbHists.back().data;
		rgbHists.back().release();
		rgbHists.pop_back();
	}
	rgbHists.clear();
	while (!edgeHists.empty())
	{
		delete edgeHists.back().data;
		edgeHists.back().release();
		edgeHists.pop_back();
	}
	edgeHists.clear();
}

/**Returns vector with the rgb histograms of the article.
*
* \return The histograms of the channels.
*/
vector<cv::Mat> ImageFeatures::getRGBHists()  { return rgbHists; }

/**Returns vector with the hsv histograms of the article.
*
* \return The histograms of the channels.
*/
vector<cv::Mat> ImageFeatures::getHSVHists()  { return hsvHists; }

/**Returns vector with the edge histograms of the article.
*
* \return The histograms of the edges.
*/
vector<cv::Mat> ImageFeatures::getEdgeHists() { return edgeHists; }

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
	this->id         = id;
	this->color      = checkColor(color);
	this->clType     = checkClType(clType);
	this->sleeveType = checkSleeveType(sleeveType);
	this->path       = path;
	cv::Mat tmp = cv::imread(path, cv::IMREAD_UNCHANGED);
	bool png = path.find(".png") != string::npos;
	if (png)
		filterAlphaArtifacts(&tmp);
	cv::Mat tmp2 = resizeImg(tmp, IMAGE_SIZE_XY, IMAGE_SIZE_XY);

	this->imgFeats = new ImageFeatures(tmp2,png);

}

/**Constructor for making a ClothArticle.
*
* \param id Id of the article.
* \param path Path to the image of the article.
* \param color Color of the article.
* \param clType Clothing type of the article.
* \param sleeveType Sleeve type of the article.
* \oaram imgFeats The ImageFeatures of the image.
*/
ClothArticle::ClothArticle(string id, string path, art_color color, art_clType clType, art_sleeveType sleeveType, ImageFeatures *imgFeats)
{
	this->id         = id;
	this->color      = color;
	this->clType     = clType;
	this->sleeveType = sleeveType;
	this->path       = path;
	this->imgFeats   = imgFeats;
}

/**Standard desturctor.
*
*/
ClothArticle::~ClothArticle() { delete imgFeats; }


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
	cv::Mat tmp2 = resizeImg(tmp, IMAGE_SIZE_XY, IMAGE_SIZE_XY);
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
ImageFeatures *ClothArticle::getImgFeats() { return imgFeats; }

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
vector<ClothArticle *> *readCatalogeFromFile(string path, bool partial)
{
	ifstream inputFile(path, ios::in);

	vector<ClothArticle*> *allArticles = new vector<ClothArticle*>();

	string line;
	while (getline(inputFile, line))
	{
		allArticles->push_back(inputParser(line, partial));
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

void saveCataloge(vector<ClothArticle*> *input, string path)
{
	ofstream outFile(path, ios::out | ios::binary | ios::ate);
	if (outFile.is_open())
	{
		saveCataloge(input, &outFile);
		outFile.close();
	}
	else
	{
		cout << "Couldn't open file." << endl;
	}
}
void saveCataloge(vector<ClothArticle*> *input, ofstream *outFile)
{
	char *data = (char*)calloc(1, sizeof(int));
	*(int*)data = input->size();
	outFile->write(data, sizeof(int));
	free(data);

	for (int i = 0; i < input->size(); i++)
	{
		saveClArticle(input->at(i), outFile);
	}
}

vector<ClothArticle*> *loadCataloge(string path)
{
	ifstream inFile(path, ios::in | ios::binary);
	if (inFile.is_open())
	{
		vector<ClothArticle*> *ret = loadCataloge(&inFile);
		inFile.close();
		return ret;
	}
	else
	{
		cout << "Couldn't open file." << endl;
		return NULL;
	}
}

vector<ClothArticle*> *loadCataloge(ifstream *inFile)
{
	char *data = (char*)calloc(1, sizeof(int));
	inFile->read(data, sizeof(int));
	int size = *(int*)data;
	free(data);

	vector<ClothArticle*> *ret = new vector<ClothArticle*>();
	for (int i = 0; i < size; i++)
	{
		ret->push_back(loadClArticle(inFile));
	}
	return ret;
}

void saveClArticle(ClothArticle *input, string path)
{
	ofstream outFile(path, ios::out | ios::binary | ios::ate);
	if (outFile.is_open())
	{
		saveClArticle(input, &outFile);
		outFile.close();
	}
	else
	{
		cout << "Couldn't open file." << endl;
	}
}

void saveClArticle(ClothArticle *input, ofstream *outFile)
{
	string id = input->getId();
	string path = input->getPath();
	art_color color = input->getColor();
	art_clType clType = input->getClType();
	art_sleeveType sleeveType = input->getSleeveType();
	ImageFeatures *imgFeats = input->getImgFeats();

	int idLength = id.length();
	int pathLength = path.length();


	int size = 2 * sizeof(int) + ((idLength + 1) + (pathLength + 1)) * sizeof(char);
	char *data = (char*)calloc(size, sizeof(char));

	*(int*)data = idLength;
	strcpy((data + sizeof(int)), id.c_str());
	*(int*)(data + sizeof(int) + (idLength + 1) * sizeof(char)) = pathLength;
	strcpy((data + 2 * sizeof(int) + (idLength + 1) * sizeof(char)), path.c_str());

	outFile->write(data, size);
	free(data);

	size = 3 * sizeof(int);
	data = (char*)calloc(size, sizeof(char));

	*(int*)data = color;
	*(int*)(data + sizeof(int)) = clType;
	*(int*)(data + 2 * sizeof(int)) = sleeveType;

	outFile->write(data, size);
	free(data);

	saveImgFeats(input->getImgFeats(), outFile);
}


ClothArticle *loadClArticle(string path)
{
	ifstream inFile(path, ios::in | ios::binary);
	if (inFile.is_open())
	{

		ClothArticle *ret = loadClArticle(&inFile);
		inFile.close();
		return ret;
	}
	else
	{
		cout << "Couldn't open file." << endl;
		return NULL;
	}
}

ClothArticle *loadClArticle(ifstream *inFile)
{
	string id;
	string path;
	art_color color;
	art_clType clType;
	art_sleeveType sleeveType;
	ImageFeatures *imgFeats;

	int idLength;
	int pathLength;


	int size = sizeof(int);
	char *data = (char*)calloc(size, sizeof(char));
	inFile->read(data, size);
	idLength = *(int*)data;
	free(data);

	size = idLength + 1;
	data = (char*)calloc(size, sizeof(char));
	inFile->read(data, size);
	id = string(data, size - 1);
	free(data);


	size = sizeof(int);
	data = (char*)calloc(size, sizeof(char));
	inFile->read(data, size);
	pathLength = *(int*)data;
	free(data);

	size = pathLength + 1;
	data = (char*)calloc(size, sizeof(char));
	inFile->read(data, size);
	path = string(data, size - 1);
	free(data);


	size = sizeof(int) * 3;
	data = (char*)calloc(size, sizeof(char));
	inFile->read(data, size);
	color = (art_color)*(int*)data;
	clType = (art_clType)*(int*)(data + sizeof(int));
	sleeveType = (art_sleeveType)*(int*)(data + 2 * sizeof(int));
	free(data);


	imgFeats = loadImgFeats(inFile);

	return new ClothArticle(id, path, color, clType, sleeveType, imgFeats);
}

/** Saves the ImageFeatures to an open file.
*
* \param input The ImageFeatures that is going to be saved.
* \param outFile An open outstream to a file.
*/
void saveImgFeats(ImageFeatures *input, ofstream *outFile)
{
	vector<cv::Mat> hists = input->getRGBHists();
	char *data = (char*)calloc(1, sizeof(int));
	*(int*)data = hists.size();
	outFile->write(data, sizeof(int));
	for (int i = 0; i < hists.size(); i++)
	{
		saveMat(hists[i], outFile);
	}
	

	hists = input->getHSVHists();
	*(int*)data = hists.size();
	outFile->write(data, sizeof(int));
	for (int i = 0; i < hists.size(); i++)
	{
		saveMat(hists[i], outFile);
	}

	hists = input->getEdgeHists();
	*(int*)data = hists.size();
	outFile->write(data, sizeof(int));
	for (int i = 0; i < hists.size(); i++)
	{
		saveMat(hists[i], outFile);
	}
	free(data);
}

/** Loads an ImageFeatures from an open file.
*
* \param inFile An open instream to a file.
*/
ImageFeatures *loadImgFeats(ifstream *inFile)
{
	int size;
	char *data = (char*)calloc(1, sizeof(int));
	inFile->read(data, sizeof(int));
	size = *(int*)data;
	vector<cv::Mat> rgb;
	for (int i = 0; i < size; i++)
	{
		rgb.push_back(loadMat(inFile));
	}

	inFile->read(data, sizeof(int));
	size = *(int*)data;
	vector<cv::Mat> hsv;
	for (int i = 0; i < size; i++)
	{
		hsv.push_back(loadMat(inFile));
	}

	inFile->read(data, sizeof(int));
	size = *(int*)data;
	vector<cv::Mat> edge;
	for (int i = 0; i < size; i++)
	{
		edge.push_back(loadMat(inFile));
	}
	free(data);

	ImageFeatures *res = new ImageFeatures(rgb, hsv, edge);

	
	while (!hsv.empty())
	{
		hsv.back().release();
		hsv.pop_back();
	}
	hsv.clear();
	while (!rgb.empty())
	{
		rgb.back().release();
		rgb.pop_back();
	}
	rgb.clear();
	while (!edge.empty())
	{
		edge.back().release();
		edge.pop_back();
	}
	edge.clear();
	
	return res;
}