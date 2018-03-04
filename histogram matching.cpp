#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

void imhist(Mat image, int histogram[])
{

	
	for (int i = 0; i < 256; i++)
	{
		histogram[i] = 0;
	}

	
	for (int y = 0; y < image.rows; y++)
		for (int x = 0; x < image.cols; x++)
			histogram[(int)image.at<uchar>(y, x)]++;

}

void cumhist(int histogram[], int cumhistogram[])
{
	cumhistogram[0] = histogram[0];

	for (int i = 1; i < 256; i++)
	{
		cumhistogram[i] = histogram[i] + cumhistogram[i - 1];
	}
}


void cumgoshist(float histogram[], float cumhistogram[])
{
	cumhistogram[0] = histogram[0];

	for (int i = 1; i < 256; i++)
	{
		cumhistogram[i] = histogram[i] + cumhistogram[i - 1];
	}
}

void histDisplay(int histogram[], const char* name)
{
	int hist[256];
	for (int i = 0; i < 256; i++)
	{
		hist[i] = histogram[i];
	}
	
	int hist_w = 512; int hist_h = 400;
	int bin_w = cvRound((double)hist_w / 256);

	Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(255, 255, 255));

	
	int max = hist[0];
	for (int i = 1; i < 256; i++) {
		if (max < hist[i]) {
			max = hist[i];
		}
	}

	for (int i = 0; i < 256; i++) {
		hist[i] = ((double)hist[i] / max)*histImage.rows;
	}


	
	for (int i = 0; i < 256; i++)
	{
		line(histImage, Point(bin_w*(i), hist_h),
			Point(bin_w*(i), hist_h - hist[i]),
			Scalar(0, 0, 0), 1, 8, 0);
	}

	namedWindow(name, CV_WINDOW_AUTOSIZE);
	imshow(name, histImage);
}

void histDis(float histogram[], const char* name)
{
	float hist[256];
	for (int i = 0; i < 256; i++)
	{
		hist[i] = histogram[i];
	}
	
	int hist_w = 512; int hist_h = 400;
	int bin_w = cvRound((double)hist_w / 256);

	Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(255, 255, 255));

	
	float max = hist[0];
	for (int i = 1; i < 256; i++) {
		if (max < hist[i]) {
			max = hist[i];
		}
	}

	

	for (int i = 0; i < 256; i++) {
		hist[i] = ((double)hist[i] / max)*histImage.rows;
	}


	
	for (int i = 0; i < 256; i++)
	{
		line(histImage, Point(bin_w*(i), hist_h),
			Point(bin_w*(i), hist_h - hist[i]),
			Scalar(0, 0, 0), 1, 8, 0);
	}

	
	namedWindow(name, CV_WINDOW_AUTOSIZE);
	imshow(name, histImage);
}

int main()
{

	Mat image = imread("equ.png", 0);

	
	int histogram[256];
	imhist(image, histogram);

	
	int size = image.rows * image.cols;
	float alpha = 255.0 / size;

	
	float PrRk[256];
	for (int i = 0; i < 256; i++)
	{
		PrRk[i] = (double)histogram[i] / size;
	}

	
	int cumhistogram[256];
	float cumgos[256];
	cumhist(histogram, cumhistogram);

	
	int Sk[256];
	for (int i = 0; i < 256; i++)
	{
		Sk[i] = cvRound((double)cumhistogram[i] * alpha);
	}


	float gos[256];
	float sigma;
	int median;
	cout << "Enter value of sigma : " << endl;
	cin >> sigma;
	cout << "Enter value of median : " << endl;
	cin >> median;


	for (int i = -median; i < 255 - median ; i++)
	{
		float value = (1 / sqrt(2 * 3.1416)*sigma)*exp(-(pow(i, 2) / (2 * pow(sigma, 2))));
		gos[i + median] = value;
	}

	histDis(gos, "Gaussian Histogram");
	cumgoshist(gos, cumgos);
	float Gz[256];
	for (int i = 0; i < 256; i++)
	{
		Gz[i] = cvRound((double)cumgos[i] * alpha);
	}

	Mat new_image = image.clone();

	for (int y = 0; y < 256; y++)
	{
		for (int x = 0; x < 256; x++)
		{
			if (Sk[y] == Gz[x] || abs(Sk[y] - Gz[x]) == 1)
			{
				Sk[y] = x;
				break;
			}
		}
	}

	for (int y = 0; y < image.rows; y++)
		for (int x = 0; x < image.cols; x++)
			new_image.at<uchar>(y, x) = saturate_cast<uchar>(Sk[image.at<uchar>(y, x)]);


	float PsSk[256];
	for (int i = 0; i < 256; i++)
	{
		PsSk[i] = 0;
	}

	for (int i = 0; i < 256; i++)
	{
		PsSk[Sk[i]] += PrRk[i];
	}

	int final[256];
	for (int i = 0; i < 256; i++)
		final[i] = cvRound(PsSk[i] * 255);

	
	namedWindow("Original Image");
	imshow("Original Image", image);

	histDisplay(histogram, "Original Histogram");



	namedWindow("Equilized Image");
	imshow("Equilized Image", new_image);

	histDisplay(final, "Equilized Histogram");

	waitKey();
	return 0;
}