#pragma once

#include "rcommon.h"
#include "ImageCommon.h"

#define TOTAL_FEATURE 200
#define MAX_WORDS 100
#define MAX_FOUND 16
#define WORD_LENGTH 4

typedef struct trFeatureWord {
	char word[WORD_LENGTH];
	Box origin;
	trFeatureWord *h_next;
}trFeatureWord;

typedef struct trFeatureWordFound {
	char word[WORD_LENGTH];
	Box origin;
	trFeatureWordFound *h_next;
	volatile MYINT nowFound;
	bool match;
	Box found[MAX_FOUND];
}trFeatureWordFound;

// Maybe NOT use
// class OcrFeatureWords {
// private:
// 	volatile MYINT nowFeature;
// 	trFeatureWord allFeature[TOTAL_FEATURE];
// public:
// 	OcrFeatureWords() {
// 		nowFeature = 0;
// 	};
// 	void AddFeatureWord(char *word);
// };

class OcrFormat {
	friend class OcrFeatureWordsFound;
private:
	volatile MYINT nowFeature;
	trFeatureWord allFeature[MAX_WORDS];

public:
	OcrFormat() {
		nowFeature = 0;
	};
	trFeatureWord* AddFeatureWord(int x, int y, int w, int h, char *word, trFeatureWord *next = NULL);
	trFeatureWord* AddFeatureWord(Box *box, char *word, trFeatureWord *next = NULL);
};

class OcrFeatureWordsFound {
private:
	volatile MYINT nowFeature;
	trFeatureWordFound allFeature[MAX_WORDS];
public:
	OcrFeatureWordsFound() {
		nowFeature = 0;
	};
	RESULT InitFound(OcrFormat *format);
	MYINT GetNumber() {
		return nowFeature;
	}
	trFeatureWordFound* AddFound(Box *box, char *word);
	Boxa* ReturnFound(int &start, int &match);
	RESULT ReturnCorner(CvPoint2D32f *csrc, CvPoint2D32f *cdst);		// both pointer to CvPoint[4];
};

RESULT trAddFeatureWord(int x, int y, int w, int h, char *word, trFeatureWord *trword, trFeatureWord *next);
void ocrInitBusinessLicense(void);

