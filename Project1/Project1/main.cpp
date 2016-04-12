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
	//�ܼ�
	DWORD dwSendSize;
	BYTE data;
	char nextStep;
	Vec3b Black; // Vec3b����m�Ϲ����Ȫ����O���A�A��3�ӦV�q�զ��Aex: [0,0,0]���¦�A[255,255,255]���զ�
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

			bool bSuccess = cap.read(imgOriginal); //����v��Ū���@�ӷs���v��
			IplImage* img = &IplImage(imgOriginal);
			imshow("Original", imgOriginal); //����^���U�Ӫ���l�v��

			/*���Uc�x�s�v��(�̦h�s10�i)*/
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

			/******************* �p�@�@�� *******************/

			//�t�~�P�_�ڭn�A�վ�A�]���P�_��b���I��
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
			//imshow("Thresholded Image", imgOriginal); //����ഫ�᪺�v��
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

/******************* �C	�� *******************/
void searchColor(){
	cvtColor(imgOriginal, imgOriginal, COLOR_BGR2HSV); //�NŪ���i�Ӫ��s�v���� BGR�ഫ�� HSV

	//���ѿ�����C��d��(�ȥi�ϥ�HSV)	���G�����V���W(H)��0�� �u�n����Ӱ�
	inRange(imgOriginal, Scalar(60, 0, 0), Scalar(150, 255, 255), imgOriginal);

	//�}��G�ɻk�῱��(�����p����)	�ϥΤ覡 dilate/erode(��J�v��, ��X�v��, ���c�ʤ���, ���I, �I�k/���Ȧ���)
	erode(imgOriginal, imgOriginal, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)), Point(-1, -1), 3);
	dilate(imgOriginal, imgOriginal, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)), Point(-1, -1), 3);

	//�ʳ��G���ȫ�ɻk(�h���p�})
	dilate(imgOriginal, imgOriginal, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)), Point(-1, -1), 3);
	erode(imgOriginal, imgOriginal, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)), Point(-1, -1), 3);

}

/****������t�Ϊ�****/
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
		/*********** ²���� ***********/
		//approxPolyDP(contours[i], contours[i], 10, TRUE);
		/* �e�u */
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(imgContours, contours, i, color, 1, 8, hierarchy, 0, Point());
	}
	imshow("Test", imgContours);
}

void setup(){
	arduino = CreateFile(("COM3"), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	//���s��arduino
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

/*�x�s�v��(�w�]�b�M�פ���Special-Project\Project1\Project1\photo��)*/
void saveImg(int n, int status){
	char addreas[100] = ".\\photo\\";
	char numstr[5];
	_itoa_s(n, numstr, 10);
	strcat_s(addreas, numstr);
	strcat_s(addreas, ".jpg");

	if (status){
		imwrite(addreas, imgOriginal);
		cout << "�x�s��Ϲ�" << endl;
	}
	else{
		imwrite(addreas, imgContours);
		cout << "�x�s��t��" << endl;
	}
}