#include "ImageRetinex.h"

#define		CHANNEL			3

#define DECALRE_ONEMAT											\
	int y_, x_;													\
	Vec3b *srcnow;
#define DECALRE_CHANNEL_ADD										\
	int chnow;													\
	int cstart_, cend_;											\
	if (channel < 0) {											\
		cstart_ = 0;											\
		cend_ = CHANNEL;										\
	}															\
	else {														\
		cstart_ = channel;										\
		cend_ = channel + 1;									\
	}
#define DECALRE_ONEMAT_CHANNEL									\
	DECALRE_ONEMAT;												\
	DECALRE_CHANNEL_ADD;
#define DECLARE_TWOMAT											\
	DECALRE_ONEMAT;												\
	double xstep_, ystep_;										\
	xstep_ = src.cols / dst.cols;								\
	ystep_ = src.rows / dst.rows;								\
	Vec3b *dstnow;
#define DECLARE_TWOMAT_CHANNEL									\
	DECLARE_TWOMAT;												\
	DECALRE_CHANNEL_ADD;

#define INIT_ONEMAT												\
	for (y_ = 0; y_ < src.rows; y_++) {							\
		for (x_ = 0; x_ < src.cols; x_++) {						\
			srcnow = &src.at<Vec3b>(y_, x_);
#define INIT_ONEMAT_CHANNEL										\
	INIT_ONEMAT;												\
	for (chnow = cstart_; chnow < cend_; chnow++) {
#define INIT_TWOMAT												\
	INIT_ONEMAT													\
	dstnow = &dst.at<Vec3b>(y_ / ystep_, x_ / xstep_);
#define INIT_TWOMAT_CHANNEL										\
	INIT_TWOMAT;												\
	for (chnow = cstart_; chnow < cend_; chnow++) {

#define EXIT_ONEMAT												\
		}														\
	}
#define EXIT_ONEMAT_CHANNEL										\
	}															\
	EXIT_ONEMAT;
#define EXIT_TWOMAT												\
	EXIT_ONEMAT
#define EXIT_TWOMAT_CHANNEL										\
	EXIT_ONEMAT_CHANNEL

void ComMatInit(Mat &src, uchar val, int channel) {
	DECALRE_ONEMAT_CHANNEL;

	INIT_ONEMAT_CHANNEL;
	(*srcnow)[chnow] = val;
	EXIT_ONEMAT_CHANNEL;
}

void TrGetMaxInMat(Mat &src, Mat &dst, int channel) {
	DECLARE_TWOMAT_CHANNEL
	ComMatInit(dst, 0);

	INIT_TWOMAT_CHANNEL
	if ((*srcnow)[chnow] > (*dstnow)[chnow]) {
		(*dstnow)[chnow] = (*srcnow)[chnow];
	}
	EXIT_TWOMAT_CHANNEL
}

void pocRetinex(char *filename)
{
	Mat image = imread(filename);
	Mat result;//用来保存结果  
	int STEP = 10;
	int dstcols = (image.cols - 1) / STEP + 1;
	int dstrows = (image.rows - 1) / STEP + 1;
	result.create(dstrows, dstcols, CV_8UC3);

	double duration;
	duration = static_cast<double>(cv::getCPUTickCount());

	TrGetMaxInMat(image, result);
	imshow("源图像", image);

	duration = static_cast<double>(cv::getCPUTickCount()) - duration;
	duration /= cv::getTickFrequency(); // the elapsed time in m  

	std::cout << duration << std::endl;
	imshow("显示结果", result);
	waitKey(0);
}
