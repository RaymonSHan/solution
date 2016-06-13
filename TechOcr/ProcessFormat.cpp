#include "ProcessFormat.h"

RESULT trAddFeatureWord(int x, int y, int w, int h, char *word, trFeatureWord *trword, trFeatureWord *next) {
	if (strlen(word) > 3)
		return RESULT_ERR;
	strncpy(trword->word, word, 3);
	trword->origin.x = x;
	trword->origin.y = y;
	trword->origin.w = w;
	trword->origin.h = h;
	trword->h_next = next;
	return RESULT_OK;
}

trFeatureWord* OcrFormat::AddFeatureWord(int x, int y, int w, int h, char *word, trFeatureWord *next) {
	RESULT result;
	MYINT now = InterInc(&nowFeature) - 1;
	if (now >= MAX_WORDS) {
		return NULL;
	}
	result = ::trAddFeatureWord(x, y, w, h, word, &allFeature[now], next);
	if (result == RESULT_OK) return &allFeature[now];
	else return NULL;
}

trFeatureWord* OcrFormat::AddFeatureWord(Box *box, char *word, trFeatureWord *next) {
	return AddFeatureWord(box->x, box->y, box->w, box->h, word, next);
}

RESULT OcrFeatureWordsFound::InitFound(OcrFormat *format) {
	int i, j, delta;
	trFeatureWord *baseword, *nowword;
	trFeatureWordFound *basefound, *nowfound;

	nowFeature = format->nowFeature;
	if (nowFeature >= MAX_WORDS) {
		return RESULT_ERR;
	}
	baseword = format->allFeature;
	basefound = allFeature;
	for (i = 0; i < nowFeature; i++) {
		nowword = &(format->allFeature[i]);
		nowfound = &allFeature[i];
		memcpy(nowfound->word, nowword->word, WORD_LENGTH);
		memcpy(&(nowfound->origin), &(nowword->origin), sizeof(Box));
		delta = nowword->h_next - baseword;
		nowfound->h_next = basefound + delta;
		nowfound->nowFound = 0;
		nowfound->match = false;
		for (j = 0; j < MAX_FOUND; j++) {
			nowfound->found[j].refcount = 1;		// for not free
		}
	}
}

trFeatureWordFound* OcrFeatureWordsFound::AddFound(Box *box, char *word) {
	int i;
	MYINT now;
	trFeatureWordFound *nowword;

	for (i = 0; i < nowFeature; i++) {
		nowword = &allFeature[i];
		if (!strncmp(word, nowword->word, strlen(nowword->word))) {
			now = InterInc(&nowword->nowFound) - 1;
			if (now >= MAX_FOUND) {
				return NULL;
			}
			memcpy(&nowword->found[now], box, sizeof(Box));
			return nowword;
		}
	}
	return NULL;
}

Boxa* OcrFeatureWordsFound::ReturnFound(int &start, int &match) {
	Boxa *boxa;
	trFeatureWordFound *nowword, *twoword;
	int i, j;
	match = 0;
	double rate, maxrate = 0.0;

	for (i = 0; i < start; i++) {
		nowword = &allFeature[i];
		nowword->match = false;
	}
	boxa = boxaCreate(nowFeature);
	for (i = start; i < nowFeature; i++) {
		nowword = &allFeature[i];

		if (nowword->nowFound == 1) {
			boxaAddBox(boxa, &(nowword->found[0]), L_CLONE);
			nowword->match = true;
			start = i;
			break;
		}
		else {
			nowword->match = false;
		}
	}
	for (i = i + 1; i < nowFeature; i++) {
		twoword = &allFeature[i];
		if (twoword->nowFound == 1) {
			if (comMatchPlace(nowword, twoword))
			{
				boxaAddBox(boxa, &(twoword->found[0]), L_CLONE);
				twoword->match = true;
				match++;
			}
			else {
				twoword->match = false;
			}
		}
	}
	return boxa;
}

