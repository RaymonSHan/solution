#include "ImageCommon.h"
#include "ImageProcess.h"
#include "ImageRetinex.h"

#define		DIM_2			2
#define		DIM_3			3
#define		CHANNEL			3

#define DECLARE_STEP_1														\
	step_[0] = (src.size[0] - 1)/ dst.size[0] + 1;
#define DECLARE_STEP_2														\
	step_[1] = (src.size[1] - 1)/ dst.size[1] + 1;							\
	step_[0] = (src.size[0] - 1)/ dst.size[0] + 1;
#define DECLARE_STEP_3														\
	step_[2] = (src.size[2] - 1)/ dst.size[2] + 1;							\
	step_[1] = (src.size[1] - 1)/ dst.size[1] + 1;							\
	step_[0] = (src.size[0] - 1)/ dst.size[0] + 1;
#define DECLARE_STEP(dim)													\
	JOIN(DECLARE_STEP_, dim)

#define DECLARE_ONEMAT(dim, type)											\
	int place_[dim];														\
	type *srcnow;
#define DECLARE_CHANNEL_ADD(dim)											\
	int chnow;																\
	int cstart_, cend_;														\
	if (channel < 0) {														\
		cstart_ = 0;														\
		cend_ = CHANNEL;													\
	}																		\
	else {																	\
		cstart_ = channel;													\
		cend_ = channel + 1;												\
	}
#define DECLARE_ONEMAT_CHANNEL(dim, type)									\
	DECLARE_ONEMAT(dim, type);												\
	DECLARE_CHANNEL_ADD(dim);
#define DECLARE_TWOMAT(dim, types, typed)									\
	DECLARE_ONEMAT(dim, types);												\
	double step_[dim];														\
	DECLARE_STEP(dim);														\
	typed *dstnow;
#define DECLARE_TWOMAT_CHANNEL(dim, types, typed)							\
	DECLARE_TWOMAT(dim, types, typed);										\
	DECLARE_CHANNEL_ADD(dim);

#define DECLARE_LOOP_1(type)												\
	for (place_[0] = 0; place_[0] < src.size[0]; place_[0]++) {				\
		srcnow = &src.at<type>(place_[0]);
#define DECLARE_LOOP_2(type)												\
	for (place_[0] = 0; place_[0] < src.size[0]; place_[0]++) {				\
		for (place_[1] = 0; place_[1] < src.size[1]; place_[1]++) {			\
			srcnow = &src.at<type>(place_[0], place_[1]);
#define DECLARE_LOOP_3(type)												\
	for (place_[0] = 0; place_[0] < src.size[0]; place_[0]++) {				\
		for (place_[1] = 0; place_[1] < src.size[1]; place_[1]++) {			\
			for (place_[2] = 0; place_[2] < src.size[2]; place_[2]++) {		\
				srcnow = &src.at<type>(place_[0], place_[1], place_[2]);
#define DECLARE_LOOP(dim, type)												\
	JOIN(DECLARE_LOOP_, dim)(type);

#define DELCARE_DST_1(type)													\
	dstnow = &dst.at<type>(place_[0] / step_[0]);
#define DELCARE_DST_2(type)													\
	dstnow = &dst.at<type>(place_[0] / step_[0], place_[1] / step_[1]);
#define DELCARE_DST_3(type)													\
	dstnow = &dst.at<type>(place_[0] / step_[0], place_[1] / step_[1], place_[2] / step_[2]);
#define DELCARE_DST(dim, type)												\
	JOIN(DELCARE_DST_, dim)(type);

#define INIT_ONEMAT(dim, type)												\
	DECLARE_LOOP(dim, type);
#define INIT_CHANNEL_ADD(dim)												\
	for (chnow = cstart_; chnow < cend_; chnow++) {
#define INIT_ONEMAT_CHANNEL(dim, type)										\
	INIT_ONEMAT(djm, type);													\
	INIT_CHANNEL_ADD(dim);
#define INIT_TWOMAT(dim, types, typed)										\
	INIT_ONEMAT(dim, types);												\
	DELCARE_DST(dim, typed);				
#define INIT_TWOMAT_CHANNEL(dim, types, typed)								\
	INIT_TWOMAT(dim, types, typed);											\
	INIT_CHANNEL_ADD(dim);

#define DECLARE_EXIT_1														\
	}
#define DECLARE_EXIT_2														\
		}																	\
	}
#define DECLARE_EXIT_3														\
			}																\
		}																	\
	}
#define DECLARE_EXIT(dim)													\
	JOIN(DECLARE_EXIT_, dim);

#define EXIT_ONEMAT(dim, type)												\
	DECLARE_EXIT(dim)
#define EXIT_ONEMAT_CHANNEL(dim, type)										\
	}																		\
	EXIT_ONEMAT(dim, type);
