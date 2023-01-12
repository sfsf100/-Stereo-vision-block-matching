//#pragma once
#include "img_manage.h"

void Calmean_std(Mat img, float *std, float *mean, int kernel);

void NCC(Mat img1, Mat img2, Mat &dshift)
{
	dshift = Mat::zeros(img1.size(), CV_8U);
	int half = (kernal - 1) / 2;//block 一半
	int width = img1.cols; //影像讀取寬度
	int high = img1.rows; //高度，因兩張影像大小相同只需取一張影像的寬高資料。
	
	float result = 0;
	int c, q;//counter
			 //float right_mean, right_std;//不要在迴圈裡
	float std1, std2, nominator;
	float res_ncc;
	float e[kernal*kernal], a[kernal*kernal];
	float mean_L, mean_R;
	for (int y = half; y <= high - half; y++)//整張影像
	{
		for (int x = half; x <= width - half; x++)
		{
			c = 0;
			mean_L = 0;
			for (int m = y - half; m <= y + half; m++)//referencfe image: block range
			{
				for (int n = x - half; n <= x + half; n++)
				{
					e[c] = img1.at<uchar>(m, n);
					mean_L += img1.at<uchar>(m, n);
					c++;
				}
			}
			mean_L /= block;

			result = 0;
			for (int j = y; j <= y; j++)//y+10
			{

				for (int i = x; i < x + search_range; i++)
				{
					if (i + half < width)//j + half < high &&  &&j - half >0
					{

						res_ncc = 0;
						std2 = 0;
						q = 0;
						std1 = 0;
						nominator = 0;
						mean_R = 0;
						for (int r = j - half; r <= (j + half); r++) //起始點範圍
						{
							for (int t = i - half; t <= (i + half); t++)//start point  end range 7x7 block
							{
								a[q] = img2.at<uchar>(r, t);
								mean_R += img2.at<uchar>(r, t);
								q++;
							}
						}
						mean_R /= block;

						for (int l = 0; l < q; l++)
						{
							nominator += (a[l] - mean_R)*(e[l] - mean_L);
							std1 += (a[l] - mean_R)*(a[l] - mean_R);
							std2 += (e[l] - mean_L)*(e[l] - mean_L);
						}


						res_ncc = (nominator / block) / (sqrt(std1 / block) * sqrt(std2 / block)); //NCC 歸一化計算

						if (res_ncc > result)
						{
							result = res_ncc;
							dshift.at<uchar>(y, x) = (uchar)(60 + (i - x) * 1);// *3 灰階度 0-255 disparity map				
						}
					}
				}
			}
		}
	}
}


