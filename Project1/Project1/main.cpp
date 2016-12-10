#include <iostream>
#include <Windows.h>
#include "winBase.h" 
#include <stdio.h>
#include <stdlib.h>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/video.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>

#include <fstream>

using namespace cv;
using namespace std;
Mat image, templ, myGoal;

int vmin = 10, vmax = 180, smin = 100, hmax = 180, hmin = 0, smax = 256;
void contourTest(int);
void DataAnalysis(Mat);
void yungyung();
void setGoal(RotatedRect);

bool backprojMode = false;
bool selectObject = false;
int trackObject = 0;
bool showHist = true;
Point origin;
Rect selection;
bool drowCircle = true;

bool set(int x, int y, int width, int height){
	origin = Point(x, y);
	selection = Rect(x, y, width, height);
	selection &= Rect(0, 0, image.cols, image.rows);

	trackObject = -1;
	return true;
}

string hot_keys =
    "\n\nHot keys: \n"
    "\tESC - quit the program\n"
    "\tc - stop the tracking\n"
    "\tb - switch to/from backprojection view\n"
    "\th - show/hide object histogram\n"
    "\tp - pause video\n"
    "To initialize tracking, select the object with mouse\n";

const char* keys ={
    "{help h | | show help message}{@camera_number| 0 | camera number}"
};

