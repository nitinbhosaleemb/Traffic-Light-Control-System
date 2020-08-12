/*******************************************************************************
 **
 **
 ** File Name: OpenCV.cpp
 ** Project: Traffic Light Control and Management System using Open CV
 ** Creation Date:
 **
 **
 *******************************************************************************/

#include "OpenCV.h"


OpenCV::OpenCV() {
}

OpenCV::OpenCV(const OpenCV& orig) {
}

OpenCV::~OpenCV() {
	pthread_mutex_destroy(&m_mutex);
}

/*!
 * @brief spawn a thread to detect vehicles on lane
 * @param videoPath - video file path or URL of IP camera or USB device
 * @return void
 */
void OpenCV::runAsThread(std::string& videoPath)
{
	int error;
	if ((error = pthread_create(&m_thread, NULL, &OpenCV::runLaneCamThread, this)) != 0)
	{
		std::cout << "Thread can't be created : " << strerror(error) << std::endl;
	}
}

/*!
 * @brief function handler thread to capture lane video and detect moving vehicles
 * @param void * - parameters needed to access static function
 * @return void
 */
void* OpenCV::runLaneCamThread(void* self)
{
	OpenCV* pOpenCV = static_cast<OpenCV *>(self);

	//run thread to capture vehicles on respective lane
	while (1)
	{
		cv::VideoCapture capVideo("../../src/test4.mp4");

		cout<<"\n######## pOpenCV->_videoPath::" <<pOpenCV->m_videoPath.c_str()<< endl;
		//capVideo.open(pOpenCV->m_videoPath.c_str());

		cv::Mat imgFrame1;
		cv::Mat imgFrame2;
		std::vector<Blob> blobs;
		cv::Point crossingLine[2];

		//check of video capture is open
		if (!capVideo.isOpened()) {                                                 // if unable to open video file
			std::cout << "error reading video file" << std::endl;                   // show error message
			// exit the current thread
			pthread_exit(NULL);                                                         // and exit program
		}

		if (capVideo.get(cv::CAP_PROP_FRAME_COUNT) < 2) {
			std::cout << "error: video file must have at least two frames";
			// exit the current thread
			pthread_exit(NULL);                                                         // and exit thread
		}

		capVideo.read(imgFrame1);
		capVideo.read(imgFrame2);

		//control line for vehicle count
		int intHorizontalLinePosition = (int)std::round((double)imgFrame1.rows * 0.35);
		intHorizontalLinePosition = intHorizontalLinePosition*1.40;
		pOpenCV->intVerticalLinePosition = (int)std::round((double)imgFrame1.cols * 0.35);

		crossingLine[0].x = 515;
		crossingLine[0].y = intHorizontalLinePosition;

		crossingLine[1].x = imgFrame1.cols - 1;
		crossingLine[1].y = intHorizontalLinePosition;

		char chCheckForEscKey = 0;
		bool blnFirstFrame = true;
		int frameCount = 2;

		while (capVideo.isOpened() && chCheckForEscKey != 27) {
			std::vector<Blob> currentFrameBlobs;
			cv::Mat imgFrame1Copy = imgFrame1.clone();
			cv::Mat imgFrame2Copy = imgFrame2.clone();
			cv::Mat imgDifference;
			cv::Mat imgThresh;
			cv::cvtColor(imgFrame1Copy, imgFrame1Copy, CV_BGR2GRAY);
			cv::cvtColor(imgFrame2Copy, imgFrame2Copy, CV_BGR2GRAY);
			cv::GaussianBlur(imgFrame1Copy, imgFrame1Copy, cv::Size(5, 5), 0);
			cv::GaussianBlur(imgFrame2Copy, imgFrame2Copy, cv::Size(5, 5), 0);
			cv::absdiff(imgFrame1Copy, imgFrame2Copy, imgDifference);
			cv::threshold(imgDifference, imgThresh, 30, 255.0, CV_THRESH_BINARY);
			//cv::imshow("imgThresh", imgThresh);
			cv::Mat structuringElement3x3 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
			cv::Mat structuringElement5x5 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
			cv::Mat structuringElement7x7 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));
			cv::Mat structuringElement15x15 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15));

			for (unsigned int i = 0; i < 2; i++) {
				cv::dilate(imgThresh, imgThresh, structuringElement5x5);
				cv::dilate(imgThresh, imgThresh, structuringElement5x5);
				cv::erode(imgThresh, imgThresh, structuringElement5x5);
			}

			cv::Mat imgThreshCopy = imgThresh.clone();
			std::vector<std::vector<cv::Point> > contours;
			cv::findContours(imgThreshCopy, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

			//pOpenCV->drawAndShowContours(imgThresh.size(), contours, "imgContours");

			std::vector<std::vector<cv::Point> > convexHulls(contours.size());

			for (unsigned int i = 0; i < contours.size(); i++) {
				cv::convexHull(contours[i], convexHulls[i]);
			}

			//pOpenCV->drawAndShowContours(imgThresh.size(), convexHulls, "imgConvexHulls");

			for (auto &convexHull : convexHulls) {
				Blob possibleBlob(convexHull);

				if (possibleBlob.currentBoundingRect.area() > 400 &&
					possibleBlob.dblCurrentAspectRatio > 0.2 &&
					possibleBlob.dblCurrentAspectRatio < 4.0 &&
					possibleBlob.currentBoundingRect.width > 30 &&
					possibleBlob.currentBoundingRect.height > 30 &&
					possibleBlob.dblCurrentDiagonalSize > 60.0 &&
					(cv::contourArea(possibleBlob.currentContour) / (double)possibleBlob.currentBoundingRect.area()) > 0.50) {
						currentFrameBlobs.push_back(possibleBlob);
				}
			}

			//pOpenCV->drawAndShowContours(imgThresh.size(), currentFrameBlobs, "imgCurrentFrameBlobs");

			if (blnFirstFrame == true) {
				for (auto &currentFrameBlob : currentFrameBlobs) {
					blobs.push_back(currentFrameBlob);
				}
			}
			else {
				pOpenCV->matchCurrentFrameBlobsToExistingBlobs(blobs, currentFrameBlobs);
			}

			//pOpenCV->drawAndShowContours(imgThresh.size(), blobs, "imgBlobs");

			imgFrame2Copy = imgFrame2.clone();	// get another copy of frame 2 since we changed the previous frame 2 copy in the processing above

			pOpenCV->drawBlobInfoOnImage(blobs, imgFrame2Copy);

			// Check the vehicle crossed the red line
			bool blnAtLeastOneBlobCrossedTheLine = pOpenCV->checkIfBlobsCrossedTheLine(blobs, intHorizontalLinePosition, pOpenCV->vehicleCount);

			//draw red line on video image for vehicle detection, or draw green if not detected
			if (blnAtLeastOneBlobCrossedTheLine == true) {
				cv::line(imgFrame2Copy, crossingLine[0], crossingLine[1], SCALAR_GREEN, 2);
			}
			else if (blnAtLeastOneBlobCrossedTheLine == false) {
				cv::line(imgFrame2Copy, crossingLine[0], crossingLine[1], SCALAR_RED, 2);
			}

			pOpenCV->drawvehicleCountOnImage(pOpenCV->vehicleCount, imgFrame2Copy);

			cv::imshow("Camera Video", imgFrame2Copy);


			// now we prepare for the next iteration
			currentFrameBlobs.clear();

			imgFrame1 = imgFrame2.clone();	// move frame 1 up to where frame 2 is

			if ((capVideo.get(cv::CAP_PROP_POS_FRAMES) + 1) < capVideo.get(cv::CAP_PROP_FRAME_COUNT)) {
				capVideo.read(imgFrame2);
			}
			else
			{
				std::cout << "@@@@@@@@@@@@ end of video\n";
				break;
			}

			blnFirstFrame = false;
			frameCount++;
			chCheckForEscKey = cv::waitKey(1);

		}
		// note that if the user did press esc, we don't need to hold the windows open, we can simply let the program end which will close the windows
		if (chCheckForEscKey != 27) {               // if the user did not press esc (i.e. we reached the end of the video)
			cv::waitKey(0);                         // hold the windows open to allow the "end of video" message to show
		}

		//release frames captured and close all video windows
		capVideo.release ();
		cv::destroyAllWindows();

		//reset the vehicle count once video capture ends
		pOpenCV->vehicleCount = 0;
	}

	// exit the current thread
	pthread_exit(NULL);
}


