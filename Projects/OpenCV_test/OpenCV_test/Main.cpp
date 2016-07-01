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



void svmANDrfTest(string filename, string testType);
void testModelWithImage(string trainingFilename, string testFilename, string testType, bool loadModel = false);



DWORD WINAPI ThreadProc();
HANDLE hPipe1, hPipe2;
BOOL Finished;

unsigned long __stdcall NET_RvThr(void * pParam) {
	BOOL fSuccess;
	char chBuf[100];
	DWORD dwBytesToWrite = (DWORD)strlen(chBuf);
	DWORD cbRead = 100;
	int i;

	while (1)
	{
		fSuccess = ReadFile(hPipe2, chBuf, dwBytesToWrite, &cbRead, NULL);
		if (fSuccess)
		{
			printf("C++ App: Received %d Bytes : ", cbRead);
			for (i = 0; i<cbRead; i++)
				printf("%c", chBuf[i]);
			printf("\n");
		}
		if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
		{
			printf("Can't Read\n");
			if (Finished)
				break;
		}
	}
	return 0;
}


#define BUFFER_SIZE 1000

DWORD WINAPI test_thread( LPVOID lpParam)
{
	cout << "Hi, friend." << endl;

	char buf[BUFFER_SIZE] = { 't','o','r','s','k','\0' };

	LPTSTR lpszPipename1 = TEXT("\\\\.\\pipe\\myNamedPipe1");
	LPTSTR lpszPipename2 = TEXT("\\\\.\\pipe\\myNamedPipe2");

	DWORD cbWritten;
	DWORD dwBytesToWrite = (DWORD)strlen(buf);


	HANDLE sem = OpenSemaphore(SEMAPHORE_MODIFY_STATE, NULL, _TEXT("sem"));
	if (sem == NULL)
	{
		printf("CreateSemaphore error: %d\n", GetLastError());
		return -1;
	}

	

	HANDLE hPipe1 = CreateFile(lpszPipename1, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

	if ((hPipe1 == NULL || hPipe1 == INVALID_HANDLE_VALUE))
	{
		printf("Could not open the pipe  - (error %d)\n", GetLastError());

	}
	else
	{
		WriteFile(hPipe1, buf, dwBytesToWrite, &cbWritten, NULL);
		**(int**)lpParam = 1;
	}

	ReleaseSemaphore(sem, 1, NULL);

	return 0;
}



int main(int argc, char* argv[])
{
	Config::get().readConfigFile(CONFIG_PATH);

	/*
	HANDLE outFile = CreateFile(
		_TEXT("D:\\tsff_front2back"), //_TEXT("D:\\test.txt"),//_TEXT("\\.\\tsff_in"),
		GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_DELETE,
		NULL,
		CREATE_ALWAYS,//OPEN_ALWAYS,
		FILE_ATTRIBUTE_HIDDEN | FILE_FLAG_DELETE_ON_CLOSE,
		NULL);

	if (outFile == NULL)
	{
		cout << "Couldn't open file {error: " << GetLastError() << "}." << endl;
	}


	DWORD cbWritten;
	char outBuf[BUFFER_SIZE];

	memset(outBuf, 0, BUFFER_SIZE);
	string msg = "imgSearch\ntestfiles/dress0.jpg\n12\nClothingType,Silhouette,Pattern,Color,Template,\n2,2,1,1,1\nNone\t";
	strcpy(outBuf, msg.c_str());

	for (int i = 0; i < 1; i++)
	{
		;// WriteFile(outFile, outBuf, msg.length(), &cbWritten, NULL);
	}
	memset(outBuf, 0, BUFFER_SIZE);

	CloseHandle(outFile);

	HANDLE readThread;
	DWORD readThreadId;
	readThread = CreateThread(
		NULL,
		0,
		readingFromFileV2,
		NULL,
		0,
		&readThreadId
		);
	*/

	webBackend("readyFile2.xx");

#define CSTEST
#ifdef CSTEST




	//Pipe Init Data
	char buf[BUFFER_SIZE];

	LPTSTR lpszPipename1 = TEXT("\\\\.\\pipe\\myNamedPipe1");
	LPTSTR lpszPipename2 = TEXT("\\\\.\\pipe\\myNamedPipe2");

	//DWORD cbWritten;
	DWORD dwBytesToWrite = (DWORD)strlen(buf);

	DWORD cbRead;
	DWORD dwBytesToRead = BUFFER_SIZE;

	//Thread Init Data
	DWORD threadId;
	HANDLE hThread = NULL;

	BOOL Write_St = TRUE;

	Finished = FALSE;




	hPipe1 = CreateNamedPipe(lpszPipename1, PIPE_ACCESS_DUPLEX/* | FILE_FLAG_OVERLAPPED*/, PIPE_TYPE_MESSAGE, 1, BUFFER_SIZE, BUFFER_SIZE, 0, NULL);
	hPipe2 = CreateNamedPipe(lpszPipename2, PIPE_ACCESS_DUPLEX/* | FILE_FLAG_OVERLAPPED*/, PIPE_TYPE_MESSAGE, 1, BUFFER_SIZE, BUFFER_SIZE, 0, NULL);

	HANDLE sem = CreateSemaphore(NULL, 0, 1, _TEXT("Global\\sem_test"));

	

	//hPipe1 = CreateFile(lpszPipename1, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	//hPipe2 = CreateFile(lpszPipename2, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);


	if ((hPipe1 == NULL || hPipe1 == INVALID_HANDLE_VALUE) || (hPipe2 == NULL || hPipe2 == INVALID_HANDLE_VALUE))
	{
		printf("Could not open the pipe  - (error {0})\n", GetLastError());

	}
	else if (sem == NULL)
	{
		printf("CreateSemaphore error: %d\n", GetLastError());
	}
	else if(false)
	{

		//hThread = CreateThread(NULL, 0, &NET_RvThr, NULL, 0, NULL);
		do
		{
			printf("Enter your message: ");
			//cin.getline(buf, 100);
			//cin.clear();
			scanf("%s", buf);
			//cin.ignore(INT_MAX);
			//cin >> buf;
			string str = string(buf);
			cout << "STRING: " << str << endl;
			//scanf("%s", buf); //cin >> buf;
			if (strcmp(buf, "quit") == 0)
				Write_St = FALSE;
			else
			{
				//WriteFile(hPipe1, buf, dwBytesToWrite, &cbWritten, NULL);
				memset(buf, 0xCC, 100);
				
			}
			

		} while (Write_St);

		CloseHandle(hPipe1);
		CloseHandle(hPipe2);
		Finished = TRUE;
	}
	else
	{
		if (true)
		{
			while(true)
			{
				string stop;
				/*
				cout << "Waiting for client sema..." << endl;
				WaitForSingleObject(sem, INFINITE);
				cout << "Releasing sema" << endl;
				ReleaseSemaphore(sem, 1, NULL);
				cout << "Sema done." << endl;
				*/

				cout << "Waiting for client..." << endl;
				ConnectNamedPipe(hPipe1, NULL);
				ReleaseSemaphore(sem, 1, NULL);
				ConnectNamedPipe(hPipe2, NULL);
				cout << "Pipes connected." << endl;

				//bool loop = true;
				//while (loop)
				//{
					ReadFile(hPipe1, buf, dwBytesToRead, &cbRead, NULL);
					cout << dwBytesToRead << endl;
					cout << cbRead << endl;

					//if (buf[0] == 'q' && buf[1] == 'u' && buf[2] == 'i' && buf[3] == 't' && buf[3] == '\0')
					//{
					//	loop = false;
					//}
					//else
					//{
						//int sum = 0;
						//for (int i = 0; i < BUFFER_SIZE; i++)
						//{
						//	cout << buf[i];
						//	sum += buf[i];
						//}
						//cout << endl;
						//memset(buf, 0xCC, BUFFER_SIZE);
						//_itoa(sum, buf, 10);
						//cout << buf << endl;
						//cin >> stop;
						
					string tmp = string(buf);
					cout << tmp << endl;
						cout << buf << endl;
						//memset(buf, 97, BUFFER_SIZE);
						dwBytesToWrite = (DWORD)strlen(buf);
						//WriteFile(hPipe2, buf, dwBytesToWrite, &cbWritten, NULL);
						memset(buf, 0, BUFFER_SIZE);
					//}
				//}
				DisconnectNamedPipe(hPipe1);
				DisconnectNamedPipe(hPipe2);
				cout << "Pipes disconnected." << endl;
			}
		}
		else
		{
			cout << "Wait failed" << endl;
		}

	}

	CloseHandle(hPipe1);
	CloseHandle(hPipe2);
	CloseHandle(sem);

	getchar();

	return 0;
#endif


	
#ifdef _DEBUG
	Config::get().printConfig();
#endif
	if (argc == 3)
	{
		cout << argv[2] << endl;
	}

	if (argc == 1)
	{
		cout << "Too few arguments, use -h for help.";
	}
	else if (string(argv[1]) == "--com")
	{



		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		

		if (SUCCEEDED(hr))
		{
			IFileOpenDialog *pFileOpen;

			// Create the FileOpenDialog object.
			hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
				IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));


			if (SUCCEEDED(hr))
			{
				// Show the Open dialog box.
				hr = pFileOpen->Show(NULL);

				// Get the file name from the dialog box.
				if (SUCCEEDED(hr))
				{
					IShellItem *pItem;
					hr = pFileOpen->GetResult(&pItem);
					if (SUCCEEDED(hr))
					{
						PWSTR pszFilePath;
						hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

						// Display the file name to the user.
						if (SUCCEEDED(hr))
						{
							MessageBox(NULL, pszFilePath, L"File Path", MB_OK);
							CoTaskMemFree(pszFilePath);
						}
						pItem->Release();
					}
				}
				pFileOpen->Release();
			}
			CoUninitialize();
		}
		return 0;


	}
	else if (string(argv[1]) == "--padding")
	{
		cv::Mat src0 = cv::imread("testfiles/shirt0.jpg", cv::IMREAD_UNCHANGED);

		cv::Mat image0 = resizeImg(src0, 300, 300);

		cv::Mat imgGray0;
		cv::cvtColor(image0, imgGray0, cv::COLOR_BGR2GRAY);

		cv::Mat binary0;
		cv::threshold(imgGray0, binary0, 248, cv::THRESH_BINARY_INV, cv::THRESH_BINARY_INV);
		binary0 = binary0 * 255;

		cv::Mat paddedImage;
		
		fixInternalPadding(image0, paddedImage);
		
		cv::imshow("paddedImage", paddedImage);

		cv::waitKey(0);
	}
	else if (string(argv[1]) == "--matching")
	{
		
		//double performTemplate

		cv::Mat src0 = cv::imread("testfiles/shirt0.jpg",cv::IMREAD_UNCHANGED);
		cv::Mat src1 = cv::imread("testfiles/dress2.jpg", cv::IMREAD_UNCHANGED);

		cv::Mat image0 = resizeImg(src0,300,300);
		cv::Mat image1 = resizeImg(src1,300,300);

		cv::Mat imgGray0;
		cv::cvtColor(image0, imgGray0, cv::COLOR_BGR2GRAY);
		cv::Mat imgGray1;
		cv::cvtColor(image1, imgGray1, cv::COLOR_BGR2GRAY);

		cv::Mat binary0;
		cv::threshold(imgGray0, binary0, 248, cv::THRESH_BINARY_INV, cv::THRESH_BINARY_INV);
		binary0 = binary0 * 255;
		cv::Mat binary1;
		cv::threshold(imgGray1, binary1, 248, cv::THRESH_BINARY_INV, cv::THRESH_BINARY_INV);
		binary1 = binary1 * 255;

		cv::Mat imgBlur0 = preformGaussianBlur(binary0);
		cv::Mat edges0 = preformCanny(imgBlur0, Config::get().CANNY_THRESH_LOW, Config::get().CANNY_THRESH_HIGH);
		cv::Mat edgesBlur0 = preformGaussianBlur(edges0);
		cv::Mat imgBlur1 = preformGaussianBlur(binary1);
		cv::Mat edges1 = preformCanny(imgBlur1, Config::get().CANNY_THRESH_LOW, Config::get().CANNY_THRESH_HIGH);
		cv::Mat edgesBlur1 = preformGaussianBlur(edges1);

		cv::Mat out0;
		onlyBackground(binary0, out0);
		out0 *= 255;
		cv::Mat out1;
		onlyBackground(binary1, out1);
		out1 *= 255;

		out0 = preformGaussianBlur(out0);
		edges0 = preformCanny(out0, Config::get().CANNY_THRESH_LOW, Config::get().CANNY_THRESH_HIGH);
		out1 = preformGaussianBlur(out1);
		edges1 = preformCanny(out1, Config::get().CANNY_THRESH_LOW, Config::get().CANNY_THRESH_HIGH);

		cv::Mat kov0 = resizeImg(edges0, Config::get().EDGE_IMAGE_SIZE_XY, Config::get().EDGE_IMAGE_SIZE_XY);
		cv::Mat kov1 = resizeImg(edges1, Config::get().EDGE_IMAGE_SIZE_XY, Config::get().EDGE_IMAGE_SIZE_XY);

		pair<double, double> outMax = performTemplateMatching(kov0, kov1);

		cout << "Max: " << outMax.first << endl;
		cout << "Tot: " << outMax.second << endl;
	}
	else if (string(argv[1]) == "-f")
	{
		frontend("readyFile2.xx");
	}
	else if (string(argv[1]) == "--window")
	{
		cout << guiFrontend("readyFile2.xx");
	}
	else if (string(argv[1]) == "-b")
	{
		if (argc == 2)
		{
			backend("readyFile2.xx", false);
		}
		else
		{
			backend("readyFile2.xx", true);
		}
	}
	else if (string(argv[1]) == "-h")
	{
		cout << "  Use --window to start a gui-frontend process." << endl
			<< "  Use -f to start a frontend process." << endl
			<< "  Use -b to start a backend process.";
	}
	else if (string(argv[1]) == "--sobel")
	{
		cv::Mat src1 = cv::imread("testfiles/checkered0.jpg");
		cv::Mat img1 = resizeImg(src1, 300, 300);

		cv::Mat gray1;
		cv::cvtColor(img1, gray1, CV_BGR2GRAY);

		cv::Mat grad_x1, grad_y1;
		cv::Mat absGrad_x1, absGrad_y1;

		cv::Sobel(gray1, grad_x1, CV_16S, 2, 0, 3);
		cv::Sobel(gray1, grad_y1, CV_16S, 0, 2, 3);

		cv::convertScaleAbs(grad_x1, absGrad_x1);
		cv::convertScaleAbs(grad_y1, absGrad_y1);

		cv::Mat all1;
		cv::addWeighted(absGrad_x1, 0.5, absGrad_y1, 0.5, 0, all1);
		cv::imshow("absboth", all1);

		cv::Sobel(gray1, grad_x1, CV_8U, 2, 0, 3);
		cv::Sobel(gray1, grad_y1, CV_8U, 0, 2, 3);

		cv::addWeighted(grad_x1, 0.5, grad_y1, 0.5, 0, all1);
		cv::imshow("both", all1);

		cv::imshow("org", gray1);
		cv::imshow("x", grad_x1);
		cv::imshow("y", grad_y1);
		cv::imshow("absx", absGrad_x1);
		cv::imshow("absy", absGrad_y1);

		cv::waitKey();

	}
	else if (string(argv[1]) == "--cluster")
	{
		const int MAX_CLUSTERS = 5;
		cv::Scalar colorTab[] =
		{
			cv::Scalar(0, 0, 255),
			cv::Scalar(0,255,0),
			cv::Scalar(255,100,100),
			cv::Scalar(255,0,255),
			cv::Scalar(0,255,255)
		};

		cv::Mat img(500, 500, CV_8UC3);
		cv::Mat img2(500, 500, CV_8UC3);
		cv::RNG rng(12345);

		

		while (true)
		{
			int k, clusterCount = rng.uniform(2, MAX_CLUSTERS + 1);
			int i, sampleCount = rng.uniform(1, 1001);
			cv::Mat points(sampleCount, 1, CV_32FC2), labels;

			clusterCount = MIN(clusterCount, sampleCount);
			cv::Mat centers;

			cv::Mat trainingData(sampleCount, 2, CV_32F);
			vector<int> tLabels;

			/* generate random sample from multigaussian distribution */
			for (k = 0; k < clusterCount; k++)
			{
				cv::Point center;
				center.x = rng.uniform(0, img.cols);
				center.y = rng.uniform(0, img.rows);
				cv::Mat pointChunk = points.rowRange(k*sampleCount / clusterCount,
					k == clusterCount - 1 ? sampleCount :
					(k + 1)*sampleCount / clusterCount);
				rng.fill(pointChunk, cv::RNG::NORMAL, cv::Scalar(center.x, center.y), cv::Scalar(img.cols*0.05, img.rows*0.05));
			}
			randShuffle(points, 1, &rng);
			kmeans(points, clusterCount, labels,
				cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1.0),
				3, cv::KMEANS_PP_CENTERS, centers);
			img = cv::Scalar::all(0);
			for (i = 0; i < sampleCount; i++)
			{
				int clusterIdx = labels.at<int>(i);
				cv::Point ipt = points.at<cv::Point2f>(i);
				circle(img, ipt, 2, colorTab[clusterIdx] / 2, cv::FILLED, cv::LINE_AA);
				trainingData.at<float>(i, 0) = points.at<cv::Point2f>(i).y;
				trainingData.at<float>(i, 1) = points.at<cv::Point2f>(i).x;
				tLabels.push_back(labels.at<int>(i));
			}
			cout << centers << endl;
			for (i = 0; i < clusterCount; i++)
			{
				cv::Point ipt = cv::Point(centers.at<float>(i, 0), centers.at<float>(i, 1));
				circle(img, ipt, 2, cv::Scalar(175, 175, 175), cv::FILLED, cv::LINE_AA);
			}

			imshow("clusters", img);
			
			cv::Mat labelsMat(tLabels.size(), 1, CV_32SC1);
			for (int i = 0; i < tLabels.size(); i++)
			{
				labelsMat.at<int>(i, 0) = tLabels[i];
			}

			cv::Ptr<cv::ml::TrainData> tData = cv::ml::TrainData::create(trainingData, cv::ml::SampleTypes::ROW_SAMPLE, labelsMat);


			cv::Ptr<cv::ml::RTrees> rt = cv::ml::RTrees::create();

			rt->setMaxDepth(20);
			rt->setMinSampleCount(20);
			rt->setMaxCategories(40);

			rt->setCalculateVarImportance(false);
			rt->setRegressionAccuracy(0.0f);
			rt->setPriors(cv::Mat());

			rt->train(tData, 0);

			cv::Vec3b green(0, 255, 0), blue(255, 0, 0), red(0, 0, 255), yellow(255, 255, 0);
			cv::Vec3b white(255, 255, 255), black(0, 0, 0), purple(255, 0, 255), frg(0, 255, 255);
			// Show the decision regions given by the SVM
			for (int i = 0; i < img2.rows; ++i)
			{
				for (int j = 0; j < img2.cols; ++j)
				{
					cv::Mat sampleMat = (cv::Mat_<float>(1, 2) << i, j);
					cv::Mat predictResponse;
					float predicted = rt->predict(sampleMat);


					if (predicted == 0)
						img2.at<cv::Vec3b>(i, j) = green / 2;
					else if (predicted == 1)
						img2.at<cv::Vec3b>(i, j) = blue / 2;
					else if (predicted == 2)
						img2.at<cv::Vec3b>(i, j) = red / 2;
					else if (predicted == 3)
						img2.at<cv::Vec3b>(i, j) = yellow / 2;
					else if (predicted == 4)
						img2.at<cv::Vec3b>(i, j) = white / 2;
					else if (predicted == 5)
						img2.at<cv::Vec3b>(i, j) = black / 2;
					else if (predicted == 6)
						img2.at<cv::Vec3b>(i, j) = purple / 2;
					else
						img2.at<cv::Vec3b>(i, j) = frg / 2;
				}
			}

			cv::imshow("RT Simple Example", img2); // show it to the user


			cv::Mat both(img2.size(), CV_8UC3);
			cv::add(img, img2, both);

			cv::imshow("both", both);




			cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::create();
			svm->setType(cv::ml::SVM::C_SVC);
			svm->setKernel(cv::ml::SVM::LINEAR);
			svm->setGamma(1); 
			//svm->setKernel(cv::ml::SVM::CHI2);
			svm->setTermCriteria(cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6));

			svm->train(tData, 0);

			for (int i = 0; i < img2.rows; ++i)
			{
				for (int j = 0; j < img2.cols; ++j)
				{
					cv::Mat sampleMat = (cv::Mat_<float>(1, 2) << i, j);
					cv::Mat predictResponse;
					float predicted = svm->predict(sampleMat);


					if (predicted == 0)
						img2.at<cv::Vec3b>(i, j) = green / 2;
					else if (predicted == 1)
						img2.at<cv::Vec3b>(i, j) = blue / 2;
					else if (predicted == 2)
						img2.at<cv::Vec3b>(i, j) = red / 2;
					else if (predicted == 3)
						img2.at<cv::Vec3b>(i, j) = yellow / 2;
					else if (predicted == 4)
						img2.at<cv::Vec3b>(i, j) = white / 2;
					else if (predicted == 5)
						img2.at<cv::Vec3b>(i, j) = black / 2;
					else if (predicted == 6)
						img2.at<cv::Vec3b>(i, j) = purple / 2;
					else
						img2.at<cv::Vec3b>(i, j) = frg / 2;
				}
			}


			cv::imshow("SVM Simple Example", img2); // show it to the user

			cv::add(img, img2, both);

			cv::imshow("both2", both);


			char key = (char)cv::waitKey();


			/*
			cv::Mat points2(5, 3, CV_32F), labels2;
			clusterCount = 2;
			cv::Mat centers2;


			points2.at<float>(0, 0) = 100;
			points2.at<float>(0, 1) = 150;
			points2.at<float>(0, 2) = 5;

			points2.at<float>(1, 0) = 250;
			points2.at<float>(1, 1) = 150;
			points2.at<float>(1, 2) = 2;

			points2.at<float>(2, 0) = 450;
			points2.at<float>(2, 1) = 350;
			points2.at<float>(2, 2) = 7;

			points2.at<float>(3, 0) = 300;
			points2.at<float>(3, 1) = 120;
			points2.at<float>(3, 2) = 3;

			points2.at<float>(4, 0) = 450;
			points2.at<float>(4, 1) = 150;
			points2.at<float>(4, 2) = 11;

			kmeans(points2, clusterCount, labels2,
				cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1.0),
				3, cv::KMEANS_PP_CENTERS, centers2);

			img = cv::Scalar::all(0);
			for (i = 0; i < 5; i++)
			{
				int clusterIdx = labels2.at<int>(i);
				cv::Point ipt = cv::Point(points2.at<float>(i, 0), points2.at<float>(i, 1));
				circle(img, ipt, (int)points2.at<float>(i, 2), colorTab[clusterIdx], cv::FILLED, cv::LINE_AA);
			}
			for (i = 0; i < 2; i++)
			{
				cv::Point ipt = cv::Point(centers2.at<float>(i, 0), centers2.at<float>(i, 1));
				circle(img, ipt, (int)centers2.at<float>(i, 2), cv::Scalar(175, 175, 175), cv::FILLED, cv::LINE_AA);
			}


			imshow("clusters", img);
			key = (char)cv::waitKey();
			*/
			if (key == 27 || key == 'q' || key == 'Q') // 'ESC'
				break;
		}



	}
	else if (string(argv[1]) == "--erosion_test") // Erosion test
	{
		cv::Mat src = cv::imread("testfiles/lindex1.jpg");
		cv::Mat img = resizeImg(src);
		if (!src.data)
			return -1;

		cv::Mat bw;
		cv::cvtColor(img, bw, CV_BGR2GRAY);

		cv::Mat bin;
		cv::threshold(bw, bin, 240, 255, CV_THRESH_BINARY_INV);

		cv::Mat gau1, gau2;
		cv::GaussianBlur(bw, gau1, cv::Size(1, 1), 0);
		cv::GaussianBlur(bw, gau2, cv::Size(3, 3), 0);

		cv::Mat res;
		cv::absdiff(gau2, gau1, res);

		cv::equalizeHist(res, res);


		////////////////
		cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,
			cv::Size(2 * 3/*erosion_size*/ + 1, 2 * 3/*erosion_size*/ + 1),
			cv::Point(3, 3/*erosion_size, erosion_size*/));

		cv::Mat erod;
		dilate(bin, erod, element);

		erode(erod, erod, element);
		////////////////



		cv::Mat out;
		onlyBackground(erod, out);
		out *= 255;

		cv::Mat edge = preformCanny(out, 80, 140);


		cv::Mat ssample = resizeImg(out, 50, 50);
		cv::Mat upsample;
		cv::resize(ssample, upsample, cv::Size(300, 300), 0.0, 0.0, cv::INTER_NEAREST);

		cv::imshow("ssample", ssample);
		cv::imshow("upsample", upsample);

		cv::imshow("out", out);
		cv::imshow("erod", erod);

		cv::imshow("bin", bin);
		cv::imshow("Gray", bw);
		cv::imshow("DoG", res);
		cv::imshow("edge", edge);

		cv::Mat kov;
		cv::bitwise_and(res, bin, kov);

		cv::Mat vok;
		cv::bitwise_and(res, edge, vok);


		cv::imshow("kov", kov);
		cv::imshow("vok", vok);


		cv::waitKey();
	}
	else if (string(argv[1]) == "--skeleton_test") //Skeleton test
	{

		cv::Mat src = cv::imread("testfiles/shirt4.jpg");
		cv::Mat img = resizeImg(src);
		if (!src.data)
			return -1;

		cv::Mat bw;
		cv::cvtColor(img, bw, CV_BGR2GRAY);
		cv::imshow("kaf", bw);
		cv::threshold(bw, bw, 240, 255, CV_THRESH_BINARY_INV);



		cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,
			cv::Size(2 * 2/*erosion_size*/ + 1, 2 * 2/*erosion_size*/ + 1),
			cv::Point(2, 2/*erosion_size, erosion_size*/));

		/// Apply the erosion operation
		cv::Mat erod;
		dilate(bw, erod, element);

		erode(erod, erod, element);

		//cv::morphologyEx(bw, erod, cv::MORPH_CLOSE, element);

		cv::imshow("Erotion", erod);


		cv::imshow("laf", bw);

		cv::Mat thinned;
		thinning(erod, thinned);

		cv::Mat inSkel;
		erod.copyTo(inSkel);
		cv::Mat skelMorph = skeletonizeMorph(&inSkel);
		cv::bitwise_not(skelMorph, skelMorph);

		//thinned /= 255;
		cv::bitwise_not(thinned, thinned);
		thinned *= 255;

		cv::Mat sum;
		cv::bitwise_xor(thinned, erod, sum);

		cv::Mat sum2;
		cv::bitwise_xor(skelMorph, erod, sum2);

		cv::Mat sum3;
		cv::bitwise_and(skelMorph, thinned, sum3);
		cv::bitwise_xor(sum3, erod, sum3);

		cv::imshow("Sum3", sum3);
		cv::imshow("sum2", sum2);
		cv::imshow("skel", skelMorph);
		cv::imshow("thinned", thinned);
		cv::imshow("erod", erod);
		cv::imshow("sum", sum);
		cv::waitKey();
		return 0;

	}
	else
	{
		cout << "Invalid arguments, use -h for help.";
	}

	return 0;
}




