
#include "ImageProcess.h"

typedef struct TrColorPoint {
	uchar b;
	uchar g;
	uchar r;
}TrColorPoint, *pTrColorPoint;

typedef struct TrGaryPoint {
	uchar g;
}TrGaryPoint, *pTrGaryPoint;

uchar RangeChar(double num) {
	if (num > 255)
		return 255;
	if (num < 0)
		return 0;
	return uchar(num);
}

// uchar RangeChar(uchar num) {
// 	if (num > 255)
// 		return 255;
// 	if (num < 0)
// 		return 0;
// 	return num;
// }

#define PROCESSDECLARE													\
	int y, x, h, w;														\
	int roiy, roix;														\
	pTrColorPoint ix;													\
	pTrGaryPoint gx;													\
	PCHAR istart;														\
	if (!src || src->depth != IPL_DEPTH_8U || ((src->nChannels != 1) && (src->nChannels != 3))) {	\
		return RESULT_ERR;												\
	}

#define PROCESSSTART													\
	istart = src->imageData;											\
	if (src->roi) {														\
		istart += (src->roi->yOffset * src->widthStep + src->roi->xOffset * src->nChannels);	\
		h = src->roi->height;											\
		w = src->roi->width;											\
	}																	\
	else {																\
		h = src->height;												\
		w = src->width;													\
	}																	\
	for (y = 0; y < h; y++) {											\
		ix = (pTrColorPoint)istart;										\
		gx = (pTrGaryPoint)istart;										\
		for (x = 0; x < w; x++) {

#define PROCESSBEGIN													\
	PROCESSDECLARE														\
	PROCESSSTART

#define PPROCESSEND														\
			ix++;														\
			gx++;														\
		}																\
		istart += src->widthStep;										\
	}


#define PROCESSBEGINTWO													\
	int y, x;															\
	pTrColorPoint  ixs, ixd;												\
	PCHAR istarts, istartd;												\
	if (!src || src->depth != IPL_DEPTH_8U || src->nChannels != 3) {	\
		return RESULT_ERR;												\
	}																	\
	istarts = src->imageData;											\
	istartd = dst->imageData;											\
	for (y = 0; y < src->height; y++) {									\
		ixs = (pTrColorPoint)istarts;										\
		ixd = (pTrColorPoint)istartd;										\
		for (x = 0; x < src->width; x++) {

#define PPROCESSENDTWO													\
			ixs++;														\
			ixd++;														\
		}																\
		istarts += src->widthStep;										\
		istartd += src->widthStep;										\
	}


RESULT TrIplImageChannelChoiceMin(IplImage *src) {
	uchar minc;
	PROCESSBEGIN;

	minc = MIN(ix->r, MIN(ix->g, ix->b));
	ix->r = ix->g = ix->b = minc;
	PPROCESSEND;
}

RESULT TrIplImageChannelChoiceMax(IplImage *src) {
	uchar minc;
	PROCESSBEGIN;
	minc = MAX(ix->r, MAX(ix->g, ix->b));
	ix->r = ix->g = ix->b = minc;
	PPROCESSEND;
}

RESULT TrIplImageChannelChoice(IplImage *src, bool isr, bool isg, bool isb, uchar r, uchar g, uchar b) {
	PROCESSBEGIN;
	if (isr)
		ix->r = r;
	if (isg)
		ix->g = g;
	if (isb)
		ix->b = b;
	PPROCESSEND;
}

RESULT TrIplImageOneColor(IplImage *src, uchar r, uchar g, uchar b, uchar delta) {
	uchar rmin, rmax, gmin, gmax, bmin, bmax;
	uchar max = 0xff;
	rmin = r > delta ? r - delta : 0;
	rmax = max - delta > r ? r + delta : max;
	gmin = g > delta ? g - delta : 0;
	gmax = max - delta > g ? g + delta : max;
	bmin = b > delta ? b - delta : 0;
	bmax = max - delta > b ? b + delta : max;
	PROCESSBEGIN;
	if (ix->r >= rmin && ix->r <= rmax &&
		ix->g >= gmin && ix->g <= gmax &&
		ix->b >= bmin && ix->b <= bmax) {
		ix->r = ix->g = ix->b = max;
	}
	else {
		ix->r = ix->g = ix->b = 0;
	}
	PPROCESSEND;
}
RESULT TrIplImageOneColor(IplImage *src, uchar g, uchar delta) {
	uchar gmin, gmax;
	uchar max = 0xff;
	gmin = g > delta ? g - delta : 0;
	gmax = max - delta > g ? g + delta : max;
	PROCESSBEGIN;
	if (gx->g >= gmin && gx->g <= gmax) {
		gx->g = max;
	}
	else {
		gx->g = 0;
	}
	PPROCESSEND;
}