void OpenCV::matchCurrentFrameBlobsToExistingBlobs(std::vector<Blob> &existingBlobs, std::vector<Blob> &currentFrameBlobs) {
    for (auto &existingBlob : existingBlobs) {
        existingBlob.blnCurrentMatchFoundOrNewBlob = false;
        existingBlob.predictNextPosition();
    }

    for (auto &currentFrameBlob : currentFrameBlobs) {
        int intIndexOfLeastDistance = 0;
        double dblLeastDistance = 100000.0;

        for (unsigned int i = 0; i < existingBlobs.size(); i++) {

            if (existingBlobs[i].blnStillBeingTracked == true) {
                double dblDistance = distanceBetweenPoints(currentFrameBlob.centerPositions.back(), existingBlobs[i].predictedNextPosition);

                if (dblDistance < dblLeastDistance) {
                    dblLeastDistance = dblDistance;
                    intIndexOfLeastDistance = i;
                }
            }
        }

        if (dblLeastDistance < currentFrameBlob.dblCurrentDiagonalSize * 0.5) {
            addBlobToExistingBlobs(currentFrameBlob, existingBlobs, intIndexOfLeastDistance);
        }
        else {
            addNewBlob(currentFrameBlob, existingBlobs);
        }

    }

    for (auto &existingBlob : existingBlobs) {
        if (existingBlob.blnCurrentMatchFoundOrNewBlob == false) {
            existingBlob.intNumOfConsecutiveFramesWithoutAMatch++;
        }
        if (existingBlob.intNumOfConsecutiveFramesWithoutAMatch >= 5) {
            existingBlob.blnStillBeingTracked = false;
        }
    }
}


