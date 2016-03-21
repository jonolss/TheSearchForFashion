#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <Windows.h>

using namespace std;

#define BACK_TO_FRONT_SLOT TEXT("\\\\.\\mailslot\\btfs_mailslot")
#define FRONT_TO_BACK_SLOT TEXT("\\\\.\\mailslot\\ftbs_mailslot")
#define EDGE_FEATURE_SIZE (Config::get().EDGE_IMAGE_SIZE_XY * Config::get().EDGE_IMAGE_SIZE_XY)
#define PI 3.141592

class Config
{

public:
	static Config& get()
	{
		static Config instance;
		return instance;
	}

	string MODEL_COLOR_EXTENTION;// = ".ColorModel.xml";
	string MODEL_CLTYPE_EXTENTION;// = ".ClTypeModel.xml";

	string TEST_FOLDER;// = "./testfiles/";
	string SAVE_EXTENTION;// = ".sv";

	int MAXIMUM_SEARCH_HITS;// = 20;
	int MAXIMUM_CLUSTER_COUNT;// = 15;

	int NUM_OF_GRAD_ANGS;// = 8;
	int EDGE_IMAGE_SIZE_XY;// = 50; //Needs to be divider of IMAGE_SIZE_XY
	int IMAGE_SIZE_XY;// = 300;
	double CANNY_THRESH_LOW;// = 80.0;
	double CANNY_THRESH_HIGH;// = 140.0;

	void readConfigFile(string path);
	void printConfig();

private:
	Config() {};
	Config(const Config&);

};


#endif