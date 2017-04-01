#include <iostream>
#include <opencv2/opencv.hpp>
//#include <opencv2/core/core.hpp>

#define WIDTH 10    //水平方向扩大的搜索范围
#define HEIGHT 2	 //垂直方向扩大的搜索范围
#define sumStepDistance 50   //滑动总距离
#define stepDistance 10		//每次滑动的距离

using namespace std;
using namespace cv;

/*
	*判断当前打印图像是否存在步进道缺陷
	*passMat:原图
	*presentMat:当前打印图
	*position:步进位置预估
*/
bool IsPassLine(const Mat &passMat, Mat &presentMat, int &position);

int main()
{
	Mat src1 = imread("12.bmp");
	Mat src2 = imread("13.bmp");

	if (src1.empty() || src2.empty())
	{
		cout << "read error!" << endl;
	}

	int position = 400;

	bool result = IsPassLine(src1, src2, position);

	cout << result << endl;
	cin.get();

	return 0;
}


/*
	*图像匹配，返回水平方向的偏移
	*src1在src2的size上，水平向外扩大WIDTH个像素点，垂直向外扩大HEIGHT个像素点
*/
int SearchXPosition(const Mat &src1, Mat &src2)     
{
	long threshold = 99999999;
	int offsetX = 0;

	Mat diff;
	Rect roi;
	Mat partOfsrc1;
	Mat partOfsrc1_gray;
	Mat src2_gray;
	for (int i = 0; i < src1.cols - src2.cols; i = i + 2)
	{
		for (int j = 0; j < src1.rows - src2.rows; j = j + 2)
		{
			roi = Rect(i, j, src2.cols, src2.rows);
			partOfsrc1 = Mat(src2.rows, src2.cols, CV_8UC3);
			src1(roi).copyTo(partOfsrc1);
			
			cvtColor(src2, src2_gray, CV_BGR2GRAY);
			cvtColor(partOfsrc1, partOfsrc1_gray, CV_BGR2GRAY);
			absdiff(partOfsrc1_gray, src2_gray, diff);

			//imwrite("src1.bmp", partOfsrc1);
			//imwrite("src2.bmp", src2);
			//absdiff(partOfsrc1, src2, diff);
			imwrite("diff.bmp", diff);
			
			Scalar sTemp = sum(diff);
	  		long s = (long)sTemp(0);
			if (s < threshold)
			{
				threshold = s;
				offsetX = i;
			}
		}
	}
	return src1.cols - src2.cols - offsetX;
}

bool IsPassLine(const Mat &passMat, Mat &presentMat, int &position)   //需要保证position位于presentMat的中间
{
	if (passMat.rows != presentMat.rows	 //|| passMat.cols != presentMat.cols
		|| position < 0
		|| position > passMat.cols)
	{
		cout << "Input of IsPassLine is error" << endl;
		return false;
	}
	vector<int> offsetLeft;
	vector<int> offsetRight;

	//滑动窗口宽度为50，滑动步进为10
	Rect window;
	int winWidth = 50;
	for (int i = WIDTH; i < (position - WIDTH) && i < WIDTH + sumStepDistance; i = i + stepDistance)
	{
		window = Rect(i, HEIGHT, winWidth, presentMat.rows - HEIGHT*2);
		Mat pPresentMat = Mat(window.height, window.width, CV_8UC3);
		presentMat(window).copyTo(pPresentMat); 

		window = Rect(i - WIDTH, 0, winWidth + WIDTH*2, presentMat.rows);
		Mat pPassMat = Mat(window.height, window.width, CV_8UC3);
		passMat(window).copyTo(pPassMat);
		
		offsetLeft.push_back(SearchXPosition(pPassMat, pPresentMat));
	}

	int start = presentMat.cols - winWidth - WIDTH - sumStepDistance;
	for (int i = start; i > (position + WIDTH) && i < (presentMat.cols - winWidth - WIDTH); i = i + stepDistance)
	{
		window = Rect(i, HEIGHT, winWidth, presentMat.rows - HEIGHT*2);
		Mat pPresentMat = Mat(window.height, window.width, CV_8UC3);
		presentMat(window).copyTo(pPresentMat);

		window = Rect(i - WIDTH, 0, winWidth + WIDTH*2, presentMat.rows);
		Mat pPassMat = Mat(window.height, window.width, CV_8UC3);
		passMat(window).copyTo(pPassMat);

		offsetRight.push_back(SearchXPosition(pPassMat, pPresentMat));
	}

	int distance = 0.0;
	int thresh = 999999;
	vector<int>::iterator t1 = offsetLeft.begin();
	vector<int>::iterator t2 = offsetRight.begin();
	
	assert(offsetLeft.size() == 5 && offsetRight.size() == 5);
	for (; t1 != offsetLeft.end(); t1++)
	{
		cout << "index："<< *t1 << "--" << *t2 << endl;
		distance += sqrt(abs(*t1 - *t2));
		t2++;
	}

	return (distance > thresh) ? true : false; 
}