#define EXIT_TWOMAT(dim, types, typed)										\
	EXIT_ONEMAT(dim, types);
#define EXIT_TWOMAT_CHANNEL(dim, types, typed)								\
	EXIT_ONEMAT_CHANNEL(dim, types);

Mat TrGetMaxInMat(Mat &src, int channel) {
	Mat dst;
	dst.create(src.size(), src.type());
	dst.setTo(Scalar(0, 0, 0));
	DECLARE_TWOMAT_CHANNEL(DIM_2, Vec3b, Vec3b);
	
	INIT_TWOMAT_CHANNEL(DIM_2, Vec3b, Vec3b);
	if ((*srcnow)[chnow] > (*dstnow)[chnow]) {
		(*dstnow)[chnow] = (*srcnow)[chnow];
	}
	EXIT_TWOMAT_CHANNEL(DIM_2, Vec3b, Vec3b);
	return dst;
}

void TrMatRgbToYuv(Mat &src) {
	DECLARE_ONEMAT(DIM_2, Vec3b);
	double yy, uu, vv;
	
	INIT_ONEMAT(DIM_2, Vec3b);
	yy = 0.299 * (*srcnow)[2] + 0.587 * (*srcnow)[1] + 0.114 * (*srcnow)[0];
	uu = -0.147 * (*srcnow)[2] - 0.289 * (*srcnow)[1] + 0.436 * (*srcnow)[0];
	vv = 0.615 * (*srcnow)[2] - 0.515 * (*srcnow)[1] - 0.100 * (*srcnow)[0];
	(*srcnow)[2] = RangeChar(yy);
	(*srcnow)[1] = RangeChar(uu);
	(*srcnow)[0] = RangeChar(vv);
	EXIT_ONEMAT(DIM_2, Vec3b);;
}

void TrMatYuvToRgb(Mat &src) {
	DECLARE_ONEMAT(DIM_2, Vec3b);
	double rr, gg, bb;

	INIT_ONEMAT(DIM_2, Vec3b);
	rr = (*srcnow)[2] + 1.140 * (*srcnow)[0];
	gg = (*srcnow)[2] - 0.394 * (*srcnow)[1] - 0.581 * (*srcnow)[0];
	bb = (*srcnow)[2] + 2.032 * (*srcnow)[1];
	(*srcnow)[2] = RangeChar(rr);
	(*srcnow)[1] = RangeChar(gg);
	(*srcnow)[0] = RangeChar(bb);
	EXIT_ONEMAT(DIM_2, Vec3b);;
}

Mat TrGetLightInMat(Mat &src, int size, double &avg) {
	Mat dst;
	dst.create(size, size, CV_8UC1);
	dst.setTo(Scalar(0, 0, 0));
	DECLARE_TWOMAT(DIM_2, Vec3b, uchar);
	double lightd;
	uchar lightc;
	int totalnum = 0;
	double totallight = 0;

	INIT_TWOMAT(DIM_2, Vec3b, uchar);
	lightd = 0.299 * (*srcnow)[2] + 0.587 * (*srcnow)[1] + 0.114 * (*srcnow)[0];
	lightc = RangeChar(lightd);
	if (lightc > *dstnow) {
		*dstnow = lightc;
	}
	if (lightc > 60) {
		totalnum++;
		totallight += lightc;
	}
	EXIT_TWOMAT(DIM_2, Vec3b, uchar);
	avg = totallight / totalnum;
	return dst;
}