/**Tests a simple search in a cataloge, with the use of SVM or RF to filter out classes.
* 
* \params trainingFilename Path to the trainging cataloge.
* \params testFilename Path image query.
* \params testType The type of classifier that is going to be used as filter.
* \params loadModel Boolean indicating if the model should be loaded from file or not.
*/
void testModelWithImage(string trainingFilename, string testFilename, string testType, bool loadModel)
{
	vector<ClothArticle*> *allArticles = readCatalogeFromFile(trainingFilename, false);
	
	ClothArticle* testItem = new ClothArticle("Test0", testFilename, "Gra", "Top", -1);

	if(testType != "None")
	{
		cv::Ptr<cv::ml::SVM> model;
		if (testType == "Color" || testType == "All")
		{
			cv::Mat multVec;
			cv::Mat featVec = createFeatureVector(testItem);// , "Color");
			cv::Mat filtVec = createFilterVector(featVec.size(), "Color", 1.0f, 0.0f);
			cv::multiply(featVec, filtVec, multVec);

			if (!loadModel)
			{
				model = makeSVMModel(allArticles, "Color");
				model->save("ColorModel.xml");
			}
			else
			{
				model = cv::Algorithm::load<cv::ml::SVM>("ColorModel.xml");
			}

			float predictResponse = model->predict(multVec);

			cout << "Predicted: " << to_string(art_color((int)predictResponse)) << endl;
			testItem->setColor(art_color((int)predictResponse));
		}
		if (testType == "ClothingType" || testType == "All")
		{
			cv::Mat multVec;
			cv::Mat featVec = createFeatureVector(testItem);// , "Color");
			cv::Mat filtVec = createFilterVector(featVec.size(), "ClothingType", 1.0f, 0.0f);
			cv::multiply(featVec, filtVec, multVec);


			if (!loadModel)
			{
				model = makeSVMModel(allArticles, "ClothingType");
				model->save("ClTypeModel.xml");
			}
			else
			{
				model = cv::Algorithm::load<cv::ml::SVM>("ClTypeModel.xml");
			}

			float predictResponse = model->predict(multVec);

			cout << "Predicted: " << to_string(art_clType((int)predictResponse)) << endl;
			testItem->setClType(art_clType((int)predictResponse));
		}
		
	}

	vector<string> nn = findClosestNeighbours(allArticles, testItem, 11, "All", testType);

	cv::namedWindow("Query", 1);
	cv::imshow("Query", testItem->getImage());

	for (int i = 0; i < nn.size(); i++)
	{
		for (int j = 0; j < allArticles->size(); j++)
		{
			if (nn[i] == allArticles->at(j)->getId())
			{
				cv::namedWindow("Result # " + to_string(i + 1), 1);
				cv::imshow("Result # " + to_string(i + 1), allArticles->at(j)->getImage());
			}
		}
	}


	cv::waitKey(0);

	if (false) //Shows some diffrent features from input image.
	{
		cv::Mat imgGray;
		cv::cvtColor(testItem->getImage(), imgGray, cv::COLOR_BGR2GRAY);
		cv::Mat binary;

		cv::threshold(imgGray, binary, 248, cv::THRESH_BINARY_INV, cv::THRESH_BINARY);

		binary = binary * 255;

		cv::Mat noBluredges = preformCanny(binary, Config::get().CANNY_THRESH_LOW, Config::get().CANNY_THRESH_HIGH);

		cv::Mat imgBlur = preformGaussianBlur(imgGray);
		cv::Mat edges = preformCanny(imgBlur, Config::get().CANNY_THRESH_LOW, Config::get().CANNY_THRESH_HIGH);

		cv::Mat coolEdge;
		cv::Mat image = testItem->getImage();
		bool png = testItem->getPath().find(".png") != string::npos;
		if(png)
		{
			cv::Mat *ch = (cv::Mat*)calloc(4, sizeof(cv::Mat));

			cv::split(image, ch);
			cv::Mat alpha = ch[3];
			cv::threshold(alpha, coolEdge, 255, cv::THRESH_BINARY_INV, cv::THRESH_BINARY);

			coolEdge = coolEdge * 255;

			cv::namedWindow("Edge0", 1);
			cv::imshow("Edge0", alpha);

			cv::namedWindow("Edge00", 1);
			cv::imshow("Edge00", coolEdge);

			for (int i = 0; i < 4; i++)
			{

				ch[i].release();
			}
			free(ch);
		}

		cv::namedWindow("Edge1", 1);
		cv::imshow("Edge1", coolEdge);

		cv::namedWindow("Edge2", 1);
		cv::imshow("Edge2", edges);

		cv::namedWindow("Edge3", 1);
		cv::imshow("Edge3", noBluredges);
			


		cv::waitKey(0);


		cv::namedWindow("Query T", 1);
		cv::imshow("Query T", testItem->getImage());

		cv::namedWindow("Query Gray", 1);
		cv::imshow("Query Gray", imgGray);

		cv::namedWindow("Query Binary", 1);
		cv::imshow("Query Binary", binary);

		cv::namedWindow("Query noBluredges", 1);
		cv::imshow("Query noBluredges", noBluredges);

		cv::namedWindow("Query imgBlur", 1);
		cv::imshow("Query imgBlur", imgBlur);

		cv::namedWindow("Query edges", 1);
		cv::imshow("Query edges", edges);


		cv::waitKey(0);
	
	}
	

}

