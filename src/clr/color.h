/*
**
** $ Herond Robaina Salles, Rafael Motta de Carvalho
** prefix: rgba
*/

#ifndef _RGBA_LIB
#define _RGBA_LIB

#include <errno.h>

#include "../../../ctollkit/src/list/list.h"

/* ========================================================================== */

#define RGBA_BLACK      (rgbaSet3b(000, 000, 000))
#define RGBA_WHITE      (rgbaSet3b(255, 255, 255))
#define RGBA_RED        (rgbaSet3b(255, 000, 000))
#define RGBA_GREEN      (rgbaSet3b(000, 255, 000))
#define RGBA_BLUE       (rgbaSet3b(000, 000, 255))
#define RGBA_MAGENTA    (rgbaSet3b(255, 000, 255))
#define RGBA_CIAN       (rgbaSet3b(000, 255, 255))
#define RGBA_YELLOW     (rgbaSet3b(255, 255, 000))

/* ========================================================================== */

enum RGBAErros_EN {
    RGBA_NO_ERROR = 0,
    RGBA_NOT_ENOUGH_SPACE = ENOMEM,
    RGBA_INVALID_ARGUMENT = EINVAL
};

typedef struct {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
} RGBAColor_ST;

/* ========================================================================== */

RGBAColor_ST rgbaSet3b (
    unsigned char red,
    unsigned char green,
    unsigned char blue
);

RGBAColor_ST rgbaSet4b (
    unsigned char red,
    unsigned char green,
    unsigned char blue,
    unsigned char alpha
);

RGBAColor_ST* rgbaAlloc (RGBAColor_ST color);

double rgbaGetDiameter (RGBAColor_ST color);

double rgbaGetDistance (RGBAColor_ST color1, RGBAColor_ST color2);

RGBAColor_ST rgbaGetCloser (RGBAColor_ST color, const CLSTList colors);

double rgbaGetSimilarityOnColorsSet (
    RGBAColor_ST color,
    const CLSTList colors,
    RGBAColor_ST* color_p
);

RGBAColor_ST rgbaBlend (RGBAColor_ST color1, RGBAColor_ST color2);

RGBAColor_ST rgbaSum (RGBAColor_ST color1, RGBAColor_ST color2);

RGBAColor_ST rgbaSub (RGBAColor_ST color1, RGBAColor_ST color2);

unsigned char rgbaGetAverage (RGBAColor_ST color);

RGBAColor_ST rgbaGetAverageColor (const CLSTList colors);

int rgbaIsEqual (RGBAColor_ST color1, RGBAColor_ST color2);

int rgbaIsPresente (RGBAColor_ST color, const CLSTList colors);

/* ========================================================================== */

CLSTList rgbaGetBasicColors (void);

/* ========================================================================== */

void rgbaPrint (RGBAColor_ST color);

void rgbaPrintColorSet (const CLSTList colors);

#endif