void TrMatDiv(Mat &src, Mat &dst, int channel) {
	DECLARE_TWOMAT_CHANNEL(DIM_2, Vec3b, uchar);

	INIT_TWOMAT_CHANNEL(DIM_2, Vec3b, uchar);
	(*srcnow)[chnow] = (uchar)((double)(*srcnow)[chnow] / (*dstnow) * 255);
	EXIT_TWOMAT_CHANNEL(DIM_2, Vec3b, uchar);
}

void TrMatDivOneChannel(Mat &src, Mat &dst, int delta, int channel) {
	DECLARE_TWOMAT_CHANNEL(DIM_2, Vec3b, uchar);
	uchar now;
	double dstdouble, dstresult;

	INIT_TWOMAT(DIM_2, Vec3b, uchar);
	dstdouble = (double)(*dstnow);
	dstresult = dstdouble / 270 * (1 + dstdouble / delta);
	INIT_CHANNEL_ADD(DIM_2);
// 	logs = log((double)(*srcnow)[chnow]);
// 	logd = log((double)(*dstnow)[0]);
// //  	logr = logs - logd * (lightexp / 5000 + 5.4) + log((double)180);
// // 	logr = logs - logd * (lightexp / 500 + 0.5) + log((double)180);
// 	logr = logs - logd * lightexp / 250 + log((double)220);
// 	now = RangeChar(exp(logr));
// 	now = RangeChar((double)((*srcnow)[chnow]) / (*dstnow)[0] * 180);

	now = RangeChar((double)((*srcnow)[chnow]) / dstresult);
	(*srcnow)[chnow] = now;
	EXIT_TWOMAT_CHANNEL(DIM_2, Vec3b, uchar);
}

void TrRetinexBalance(Mat &src, int size, int delta) {
	int dstcols, dstrows;
	Mat smalldiv;
	Mat dst;
	double avg;

	smalldiv = TrGetLightInMat(src, size, avg);
	resize(smalldiv, dst, src.size(), INTER_LANCZOS4);
	TrMatDivOneChannel(src, dst, delta);
}

void ComImShow(const string& name, Mat &src) {
	RECT	rect;
	HWND	hWnd;
	Size	size;
	Mat		dst;

	hWnd = ::GetDesktopWindow();
	::GetWindowRect(hWnd, &rect);

	float scale = MAX((float)src.cols / (rect.right - rect.left - 50),
		(float)src.rows / (rect.bottom - rect.top - 100));
	if (scale > (float)1) {
		size.width = (int)(src.cols / scale);
		size.height = (int)(src.rows / scale);
		dst.create(size, src.type());
		resize(src, dst, size, CV_INTER_LINEAR);
		imshow(name, dst);
		dst.release();
	}
	else {
		imshow(name, src);
	}
}

Mat pocHist(Mat &src, int size) {
	DECLARE_ONEMAT(DIM_2, Vec3b);
	int histrange = (255 / size) + 1;
	int dimsize[] = { size, size, size };
	int *histnow;
	Mat hist;

	hist.create(3, dimsize, CV_32SC1);
	hist.setTo(Scalar(0, 0, 0));
	INIT_ONEMAT(DIM_2, Vec3b);
	histnow = &hist.at<int>((*srcnow)[0] / histrange, (*srcnow)[1] / histrange, (*srcnow)[2] / histrange);
	(*histnow)++;
	EXIT_ONEMAT(DIM_2, Vec3b);
	return hist;
}
Mat pocIntegralHist(Mat &src, int range) {
	Mat dst;
// 	int dimsize[] = { src.size[0], size, size };
	dst.create(3, src.size, CV_32SC1);
	dst.setTo(Scalar(0, 0, 0));
	DECLARE_ONEMAT(DIM_3, int);
	int &sz = place_[0], &sy = place_[1], &sx = place_[2];
	int rx, ry, rz;
	int *dstnow;

	INIT_ONEMAT(DIM_3, int);
	for (rz = MAX(0, sz - range); rz < MIN(src.size[0], sz + range + 1); rz++) {
		for (ry = MAX(0, sy - range); ry < MIN(src.size[1], sy + range + 1); ry++) {
			for (rx = MAX(0, sx - range); rx < MIN(src.size[2], sx + range + 1); rx++) {
				dstnow = &dst.at<int>(rz, ry, rx);
				*dstnow += *srcnow;
			}
		}
	}
	EXIT_ONEMAT(DIM_3, int);
	return dst;
}

