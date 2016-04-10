#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <iostream>
using namespace cv;
using namespace std;

int thresh = 50;
IplImage* img = NULL;
IplImage* img0 = NULL;
CvMemStorage* storage = NULL;
const char * wndname = "三角形DEMO";


// 兩個向量之間找到角度的餘弦
// from pt0->pt1 and from pt0->pt2
double angle(CvPoint* pt1, CvPoint* pt2, CvPoint* pt0)
{
	double dx1 = pt1->x - pt0->x;
	double dy1 = pt1->y - pt0->y;
	double dx2 = pt2->x - pt0->x;
	double dy2 = pt2->y - pt0->y;
	return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

// 回傳圖像中找到的輪廓陣列
// 陣列儲存在內存儲存器(大陸翻譯 感覺是暫存器的意思)中
CvSeq* findSquares4(IplImage* img, CvMemStorage* storage)
{
	CvSeq* contours;
	int i, c, l, N = 11;
	CvSize sz = cvSize(img->width & -2, img->height & -2);

	IplImage* timg = cvCloneImage(img);
	IplImage* gray = cvCreateImage(sz, 8, 1);
	IplImage* pyr = cvCreateImage(cvSize(sz.width / 2, sz.height / 2), 8, 3);
	IplImage* tgray;
	CvSeq* result;
	double s, t;
	// 創建一個空陣列用於儲存輪廓和頂點
	CvSeq* squares = cvCreateSeq(0, sizeof(CvSeq), sizeof(CvPoint), storage);

	cvSetImageROI(timg, cvRect(0, 0, sz.width, sz.height));
	// 過濾雜音
	cvPyrDown(timg, pyr, 7);
	cvPyrUp(pyr, timg, 7);
	tgray = cvCreateImage(sz, 8, 1);

	// find squares in every color plane of the image
	for (c = 0; c < 3; c++)
	{
		// 提取 the c-th color plane
		cvSetImageCOI(timg, c + 1);
		cvCopy(timg, tgray, 0);

		// try several threshold levels
		for (l = 0; l < N; l++)
		{
			// hack: use Canny instead of zero threshold level.
			// Canny helps to catch squares with gradient shading  
			if (l == 0)
			{
				// apply Canny. Take the upper threshold from slider
				// and set the lower to 0 (which forces edges merging)
				cvCanny(tgray, gray, 0, thresh, 5);
				// dilate canny output to remove potential
				// holes between edge segments
				cvDilate(gray, gray, 0, 1);
			}
			else
			{
				// apply threshold if l!=0:
				//     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
				cvThreshold(tgray, gray, (l + 1) * 255 / N, 255, CV_THRESH_BINARY);
			}

			// 找到所有輪廓並儲存於陣列中
			cvFindContours(gray, storage, &contours, sizeof(CvContour),
				CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));

			// test each contour
			while (contours)
			{
				// approximate contour with accuracy proportional
				// to the contour perimeter
				result = cvApproxPoly(contours, sizeof(CvContour), storage,
					CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.02, 0);
				// 三角形輪廓應當有3個頂點，3條邊
				// relatively large area (過濾干擾的輪廓)
				// cvCheckContourConvexity 保證凸面
				// cvContourArea計算三角形區域面積，去掉一些不相干的區域
				if (result->total == 3 &&
					fabs(cvContourArea(result, CV_WHOLE_SEQ)) > 1000 &&
					fabs(cvContourArea(result, CV_WHOLE_SEQ)) < 100000 &&
					cvCheckContourConvexity(result))
				{


					for (i = 0; i < 3; i++)
						cvSeqPush(squares,
						(CvPoint*)cvGetSeqElem(result, i));
				}

				// 繼續搜尋下個輪廓
				contours = contours->h_next;
			}
		}
	}

	// release all the temporary images
	cvReleaseImage(&gray);
	cvReleaseImage(&pyr);
	cvReleaseImage(&tgray);
	cvReleaseImage(&timg);

	return squares;
}


// the function draws all the squares in the image
void drawSquares(IplImage* img, CvSeq* squares)
{
	CvSeqReader reader;
	IplImage* cpy = cvCloneImage(img);
	int i;

	// initialize reader of the sequence
	cvStartReadSeq(squares, &reader, 0);

	// read 4 sequence elements at a time (all vertices of a square)
	for (i = 0; i < squares->total; i += 3)
	{
		CvPoint pt[3], *rect = pt;
		int count = 3;

		// read 3 vertices
		CV_READ_SEQ_ELEM(pt[0], reader);
		CV_READ_SEQ_ELEM(pt[1], reader);
		CV_READ_SEQ_ELEM(pt[2], reader);

		// cvPolyLine函數畫出三角形輪廓
		cvPolyLine(cpy, &rect, &count, 1, 1, CV_RGB(0, 255, 0), 3, CV_AA, 0);
	}

	// show the resultant image
	cvShowImage(wndname, cpy);
	cvReleaseImage(&cpy);
}

char* names[] = { "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg",
"7.jpg", "8.jpg", "9.jpg", "10.jpg", 0 };

int main(int argc, char** argv)
{
	int c, i;
	// create memory storage that will contain all the dynamic data
	storage = cvCreateMemStorage(0);
	for (i = 0; names[i] != 0; i++){
		img0 = cvLoadImage(names[i], 1);
		if (!img0)
		{
			printf("不能載入第%d張，載入下一張", i);
			continue;
		}
		img = cvCloneImage(img0);

		cvNamedWindow(wndname, 1);

		drawSquares(img, findSquares4(img, storage));


		// release both images
		cvReleaseImage(&img);
		cvReleaseImage(&img0);
		// clear memory storage - reset free space position
		cvClearMemStorage(storage);

		cout << "按任意鍵繼續：" << endl << fflush;
		cin.get();
	}

	cvDestroyWindow(wndname);
	return 0;
}