RESULT TrIplImageRateOneColor(IplImage *src, uchar r, uchar g, uchar b, uchar delta) {
	double rate, rr, gg, bb;
	uchar rmin, rmax, gmin, gmax, bmin, bmax;
	uchar max = 0xff;
	rmin = r > delta ? r - delta : 0;
	rmax = max - delta > r ? r + delta : max;
	gmin = g > delta ? g - delta : 0;
	gmax = max - delta > g ? g + delta : max;
	bmin = b > delta ? b - delta : 0;
	bmax = max - delta > b ? b + delta : max;
	PROCESSBEGIN;
	rate = 0;
	if (r > rmin && r >= g && r >= b && ix->r != 0) {
		rate =  (double)r / ix->r;
	}
	if (g > gmin && g >= r && g >= b && ix->g != 0) {
		rate = (double)g / ix->g;
	}
	if (b > bmin && b >= r && b >= g && ix->b != 0) {
		rate = (double)b / ix->b;
	}
	rr = rate * ix->r;
	gg = rate * ix->g;
	bb = rate * ix->b;
	if (rate != 0 && rr < 255 && gg < 255 && bb < 255 &&
		ix->r >= rmin && ix->r <= rmax &&
		ix->g >= gmin && ix->g <= gmax &&
		ix->b >= bmin && ix->b <= bmax) {
		ix->r = ix->g = ix->b = max;
	}
	else {
		ix->r = ix->g = ix->b = 0;
	}

	PPROCESSEND;
}

RESULT TrIplImageRgbToYuv(IplImage *src) {
	double yy, uu, vv;
	PROCESSBEGIN;
	yy = 0.299 * ix->r + 0.587 * ix->g + 0.114 * ix->b;
	uu = -0.147 * ix->r - 0.289 * ix->g + 0.436 * ix->b;
	vv = 0.615 * ix->r - 0.515 * ix->g - 0.100 * ix->b;
	ix->r = RangeChar(yy);
	ix->g = RangeChar(uu);
	ix->b = RangeChar(vv);
	PPROCESSEND;
}

RESULT TrIplImageYuvToRgb(IplImage *src) {
	double rr, gg, bb;
	PROCESSBEGIN;
	rr = ix->r + 1.140 * ix->b;
	gg = ix->r - 0.394 * ix->g - 0.581 * ix->b;
	bb = ix->r + 2.032 * ix->g;
	ix->r = RangeChar(rr);
	ix->g = RangeChar(gg);
	ix->b = RangeChar(bb);
	PPROCESSEND;
}

RESULT TrIplImageRgbToHsl(IplImage *src) {
	double R, G, B, Max, Min, del_R, del_G, del_B, del_Max;
	double H, S, L;
	PROCESSBEGIN;
	R = (double)ix->r / 255.0;       //Where RGB values = 0 ÷ 255
	G = (double)ix->g / 255.0;
	B = (double)ix->b / 255.0;

	Min = MIN(R, MIN(G, B));    //Min. value of RGB
	Max = MAX(R, MAX(G, B));    //Max. value of RGB
	del_Max = Max - Min;        //Delta RGB value

	L = (Max + Min) / 2.0;

	if (del_Max == 0)           //This is a gray, no chroma...
	{
		//H = 2.0/3.0;          //Windows下S值为0时，H值始终为160（2/3*240）
		H = 0;                  //HSL results = 0 ÷ 1
		S = 0;
	}
	else                        //Chromatic data...
	{
		if (L < 0.5) S = del_Max / (Max + Min);
		else         S = del_Max / (2 - Max - Min);

		del_R = (((Max - R) / 6.0) + (del_Max / 2.0)) / del_Max;
		del_G = (((Max - G) / 6.0) + (del_Max / 2.0)) / del_Max;
		del_B = (((Max - B) / 6.0) + (del_Max / 2.0)) / del_Max;

		if (R == Max) H = del_B - del_G;
		else if (G == Max) H = (1.0 / 3.0) + del_R - del_B;
		else if (B == Max) H = (2.0 / 3.0) + del_G - del_R;

		if (H < 0)  H += 1;
		if (H > 1)  H -= 1;
	}
	ix->r = RangeChar(H * 255);
	ix->g = RangeChar(S * 255);
	ix->b = RangeChar(L * 255);
	PPROCESSEND;
}