void ComSetToInRange(Mat &src, Vec3b point, int range) {
	DECLARE_ONEMAT(DIM_3, int);

	INIT_ONEMAT(DIM_3, int);
	if (abs(place_[2] - point[2]) <= range && 
		abs(place_[1] - point[1]) <= range && 
		abs(place_[0] - point[0]) <= range) {
		*srcnow = 0;
	}
	EXIT_ONEMAT(DIM_3, int);
}

int pocGetMaxIntegra(Mat &src, Vec3b &vec) {
	DECLARE_ONEMAT(DIM_3, int);
	int count = 0;

	INIT_ONEMAT(DIM_3, int);
	if (*srcnow > count) {
		count = *srcnow;
		vec = { (uchar)place_[0], (uchar)place_[1], (uchar)place_[2] };
	}
	EXIT_ONEMAT(DIM_3, int);
	return count;
}

Vec3b ComRemoveTopN(Mat &src, int max, int range) {
	Vec3b vec;
	int count;

	count = pocGetMaxIntegra(src, vec);
	for (int i = 0; i < max; i++) {
		ComSetToInRange(src, vec, range);
		count = pocGetMaxIntegra(src, vec);
	}
	return vec;
}

void ComGetMinMaxByRange(int size, int range, Vec3b &vec, Vec3b &min, Vec3b &max) {
	int step = (255 / size) + 1;
	for (int i = 0; i < 3; i++) {
		min[i] = RangeChar(step * (vec[i] - range));
		max[i] = RangeChar(step * (vec[i] + ( 1 + range)));
	}
}
bool ComVecInRange(Vec3b &vec, Vec3b &min, Vec3b &max) {
	for (int i = 0; i < 3; i++) {
		if (min[i] > vec[i] || max[i] < vec[i]) {
			return false;
		}
	}
	return true;
};

Mat ComGetImageByColorRange(Mat &image, Mat &inter, int range) {
	Vec3b vec, min, max;
	Mat src;
	int count;
	src = image.clone();
	DECLARE_ONEMAT(DIM_2, Vec3b);
// 	int tt = 0, ff = 0;

	count = pocGetMaxIntegra(inter, vec);
	ComGetMinMaxByRange(inter.size[0], range, vec, min, max);
	INIT_ONEMAT(DIM_2, Vec3b);
	if (!ComVecInRange(*srcnow, min, max)) {
		(*srcnow)[2] = (*srcnow)[1] = (*srcnow)[0] = 255;
// 		ff++;
	}
// 	else {
// 		tt++;
// 	}
	EXIT_ONEMAT(DIM_2, Vec3b);
	return src;
}

int GlobalStep = 4;
int GlobalRange = 2;
int GlobalMax = 0;

