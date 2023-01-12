#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include <time.h>
#include <limits> 
#include <cmath>
#include <algorithm>
#include <cstdlib>

#define PI 3.14
#define block  (float)49 //block 7x7
#define kernal  7 
#define search_range  60  //Block matching search range
#define xstart 0
#define ystart 0
#define yend 0
#define thr 1.5
#define LRCthr 1

using namespace std; //C++ »yªk¥Î
using namespace cv;

void NCC(Mat img1, Mat img2, Mat &dshift);
Mat ncc(Mat left, Mat right, Mat &dshift, string type);
Mat RLChecky(Mat dshift1, Mat dshift2);
Mat LRChecky(Mat dshift1, Mat dshift2);