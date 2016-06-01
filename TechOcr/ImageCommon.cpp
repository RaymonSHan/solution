
#include "ImageCommon.h"

Pix* trPixCreateFromIplImage(IplImage *img) {
	Pix *pix = NULL;
	int x, y;
	char *istart, *ix;
	l_uint32 *pstart, *px;

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
			ix = istart;
			px = pstart;
			for (x = 0; x < (img->width + 3) / 4; x++) {
				*px++ = (*ix << 24) + (*(ix + 1) << 16) + (*(ix + 2) << 8) + *(ix + 3);
				ix += 4;
			}
			istart += img->widthStep;
			pstart += pix->wpl;
		}
	}
	if (img->nChannels == 3) {
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