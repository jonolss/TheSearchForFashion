#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <string>
#include <fstream>
#include <Windows.h>

using namespace std;

#define BACK_TO_FRONT_SLOT TEXT("\\\\.\\mailslot\\btfs_mailslot")
#define FRONT_TO_BACK_SLOT TEXT("\\\\.\\mailslot\\ftbs_mailslot")

static string MODEL_COLOR_EXTENTION  = ".ColorModel.xml";
static string MODEL_CLTYPE_EXTENTION = ".ClTypeModel.xml";

static string TEST_FOLDER = "./testfiles/";
static string SAVE_EXTENTION = ".sv";

static int MAXIMUM_SEARCH_HITS = 20;

#define EDGE_FEATURE_SIZE (EDGE_IMAGE_SIZE_XY * EDGE_IMAGE_SIZE_XY)

static int EDGE_IMAGE_SIZE_XY = 50; //Needs to be divider of IMAGE_SIZE_XY
static int IMAGE_SIZE_XY = 300;
static double CANNY_THRESH_LOW = 80.0;
static double CANNY_THRESH_HIGH = 140.0;

void readConfigFile(string path);
void printConfig();

#endif