double Hue2RGB(double v1, double v2, double vH)
{
	if (vH < 0) vH += 1;
	if (vH > 1) vH -= 1;
	if (6.0 * vH < 1) return v1 + (v2 - v1) * 6.0 * vH;
	if (2.0 * vH < 1) return v2;
	if (3.0 * vH < 2) return v1 + (v2 - v1) * ((2.0 / 3.0) - vH) * 6.0;
	return (v1);
}

RESULT TrIplImageHslToRgb(IplImage *src) {
	double R, G, B;
	double var_1, var_2;
	double H, S, L;
	PROCESSBEGIN;
	H = (double)ix->r / 255.0;       //Where RGB values = 0 ÷ 255
	S = (double)ix->g / 255.0;
	L = (double)ix->b / 255.0;

	if (S == 0)                       //HSL values = 0 ÷ 1
	{
		R = L * 255.0;                   //RGB results = 0 ÷ 255
		G = L * 255.0;
		B = L * 255.0;
	}
	else
	{
		if (L < 0.5) var_2 = L * (1 + S);
		else         var_2 = (L + S) - (S * L);

		var_1 = 2.0 * L - var_2;

		R = 255.0 * Hue2RGB(var_1, var_2, H + (1.0 / 3.0));
		G = 255.0 * Hue2RGB(var_1, var_2, H);
		B = 255.0 * Hue2RGB(var_1, var_2, H - (1.0 / 3.0));
	}
	ix->r = RangeChar(R);
	ix->g = RangeChar(G);
	ix->b = RangeChar(B);
	PPROCESSEND;

}

RESULT TrIplImageDeltaColor(IplImage *src, uchar r, uchar g, uchar b) {
	PROCESSBEGIN;
	ix->r = 255 - abs(ix->r - r);
	ix->g = 255 - abs(ix->g - g);
	ix->b = 255 - abs(ix->b - b);
	PPROCESSEND;
}

RESULT TrIplImageToWhite(IplImage *src, uchar delta) {
	uchar min, max;
	PROCESSBEGIN;
	min = MIN(ix->r, MIN(ix->g, ix->b));
	max = MAX(ix->r, MAX(ix->g, ix->b));
	if (max - min > delta) {
		if (ix->r != max) {
			int aaa = 1;
		}
		ix->r = ix->g = ix->b = max;

	}
// 	yy = 0.299 * ix->r + 0.587 * ix->g + 0.114 * ix->b;
// 	if (yy > delta)
	PPROCESSEND;
}

RESULT TrIplImageSetOne(IplImage *src) {
	PROCESSBEGIN;
 	ix->g = ix->b = 0;
	PPROCESSEND;
}

RESULT TrIplImageDiv(IplImage *src, IplImage *dst) {
	double rate;
	PROCESSBEGINTWO;
// 	rate = ixs->r + ixs->g + ixs->b;
// 	rate = ixs->r / 128;
// 	ixd->r = RangeChar((double)ixd->r / rate);
// 	ixd->g = RangeChar((double)ixd->g / rate);
// 	ixd->b = RangeChar((double)ixd->b / rate);

	if (ixs->r > 128) {
		ixd->r -= ixs->r - 128;
	}
	else {
		ixd->r += 128 - ixs->r;
	}
	ixd->g = ixd->b = 0;
	PPROCESSENDTWO;
}

RESULT TrIplImageNot(IplImage *src, bool isr, bool isg, bool isb) {
	PROCESSBEGIN;
	if (isr) {
		ix->r = 255 - ix->r;
	}
	if (isg) {
		ix->g = 255 - ix->g;
	}
	if (isb) {
		ix->b = 255 - ix->b;
	}
	PPROCESSEND;
}

