/*
** $ Herond Robaina Salles, Rafael Motta de Carvalho
** Prefix: img
*/

#ifndef _IMG_LIB
#define _IMG_LIB

#include <errno.h>

#include "jpeglib.h"
#include "GL/glut.h"

#include "../lst/list.h"
#include "../clr/color.h"

/* ========================================================================== */

enum IMGErros_EN {
    IMG_NO_ERROR = 0,
    IMG_NOT_ENOUGH_SPACE = ENOMEM,
    IMG_INVALID_ARGUMENT = EINVAL
};

enum IMGHistogramModes_EN {
    IMG_RED_CHANNEL = 0,  IMG_GREEN_CHANNEL = 1,
    IMG_BLUE_CHANNEL = 2, IMG_MEAN_CHANNEL = 3
};

enum IMGRescalingMethods_EN {
    IMG_BICUBIC = 0
};

struct IMGImage_ST;

typedef struct IMGImage_ST* IMGImage;

/* ========================================================================== */

IMGImage imgLoadJPG (const char* file_name);

IMGImage imgAlloc (
    unsigned width, unsigned height,
    RGBColor_ST background
);

IMGImage imgCopy (const IMGImage self);

void imgDestroy (IMGImage* self_p);

void imgSetColorPixel (
    IMGImage self,
    unsigned x, unsigned y,
    RGBColor_ST color
);

RGBColor_ST imgGetColorPixel (const IMGImage self, unsigned x, unsigned y);

int imgGetWidth (const IMGImage self);

int imgGetHeight (const IMGImage self);

int imgGetNumPixels (const IMGImage self);

int* imgGetHistogram (const IMGImage self, int mode);

LSTList imgGetJaxColors (
    const IMGImage self,
    unsigned jump_size,
    double tolerance
);

LSTList imgGetKmeansColors (
    const IMGImage self,
    unsigned num_colors,
    unsigned max_iter,
    double tolerance
);

IMGImage imgGetSector (
    const IMGImage self,
    int x, int y,
    unsigned width, unsigned height
);

void imgSubtract (IMGImage self, const IMGImage other_image);

void imgDesaturate (IMGImage self);

void imgQuantize (IMGImage self, const LSTList colors);

unsigned char imgGetBHThreshold (const IMGImage self);

unsigned char imgGetOtsuThreshold (const IMGImage self);

void imgThresholded (IMGImage self, unsigned char threshold);

RGBColor_ST imgApplyConvolutionFilter (const IMGImage self, double** filter);

void imgDraw (const IMGImage self, unsigned x, unsigned y);

/* ========================================================================== */

#endif