int main( int argc, const char** argv ){
    VideoCapture cap;
    Rect trackWindow;
    int hsize = 16;
    float hranges[] = {0,180};
    const float* phranges = hranges;
	Mat colorMat;

    cap.open(0);

    if( !cap.isOpened() ){
        cout << "***Could not initialize capturing...***\n";
        cout << "Current parameter's value: \n";
		waitKey();
        return -1;
    }
    cout << hot_keys;
    namedWindow( "Histogram", 0 );
    namedWindow( "CamShift Demo", 0 );

    Mat frame, hsv, hue, mask, hist, histimg = Mat::zeros(200, 320, CV_8UC3), backproj;
	int x = 240, y = 180, width = 160, height = 120;
    bool paused = false, select = false;

    for(;;){
		drowCircle = true;
        if( !paused ){
            cap >> frame;
			if (frame.empty()){
				cout << "Can't find camara!!" << endl;
				waitKey();
				break;
			}
        }

        frame.copyTo(image);
		/** Rows:480, Cols:640 **/
        if( !paused ){
            cvtColor(image, hsv, COLOR_BGR2HSV);
			
			
			if (trackObject){
				int _vmin = vmin, _vmax = vmax;

				inRange(hsv, Scalar(hmin, 50, 30),
					Scalar(hmax, 256, 256), mask);
				//inRange(hsv, Scalar(hmin, smin, vmin), Scalar(hmax, smax, vmax), mask);
				//¿±µÈ«á«I»k
				erode(hsv, hsv, Mat(), Point(-1, -1), 5);
				dilate(hsv, hsv, Mat(), Point(-1, -1), 3);
				
				int ch[] = { 0, 0 };
				hue.create(hsv.size(), hsv.depth());
				mixChannels(&hsv, 1, &hue, 1, ch, 1);

				if (trackObject < 0){
					templ = Mat(image, selection);
					imwrite("templ.jpg", templ);
					imshow("templ", templ);

					// Object has been selected by user, set up CAMShift search properties once
					Mat roi(hue, selection), maskroi(mask, selection);
					calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
					normalize(hist, hist, 0, 255, NORM_MINMAX);

					trackWindow = selection;
					trackObject = 1; // Don't set up again, unless user selects new ROI

					histimg = Scalar::all(0);
					int binW = histimg.cols / hsize;
					Mat buf(1, hsize, CV_8UC3);
					for (int i = 0; i < hsize; i++)
						buf.at<Vec3b>(i) = Vec3b(saturate_cast<uchar>(i*180. / hsize), 255, 255);
					cvtColor(buf, buf, COLOR_HSV2BGR);

					for (int i = 0; i < hsize; i++){
						int val = saturate_cast<int>(hist.at<float>(i)*histimg.rows / 255);
						rectangle(histimg, Point(i*binW, histimg.rows),
							Point((i + 1)*binW, histimg.rows - val),
							Scalar(buf.at<Vec3b>(i)), -1, 8);
					}
				}

				// Perform CAMShift
				calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
				backproj &= mask;
				

				RotatedRect trackBox;
				try{
					trackBox = CamShift(backproj, trackWindow, TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));
					//yungyung();
					setGoal(trackBox);
				}	
				catch (Exception e){
					trackWindow = Rect(50, 128, image.cols*0.8, image.rows*0.8);
					destroyWindow("myGoal");
					imshow("CamShift Demo", image);
					imshow("Histogram", histimg);
					drowCircle = false;
				}
				if (trackBox.size.height < 50 || trackBox.size.width < 50){
					waitKey(100);
					trackWindow = Rect(50, 128, image.cols*0.8, image.rows*0.8);
					destroyWindow("myGoal");
					imshow("CamShift Demo", image);
					imshow("Histogram", histimg);
					drowCircle = false;
				}

                if( backprojMode )
                    cvtColor( backproj, image, COLOR_GRAY2BGR );
				if (drowCircle)
					ellipse( image, trackBox, Scalar(0,0,255), 3, 8);
            }
			else
				rectangle(image, Point(x-1, y-1), Point(x+width+1, y+height+1), Scalar(0, 0, 255));
        }
        else if( trackObject < 0 )
            paused = false;
		

		//±N¿ï¨úªº½d³ò¤Ï¥Õ
		/********
        if( selectObject && selection.width > 0 && selection.height > 0 )
        {
            Mat roi(image, selection);
            bitwise_not(roi, roi);
        }
		********/

        imshow( "CamShift Demo", image );
        imshow( "Histogram", histimg );
		
		int i, j;
        char c = (char)waitKey(10);
        if( c == 27 )
            break;
		else if (c >= 0){
			switch (c){
			case 'b':
				backprojMode = !backprojMode;
				break;
			case 'c':
				trackObject = 0;
				histimg = Scalar::all(0);
				break;
			case 'h':
				showHist = !showHist;
				if (!showHist)
					destroyWindow("Histogram");
				else
					namedWindow("Histogram", 1);
				break;
			case 'p':
				paused = !paused;
				break;
			case 's':
				if (select){
					trackObject = 0;
					destroyWindow("templ");
					destroyWindow("myGoal");
					histimg = Scalar::all(0);
				}
				else{
					set(x, y, width, height);
					cvtColor(image, hsv, COLOR_BGR2HSV);
					hsv.copyTo(colorMat);

					DataAnalysis(colorMat);
				}
				select = !select;
				break;
			case 'f':
				cvtColor(image, hsv, COLOR_BGR2HSV);
				hsv.copyTo(colorMat);

				DataAnalysis(colorMat);
				break;
			default:
				break;
			}
		}
		
    }

    return 0;
}

void setGoal(RotatedRect lulu){
	Rect tongtong;
	double x, y, width, height;
	x = lulu.center.x - lulu.size.width / 2;
	if (x < 0){
		width = lulu.size.width + x;
		x = 0;
	}
	else if ((x + lulu.size.width) > image.cols){
		width = image.cols - x - 1;
		if (width > image.cols){
			width = image.cols - 1;
		}
	}
	else{
		width = lulu.size.width;
	}
	y = lulu.center.y - lulu.size.height / 2;
	if (y < 0){
		height = lulu.size.height / 2 + y;
		y = 0;
	}
	else if ((y + lulu.size.height) > image.rows){
		height = image.rows - y - 1;
		if (height > image.rows){
			height = image.rows - 1;
		}
	}
	else{
		height = lulu.size.height;
	}
	cout << image.rows << ", " << image.cols << endl;
	cout << "lulu:width, height" << lulu.size.width << ", " << lulu.size.height << endl;
	cout << "tongtong: width, height" << width << ", " << height << endl;

	tongtong = Rect(x, y, floor(width), floor(height));
	cout << tongtong << endl;
	myGoal = Mat(image, tongtong);
	imshow("myGoal", myGoal);
	templ = imread("templ.jpg");
}