RESULT TrIplImageOr(IplImage *src, IplImage *dst) {
	PROCESSBEGINTWO;
	ixd->r = MIN(ixs->r, ixd->r);


	PPROCESSENDTWO;
}
RESULT TrIplImageSub(IplImage *src, IplImage *dst) {
	double rate;
	PROCESSBEGINTWO;
// 	rate = 0.525* ixs->r - 0.52 * ixd->r + 127;
// 	rate = 1.19 * ixs->r - 1.9 * ixd->r + 250;
// 	rate = 1.28 * ixs->r - 1.4 * ixd->r + 200;

// 	rate = 2.1 * ixs->r - 2.9 * ixd->r + 200;		// right is good
// 	rate = 2.1 * ixs->r - 2.2 * ixd->r + 200;		// left is good

// 	rate = 2.1 * ixs->r - 2.2 * ixd->r + 300 - (255 - ixs->r) / 100 * ixd->r;		// right is good
// 	rate = 2.1 * ixs->r - 2.2 * ixd->r + 400 - (700 - ixs->r) / 280 * ixd->r;		// seem good
// 	rate = 3.0 * ixs->r - 2.4 * ixd->r + 250 - (600 - ixs->r) / 280 * ixd->r;		// good 2
// 	rate = 3.7 * ixs->r - 2.9 * ixd->r + 250 - (560 - ixs->r) / 280 * ixd->r;		// good 2 above for pn1.jpg

// 	rate = 2.1 * ixs->r - 1.4 * ixd->r + 100;		// right is good
// 	ixd->r = RangeChar(rate);

	ixd->r = (ixs->r >> 1) + 127 - (ixd->r >> 1);
	ixd->g = 0;
	ixd->b = 0;



	PPROCESSENDTWO;
}

RESULT TrIplImageCmp(IplImage *src, IplImage *dst) {
	PROCESSBEGINTWO;
	if (ixd->r < 128 && ixs->r - ixd->r > ixs->r / 10) {
		ixd->r = 0;
	}
	else {
		ixd->r = 255;
	}
	ixd->g = 0;
	ixd->b = 0;

	PPROCESSENDTWO;
}
RESULT TrIplImageBeOne(IplImage *src, IplImage *dst) {
	double rate;
	PROCESSBEGINTWO;
	rate = ixd->r - ((double)ixs->r - 127)  * 1.2;
	ixd->r = RangeChar(rate);
	PPROCESSENDTWO;
}


RESULT TrIplImageColorCount(IplImage *src, int *count, int extent) {
	int crange = COLOR_RANGE;
	memset(count, 0, sizeof(int) * COLOR_RANGE * COLOR_RANGE * COLOR_RANGE);
	int order, rr, gg, bb;
	int i, maxr, maxg, maxb, maxv = 0;
	PROCESSBEGIN;
	if (ix->r < 128 || ix->g < 128 || ix->b < 128) {
		for (rr = MAX(0, (ix->r >> COLOR_RSHIFT) - extent); rr <= MIN(COLOR_RANGE - 1, (ix->r >> COLOR_RSHIFT) + extent); rr++) {
			for (gg = MAX(0, (ix->g >> COLOR_RSHIFT) - extent); gg <= MIN(COLOR_RANGE - 1, (ix->g >> COLOR_RSHIFT) + extent); gg++) {
				for (bb = MAX(0, (ix->b >> COLOR_RSHIFT) - extent); bb <= MIN(COLOR_RANGE - 1, (ix->b >> COLOR_RSHIFT) + extent); bb++) {
					order = rr * COLOR_RANGE * COLOR_RANGE + gg * COLOR_RANGE + bb;
					count[order] ++;
				}
			}
		}
	}	PPROCESSEND;
	for (rr = 0; rr < COLOR_RANGE; rr++) {
		for (gg = 0; gg < COLOR_RANGE; gg++) {
			for (bb = 0; bb < COLOR_RANGE; bb++) {
				order = rr * COLOR_RANGE * COLOR_RANGE + gg * COLOR_RANGE + bb;
				if (count[order] > maxv) {
					maxv = count[order];
					maxr = rr;
					maxg = gg;
					maxb = bb;
				}
			}
		}
	}
	for (rr = MAX(0, maxr - extent); rr < MIN(COLOR_RANGE - 1, maxr + extent); rr++) {
		for (gg = MAX(0, maxg - extent); gg < MIN(COLOR_RANGE - 1, maxg + extent); gg++) {
			for (bb = MAX(0, maxb - extent); bb < MIN(COLOR_RANGE - 1, maxb + extent); bb++) {
				order = rr * COLOR_RANGE * COLOR_RANGE + gg * COLOR_RANGE + bb;
				count[order] = maxv;
			}
		}
	}
	int cc = 0;
	PROCESSSTART;
	order = (ix->r >> COLOR_RSHIFT) * COLOR_RANGE * COLOR_RANGE + (ix->g >> COLOR_RSHIFT) * COLOR_RANGE + (ix->b >> COLOR_RSHIFT);
	if (count[order] == maxv) {
		cc++;
		ix->r = ix->g = ix->b = 0;
	}

	PPROCESSEND;
	return RESULT_OK;
}