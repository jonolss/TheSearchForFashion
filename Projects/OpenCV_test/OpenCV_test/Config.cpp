#include "Config.h"


void Config::readConfigFile(string path)
{
	ifstream inFile(path, ios::in);
	if (inFile.is_open())
	{
		string line;
		getline(inFile, line);
		MODEL_COLOR_EXTENTION = line.substr((line.find('=') + 1), line.length() - (line.find('=') + 1));
		getline(inFile, line);
		MODEL_CLTYPE_EXTENTION = line.substr((line.find('=') + 1), line.length() - (line.find('=') + 1));

		getline(inFile, line);
		TEST_FOLDER = line.substr((line.find('=') + 1), line.length() - (line.find('=') + 1));
		getline(inFile, line);
		SAVE_EXTENTION = line.substr((line.find('=') + 1), line.length() - (line.find('=') + 1));

		getline(inFile, line);
		line = line.substr((line.find('=') + 1), line.length() - (line.find('=') + 1));
		MAXIMUM_SEARCH_HITS = atoi(line.c_str());
		getline(inFile, line);
		line = line.substr((line.find('=') + 1), line.length() - (line.find('=') + 1));
		MAXIMUM_CLUSTER_COUNT = atoi(line.c_str());

		getline(inFile, line);
		line = line.substr((line.find('=') + 1), line.length() - (line.find('=') + 1));
		NUM_OF_GRAD_ANGS = atoi(line.c_str());
		getline(inFile, line);
		line = line.substr((line.find('=') + 1), line.length() - (line.find('=') + 1));
		EDGE_IMAGE_SIZE_XY = atoi(line.c_str());
		getline(inFile, line);
		line = line.substr((line.find('=') + 1), line.length() - (line.find('=') + 1));
		IMAGE_SIZE_XY = atoi(line.c_str());

		getline(inFile, line);
		line = line.substr((line.find('=') + 1), line.length() - (line.find('=') + 1));
		CANNY_THRESH_LOW = atof(line.c_str());
		getline(inFile, line);
		line = line.substr((line.find('=') + 1), line.length() - (line.find('=') + 1));
		CANNY_THRESH_HIGH = atof(line.c_str());

		inFile.close();
	}
	else
	{
		cout << "Couldn't open config-file." << endl;
	}
}


void Config::printConfig()
{
	cout << "MODEL_COLOR_EXTENTION " << MODEL_COLOR_EXTENTION << endl;
	cout << "MODEL_CLTYPE_EXTENTION " << MODEL_CLTYPE_EXTENTION << endl;
	cout << "TEST_FOLDER " << TEST_FOLDER << endl;
	cout << "SAVE_EXTENTION " << SAVE_EXTENTION << endl;
	cout << "MAXIMUM_SEARCH_HITS " << MAXIMUM_SEARCH_HITS << endl;
	cout << "MAXIMUM_CLUSTER_COUNT " << MAXIMUM_CLUSTER_COUNT << endl;
	cout << "EDGE_IMAGE_SIZE_XY " << EDGE_IMAGE_SIZE_XY << endl;
	cout << "IMAGE_SIZE_XY " << IMAGE_SIZE_XY << endl;
	cout << "CANNY_THRESH_LOW " << CANNY_THRESH_LOW << endl;
	cout << "CANNY_THRESH_HIGH " << CANNY_THRESH_HIGH << endl;
}