void svmANDrfTest(string filename, string testType)
{
	vector<ClothArticle*> *allArticles = readCatalogeFromFile(filename, false);

	int totSize = (int)allArticles->size();
	int partSize = totSize / 10;


	double bestRTHitRatio = 0.0;
	double bestSVMHitRatio = 0.0;

	int totRTHits = 0;
	int totSVMHits = 0;
	for (int t = 0; t < 10; t++)
	{
		vector<ClothArticle*> trainingArticles;
		vector<ClothArticle*> testArticles;

		for (int r = 0; r < totSize; r++)
		{
			if (t == r / partSize)
				testArticles.push_back(allArticles->at(r));
			else
				trainingArticles.push_back(allArticles->at(r));
		}

		cv::Ptr<cv::ml::SVM> modelSVM;
		cv::Ptr<cv::ml::RTrees> modelRF;
		modelSVM = makeSVMModel(&trainingArticles, testType);
		modelRF = makeRTModel(&trainingArticles, testType);

		int svmHits = 0;
		int rtHits = 0;

		for (int i = 0; i < testArticles.size(); i++)
		{
			cv::Mat multVec;
			cv::Mat featVec = createFeatureVector(testArticles[i]);// , "Color");
			cv::Mat filtVec = createFilterVector(featVec.size(), testType, 1.0f, 0.0f);
			cv::multiply(featVec, filtVec, multVec);

			float predictSVMResponse = modelSVM->predict(multVec);
			float predictRTResponse = modelRF->predict(multVec);

			if (testType == "Color")
			{
				if (art_color((int)predictRTResponse) == testArticles.at(i)->getColor())
					rtHits++;

				if (art_color((int)predictSVMResponse) == testArticles.at(i)->getColor())
					svmHits++;
			}
			else if (testType == "ClothingType")
			{
				
				if (art_clType((int)predictRTResponse) == testArticles.at(i)->getClType())
					rtHits++;
				if (art_clType((int)predictSVMResponse) == testArticles.at(i)->getClType())
					svmHits++;
				
				if(false)
				{
					cout << "RF:  " << to_string(art_clType((int)predictRTResponse)) << endl;
					cout << "SVM: " << to_string(art_clType((int)predictSVMResponse)) << endl;
					cout << "Act: " << to_string(testArticles.at(i)->getClType()) << endl;

					cv::namedWindow("check", 1);
					cv::imshow("check", testArticles.at(i)->getImage());
					cv::waitKey(0);
				}
			}

		}
		cout << t << " RT Hits: " << rtHits << "\tRT Hitratio: " << (double)rtHits / (double)partSize << endl;
		cout << t << " SVM Hits: " << svmHits << "\tSVM Hitratio: " << (double)svmHits / (double)partSize << endl;

		bestRTHitRatio = max(bestRTHitRatio, (double)rtHits / (double)partSize);
		bestSVMHitRatio = max(bestSVMHitRatio, (double)svmHits / (double)partSize);

		totRTHits += rtHits;
		totSVMHits += svmHits;
	}

	for (int i = 0; i < allArticles->size(); i++)
	{
		delete(allArticles->at(i));
	}
	allArticles->clear();
	delete allArticles;

	double hitRatioRT = (double)totRTHits / (double)totSize;
	double hitRatioSVM = (double)totSVMHits / (double)totSize;

	std::cout << "RT  Total Hitratio: " << hitRatioRT << "\tTotal hits: " << totRTHits << endl;
	std::cout << "SVM Total Hitratio: " << hitRatioSVM << "\tTotal hits: " << totSVMHits << endl;

	std::cout << "RT  Best HitRatio: " << bestRTHitRatio << endl;
	std::cout << "SVM Best HitRatio: " << bestSVMHitRatio << endl;

}