void OnStepChange(int step, void *image) {
	double duration;
	duration = static_cast<double>(cv::getCPUTickCount());
	Mat src = ((Mat*)image)->clone();
	Mat hist, inter;
	std::ostringstream str;

 	if (false) {
		TrRetinexBalance(src);			// Jun. 23 '16
 	}

	TrRetinexBalance(src);
	ComImShow("src", src);
	if (GlobalStep < 1) {
		GlobalStep = 1;
	}
	int globalstep = 1 << GlobalStep;
	TrMatRgbToYuv(src);

	Mat YChannel, UChannel, VChannel;
	vector<Mat> channels(3);
	split(src, channels);
// 	Mat eqsrc;
	VChannel = channels.at(0);
	UChannel = channels.at(1);
	YChannel = channels.at(2);

// 	equalizeHist(VChannel, VChannel);
	merge(channels, src);

	hist = pocHist(src, globalstep);
	inter = pocIntegralHist(hist, GlobalRange);

	Vec3b vec, min, max;

	vec = ComRemoveTopN(inter, GlobalMax, GlobalRange);
	src = ComGetImageByColorRange(src, inter, GlobalRange);
	ComGetMinMaxByRange(inter.size[0], GlobalRange, vec, min, max);

	duration = static_cast<double>(cv::getCPUTickCount()) - duration;
	duration /= cv::getTickFrequency(); // the elapsed time in ms
//  	str << src.rows << " * " << src.cols << "   ";
	str << (double)min[0] << " " << (double)min[1] << " " << (double)min[2] << " ";
	str << (double)max[0] << " " << (double)max[1] << " " << (double)max[2] << " ";

 	str << duration << "s";
	TrMatYuvToRgb(src);
	putText(src, str.str(), Point(0, src.rows - 30), CV_FONT_HERSHEY_TRIPLEX, src.rows / 600, Scalar(23, 123, 223), src.rows / 300, src.rows / 300);
	ComImShow("dst", src);
	src.release();
// 	waitKey(1);

}

void pocRetinex(char *filename)
{
	Mat image = imread(filename);

	namedWindow("src");
	namedWindow("dst");

	createTrackbar("Step", "dst", &GlobalStep, 8, OnStepChange, &image);
	createTrackbar("Range", "dst", &GlobalRange, 8, OnStepChange, &image);
	createTrackbar("Max", "dst", &GlobalMax, 16, OnStepChange, &image);
	OnStepChange(GlobalStep, &image);

	waitKey(0);
	destroyAllWindows();

}



#include <math.h>  

//#define USE_EXACT_SIGMA  


#define pc(image, x, y, c) image->imageData[(image->widthStep * y) + (image->nChannels * x) + c]  

#define INT_PREC 1024.0  
#define INT_PREC_BITS 10  

inline double int2double(int x) { return (double)x / INT_PREC; }
inline int double2int(double x) { return (int)(x * INT_PREC + 0.5); }

inline int int2smallint(int x) { return (x >> INT_PREC_BITS); }
inline int int2bigint(int x) { return (x << INT_PREC_BITS); }

//  
// CreateKernel  
//  
// Summary:  
// Creates a normalized 1 dimensional gaussian kernel.  
//  
// Arguments:  
// sigma - the standard deviation of the gaussian kernel.  
//  
// Returns:  
// double* - an array of values of length ((6*sigma)/2) * 2 + 1.  
//  
// Note:  
// Caller is responsable for deleting the kernel.  
//  
double*
CreateKernel(double sigma)
{
	int i, x, filter_size;
	double* filter;
	double sum;

	// Reject unreasonable demands  
	if (sigma > 200) sigma = 200;

	// get needed filter size (enforce oddness)  
	filter_size = (int)floor(sigma * 6) / 2;
	filter_size = filter_size * 2 + 1;

	// Allocate kernel space  
	filter = new double[filter_size];

	// Calculate exponential  
	sum = 0;
	for (i = 0; i < filter_size; i++) {
		x = i - (filter_size / 2);
		filter[i] = exp(-(x*x) / (2 * sigma*sigma));

		sum += filter[i];
	}

	// Normalize  
	for (i = 0, x; i < filter_size; i++)
		filter[i] /= sum;

	return filter;
}

//  
// CreateFastKernel  
//  
// Summary:  
// Creates a faster gaussian kernal using integers that  
// approximate floating point (leftshifted by 8 bits)  
//  
// Arguments:  
// sigma - the standard deviation of the gaussian kernel.  
//  
// Returns:  
// int* - an array of values of length ((6*sigma)/2) * 2 + 1.  
//  
// Note:  
// Caller is responsable for deleting the kernel.  
//  