void yungyung(){

	Mat image2 = myGoal;

	/*if (!templ.data || image2.data){
		cout << "I don't know what happened." << endl;
		cout << "But, I shall end up your program." << endl
			<< "GoodBye!" << endl;
	}*/
    
	//-- Step 1: Detect the keypoints using SURF Detector
	int minHessian = 400;

	SurfFeatureDetector detector(minHessian);

	vector<KeyPoint> keypoint1, keypoint2;

	detector.detect(templ, keypoint1);
	detector.detect(image2, keypoint2);

	//-- Draw keypoints
	Mat img_keypoint1; Mat img_keypoint2;

	drawKeypoints(templ, keypoint1, img_keypoint1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	drawKeypoints(image2, keypoint2, img_keypoint2, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

	//-- Step 2: Calculate descriptors (feature vectors)
	SurfDescriptorExtractor extractor;

	Mat descriptor1, descriptor2;

	extractor.compute(templ, keypoint1, descriptor1);
	extractor.compute(image2, keypoint2, descriptor2);

	//-- Show detected (drawn) keypoints
	imshow("Keypoints 1", img_keypoint1);
	imshow("Keypoints 2", img_keypoint2);

	//-- Step 3: Matching descriptor vectors with a brute force matcher
	BFMatcher matcher(NORM_L2);
	vector< DMatch > matches;
	matcher.match(descriptor1, descriptor2, matches);

	//-- Draw matches
	Mat img_matches;
	drawMatches(templ, keypoint1, image2, keypoint2, matches, img_matches);

	//-- Show detected matches
	imshow("Matches", img_matches);
}

void DataAnalysis(Mat mat){
	//int x = 240, y = 180, width = 160, height = 120;
	Vec3b co;
	fstream file;
	int n = 0;
	file.open("C:\\Users\\user\\Desktop\\Data Analysis.txt", ios::out);
	float totalH = 0.0, totalV = 0.0, totalS = 0.0;
	float totalH2 = 0.0, totalV2 = 0.0, totalS2 = 0.0;
	float sigmaH = 0.0, sigmaS = 0.0, sigmaV = 0.0;
	for (int i = 240; i < 400; i += 3)
		for (int j = 180; j < 300; j += 1){
			co = mat.at<Vec3b>(j, i);
			//cout << ++n << ". " << co << "\tcols:" << i << ", rows:" << j << endl;
			file << n+1 << ". "<< co << endl;
			int H = co.val[0];
			int S = co.val[1];
			int V = co.val[2];
			totalH += H;
			totalS += S;
			totalV += V;
			totalH2 += H * H;
			totalS2 += S * S;
			totalV2 += V * V;
			n++;
		}
	sigmaH = sqrt(totalH2 / n - (totalH * totalH) / (n*n));
	sigmaS = sqrt(totalS2 / n - (totalS * totalS) / (n*n));
	sigmaV = sqrt(totalV2 / n - (totalV * totalV) / (n*n));

	if (sigmaS > 10){
		smax = totalS / n + 10;
		smin = totalS / n - 10;
	}
	else{
		smax = totalS / n + 2 * sigmaS;
		smin = totalS / n - 2 * sigmaS;
	}

	hmax = totalH / n + sigmaH * 2;
	hmin = totalH / n - sigmaH * 2;
	
	if (hmax > 180)
		hmax = 180;
	if (hmin < 0)
		hmin = 0;


	file <<"H 標準差：" << sigmaH << " 平均數：" << totalH / n << endl;
	file << "S 標準差：" << sigmaS << " 平均數：" << totalS / n << endl;
	file << "V 標準差：" << sigmaV << " 平均數：" << totalV / n << endl;
	cout << "Success" << endl;
	file.close();
}
