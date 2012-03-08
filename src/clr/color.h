/*
**
** $ Herond Robaina Salles, Rafael Motta de Carvalho
** Prefix: rgb
*/

#ifndef _RGB_LIB
#define _RGB_LIB

#include <errno.h>

#include "../lst/list.h"

/* ========================================================================== */

#define RGB_BLACK      (rgbSet(000, 000, 000))
#define RGB_WHITE      (rgbSet(255, 255, 255))
#define RGB_RED        (rgbSet(255, 000, 000))
#define RGB_GREEN      (rgbSet(000, 255, 000))
#define RGB_BLUE       (rgbSet(000, 000, 255))
#define RGB_MAGENTA    (rgbSet(255, 000, 255))
#define RGB_CIAN       (rgbSet(000, 255, 255))
#define RGB_YELLOW     (rgbSet(255, 255, 000))

/* ========================================================================== */

enum RGBErros_EN {
    RGB_NO_ERROR = 0,
    RGB_NOT_ENOUGH_SPACE = ENOMEM,
    RGB_INVALID_ARGUMENT = EINVAL
};

typedef struct {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} RGBColor_ST;

/* ========================================================================== */

RGBColor_ST rgbSet (unsigned char red, unsigned char green, unsigned char blue);

RGBColor_ST* rgbAlloc (RGBColor_ST self);

double rgbGetDiameter (RGBColor_ST self);

double rgbGetDistance (RGBColor_ST self, RGBColor_ST color);

RGBColor_ST rgbaGetCloser (RGBColor_ST color, const LSTList colors);

double rgbGetSimilarityOnColorsSet (
    RGBColor_ST self,
    const LSTList colors,
    RGBColor_ST* color_p
);

RGBColor_ST rgbBlend (RGBColor_ST self, RGBColor_ST color);

RGBColor_ST rgbSum (RGBColor_ST self, RGBColor_ST color);

RGBColor_ST rgbSub (RGBColor_ST self, RGBColor_ST color);

unsigned char rgbGetAverage (RGBColor_ST self);

RGBColor_ST rgbGetAverageColor (const LSTList colors);

int rgbIsEqual (RGBColor_ST self, RGBColor_ST color);

int rgbIsPresente (RGBColor_ST self, const LSTList colors);

/* ========================================================================== */

LSTList rgbGetBasicColors (void);

/* ========================================================================== */

void rgbPrint (RGBColor_ST self);

void rgbPrintColorSet (const LSTList colors);

#endif