int*
CreateFastKernel(double sigma)
{
	double* fp_kernel;
	int* kernel;
	int i, filter_size;

	// Reject unreasonable demands  
	if (sigma > 200) sigma = 200;

	// get needed filter size (enforce oddness)  
	filter_size = (int)floor(sigma * 6) / 2;
	filter_size = filter_size * 2 + 1;

	// Allocate kernel space  
	kernel = new int[filter_size];

	fp_kernel = CreateKernel(sigma);

	for (i = 0; i < filter_size; i++)
		kernel[i] = double2int(fp_kernel[i]);

	delete fp_kernel;

	return kernel;
}

//  
// FilterGaussian  
//  
// Summary:  
// Performs a gaussian convolution for a value of sigma that is equal  
// in both directions.  
//  
// Arguments:  
// img - the image to be filtered in place.  
// sigma - the standard deviation of the gaussian kernel to use.  
//  
void
FilterGaussian(IplImage* img, double sigma)
{
	int i, j, k, source, filter_size;
	int* kernel;
	IplImage* temp;
	int v1, v2, v3;

	// Reject unreasonable demands  
	if (sigma > 200) sigma = 200;

	// get needed filter size (enforce oddness)  
	filter_size = (int)floor(sigma * 6) / 2;
	filter_size = filter_size * 2 + 1;

	kernel = CreateFastKernel(sigma);

	temp = cvCreateImage(cvSize(img->width, img->height), img->depth, img->nChannels);

	// filter x axis  
	for (j = 0; j < temp->height; j++)
		for (i = 0; i < temp->width; i++) {

			// inner loop has been unrolled  

			v1 = v2 = v3 = 0;
			for (k = 0; k < filter_size; k++) {

				source = i + filter_size / 2 - k;

				if (source < 0) source *= -1;
				if (source > img->width - 1) source = 2 * (img->width - 1) - source;

				v1 += kernel[k] * (unsigned char)pc(img, source, j, 0);
				if (img->nChannels == 1) continue;
				v2 += kernel[k] * (unsigned char)pc(img, source, j, 1);
				v3 += kernel[k] * (unsigned char)pc(img, source, j, 2);

			}

			// set value and move on  
			pc(temp, i, j, 0) = (char)int2smallint(v1);
			if (img->nChannels == 1) continue;
			pc(temp, i, j, 1) = (char)int2smallint(v2);
			pc(temp, i, j, 2) = (char)int2smallint(v3);

		}

	// filter y axis  
	for (j = 0; j < img->height; j++)
		for (i = 0; i < img->width; i++) {

			v1 = v2 = v3 = 0;
			for (k = 0; k < filter_size; k++) {

				source = j + filter_size / 2 - k;

				if (source < 0) source *= -1;
				if (source > temp->height - 1) source = 2 * (temp->height - 1) - source;

				v1 += kernel[k] * (unsigned char)pc(temp, i, source, 0);
				if (img->nChannels == 1) continue;
				v2 += kernel[k] * (unsigned char)pc(temp, i, source, 1);
				v3 += kernel[k] * (unsigned char)pc(temp, i, source, 2);

			}

			// set value and move on  
			pc(img, i, j, 0) = (char)int2smallint(v1);
			if (img->nChannels == 1) continue;
			pc(img, i, j, 1) = (char)int2smallint(v2);
			pc(img, i, j, 2) = (char)int2smallint(v3);

		}


	cvReleaseImage(&temp);

	delete kernel;

}

