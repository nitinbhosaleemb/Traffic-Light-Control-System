/*******************************************************************************
 **
 ** File Name: OpenCV.h
 ** Project: Traffic Light Control and Management System using Open CV
 ** Creation Date:
 **
 *******************************************************************************/

#ifndef OPENCV_H_
#define OPENCV_H_

#include "Blob.h"
#include <fstream>
#include <string>
#include <iomanip>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include <ctime>
#include <cmath>
#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/types_c.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <thread>
#include <unistd.h>

#pragma warning(disable : 4996)

#define SHOW_STEPS // un-comment | comment this line to show steps or not

using namespace std;
using namespace cv;

const cv::Scalar SCALAR_BLACK = cv::Scalar(0.0, 0.0, 0.0);
const cv::Scalar SCALAR_WHITE = cv::Scalar(255.0, 255.0, 255.0);
const cv::Scalar SCALAR_YELLOW = cv::Scalar(0.0, 255.0, 255.0);
const cv::Scalar SCALAR_GREEN = cv::Scalar(0.0, 200.0, 0.0);
const cv::Scalar SCALAR_RED = cv::Scalar(0.0, 0.0, 255.0);

class OpenCV {


public:

	//param constructor to initialise the video stream path
	// i.e. IP camera or local video on file system
	OpenCV(const std::string& videoPath)
	{
		m_videoPath = videoPath;
		m_thread = 0;
		pthread_mutex_init(&m_mutex, NULL);
		intVerticalLinePosition = 0;
		vehicleCount = 0;
	}

	OpenCV();
	OpenCV(const OpenCV& orig);
	virtual ~OpenCV();

	void runAsThread(std::string& videoPath);

	static void* runLaneCamThread(void* self);

	// function prototypes
	void matchCurrentFrameBlobsToExistingBlobs(std::vector<Blob> &existingBlobs, std::vector<Blob> &currentFrameBlobs);
	void addBlobToExistingBlobs(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs, int &intIndex);
	void addNewBlob(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs);
	double distanceBetweenPoints(cv::Point point1, cv::Point point2);
	void drawAndShowContours(cv::Size imageSize, std::vector<std::vector<cv::Point> > contours, std::string strImageName);
	void drawAndShowContours(cv::Size imageSize, std::vector<Blob> blobs, std::string strImageName);
	bool checkIfBlobsCrossedTheLine(std::vector<Blob> &blobs, int &intHorizontalLinePosition, int &vehicleCount);
	void drawBlobInfoOnImage(std::vector<Blob> &blobs, cv::Mat &imgFrame2Copy);
	void drawvehicleCountOnImage(int &vehicleCount, cv::Mat &imgFrame2Copy);

	int getvehicleCount()  {
		return vehicleCount;
	}

	void setvehicleCount(int vehicleCount) {
		this->vehicleCount = vehicleCount;
	}
private:
	std::string m_videoPath;
	pthread_t m_thread;
	pthread_mutex_t m_mutex;
	int intVerticalLinePosition;
	int vehicleCount;
};

#endif /* OPENCV_H_ */