void OpenCV::addBlobToExistingBlobs(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs, int &intIndex) {
    existingBlobs[intIndex].currentContour = currentFrameBlob.currentContour;
    existingBlobs[intIndex].currentBoundingRect = currentFrameBlob.currentBoundingRect;
    existingBlobs[intIndex].centerPositions.push_back(currentFrameBlob.centerPositions.back());
    existingBlobs[intIndex].dblCurrentDiagonalSize = currentFrameBlob.dblCurrentDiagonalSize;
    existingBlobs[intIndex].dblCurrentAspectRatio = currentFrameBlob.dblCurrentAspectRatio;
    existingBlobs[intIndex].blnStillBeingTracked = true;
    existingBlobs[intIndex].blnCurrentMatchFoundOrNewBlob = true;
}


void OpenCV::addNewBlob(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs) {
    currentFrameBlob.blnCurrentMatchFoundOrNewBlob = true;
    existingBlobs.push_back(currentFrameBlob);
}


double OpenCV::distanceBetweenPoints(cv::Point point1, cv::Point point2) {
    int intX = abs(point1.x - point2.x);
    int intY = abs(point1.y - point2.y);

    return(sqrt(pow(intX, 2) + pow(intY, 2)));
}


void OpenCV::drawAndShowContours(cv::Size imageSize, std::vector<std::vector<cv::Point> > contours, std::string strImageName) {
	cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK);
    cv::drawContours(image, contours, -1, SCALAR_WHITE, -1);
    cv::imshow(strImageName, image);
}


void OpenCV::drawAndShowContours(cv::Size imageSize, std::vector<Blob> blobs, std::string strImageName) {
    cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK);
    std::vector<std::vector<cv::Point> > contours;

    for (auto &blob : blobs) {
        if (blob.blnStillBeingTracked == true) {
            contours.push_back(blob.currentContour);
        }
    }

    cv::drawContours(image, contours, -1, SCALAR_WHITE, -1);
    cv::imshow(strImageName, image);
}


bool OpenCV::checkIfBlobsCrossedTheLine(std::vector<Blob> &blobs, int &intHorizontalLinePosition, int &vehicleCount) {
	bool blnAtLeastOneBlobCrossedTheLine = false;

    for (auto blob : blobs) {
        if (blob.blnStillBeingTracked == true && blob.centerPositions.size() >= 2) {
            int prevFrameIndex = (int)blob.centerPositions.size() - 2;
            int currFrameIndex = (int)blob.centerPositions.size() - 1;

			// Left way
			if (blob.centerPositions[prevFrameIndex].y > intHorizontalLinePosition && blob.centerPositions[currFrameIndex].y <= intHorizontalLinePosition && blob.centerPositions[currFrameIndex].x > 350) {
				vehicleCount++;
                blnAtLeastOneBlobCrossedTheLine = true;
            }
        }
    }

    return blnAtLeastOneBlobCrossedTheLine;
}

void OpenCV::drawBlobInfoOnImage(std::vector<Blob> &blobs, cv::Mat &imgFrame2Copy) {
    for (unsigned int i = 0; i < blobs.size(); i++) {
        if (blobs[i].blnStillBeingTracked == true) {
            cv::rectangle(imgFrame2Copy, blobs[i].currentBoundingRect, SCALAR_RED, 2);

			int intFontFace = cv::FONT_HERSHEY_SIMPLEX;
			double dblFontScale = (imgFrame2Copy.rows * imgFrame2Copy.cols) / 300000.0;
            int intFontThickness = (int)std::round(dblFontScale * 1.0);

            cv::putText(imgFrame2Copy, std::to_string(i), blobs[i].centerPositions.back(), intFontFace, dblFontScale, SCALAR_GREEN, intFontThickness);
        }
    }
}


void OpenCV::drawvehicleCountOnImage(int &vehicleCountRight, cv::Mat &imgFrame2Copy) {
    int intFontFace = cv::FONT_HERSHEY_SIMPLEX;
    double dblFontScale = (imgFrame2Copy.rows * imgFrame2Copy.cols) / 450000.0;
    int intFontThickness = (int)std::round(dblFontScale * 2.5);

	// Right way
	cv::Size textSize = cv::getTextSize(std::to_string(vehicleCountRight), intFontFace, dblFontScale, intFontThickness, 0);
	cv::putText(imgFrame2Copy, "Vehicle count:" + std::to_string(vehicleCountRight), cv::Point(160,45), intFontFace, dblFontScale, SCALAR_RED, intFontThickness);
}