//  
// FastFilter  
//  
// Summary:  
// Performs gaussian convolution of any size sigma very fast by using  
// both image pyramids and seperable filters.  Recursion is used.  
//  
// Arguments:  
// img - an IplImage to be filtered in place.  
//  
void
FastFilter(IplImage *img, double sigma)
{
	int filter_size;

	// Reject unreasonable demands  
	if (sigma > 200) sigma = 200;

	// get needed filter size (enforce oddness)  
	filter_size = (int)floor(sigma * 6) / 2;
	filter_size = filter_size * 2 + 1;

	// If 3 sigma is less than a pixel, why bother (ie sigma < 2/3)  
	if (filter_size < 3) return;

	// Filter, or downsample and recurse  
	if (filter_size < 10) {

#ifdef USE_EXACT_SIGMA  
		FilterGaussian(img, sigma)
#else  
		cvSmooth(img, img, CV_GAUSSIAN, filter_size, filter_size);
#endif  

	}
	else {
		if (img->width < 2 || img->height < 2) return;

		IplImage* sub_img = cvCreateImage(cvSize(img->width / 2, img->height / 2), img->depth, img->nChannels);

		cvPyrDown(img, sub_img);

		FastFilter(sub_img, sigma / 2.0);

		cvResize(sub_img, img, CV_INTER_LINEAR);

		cvReleaseImage(&sub_img);
	}

}

//  
// Retinex  
//  
// Summary:  
// Basic retinex restoration.  The image and a filtered image are converted  
// to the log domain and subtracted.  
//  
// Arguments:  
// img - an IplImage to be enhanced in place.  
// sigma - the standard deviation of the gaussian kernal used to filter.  
// gain - the factor by which to scale the image back into visable range.  
// offset - an offset similar to the gain.  
//  
void
Retinex(IplImage *img, double sigma, int gain, int offset)
{
	IplImage *A, *fA, *fB, *fC;

	// Initialize temp images  
	fA = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_32F, img->nChannels);
	fB = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_32F, img->nChannels);
	fC = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_32F, img->nChannels);

	// Compute log image  
	cvConvert(img, fA);
	cvLog(fA, fB);

	// Compute log of blured image  
	A = cvCloneImage(img);
	FastFilter(A, sigma);
	cvConvert(A, fA);
	cvLog(fA, fC);

	// Compute difference  
	cvSub(fB, fC, fA);

	// Restore  
	cvConvertScale(fA, img, gain, offset);

	// Release temp images  
	cvReleaseImage(&A);
	cvReleaseImage(&fA);
	cvReleaseImage(&fB);
	cvReleaseImage(&fC);

}

//  
// MultiScaleRetinex  
//  
// Summary:  
// Multiscale retinex restoration.  The image and a set of filtered images are  
// converted to the log domain and subtracted from the original with some set  
// of weights. Typicaly called with three equaly weighted scales of fine,  
// medium and wide standard deviations.  
//  
// Arguments:  
// img - an IplImage to be enhanced in place.  
// sigma - the standard deviation of the gaussian kernal used to filter.  
// gain - the factor by which to scale the image back into visable range.  
// offset - an offset similar to the gain.  
//  
void
MultiScaleRetinex(IplImage *img, int scales, double *weights, double *sigmas, int gain, int offset)
{
	int i;
	double weight;
	IplImage *A, *fA, *fB, *fC;

	// Initialize temp images  
	fA = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_32F, img->nChannels);
	fB = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_32F, img->nChannels);
	fC = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_32F, img->nChannels);


	// Compute log image  
	cvConvert(img, fA);
	cvLog(fA, fB);

	// Normalize according to given weights  
	for (i = 0, weight = 0; i < scales; i++)
		weight += weights[i];

	if (weight != 1.0) cvScale(fB, fB, weight);

	// Filter at each scale  
	for (i = 0; i < scales; i++) {
		A = cvCloneImage(img);
		FastFilter(A, sigmas[i]);

		cvConvert(A, fA);
		cvLog(fA, fC);
		cvReleaseImage(&A);

		// Compute weighted difference  
		cvScale(fC, fC, weights[i]);
		cvSub(fB, fC, fB);
	}

	// Restore  
	cvConvertScale(fB, img, gain, offset);

	// Release temp images  
	cvReleaseImage(&fA);
	cvReleaseImage(&fB);
	cvReleaseImage(&fC);
}

