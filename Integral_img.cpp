#include "img_manage.h"

void ConverToGrayandDouble(Mat img, Mat &img_gray)
{
	Mat imgTemp;
	if (img.channels() == 3)
		cvtColor(img, imgTemp, CV_BGR2GRAY);
	else
		img.copyTo(imgTemp);

	//Generate the floating point image.
	//convert 0~255 range into 0~1
	imgTemp.convertTo(img_gray, CV_64F);
	img_gray = img_gray / 255.0;
}


double GetValueID(Mat img, int i, int j, int n)//取積分圖數值 uchar??
{
	double value;
	double A, B, C, D;
	A = img.at<double>(j + n, i + n);
	B = (i - n - 1 < 0) ? 0 : img.at<double>(j + n, i - n - 1);//條件為false ,否則計算
	C = (j - n - 1 < 0) ? 0 : img.at<double>(j - n - 1, i + n);
	D = ((j - n - 1 < 0) || (i - n - 1 < 0)) ? 0 : img.at<double>(j - n - 1, i - n - 1);
	value = A - B - C + D;
	return value;
}

void intergrate_img(Mat img, Mat &inter_img)//製作積分圖
{
	Mat gray;
	//cvtColor(img, gray, COLOR_BGR2GRAY);
	ConverToGrayandDouble(img, gray);
	inter_img = Mat::zeros(gray.rows, gray.cols, CV_64F);
	inter_img.at<double>(0, 0) = gray.at<double>(0, 0);

	for (int r = 1; r < gray.rows; r++)//y軸 (0,y)
		inter_img.at<double>(r, 0) = inter_img.at<double>(r - 1, 0) + gray.at<double>(r, 0);

	for (int c = 1; c < gray.cols; c++)//x軸(x,0)
		inter_img.at<double>(0, c) = inter_img.at<double>(0, c - 1) + gray.at<double>(0, c);

	double *ptr, *ptrB, *ptrD;
	for (int r = 1; r < img.rows; r++)//y軸 (0,y)
	{
		ptr = gray.ptr<double>(r);
		ptrB = inter_img.ptr<double>(r - 1);//A is (r-1,c-1)
		ptrD = inter_img.ptr<double>(r);//C is (r,c-1)
		for (int c = 1; c < img.cols; c++)
			ptrD[c] = ptrD[c - 1] + ptr[c] + ptrB[c] - ptrB[c - 1];//新增積分區塊
	}
}

void Calmean_std(Mat img, float *std, float *mean, int kernel)//兩張影像的
{
	int bound = (kernel - 1) / 2;
	Mat img_rel;//output
	//使用積分圖加速(事先計算好區塊)	
	intergrate_img(img, img_rel);
	int total = kernel * kernel;

	// pos;//座標儲存位置
	//int col = img.cols;
	float sum;
	for (int j = bound; j < img.rows - bound; j++)
	{
		for (int i = bound; i < img.cols - bound; i++)
		{
			unsigned long pos = j*img.cols + i;//相對位置
			sum = GetValueID(img_rel, i, j, bound) * 255;
			mean[pos] = sum / block;
			std[pos] = 0;
			//local block
			for (int a = -bound; a <= bound; a++)
			{
				uchar *ptr1 = img.ptr<uchar>(j + a);
				for (int b = -bound; b <= bound; b++)
				{
					float diff = *(ptr1 + i + b) - mean[pos];
					std[pos] += diff*diff;//2次方
				}
			}
			std[pos] = sqrt(std[pos] / block);//standard diff
											  //printf("%.2f\n", mean[pos]);
		}
	}
}