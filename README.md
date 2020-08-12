# Traffic Control and Management System using  BMC 2835 Raspberry PI and OpenCV

**Using Vehicle Detection, Tracking and Counting by OpenCV and Controlling Traffic Light**

This project focuses on detecting, tracking and counting vehicles by using "[Blob Detection](http://www.learnopencv.com/blob-detection-using-opencv-python-c/)" method under OpenCV library. The unique ID is assigned to the each vehicle so it can not be counted more than once. This project was developed by OpenCV library. This vehicle count is taken as input to the Traffic Control System, which runs a algorithm to make traffic flow ease by increasing GREEN signal time.


## Theory
Target representation and localization is mostly a bottom-up process. These methods give a variety of tools for identifying the moving object. Locating and tracking the target object successfully is dependent on the algorithm. For example, using blob tracking is useful for identifying, tracking human/vehicle movement because a person's/vehicle's profile changes dynamically. Typically the computational complexity for these algorithms is low. The project utlizes the [Blob Tracking](https://github.com/ahmetozlu/vehicle_counting/blob/master/VehicleDetectionAndCounting/Blob.cpp) module which provides various parameters for connecting blobs on video frames in order to determine movements of vehicles. 

## Installation

**Building the project using CMake from the command-line:**

**Libraries Required To Compile:: **
**OpenCV, **
**PTHREAD, **
**BMC 2835, **
**Wiring PI **

** Linux: **

    export OpenCV_DIR="~/OpenCV/build"
    mkdir build
    cd build
    cmake -D OpenCV_DIR=$OpenCV_DIR ../
    make 


**What is Blob Detection?**

A Blob is a group of connected pixels in an image that share some common property ( E.g grayscale value ). Blob detection methods are aimed at detecting regions in a digital image that differ in properties, such as brightness or color, compared to surrounding regions. Informally, a blob is a region of an image in which some properties are constant or approximately constant; all the points in a blob can be considered in some sense to be similar to each other. The most common method for blob detection is convolution.


**How does Blob detection work ?**

The algorithm for extracting blobs from an image:

1. Convert the source image to binary images by applying thresholding with several thresholds from minThreshold (inclusive) to maxThreshold (exclusive) with distance thresholdStep between neighboring thresholds.

2. Extract connected components from every binary image by findContours and calculate their centers.

3. Group centers from several binary images by their coordinates. Close centers form one group that corresponds to one blob, which is controlled by the minDistBetweenBlobs parameter.

4. From the groups, estimate final centers of blobs and their radiuses and return as locations and sizes of keypoints.


