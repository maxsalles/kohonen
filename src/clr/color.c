/*
** $ Herond Robaina Salles, Rafael Motta de Carvalho
** Prefix: rgb
*/

#ifndef _RGB_IMP
#define _RGB_IMP

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "color.h"

/* ========================================================================== */

static int rgb_current_error = RGB_NO_ERROR;

/* ========================================================================== */

RGBColor_ST rgbSet (
    unsigned char red,
    unsigned char green,
    unsigned char blue
) {
    RGBColor_ST color_return = { red, green, blue };

    return color_return;
};

RGBColor_ST* rgbAlloc (RGBColor_ST self) {
    RGBColor_ST* color_p = (RGBColor_ST*) malloc(sizeof(RGBColor_ST));

    if (color_p) {
        color_p->red = self.red;
        color_p->green = self.green;
        color_p->blue = self.blue;
    } else rgb_current_error = RGB_NOT_ENOUGH_SPACE;

    return color_p;
}

double rgbGetDiameter (RGBColor_ST self) {
    return sqrt(
        pow((double) self.red, 2.0) +
        pow((double) self.green, 2.0) +
        pow((double) self.blue, 2.0)
    );
}

double rgbGetDistance (RGBColor_ST self, RGBColor_ST color) {
    return sqrt(
        pow((double) (self.red - color.red), 2.0) +
        pow((double) (self.green - color.green), 2.0) +
        pow((double) (self.blue - color.blue), 2.0)
    );
}

RGBColor_ST rgbGetCloser (RGBColor_ST self, const LSTList colors) {
    int i;
    RGBColor_ST curr_color;
    RGBColor_ST color_return = * (RGBColor_ST*) lstLookElement(colors, 0);
    double shortest_distance = rgbGetDistance(self, color_return);
    double curr_distance = 0.0;

    for (i = 1; i < lstGetLength(colors); i ++) {
        curr_color = * (RGBColor_ST*) lstLookElement(colors, i);
        curr_distance = rgbGetDistance(self, curr_color);

        if (curr_distance < shortest_distance) {
            shortest_distance = curr_distance;
            color_return = curr_color;
        }
    }

    return color_return;
}

double rgbGetSimilarityOnColorsSet (
    RGBColor_ST self,
    const LSTList colors,
    RGBColor_ST* color_p
) {
    int i;
    RGBColor_ST curr_color = * (RGBColor_ST*) lstLookElement(colors, 0);
    RGBColor_ST color_aux;
    double shortest_distance = rgbGetDistance(self, curr_color);
    double curr_distance = rgbGetDistance(self, curr_color);

    for (i = 1; i < lstGetLength(colors); i ++) {
        curr_color = * (RGBColor_ST*) lstLookElement(colors, i);
        curr_distance = rgbGetDistance(self, curr_color);

        if (curr_distance < shortest_distance) {
            shortest_distance = curr_distance;
            color_aux = curr_color;
            if (color_p) {
                color_p->red = curr_color.red;
                color_p->green = curr_color.green;
                color_p->blue = curr_color.blue;
            }
        }
    }

    return 1.0 - (shortest_distance / rgbGetDiameter(RGB_WHITE));
}

RGBColor_ST rgbBlend (RGBColor_ST self, RGBColor_ST color) {
    return rgbSet(
        (self.red + color.red) / 2,
        (self.green + color.green) / 2,
        (self.blue + color.blue) / 2
    );
}

RGBColor_ST rgbSum (RGBColor_ST self, RGBColor_ST color) {
    return rgbSet(
        self.red + color.red,
        self.green + color.green,
        self.blue + color.blue
    );
}

RGBColor_ST rgbSub (RGBColor_ST self, RGBColor_ST color) {
    return rgbSet(
        self.red < color.red ? 0 : self.red - color.red,
        self.green < color.green ? 0 : self.green - color.green,
        self.blue < color.blue ? 0 : self.blue - color.blue
    );
}

unsigned char rgbGetAverage (RGBColor_ST self) {
    return (self.red + self.green + self.blue) / 3;
}

RGBColor_ST rgbGetAverageColor (const LSTList colors) {
    int i, length = lstGetLength(colors);
    double red = 0.0, green = 0.0, blue = 0.0;
    RGBColor_ST* curr_color_p = NULL;

    for (i = 0; i < length; i ++) {
        curr_color_p = (RGBColor_ST*) lstLookElement(colors, i);
        red += curr_color_p->red;
        green += curr_color_p->green;
        blue += curr_color_p->blue;
    }

    return rgbSet(red / length, green / length, blue / length);
}

int rgbIsEqual (RGBColor_ST self, RGBColor_ST color) {
    return (
        self.red == color.red &&
        self.green == color.green &&
        self.blue == color.blue
    );
}

int rgbIsPresente (RGBColor_ST self, const LSTList colors) {
    int i;

    for (i = 0; i < lstGetLength(colors); i ++)
        if (rgbIsEqual(self, * (RGBColor_ST*) lstLookElement(colors, i)))
            return 1;

    return 0;
}

/* ========================================================================== */

LSTList rgbGetBasicColors () {
    int i;
    static RGBColor_ST basic_colors_vc[] = {
        { 000, 000, 000 }, { 255, 255, 255 },
        { 255, 000, 000 }, { 000, 255, 000 },
        { 000, 000, 255 }, { 255, 000, 255 },
        { 000, 255, 255 }, { 255, 255, 000 }
    };
    RGBColor_ST* curr_color_p;
    LSTList basic_colors = lstNew();

    if (basic_colors)
        for (i = 0; i < 8; i ++) {
            curr_color_p = rgbAlloc(basic_colors_vc[i]);

            if (curr_color_p) lstAppend(basic_colors, curr_color_p);
            else {
                rgb_current_error = RGB_NOT_ENOUGH_SPACE;
                lstDestroy(&basic_colors);

                return NULL;
            }
        }
    else rgb_current_error = RGB_NOT_ENOUGH_SPACE;

    return basic_colors;
}

/* ========================================================================== */

void rgbPrint (RGBColor_ST self) {
    printf("(%03i %03i %03i)\n", self.red, self.green, self.blue);
}


void rgbPrintColorSet (const LSTList colors) {
    int i;

    for (i = 0; i < lstGetLength(colors); i ++) {
        printf("%i: ", i);
        rgbPrint(* (RGBColor_ST*) lstLookElement(colors, i));
    }
}

/* ========================================================================== */

#endif

