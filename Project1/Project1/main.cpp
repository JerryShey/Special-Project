#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <Windows.h>
#include "winBase.h" 
#include <stdio.h>
#include <stdlib.h>
#include "opencv/cv.h"

using namespace cv;
using namespace std;

HANDLE arduino;
bool Ret;
int Cols;
DCB dcb;
Mat imgOriginal;
Mat imgContours;

void setup();
char myMove(int, int);
void searchColor();
void contourTest();
void saveImg(int, int);

int main(int argc, char** argv)
{
	//變數
	DWORD dwSendSize;
	BYTE data;
	char nextStep;
	Vec3b Black; // Vec3b為色彩圖像素值的類別型態，由3個向量組成，ex: [0,0,0]為黑色，[255,255,255]為白色
	int num, avrg, photoNum = 0;

	cout << "Press c to save your webCam's image" << endl;
	cout << "Press v to save the image after ContourTest" << endl;
	try{
		//code start
		VideoCapture cap(1); //capture the video from web cam

		if (!cap.isOpened())  // if not success, exit program
			throw 0;

//		setup();

		while (true)
		{

			bool bSuccess = cap.read(imgOriginal); //自攝影機讀取一個新的影像
			IplImage* img = &IplImage(imgOriginal);
			imshow("Original", imgOriginal); //顯示擷取下來的原始影像

			/*按下c儲存影像(最多存10張)*/
			char save = waitKey(1);
			if (save == 'v'){
				if (photoNum == 20){
					photoNum = 0;
				}
				photoNum++;
				saveImg(photoNum, 0);
			}
			else if (save == 'c'){
				if (photoNum == 20){
					photoNum = 0;
				}
				photoNum++;
				saveImg(photoNum, 1);
			}
			
			if (!bSuccess) //if not success, break loop
				throw 7;

			contourTest();
			//searchColor();

			/******************* 計　　算 *******************/

			//另外判斷我要再調整，因為判斷實在有點爛
			/*num = 0;
			avrg = 0;
			Cols = imgOriginal.cols / 3 - 20;

			for (int i = 20; i < Cols; i += 3){
				for (int j = 0; j < imgOriginal.rows; j++){
					if (imgOriginal.at<Vec3b>(j, i) != Black){
						num += 1;
						avrg += i;
					}
				}
			}

			if (num>50)
				nextStep = myMove(num, avrg);
			else
				nextStep = '1';

			cout << nextStep << endl;
			/*
			data = nextStep;
			Ret = WriteFile(arduino, &data, sizeof(data), &dwSendSize, NULL);
			if (!Ret)
				throw 5;
*/
			//imshow("Thresholded Image", imgOriginal); //顯示轉換後的影像
			//waitKey(1);
		}
	}
	catch (int num){
		if (num == 0)
			printf("Cannot open the web cam\n");
		else if (num == 1)
			printf("PORT COULD NOT OPEN\n");
		else if (num == 2)
			printf("SET UP FAILED\n");
		else if (num == 3)
			printf("CLEAR FAILED\n");
		else if (num == 4)
			printf("SetCommState FAILED\n");
		else if (num == 5)
			printf("SEND FAILED\n");
		else if (num == 6)
			printf("FINISH\n");
		else if (num == 7)
			printf("Cannot read a frame from video stream\n");
		CloseHandle(arduino);
		system("PAUSE");
	}
	return(0);
}

char myMove(int n, int total){
	total /= n;
	if (total >= Cols / 2 * 1.3)
		return '1';
	else if (total <= (Cols / 2 * 0.7))
		return '2';
	else
		return '0';
}

/******************* 顏	色 *******************/
void searchColor(){
	cvtColor(imgOriginal, imgOriginal, COLOR_BGR2HSV); //將讀取進來的新影像由 BGR轉換為 HSV

	//辨識選取的顏色範圍(僅可使用HSV)	註：紅色跨越色頻(H)的0度 只好分兩個做
	inRange(imgOriginal, Scalar(60, 0, 0), Scalar(150, 255, 255), imgOriginal);

	//開放：傾蝕後膨脹(移除小物件)	使用方式 dilate/erode(輸入影像, 輸出影像, 結構性元素, 錨點, 侵蝕/膨脹次數)
	erode(imgOriginal, imgOriginal, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)), Point(-1, -1), 3);
	dilate(imgOriginal, imgOriginal, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)), Point(-1, -1), 3);

	//封閉：膨脹後傾蝕(去除小洞)
	dilate(imgOriginal, imgOriginal, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)), Point(-1, -1), 3);
	erode(imgOriginal, imgOriginal, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)), Point(-1, -1), 3);

}

/****偵測邊緣用的****/
void contourTest(){
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	RNG rng(12345);
	
	cvtColor(imgOriginal, imgContours, COLOR_BGR2GRAY);
	
	blur(imgContours, imgContours, Size(3, 3));
	Canny(imgContours, imgContours, 100, 200, 3);
	findContours(imgContours, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

	imgContours = Mat::zeros(imgContours.size(), CV_8UC3);
	
	for (int i = 0; i< contours.size(); i++)
	{
		/*********** 簡化邊 ***********/
		//approxPolyDP(contours[i], contours[i], 10, TRUE);
		/* 畫線 */
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(imgContours, contours, i, color, 1, 8, hierarchy, 0, Point());
	}
	imshow("Test", imgContours);
}

void setup(){
	arduino = CreateFile(("COM3"), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	//未連接arduino
	if (arduino == INVALID_HANDLE_VALUE)
		throw 1;

	Ret = SetupComm(arduino, 1024, 1024);
	if (!Ret)
		throw 2;

	Ret = PurgeComm(arduino, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	if (!Ret)
		throw 3;

	GetCommState(arduino, &dcb);
	dcb.DCBlength = sizeof(DCB);
	dcb.BaudRate = 9600;
	dcb.fBinary = TRUE;
	dcb.ByteSize = 8;
	dcb.fParity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;

	Ret = SetCommState(arduino, &dcb);
	if (!Ret)
		throw 4;
}

/*儲存影像(預設在專案中的Special-Project\Project1\Project1\photo中)*/
void saveImg(int n, int status){
	char addreas[100] = ".\\photo\\";
	char numstr[5];
	_itoa_s(n, numstr, 10);
	strcat_s(addreas, numstr);
	strcat_s(addreas, ".jpg");

	if (status){
		imwrite(addreas, imgOriginal);
		cout << "儲存原圖像" << endl;
	}
	else{
		imwrite(addreas, imgContours);
		cout << "儲存邊緣圖" << endl;
	}
}