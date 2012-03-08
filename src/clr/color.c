/*
**
** $ Herond Robaina Salles, Rafael Motta de Carvalho
** prefix: rgba
*/

#ifndef _RGBA_IMP
#define _RGBA_IMP

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "color.h"

/* ========================================================================== */

static int current_error = RGBA_NO_ERROR;

/* ========================================================================== */

RGBAColor_ST rgbaSet3b (
    unsigned char red,
    unsigned char green,
    unsigned char blue
) {
    RGBAColor_ST color_return = { red, green, blue, 255 };

    return color_return;
};

RGBAColor_ST rgbaSet4b (
    unsigned char red,
    unsigned char green,
    unsigned char blue,
    unsigned char alpha
) {
    RGBAColor_ST color_return = { red, green, blue, alpha };

    return color_return;
};

RGBAColor_ST* rgbaAlloc (RGBAColor_ST color) {
    RGBAColor_ST* color_ptr_return = (RGBAColor_ST*) malloc(
        sizeof(RGBAColor_ST)
    );

    if (color_ptr_return) {
        color_ptr_return->red = color.red;
        color_ptr_return->green = color.green;
        color_ptr_return->blue = color.blue;
        color_ptr_return->alpha = color.alpha;
    } else current_error = RGBA_NOT_ENOUGH_SPACE;

    return color_ptr_return;
}

double rgbaGetDiameter (RGBAColor_ST color) {
    return sqrt(
        pow((double) color.red, 2.0) +
        pow((double) color.green, 2.0) +
        pow((double) color.blue, 2.0)
    );
}

double rgbaGetDistance (RGBAColor_ST color1, RGBAColor_ST color2) {
    return sqrt(
        pow((double) (color1.red - color2.red), 2.0) +
        pow((double) (color1.green - color2.green), 2.0) +
        pow((double) (color1.blue - color2.blue), 2.0)
    );
}

RGBAColor_ST rgbaGetCloser (RGBAColor_ST color, const CLSTList colors) {
    int i;
    RGBAColor_ST current_color;
    RGBAColor_ST color_return = * (RGBAColor_ST*) clstLookElement(colors, 0);
    double shortest_distance = rgbaGetDistance(color, color_return);
    double current_distance = 0.0;

    for (i = 1; i < clstGetLength(colors); i++) {
        current_color = * (RGBAColor_ST*) clstLookElement(colors, i);
        current_distance = rgbaGetDistance(color, current_color);

        if (current_distance < shortest_distance) {
            shortest_distance = current_distance;
            color_return = current_color;
        }
    }

    return color_return;
}

double rgbaGetSimilarityOnColorsSet (
    RGBAColor_ST color,
    const CLSTList colors,
    RGBAColor_ST* color_p
) {
    int i;
    RGBAColor_ST current_color = * (RGBAColor_ST*) clstLookElement(colors, 0);
    RGBAColor_ST color_aux;
    double shortest_distance = rgbaGetDistance(color, current_color);
    double current_distance = rgbaGetDistance(color, current_color);

    for (i = 1; i < clstGetLength(colors); i ++) {
        current_color = * (RGBAColor_ST*) clstLookElement(colors, i);
        current_distance = rgbaGetDistance(color, current_color);

        if (current_distance < shortest_distance) {
            shortest_distance = current_distance;
            color_aux = current_color;
            if (color_p) {
                color_p->red = current_color.red;
                color_p->green = current_color.green;
                color_p->blue = current_color.blue;
                color_p->alpha = current_color.alpha;
            }
        }
    }

    return 1.0 - (shortest_distance / rgbaGetDiameter(RGBA_WHITE));
}

RGBAColor_ST rgbaBlend (RGBAColor_ST color1, RGBAColor_ST color2) {
    return rgbaSet3b(
        (color1.red + color2.red) / 2,
        (color1.green + color2.green) / 2,
        (color1.blue + color2.blue) / 2
    );
}

RGBAColor_ST rgbaSum (RGBAColor_ST color1, RGBAColor_ST color2) {
    return rgbaSet3b(
        color1.red + color2.red,
        color1.green + color2.green,
        color1.blue + color2.blue
    );
}

RGBAColor_ST rgbaSub (RGBAColor_ST color1, RGBAColor_ST color2) {
    return rgbaSet3b(
        color1.red < color2.red ? 0 : color1.red - color2.red,
        color1.green < color2.green ? 0 : color1.green - color2.green,
        color1.blue < color2.blue ? 0 : color1.blue - color2.blue
    );
}

unsigned char rgbaGetAverage (RGBAColor_ST color) {
    return (color.red + color.green + color.blue) / 3;
}

RGBAColor_ST rgbaGetAverageColor (const CLSTList colors) {
    int i, length = clstGetLength(colors);
    double red = 0.0, green = 0.0, blue = 0.0;
    RGBAColor_ST* current_color_p = NULL;

    for (i = 0; i < length; i ++) {
        current_color_p = (RGBAColor_ST*) clstLookElement(colors, i);
        red += current_color_p->red;
        green += current_color_p->green;
        blue += current_color_p->blue;
    }

    return rgbaSet3b(red / length, green / length, blue / length);
}

int rgbaIsEqual (RGBAColor_ST color1, RGBAColor_ST color2) {
    return (
        color1.red == color2.red &&
        color1.green == color2.green &&
        color1.blue == color2.blue
    );
}

int rgbaIsPresente (RGBAColor_ST color, const CLSTList colors) {
    int i;

    for (i = 0; i < clstGetLength(colors); i ++)
        if (rgbaIsEqual(color, * (RGBAColor_ST*) clstLookElement(colors, i)))
            return 1;

    return 0;
}

/* ========================================================================== */

CLSTList rgbaGetBasicColors () {
    int i;
    static RGBAColor_ST basic_colors_vc[] = {
        { 000, 000, 000, 255 }, { 255, 255, 255, 255 },
        { 255, 000, 000, 255 }, { 000, 255, 000, 255 },
        { 000, 000, 255, 255 }, { 255, 000, 255, 255 },
        { 000, 255, 255, 255 }, { 255, 255, 000, 255 }
    };
    RGBAColor_ST* current_color_p;
    CLSTList basic_colors = clstNew();

    if (basic_colors)
        for (i = 0; i < 8; i ++) {
            current_color_p = rgbaAlloc(basic_colors_vc[i]);

            if (current_color_p)
                clstAppend(basic_colors, current_color_p);
            else {
                current_error = RGBA_NOT_ENOUGH_SPACE;
                clstDestroy(&basic_colors);

                return NULL;
            }
        }
    else current_error = RGBA_NOT_ENOUGH_SPACE;

    return basic_colors;
}

/* ========================================================================== */

void rgbaPrint (RGBAColor_ST color) {
    printf(
        "(%03i %03i %03i %03i)\n",
        color.red, color.green, color.blue, color.alpha
    );
}


void rgbaPrintColorSet (const CLSTList colors) {
    int i;

    for (i = 0; i < clstGetLength(colors); i ++) {
        printf("%i: ", i);
        rgbaPrint(* (RGBAColor_ST*) clstLookElement(colors, i));
    }
}

/* ========================================================================== */

#endif

