#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

void detect(Mat& src, int x, int col, vector<Point>& out) {
	Rect roi = Rect(x, 0, 1, src.rows);
	Mat dst;
	src(roi).copyTo(dst);
	for (int i = 10; i < dst.rows; i++) {    //决定从第几行开始检查
		uchar *ptr = dst.ptr<uchar>(i);
		if (ptr[0] == 0) {
			out.push_back(Point(col, i));
			i += 10;   //跳过该缺陷
		}
	}
}

void main() {

	Mat src = imread("test2.bmp", 0);
	Mat dst;
	threshold(src, dst, 50, 255, THRESH_BINARY);
	Mat element = getStructuringElement(MORPH_RECT, Size(10, 10)); //注意MORPH_RECT的选择
	Mat out;
	dilate(dst, out, element); 
	imwrite("out.bmp", out);

	vector<int> blackLine;
	
	//得到两条黑线的位置
	uchar* data = out.ptr<uchar>(500);
	for (int i = 1; i < out.cols; i++) {
		if (data[i] - data[i - 1] > 0) {
			blackLine.push_back(i);
		}
	}

	//将黑线内的区域抠出来
	int start, end;
	start = blackLine[0] + 10;
	end = blackLine[1] - 120;
	Rect roi = Rect(start, 0, end - start, src.rows);
	Mat src1;
	src(roi).copyTo(src1);
	imshow("src1", src1);

	///////////////////////////////////////////////////////////////开始检测///////////////////////////////////////////////////////////////
	Mat dst1, edge;
	//创建src同类型大小的矩阵
	dst.create(src1.size(), src1.type());
	//3X3内核来降噪
	blur(src1, edge, Size(3, 3));

	threshold(src1, dst1, 180, 255, THRESH_BINARY_INV);   //注意模式的选择，将小于180的全置为白色

	//腐蚀和膨胀,因为是对白色区域进行操作，先取反
	Mat element1 = getStructuringElement(MORPH_RECT, Size(7, 7)); //注意MORPH_RECT的选择
	Mat out1;
	dilate(dst1, out1, element1);
	imwrite("out1.bmp", out1);

	//通过计算梯度找出每一列的位置
	vector<Point> result;
	int col = 0;
	uchar* ptr = out1.ptr<uchar>(100);
	for (int i = 1; i < out1.cols; i++) {
		if (ptr[i] - ptr[i - 1] > 0) {
			col++;
			detect(out1, i + 18, col, result);
		}
	}

	for (int i = 0; i < result.size(); i++) {
		cout << result[i] << endl;
	}

	


	//imshow("dst", dst);
	//Canny(src, dst, 150, 200, 3);
	//imshow("dst", dst);
	/*
	Mat src = imread("test1.bmp", 0);
	//Mat src1 = imread("test1.bmp", 0);
	Mat dst, edge;
	//创建src同类型大小的矩阵
	dst.create(src.size(), src.type());
	//3X3内核来降噪
	blur(src, edge, Size(3, 3));

	threshold(src, dst, 150, 255, THRESH_BINARY_INV);   //注意模式的选择
	
	//腐蚀和膨胀,因为是对白色区域进行操作，先取反
	Mat element = getStructuringElement(MORPH_RECT, Size(7, 7)); //注意MORPH_RECT的选择
	Mat out;
	dilate(dst, out, element);	imshow("dst", out);

	
	Mat dst, cdst;
	Canny(src, dst, 50, 200, 3);
	cvtColor(dst, cdst, CV_GRAY2BGR);

	vector<Vec4i> lines;
	HoughLinesP(dst, lines, 1, CV_PI / 180, 50, 50, 10);
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		line(cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
	}
	imshow("detected lines", cdst);
	*/

	cin.get();
}