#include "img_manage.h"

/*
* main.cpp
*
* Copyright (C) Lin Shu You, 2022
*
* This applies stereo vision Block matching.
* Include NCC matcing, integral image, LRC check
* Last update date: 2022 / 07 / 26
*
*/

int main(void)
{
	Mat image1 = imread("D:\\im2.png");//color image
	Mat image2 = imread("D:\\im1.png");
	//cout << "\ninput image size: " << image1.cols << " " << image1.rows << " " << image1.channels() << "\n";
	double time1 = static_cast<double>(getTickCount());
	Mat disp1, disp2;// = Mat::zeros(image1.rows * 2, image1.cols * 2, CV_8UC3);//,disp2;
	Mat dshift1, dshift2; //16位元 視差儲存 

	//Mat dshift = Mat::zeros(image1.size(), CV_8U);
	cvtColor(image1, image1, COLOR_BGR2GRAY);//use gray_level image
	cvtColor(image2, image2, COLOR_BGR2GRAY);

	//no integral image processing
	//NCC(image1, image2, dshift1);

	//integral image processing
	disp1 = ncc(image1, image2, dshift1, "right");
	disp2 = ncc(image1, image2, dshift2, "left");

	imshow("Display img1", disp1);
	imshow("Display img2", disp2);
	////imwrite("l.png", disp1);//dshift1 ncc程式內傳入
	////imwrite("2.png", disp2);//dshift1 ncc程式內傳入
	//imshow("Display img1", dshift1);
	//Mat result = Mat::zeros(dshift1.size(), CV_16U);
	//Mat result2 = Mat::zeros(dshift1.size(), CV_16U);

	//////
	time1 = ((double)getTickCount() - time1) / getTickFrequency(); //計時器结束
	printf("CPU所運算的時間：%f s\n", time1);
	waitKey(0);//邊
	system("pause");
	return 0;
}