Mat ncc(Mat left, Mat right, Mat &dshift, string type)
{
	dshift = Mat::zeros(left.size(), CV_16U);
	int half = (kernal - 1) / 2;//block 一半
	int width = left.cols; //影像讀取寬度
	int high = left.rows; //高度，因兩張影像大小相同只需取一張影像的寬高資料。
	float result = 0;
	int c, q;//counter
	float std2;
	float res_ncc;
	float e[kernal*kernal], a[kernal*kernal];
	Mat disparity(high, width, 0);//Bulid disparity map image.

	double time1 = static_cast<double>(getTickCount());

	int size = width*high;
	float *std1 = (float*)malloc(sizeof(float)*size);
	float *mean1 = (float*)malloc(sizeof(float) * size);
	float *std22 = (float*)malloc(sizeof(float)*size);
	float *mean2 = (float*)malloc(sizeof(float)*size);
	Calmean_std(right, std1, mean1, kernal);
	Calmean_std(left, std22, mean2, kernal);
	time1 = ((double)getTickCount() - time1) / getTickFrequency(); //計時器结束
	printf("Integral image CPU所運算的時間：%f s\n", time1);
	if (type == "right")//
	{
		for (int y = half; y <= high - half; y++)
		{
			for (int x = width - half; x >= half; x--)
			{
				c = 0;
				int posr = y*right.cols + x;
				for (int m = y - half; m <= y + half; m++)//referencfe image: block range
				{
					for (int n = x - half; n <= x + half; n++)
					{
						e[c] = right.at<uchar>(m, n);
						c++;
					}
				}
				//printf("right_mean %.1f mean %.1f\n", right_mean,mean1[posr]);
				result = 0;
				for (int j = y; j <= y + yend; j++)//y+10
				{
					for (int i = x; i > x - search_range; i--)
					{
						if (j - half > 0 && i - half > 0 && j + half < high)
						{
							res_ncc = 0;
							std2 = 0;
							q = 0;
							int posl = j*left.cols + i;
							/*if (std22[posl] <= thr)
							continue;*/
							for (int r = j - half; r <= (j + half); r++) //起始點範圍
							{
								for (int t = i - half; t <= (i + half); t++)//start point  end range 7x7 block
								{

									a[q] = left.at<uchar>(r, t);
									q++;
									//printf(" x=%d y=%d r=%d t=%d  \n",x,y,r,t);
								}
							}
							//printf("left mean %.1f mean %.1f\n", left_mean,mean2[posl]);		
							for (int l = 0; l < q; l++)
							{
								std2 += (a[l] - mean2[posl])*(e[l] - mean1[posr]);
							}
							res_ncc = abs(std2 / block) / (std22[posl] * std1[posr]); //NCC 歸一化計算
																					  //printf("%d %d %d %d %.1f\n", y, x,j,i, res_ncc);
							if (res_ncc > result)
							{
								result = res_ncc;
								disparity.at<uchar>(y, x) = (uchar)(60 + (x - i) * 3);// *3 灰階度 0-255 disparity map				
								dshift.at<ushort>(y, x) = (x - i);	 //printf("%d %d %d %d %d %.1f\n", y, x,j,i, (i - x),res_ncc);    					
																	 //printf("%d %d %d %d ncc=%.1f\n", y, x,j,i,res_ncc);
							}
						}
					}
				}
			}
		}
		return disparity;
	}
	else
	{
		for (int y = half; y <= high - half; y++)//整張影像
		{
			for (int x = half; x <= width - half; x++)
			{
				c = 0;
				for (int m = y - half; m <= y + half; m++)//referencfe image: block range
				{
					for (int n = x - half; n <= x + half; n++)
					{
						e[c] = left.at<uchar>(m, n);
						c++;
					}
				}
				int posr = y*left.cols + x;
				result = 0;
				for (int j = y; j <= y; j++)//y+10
				{
					for (int i = x; i < x + search_range; i++)
					{
						if (i + half < width)//j + half < high &&  &&j - half >0
						{
							//res_ncc = 0;
							std2 = 0;
							q = 0;

							int posl = j*left.cols + i;
							for (int r = j - half; r <= (j + half); r++) //起始點範圍
							{
								for (int t = i - half; t <= (i + half); t++)//start point  end range 7x7 block
								{
									a[q] = right.at<uchar>(r, t);
									q++;
								}
							}

							for (int l = 0; l < q; l++)
								std2 += (a[l] - mean1[posl])*(e[l] - mean2[posr]);

							res_ncc = (std2 / block) / (std22[posr] * std1[posl]); //NCC 歸一化計算

							if (res_ncc > result)
							{
								result = res_ncc;
								disparity.at<uchar>(y, x) = (uchar)(60 + (i - x) * 3);
								dshift.at<uchar>(y, x) = (uchar)(60 + (i - x) * 1);// *3 灰階度 0-255 disparity map				
							}
						}
					}
				}
			}
		}
		return disparity;
	}

	free(mean1);
	free(mean2);
	free(std1);
	free(std22);
}

// 左右一致性去遮擋
Mat LRChecky(Mat dshift1, Mat dshift2)  //right left
{
	int d;
	Mat dis = Mat::zeros(dshift1.size(), CV_16U);
	for (int j = 0; j < dshift1.rows; j++)
	{
		ushort *ptr1 = dshift2.ptr<ushort>(j);// left image pointer 16bit information
											  //ushort *ptrd = dshift1.ptr<ushort>(j);//right image
		for (int i = 0; i < dshift1.cols; i++)
		{
			d = ptr1[i];
			if (abs(dshift1.at<ushort>(j, i + d) - d) <= LRCthr)
			{
				dis.at<ushort>(j, i) = d;
				//dis.at<uchar>(j, i) = 255;
			}
			//dis.at<uchar>(j, i) = 0;
		}
	}
	return dis;
}

Mat RLChecky(Mat dshift1, Mat dshift2)  // left right
{
	int d;
	Mat dis = Mat::zeros(dshift1.size(), CV_16U);
	for (int j = 0; j < dshift1.rows; j++)
	{
		//ushort *ptr1 = dshift1.ptr<ushort>(j);// left image pointer 16bit information
		ushort *ptrd = dshift2.ptr<ushort>(j);//right image
		for (int i = 0; i < dshift1.cols; i++)
		{
			d = ptrd[i];
			if (abs(dshift1.at<ushort>(j, i - d) - d) <= LRCthr)
			{
				dis.at<ushort>(j, i) = d;
				//dis.at<uchar>(j, i) = 255;
			}
			//dis.at<uchar>(j, i) = 0;
		}
	}
	return dis;
}
