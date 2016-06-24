#include "ImageCommon.h"
#include "ImageProcess.h"
#include "ImageRetinex.h"

#define		DIM_2			2
#define		DIM_3			3
#define		CHANNEL			3

#define DECALRE_STEP_1														\
	step_[0] = (src.size[0] - 1)/ dst.size[0] + 1;
#define DECALRE_STEP_2														\
	step_[1] = (src.size[1] - 1)/ dst.size[1] + 1;							\
	step_[0] = (src.size[0] - 1)/ dst.size[0] + 1;
#define DECALRE_STEP_3														\
	step_[2] = (src.size[2] - 1)/ dst.size[2] + 1;							\
	step_[1] = (src.size[1] - 1)/ dst.size[1] + 1;							\
	step_[0] = (src.size[0] - 1)/ dst.size[0] + 1;
#define DECALRE_STEP(dim)													\
	JOIN(DECALRE_STEP_, dim)

#define DECALRE_ONEMAT(dim, type)											\
	int place_[dim];														\
	type *srcnow;
#define DECALRE_CHANNEL_ADD(dim, type)										\
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
#define DECALRE_ONEMAT_CHANNEL(dim, type)									\
	DECALRE_ONEMAT(dim, type);												\
	DECALRE_CHANNEL_ADD(dim, type);
#define DECLARE_TWOMAT(dim, type)											\
	DECALRE_ONEMAT(dim, type);												\
	double step_[dim];														\
	DECALRE_STEP(dim);														\
	type *dstnow;
#define DECLARE_TWOMAT_CHANNEL(dim, type)									\
	DECLARE_TWOMAT(dim, type);												\
	DECALRE_CHANNEL_ADD(dim, type);

#define DECALRE_LOOP_1(type)												\
	for (place_[0] = 0; place_[0] < src.size[0]; place_[0]++) {				\
		srcnow = &src.at<type>(place_[0]);
#define DECALRE_LOOP_2(type)												\
	for (place_[0] = 0; place_[0] < src.size[0]; place_[0]++) {				\
		for (place_[1] = 0; place_[1] < src.size[1]; place_[1]++) {			\
			srcnow = &src.at<type>(place_[0], place_[1]);
#define DECALRE_LOOP_3(type)												\
	for (place_[0] = 0; place_[0] < src.size[0]; place_[0]++) {				\
		for (place_[1] = 0; place_[1] < src.size[1]; place_[1]++) {			\
			for (place_[2] = 0; place_[2] < src.size[2]; place_[2]++) {		\
				srcnow = &src.at<type>(place_[0], place_[1], place_[2]);
#define DECALRE_LOOP(dim, type)												\
	JOIN(DECALRE_LOOP_, dim)(type);

#define DELCARE_DST_1(type)													\
	dstnow = &dst.at<type>(place_[0] / step_[0]);
#define DELCARE_DST_2(type)													\
	dstnow = &dst.at<type>(place_[0] / step_[0], place_[1] / step_[1]);
#define DELCARE_DST_3(type)													\
	dstnow = &dst.at<type>(place_[0] / step_[0], place_[1] / step_[1], place_[2] / step_[2]);
#define DELCARE_DST(dim, type)												\
	JOIN(DELCARE_DST_, dim)(type);

#define INIT_ONEMAT(dim, type)												\
	DECALRE_LOOP(dim, type);
#define INIT_CHANNEL_ADD(dim, type)											\
	for (chnow = cstart_; chnow < cend_; chnow++) {
#define INIT_ONEMAT_CHANNEL(dim, type)										\
	INIT_ONEMAT(djm, type);													\
	INIT_CHANNEL_ADD(dim, type);
#define INIT_TWOMAT(dim, type)												\
	INIT_ONEMAT(dim, type);													\
	DELCARE_DST(dim, type);				
#define INIT_TWOMAT_CHANNEL(dim, type)										\
	INIT_TWOMAT(dim, type);													\
	INIT_CHANNEL_ADD(dim, type);

#define DECALRE_EXIT_1														\
	}
#define DECALRE_EXIT_2														\
		}																	\
	}
#define DECALRE_EXIT_3														\
			}																\
		}																	\
	}
#define DECALRE_EXIT(dim)													\
	JOIN(DECALRE_EXIT_, dim);

#define EXIT_ONEMAT(dim, type)												\
	DECALRE_EXIT(dim)
#define EXIT_ONEMAT_CHANNEL(dim, type)										\
	}																		\
	EXIT_ONEMAT(dim, type);
#define EXIT_TWOMAT(dim, type)												\
	EXIT_ONEMAT(dim, type);
#define EXIT_TWOMAT_CHANNEL(dim, type)										\
	EXIT_ONEMAT_CHANNEL(dim, type);

void TrGetMaxInMat(Mat &src, Mat &dst, int channel) {
	DECLARE_TWOMAT_CHANNEL(DIM_2, Vec3b);
	
	dst.setTo(Scalar(0, 0, 0));
	INIT_TWOMAT_CHANNEL(DIM_2, Vec3b);
	if ((*srcnow)[chnow] > (*dstnow)[chnow]) {
		(*dstnow)[chnow] = (*srcnow)[chnow];
	}
	EXIT_TWOMAT_CHANNEL(DIM_2, Vec3b);
}