//  
// MultiScaleRetinexCR  
//  
// Summary:  
// Multiscale retinex restoration with color restoration.  The image and a set of  
// filtered images are converted to the log domain and subtracted from the  
// original with some set of weights. Typicaly called with three equaly weighted  
// scales of fine, medium and wide standard deviations. A color restoration weight  
// is then applied to each color channel.  
//  
// Arguments:  
// img - an IplImage to be enhanced in place.  
// sigma - the standard deviation of the gaussian kernal used to filter.  
// gain - the factor by which to scale the image back into visable range.  
// offset - an offset similar to the gain.  
// restoration_factor - controls the non-linearaty of the color restoration.  
// color_gain - controls the color restoration gain.  
//  
void
MultiScaleRetinexCR(IplImage *img, int scales, double *weights, double *sigmas,
	int gain, int offset, double restoration_factor, double color_gain)
{
	int i;
	double weight;
	IplImage *A, *B, *C, *fA, *fB, *fC, *fsA, *fsB, *fsC, *fsD, *fsE, *fsF;

	// Initialize temp images  
	fA = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_32F, img->nChannels);
	fB = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_32F, img->nChannels);
	fC = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_32F, img->nChannels);
	fsA = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_32F, 1);
	fsB = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_32F, 1);
	fsC = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_32F, 1);
	fsD = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_32F, 1);
	fsE = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_32F, 1);
	fsF = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_32F, 1);

	// Compute log image  
	cvConvert(img, fB);
	cvLog(fB, fA);

	// Normalize according to given weights  
	for (i = 0, weight = 0; i < scales; i++)
		weight += weights[i];

	if (weight != 1.0) cvScale(fA, fA, weight);

	// Filter at each scale  
	for (i = 0; i < scales; i++) {
		A = cvCloneImage(img);
		FastFilter(A, sigmas[i]);

		cvConvert(A, fB);
		cvLog(fB, fC);
		cvReleaseImage(&A);

		// Compute weighted difference  
		cvScale(fC, fC, weights[i]);
		cvSub(fA, fC, fA);
	}

	// Color restoration  
	if (img->nChannels > 1) {
		A = cvCreateImage(cvSize(img->width, img->height), img->depth, 1);
		B = cvCreateImage(cvSize(img->width, img->height), img->depth, 1);
		C = cvCreateImage(cvSize(img->width, img->height), img->depth, 1);

		// Divide image into channels, convert and store sum  
		cvCvtPixToPlane(img, A, B, C, NULL);

		cvConvert(A, fsA);
		cvConvert(B, fsB);
		cvConvert(C, fsC);

		cvReleaseImage(&A);
		cvReleaseImage(&B);
		cvReleaseImage(&C);

		// Sum components  
		cvAdd(fsA, fsB, fsD);
		cvAdd(fsD, fsC, fsD);

		// Normalize weights  
		cvDiv(fsA, fsD, fsA, restoration_factor);
		cvDiv(fsB, fsD, fsB, restoration_factor);
		cvDiv(fsC, fsD, fsC, restoration_factor);

		cvConvertScale(fsA, fsA, 1, 1);
		cvConvertScale(fsB, fsB, 1, 1);
		cvConvertScale(fsC, fsC, 1, 1);

		// Log weights  
		cvLog(fsA, fsA);
		cvLog(fsB, fsB);
		cvLog(fsC, fsC);

		// Divide retinex image, weight accordingly and recombine  
		cvCvtPixToPlane(fA, fsD, fsE, fsF, NULL);

		cvMul(fsD, fsA, fsD, color_gain);
		cvMul(fsE, fsB, fsE, color_gain);
		cvMul(fsF, fsC, fsF, color_gain);

		cvCvtPlaneToPix(fsD, fsE, fsF, NULL, fA);
	}

	// Restore  
	cvConvertScale(fA, img, gain, offset);

	// Release temp images  
	cvReleaseImage(&fA);
	cvReleaseImage(&fB);
	cvReleaseImage(&fC);
	cvReleaseImage(&fsA);
	cvReleaseImage(&fsB);
	cvReleaseImage(&fsC);
	cvReleaseImage(&fsD);
	cvReleaseImage(&fsE);
	cvReleaseImage(&fsF);
}