RESULT OcrFeatureWordsFound::ReturnCorner(CvPoint2D32f *csrc, CvPoint2D32f *cdst) {		// both pointer to CvPoint[4];
	int tl = MAXINT, tr = -1 * MAXINT, bl = MAXINT, br = -1 * MAXINT;
	int ntl, ntr, nbl, nbr;
	int i;
	Box *now;

	for (i = 0; i < nowFeature; i++) {
		if (allFeature[i].match) {
			now = &(allFeature[i].origin);
			if (now->x + now->y < tl) {
				tl = now->x + now->y;
				ntl = i;
			}
			if (now->x + now->w - now->y > tr) {
				tr = now->x + now->w - now->y;
				ntr = i;
			}
			if (now->x - now->y - now->h < bl) {
				bl = now->x - now->y - now->h;
				nbl = i;
			}
			if (now->x + now->y + now->w + now->h) {
				br = now->x + now->y + now->w + now->h;
				nbr = i;
			}
		}
	}
	(csrc + 0)->x = (float)allFeature[ntl].origin.x;
	(csrc + 0)->y = (float)allFeature[ntl].origin.y;
	(cdst + 0)->x = (float)allFeature[ntl].found[0].x;
	(cdst + 0)->y = (float)allFeature[ntl].found[0].y;

	(csrc + 1)->x = (float)allFeature[ntr].origin.x + allFeature[ntr].origin.w;
	(csrc + 1)->y = (float)allFeature[ntr].origin.y;
	(cdst + 1)->x = (float)allFeature[ntr].found[0].x + allFeature[ntr].found[0].w;
	(cdst + 1)->y = (float)allFeature[ntr].found[0].y;

	(csrc + 2)->x = (float)allFeature[nbr].origin.x + allFeature[nbr].origin.w;
	(csrc + 2)->y = (float)allFeature[nbr].origin.y + allFeature[nbr].origin.h;
	(cdst + 2)->x = (float)allFeature[nbr].found[0].x + allFeature[nbr].found[0].w;
	(cdst + 2)->y = (float)allFeature[nbr].found[0].y + allFeature[nbr].found[0].h;

	(csrc + 3)->x = (float)allFeature[nbl].origin.x;
	(csrc + 3)->y = (float)allFeature[nbl].origin.y + allFeature[nbl].origin.h;
	(cdst + 3)->x = (float)allFeature[nbl].found[0].x;
	(cdst + 3)->y = (float)allFeature[nbl].found[0].y + allFeature[nbl].found[0].h;
	return RESULT_OK;
}

OcrFormat BusinessLicense;

void ocrInitBusinessLicense(void) {
	trFeatureWord *now;
	now = BusinessLicense.AddFeatureWord(622, 1638, 61, 69, "名");
	now = BusinessLicense.AddFeatureWord(962, 1641, 69, 66, "称");
	now = BusinessLicense.AddFeatureWord(624, 1758, 65, 68, "类");
	now = BusinessLicense.AddFeatureWord(964, 1757, 66, 65, "型");
	now = BusinessLicense.AddFeatureWord(623, 1872, 68, 68, "住");
	now = BusinessLicense.AddFeatureWord(964, 1873, 68, 65, "所");

	now = BusinessLicense.AddFeatureWord(627, 2100, 64, 64, "注");
	now = BusinessLicense.AddFeatureWord(738, 2098, 68, 64, "册");
	now = BusinessLicense.AddFeatureWord(854, 2095, 62, 70, "资");
	now = BusinessLicense.AddFeatureWord(965, 2095, 68, 70, "本");
	
	now = BusinessLicense.AddFeatureWord(625, 2212, 67, 70, "成");
	now = BusinessLicense.AddFeatureWord(740, 2215, 65, 63, "立");
	now = BusinessLicense.AddFeatureWord(853, 2212, 65, 70, "日");
//	now = BusinessLicense.AddFeatureWord(967, 2212, 65, 68, "期");

//	now = BusinessLicense.AddFeatureWord(625, 2332, 65, 71, "营");
	now = BusinessLicense.AddFeatureWord(739, 2332, 67, 65, "业");
//	now = BusinessLicense.AddFeatureWord(853, 2330, 65, 68, "期");
	now = BusinessLicense.AddFeatureWord(970, 2330, 63, 66, "限");

	now = BusinessLicense.AddFeatureWord(624, 2451, 67, 64, "经");
//	now = BusinessLicense.AddFeatureWord(739, 2448, 66, 70, "营");
	now = BusinessLicense.AddFeatureWord(853, 2445, 67, 71, "范");
	now = BusinessLicense.AddFeatureWord(971, 2447, 59, 67, "围");

	now = BusinessLicense.AddFeatureWord(1562, 3436, 71, 67, "登");
	now = BusinessLicense.AddFeatureWord(1680, 3435, 71, 69, "记");
	now = BusinessLicense.AddFeatureWord(1798, 3435, 74, 70, "机");
	now = BusinessLicense.AddFeatureWord(1920, 3433, 67, 72, "关");
	return;
}

