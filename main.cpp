#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>


using namespace cv;
using namespace std;


int main(int argc, char *argv[]) {


	int xa, xb, bin, bin2, binSum, binSum2, width, height, pointX, pointY, min, min2, widthArray[1][2], heightArray[1][2];

	ifstream ifs("c:/data/test/test.tsv");
	if (!ifs) {
		return -1;
	}
	int linenum = 0;
	string str;
	while(getline(ifs, str))
	{
		linenum += 1;
		std::cout << linenum << std::endl;
	}
	vector<vector<string> > arr{ {linenum}, {2} };
	String arr[20000];
	int arr2[20000];
	for (int i = 0; i < linenum; i++) {
		for (int j = 0; j < linenum; j++) {
			getline(ifs, str, '\t');
			arr[i][j] = str;
		}
	}

	char filename[100];
	for (int i = 0; i < 30; i++)
	{
		sprintf_s(filename, "c:/data/test/test_%d.jpg", i);
		Mat img = imread(filename);
		if (img.empty())
		{
			return(-1);
		}
		cout << filename << endl;


		Mat gray_img, blur_img, bin_img;
		// グレースケール
		cvtColor(img, gray_img, CV_BGR2GRAY);
		// ガウシアンフィルタ
		GaussianBlur(gray_img, blur_img, Size(9, 9), 0);
		// 二値化
		threshold(blur_img, bin_img, 250, 255, cv::THRESH_BINARY_INV);
		//ラべリング処理
		Mat labelImage(img.size(), CV_32S);
		int nLabels = connectedComponents(bin_img, labelImage, 4);
		nLabels = nLabels - 1;
		//std::cout << nLabels  << std::endl;

		//複数
		if (nLabels > 1)
		{
			//imwrite("c:/data/multi/test_10.jpg", img);
			std::cout << "複数" << std::endl;
			namedWindow("bin", WINDOW_NORMAL);
			imshow("bin", bin_img);
			waitKey(0);
			destroyAllWindows();
			continue;
		}


		// 横のピクセル数
		binSum = 0, binSum2 = 0;
		for (int y = 0; y < img.rows; y++)
		{
			for (int x = 0; x < img.cols; x++)
			{
				bin = bin_img.at<unsigned char>(y, x);
				bin = bin / 255;
				if (bin == 1)
				{
					binSum++;
					pointY = y;
					pointX = x;
				}
			}
			if (binSum > binSum2)
			{
				binSum2 = binSum;
				binSum = 0;
				widthArray[0][0] = pointY;
				widthArray[0][1] = pointX;
			}
			else {
				binSum = 0;
			}

		}
		width = binSum2;

		//　縦のピクセル数
		binSum = 0, binSum2 = 0;
		for (int x = 0; x < img.cols; x++)
		{

			for (int y = 0; y < img.rows; y++)
			{
				bin = bin_img.at<unsigned char>(y, x);
				bin = bin / 255;
				if (bin == 1)
				{
					binSum++;
					pointY = y;
					pointX = x;

				}
			}
			if (binSum > binSum2)
			{
				binSum2 = binSum;
				binSum = 0;
				heightArray[0][0] = pointY;
				heightArray[0][1] = pointX;
			}
			else {
				binSum = 0;
			}

		}
		height = binSum2;

		//靴下のxとyの二点間を求める
		double x1, y1, x2, y2, mx, my, dist;
		x1 = heightArray[0][1];
		y1 = widthArray[0][0];
		x2 = heightArray[0][1];
		y2 = heightArray[0][0];
		mx = abs(x2 - x1);
		my = abs(y2 - y1);
		dist = sqrt(pow(mx, 2) + pow(my, 2));
		dist = dist / img.rows;

		double heightD = static_cast<double>(height);
		double widthD = static_cast<double>(width);

		std::cout << "横" << width << std::endl;
		std::cout << "縦" << height << std::endl;
		std::cout << heightD / img.rows << std::endl;
		std::cout << widthD / img.cols << std::endl;
		std::cout << "y" << widthArray[0][0] << std::endl;
		std::cout << "x" << widthArray[0][1] << std::endl;
		std::cout << "y" << heightArray[0][0] << std::endl;
		std::cout << "x" << heightArray[0][1] << std::endl;
		std::cout << "二点間" << dist << std::endl;

		//靴下
		if ((heightD / img.cols) >= 0.6 && (widthD / img.rows) <= 0.37 && dist <= 0.25)
		{
			std::cout << "靴下" << std::endl;
		}
		//上着
		else if (1 <= (heightD / widthD) && (heightD / widthD) < 3.1)
		{
			min = 0, min2 = 0;
			for (int i = 0; i < 5; i++) {
				binSum = 0;
				binSum2 = 0;
				for (int j = 0; j < img.size().height; j++) {
					xa = img.cols / 2 + i;
					xb = img.cols / 2 - i;
					bin = bin_img.at<unsigned char>(j, xa);
					bin2 = bin_img.at<unsigned char>(j, xb);
					bin = bin / 255;
					bin2 = bin2 / 255;
					if (bin == 1) {
						binSum++;
					}
					if (bin2 == 1) {
						binSum2++;
					}
					if (binSum > min) {
						min = binSum;
					}
					if (binSum2 > min2) {
						min2 = binSum2;
					}

					if (min > min2) {
						min = min2;
					}
				}
			}
			if (min >= img.rows * 0.8) {

				cout << "上着、スカート" << endl;
			}
			else {
				cout << "長ズボン" << endl;
			}
		}
		//パンツ
		else if (0.5 <= (heightD / widthD) && (heightD / widthD) < 1)
		{
			std::cout << "パンツ" << std::endl;
		}
		//その他
		else {
			std::cout << "その他" << std::endl;
		}


		namedWindow("bin", WINDOW_NORMAL);
		imshow("bin", bin_img);
		waitKey(0);
		destroyAllWindows();
	}

	return(0);
}
