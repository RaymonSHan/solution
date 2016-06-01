
#include "ImageCommon.h"

Pix* trPixCreateFromIplImage(IplImage *img) {
	Pix *pix = NULL;
	int x, y;
	char *istart, *ix;
	l_uint32 *pstart, *px;

// only supply 8 bit gray and 24 bit color image now.
	if (!img || img->depth != 8 || (img->nChannels != 1 && img->nChannels != 3)) {
		return NULL;
	}
	pix = pixCreateNoInit(img->width, img->height, img->nChannels == 1 ? 8 : 32);
	if (!pix) {
		return NULL;
	}

	istart = img->imageData;
	pstart = pix->data;
	if (img->nChannels == 1) {
		for (y = 0; y < img->height; y++) {
// for gray img, one for big-end in 4 byte, another for small-end. So switch the order
			ix = istart;
			px = pstart;
			for (x = 0; x < (img->width + 3) / 4; x++) {
				*px++ = (*ix << 24) + (*(ix + 1) << 16) + (*(ix + 2) << 8) + *(ix + 3);
				ix += 4;
			}
// widthStep and wpl are 4 byte align
			istart += img->widthStep;
			pstart += pix->wpl;
		}
	}
	if (img->nChannels == 3) {
// for IplImage, use 3 byte for one pixel in 24 bit image, while Pix use 4 byte.
		for (y = 0; y < img->height; y++) {
			ix = istart;
			px = pstart;
			for (x = 0; x < img->width; x++) {
				*px++ = (*(l_uint32 *)ix) << 8;
				ix += 3;
			}
			istart += img->widthStep;
			pstart += pix->wpl;
		}
	}
	return pix;
}

void trShowImage(char *name, IplImage *img, Boxa *box = NULL) {

}