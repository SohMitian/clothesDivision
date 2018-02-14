#pragma warning(disable:4996)
#include <iostream>
#include <fstream>	
#include <string>
#include <vector>
#include <stdio.h>
#include<sstream> 
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>


using namespace cv;
using namespace std;


int main(int argc, char *argv[]) {


	int xa, xb, bin, bin2, binSum, binSum2, width, height, pointX, pointY, min, min2, widthArray[1][2], heightArray[1][2], lineSum1, lineSum2;
	vector<Vec2f> lines;
	char filename[100];
	char filename_0[100];
	char filename_1[100];
	char filename_2[100];
	char filename_3[100];
	char filename_4[100];
	char filename_5[100];
	char filename_6[100];



	/******************TSV読み込み******************/
	const string file_name = "C:/data/test/train_master.tsv";
	ifstream file(file_name.c_str());
	string temp;
	string result[2];
	vector<string> items;
	vector<int> items_i;
	int count = 0;

	while (getline(file, temp)) {
		string token;
		stringstream ss, ss2;

		istringstream stream(temp);
		if (count >= 1)
		{
			while (getline(stream, token, '\t')) {
				ss << token;
				ss >> result[0];

			}
			ss2 << token;
			ss2 >> result[1];
			items.push_back(result[0]);
			items_i.push_back(std::stoi(result[1]));
		}
		count = 1;

	}
	/******************終わり******************/

	/******************TSV書き込み******************/
	int raberu[6] = { 1,2,3,4,5,6 };
	int i;
	FILE *fp0, *fp1, *fp2, *fp3, *fp4, *fp5, *fp6;
	fp0 = fopen("C:/data/multi.tsv", "w+");             /* 書き出しモードでテキストファイルをオープン */
	fp1 = fopen("C:/data/socks.tsv", "w+");
	fp2 = fopen("C:/data/tops.tsv", "w+");
	fp3 = fopen("C:/data/L_pants.tsv", "w+");
	fp4 = fopen("C:/data/pants.tsv", "w+");
	fp5 = fopen("C:/data/other.tsv", "w+");
	fp6 = fopen("C:/data/line.tsv", "w+");


	for (int i = 0; i <= 50; i++)
	{
		sprintf_s(filename, "c:/data/test/test/test_%d.jpg", i); //元画像
		sprintf_s(filename_0, "c:/data/test/multi/test_%d.jpg", i);
		sprintf_s(filename_1, "c:/data/test/socks/test_%d.jpg", i);
		sprintf_s(filename_2, "c:/data/test/tops/test_%d.jpg", i);
		sprintf_s(filename_3, "c:/data/test/L_pants/test_%d.jpg", i);
		sprintf_s(filename_4, "c:/data/test/pants/test_%d.jpg", i);
		sprintf_s(filename_5, "c:/data/test/other/test_%d.jpg", i);
		sprintf_s(filename_6, "c:/data/test/line/test_%d.jpg", i);
		/******************終わり******************/

		Mat img = imread(filename);
		if (img.empty())
		{
			return(-1);
		}
		cout << filename << endl;

		char* cstr = new char[items[i].size()]; // メモリ確保
		strcpy(cstr, items[i].c_str());        // コピー
		cstr;

		Mat gray_img, blur_img, bin_img, canny_img;
		cvtColor(img, gray_img, CV_BGR2GRAY); // グレースケール
		GaussianBlur(gray_img, blur_img, Size(9, 9), 0); // ガウシアンフィルタ
		threshold(blur_img, bin_img, 250, 255, cv::THRESH_BINARY_INV); // 二値化
		Mat labelImage(img.size(), CV_32S); //ラべリング処理
		int nLabels = connectedComponents(bin_img, labelImage, 4);
		nLabels = nLabels - 1;

		//複数
		if (nLabels > 1)
		{
			imwrite(filename_0, img);
			fprintf(fp0, "%s", cstr);
			fprintf(fp0, "\t");
			fprintf(fp0, "%d", items_i[i]);
			fprintf(fp0, "\n");
			continue;
		}


		// Xのピクセルで最も長い座標
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

		//　Yのピクセルで最も長い座標
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

		//靴下
		if ((heightD / img.rows) <= 0.95 && (widthD / img.cols) <= 0.75 && dist <= 0.55)
		{
			imwrite(filename_1, img);
			fprintf(fp1, "%s", cstr);
			fprintf(fp1, "\t");
			fprintf(fp1, "%d", items_i[i]);
			fprintf(fp1, "\n");
			continue;
		}

		//上着
		if (1 <= (heightD / widthD) && (heightD / widthD) < 1.5)
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
			//ラインが入っているかを判定
			if (min >= img.rows * 0.8) {
				lineSum1 = 0, lineSum2 = 0;
				Canny(gray_img, canny_img, 200, 200);
				HoughLines(canny_img, lines, 1, CV_PI / 180, 150);
				for (size_t i = 0; i < lines.size(); i++)
				{
					float rho = lines[i][0];
					float theta = lines[i][1];
					if (theta >= 1.5 && theta < 1.65) { //横
						lineSum1++;
						if (lineSum1 == 201) {
							break;
						}
					}
					else if (theta >= 0 && theta < 0.2) { //縦
						lineSum2++;
						if (lineSum2 == 201) {
							break;
						}
					}

				}
				if (lineSum1 >= 10 || lineSum2 >= 10) {
					imwrite(filename_6, img);
					fprintf(fp6, "%s", cstr);
					fprintf(fp6, "\t");
					fprintf(fp6, "%d", items_i[i]);
					fprintf(fp6, "\n");
					continue;
				}

				imwrite(filename_2, img);
				fprintf(fp2, "%s", cstr);
				fprintf(fp2, "\t");
				fprintf(fp2, "%d", items_i[i]);
				fprintf(fp2, "\n");
				continue;
			}
			//長ズボン
			else {
				imwrite(filename_3, img);
				fprintf(fp3, "%s", cstr);
				fprintf(fp3, "\t");
				fprintf(fp3, "%d", items_i[i]);
				fprintf(fp3, "\n");
				continue;
			}
		}
		//パンツ
		else if (0.5 <= (heightD / widthD) && (heightD / widthD) < 1)
		{
			imwrite(filename_4, img);
			fprintf(fp4, "%s", cstr);
			fprintf(fp4, "\t");
			fprintf(fp4, "%d", items_i[i]);
			fprintf(fp4, "\n");
			continue;
		}
		//その他
		else {
			imwrite(filename_5, img);
			fprintf(fp5, "%s", cstr);
			fprintf(fp5, "\t");
			fprintf(fp5, "%d", items_i[i]);
			fprintf(fp5, "\n");
			continue;
		}
	}
	fclose(fp0);     /* ファイルのクローズ */
	fclose(fp1);     /* ファイルのクローズ */
	fclose(fp2);     /* ファイルのクローズ */
	fclose(fp3);     /* ファイルのクローズ */
	fclose(fp4);     /* ファイルのクローズ */
	fclose(fp5);     /* ファイルのクローズ */
	fclose(fp6);     /* ファイルのクローズ */

	return(0);
}
