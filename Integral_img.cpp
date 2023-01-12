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


double GetValueID(Mat img, int i, int j, int n)//���n���ϼƭ� uchar??
{
	double value;
	double A, B, C, D;
	A = img.at<double>(j + n, i + n);
	B = (i - n - 1 < 0) ? 0 : img.at<double>(j + n, i - n - 1);//����false ,�_�h�p��
	C = (j - n - 1 < 0) ? 0 : img.at<double>(j - n - 1, i + n);
	D = ((j - n - 1 < 0) || (i - n - 1 < 0)) ? 0 : img.at<double>(j - n - 1, i - n - 1);
	value = A - B - C + D;
	return value;
}

void intergrate_img(Mat img, Mat &inter_img)//�s�@�n����
{
	Mat gray;
	//cvtColor(img, gray, COLOR_BGR2GRAY);
	ConverToGrayandDouble(img, gray);
	inter_img = Mat::zeros(gray.rows, gray.cols, CV_64F);
	inter_img.at<double>(0, 0) = gray.at<double>(0, 0);

	for (int r = 1; r < gray.rows; r++)//y�b (0,y)
		inter_img.at<double>(r, 0) = inter_img.at<double>(r - 1, 0) + gray.at<double>(r, 0);

	for (int c = 1; c < gray.cols; c++)//x�b(x,0)
		inter_img.at<double>(0, c) = inter_img.at<double>(0, c - 1) + gray.at<double>(0, c);

	double *ptr, *ptrB, *ptrD;
	for (int r = 1; r < img.rows; r++)//y�b (0,y)
	{
		ptr = gray.ptr<double>(r);
		ptrB = inter_img.ptr<double>(r - 1);//A is (r-1,c-1)
		ptrD = inter_img.ptr<double>(r);//C is (r,c-1)
		for (int c = 1; c < img.cols; c++)
			ptrD[c] = ptrD[c - 1] + ptr[c] + ptrB[c] - ptrB[c - 1];//�s�W�n���϶�
	}
}

void Calmean_std(Mat img, float *std, float *mean, int kernel)//��i�v����
{
	int bound = (kernel - 1) / 2;
	Mat img_rel;//output
	//�ϥοn���ϥ[�t(�ƥ��p��n�϶�)	
	intergrate_img(img, img_rel);
	int total = kernel * kernel;

	// pos;//�y���x�s��m
	//int col = img.cols;
	float sum;
	for (int j = bound; j < img.rows - bound; j++)
	{
		for (int i = bound; i < img.cols - bound; i++)
		{
			unsigned long pos = j*img.cols + i;//�۹��m
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
					std[pos] += diff*diff;//2����
				}
			}
			std[pos] = sqrt(std[pos] / block);//standard diff
											  //printf("%.2f\n", mean[pos]);
		}
	}
}