void TrGetLightInMat(Mat &src, Mat &dst) {
	DECLARE_TWOMAT(DIM_2, Vec3b);
	double lightd;
	uchar lightc;

	dst.setTo(Scalar(0, 0, 0));
	INIT_TWOMAT(DIM_2, Vec3b);
	lightd = 0.299 * (*srcnow)[2] + 0.587 * (*srcnow)[1] + 0.114 * (*srcnow)[0];
	lightc = RangeChar(lightd);
	if (lightc > (*dstnow)[0]) {
		(*dstnow)[0] = (*dstnow)[1] = (*dstnow)[2] = lightc;
	}
	EXIT_TWOMAT(DIM_2, Vec3b);
}

void TrMatDiv(Mat &src, Mat &dst, int channel) {
	DECLARE_TWOMAT_CHANNEL(DIM_2, Vec3b);

	INIT_TWOMAT_CHANNEL(DIM_2, Vec3b);
	(*srcnow)[chnow] = (uchar)((double)(*srcnow)[chnow] / (*dstnow)[chnow] * 255);
	EXIT_TWOMAT_CHANNEL(DIM_2, Vec3b);
}

void TrMatDivOneChannel(Mat &src, Mat &dst, int delta, int channel) {
	DECLARE_TWOMAT_CHANNEL(DIM_2, Vec3b);
	uchar now;
	double dstdouble, dstresult;

	INIT_TWOMAT(DIM_2, Vec3b);
	dstdouble = (double)(*dstnow)[0];
	dstresult = dstdouble / 270 * (1 + dstdouble / delta);
	INIT_CHANNEL_ADD(DIM_2, Vec3b);
// 	logs = log((double)(*srcnow)[chnow]);
// 	logd = log((double)(*dstnow)[0]);
// //  	logr = logs - logd * (lightexp / 5000 + 5.4) + log((double)180);
// // 	logr = logs - logd * (lightexp / 500 + 0.5) + log((double)180);
// 	logr = logs - logd * lightexp / 250 + log((double)220);
// 	now = RangeChar(exp(logr));
// 	now = RangeChar((double)((*srcnow)[chnow]) / (*dstnow)[0] * 180);

	now = RangeChar((double)((*srcnow)[chnow]) / dstresult);
	(*srcnow)[chnow] = now;
	EXIT_TWOMAT_CHANNEL(DIM_2, Vec3b);
}

void TrRetinexBalance(Mat &src, int size, int delta) {
	int dstcols, dstrows;
	Mat smalldiv;
	Mat dst;

	dstcols = dstrows = size;
	smalldiv.create(dstrows, dstcols, CV_8UC3);
	dst.create(src.size(), src.type());
	TrGetLightInMat(src, smalldiv);
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
	DECALRE_ONEMAT(DIM_2, Vec3b);
	int histrange = (255 / size) + 1;
	int dimsize[] = { size, size, size };
	int *histnow;
	Mat hist;

	hist.create(3, dimsize, CV_32SC1);
	hist.setTo(Scalar(0, 0, 0));
	INIT_ONEMAT(DIM_2, Vec3b);
	histnow = &hist.at<int>((*srcnow)[2] / histrange, (*srcnow)[1] / histrange, (*srcnow)[0] / histrange);
	(*histnow)++;
	EXIT_ONEMAT(DIM_2, Vec3b);
	return hist;
}
Mat pocIntegralHist(Mat &src, int size, int range) {
	DECALRE_ONEMAT(DIM_3, int);
	int sx, sy, sz;
	int rx, ry, rz;
	int *histnow;
	int *internow;
	int dimsize[] = { size, size, size };
	Mat inter;
	inter.create(3, dimsize, CV_32SC1);
	inter.setTo(Scalar(0, 0, 0));

	INIT_ONEMAT(DIM_3, int);
// 	for (sz = 0; sz < size; sz++) {
// 		for (sy = 0; sy < size; sy++) {
// 			for (sx = 0; sx < size; sx++) {
				histnow = &src.at<int>(sx, sy, sz);
				for (rz = MAX(0, sz - range); rz < MIN(size, sz + range); rz++) {
					for (ry = MAX(0, sz - range); ry < MIN(size, ry + range); rz++) {
						for (rx = MAX(0, rx - range); rx < MIN(size, rx + range); rz++) {
							internow = &inter.at<int>(rx, ry, rz);
							*internow += *histnow;
						}
					}
				}
	EXIT_ONEMAT(DIM_3, int);
// 			}
// 		}
// 	}
	return inter;
}

int GlobalStep = 1000;

void OnStepChange(int step, void *image) {
	double duration;
	duration = static_cast<double>(cv::getCPUTickCount());
	Mat src = ((Mat*)image)->clone();
	Mat hist;
	std::ostringstream str;

// 	if (false) {
		TrRetinexBalance(src);			// Jun. 23 '16
// 	}


	duration = static_cast<double>(cv::getCPUTickCount()) - duration;
	duration /= cv::getTickFrequency(); // the elapsed time in ms
 	str << src.rows << " * " << src.cols << "   ";
 	str << duration << "ms";
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
	ComImShow("src", image);

	createTrackbar("STEP", "dst", &GlobalStep, 2000, OnStepChange, &image);
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