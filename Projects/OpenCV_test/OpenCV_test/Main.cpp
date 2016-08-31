#include <iostream>
#include <fstream>

#include <shobjidl.h>


#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/core.hpp>

#include "Config.h"
#include "ClothArticle.h"
#include "ImageUtilities.h"
#include "ImageSearcher.h"

#define CONFIG_PATH "TSFS.conf"

using namespace std;

int main(int argc, char* argv[])
{
	Config::get().readConfigFile(CONFIG_PATH);

	webBackend("readyFile2.xx");

	return 0;
}