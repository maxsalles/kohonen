/*
** $ Herond Robaina Salles, Rafael Motta de Carvalho
** Prefix: img
*/

#ifndef _IMG_IMP
#define _IMG_IMP

#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <time.h>

#include "GL/gl.h"
#include "image.h"

/* ========================================================================== */

static int img_current_error = IMG_NO_ERROR;

static const int IMG_NUM_COMPONENTS = 3;
static enum IMGComponents_EN { IMG_RED = 0, IMG_GREEN = 1, IMG_BLUE = 2 };

/* ========================================================================== */

struct IMGImage_ST {
    int components;
    unsigned width;
    unsigned height;
    unsigned char* data;
};

/* ========================================================================== */

static void decode (struct jpeg_decompress_struct* cinfo, IMGImage image) {
    int i, row_span = 0, rows_read = 0;

    if (cinfo && image) {
	    jpeg_read_header(cinfo, TRUE);
	    jpeg_start_decompress(cinfo);

	    image->components = cinfo->num_components;
	    image->width = cinfo->image_width;
	    image->height = cinfo->image_height;

	    row_span = image->components * image->width;
	    image->data = (unsigned char*) malloc(
	        sizeof(unsigned char) * row_span * image->height
	    );

	    unsigned char** row_ptr = (unsigned char**) malloc(
	        sizeof(unsigned char*) * image->height
        );

        if (image->data && row_ptr) {
            for (i = 0; i < image->height; i ++)
                row_ptr[image->height - i - 1] = &(image->data[i * row_span]);

	        rows_read = 0;
	        while (cinfo->output_scanline < cinfo->output_height)
		        rows_read += jpeg_read_scanlines(
		            cinfo, &row_ptr[rows_read], cinfo->output_height - rows_read
                );

	        free(row_ptr);

	        jpeg_finish_decompress(cinfo);
	   } else img_current_error = IMG_NOT_ENOUGH_SPACE;
    } else img_current_error = IMG_INVALID_ARGUMENT;
}

static void clear (IMGImage self) {
    if (self && self->data) {
        free(self->data);
        self->data = NULL;
        self->width = 0;
        self->height = 0;
    } else img_current_error = IMG_INVALID_ARGUMENT;
}

/*
** Aux functions
*/

static RGBColor_ST pixelToColor (const unsigned char* pixel) {
    if (pixel) {
        RGBColor_ST color_return = {
            pixel[IMG_RED], pixel[IMG_GREEN], pixel[IMG_BLUE]
        };

        return color_return;
    }

    img_current_error = IMG_INVALID_ARGUMENT;

    return RGB_BLACK;
}

static unsigned char* getPixel (const IMGImage image, unsigned x, unsigned y) {
    if (image && image->data && x < image->width && y < image->height)
       return image->data + (y * image->width + x) * image->components;
    img_current_error = IMG_INVALID_ARGUMENT;

    return NULL;
}

/*
** Kmeans
*/

static unsigned getPositionOfColorMoreSimilar (
    LSTList colors, unsigned char* pixel
) {
    int i, position_return = 0;
    double shortest_distance = DBL_MAX, curr_distance;
    RGBColor_ST color = pixelToColor(pixel), curr_color;

    for (i = 0; i < lstGetLength(colors); i ++) {
        curr_color = pixelToColor((unsigned char*) lstLookElement(colors, i));
        curr_distance = rgbGetDistance(color, curr_color);

        if (curr_distance < shortest_distance) {
            position_return = i;
            shortest_distance = curr_distance;
        }
    }

    return position_return;
}

static LSTList initCenters (const IMGImage image, int num_colors) {
    int i, x, y;
    unsigned char* pixel = NULL;
    LSTList colors_return = lstNew();

    srand(time(NULL));
    for (i = 0; i < num_colors; i ++) {
        x = rand() % image->width;
        y = rand() % image->height;

        pixel = getPixel(image,  x, y);
        if (! rgbIsPresente(pixelToColor(pixel), colors_return))
            lstAppend(colors_return, pixel);
        else i --;
    }

    return colors_return;
}

static LSTList setColorsCluster (
    const IMGImage image, const LSTList centers
) {
    int i, j;
    unsigned char* pixel = NULL;
    LSTList clusters = lstNew(), curr_cluster = NULL;

    for (i = 0; i < lstGetLength(centers); i ++) lstAppend(clusters, lstNew());

    for (i = 0; i < image->width; i ++)
        for (j = 0; j < image->height; j ++) {
            pixel = getPixel(image, i, j);
            curr_cluster = (LSTList) lstLookElement(clusters,
                getPositionOfColorMoreSimilar(centers, pixel)
            );

            lstAppend(curr_cluster, pixel);
        }

    return clusters;
}

LSTList getCenters (const LSTList clusters) {
    int i, length = lstGetLength(clusters);
    RGBColor_ST average_color;
    LSTList centers = lstNew(), cluster = NULL;

    for (i = 0; i < length; i ++) {
        cluster = (LSTList) lstLookElement(clusters, i);
        average_color = rgbGetAverageColor(cluster);
        lstAppend(centers, rgbAlloc(average_color));
    }

    return centers;
}

double similarity (LSTList colors1, LSTList colors2) {
    int i, length = lstGetLength(colors1);
    double similarity_average = 0.0;
    RGBColor_ST color1, color2;

    for (i = 0; i < length; i ++) {
        color1 = pixelToColor((unsigned char*) lstLookElement(colors1, i));
        color2 = pixelToColor((unsigned char*) lstLookElement(colors2, i));
        similarity_average += rgbGetDistance(color1, color2);
    }

    return similarity_average / length;
}

/* ========================================================================== */

IMGImage imgLoadJPG (const char* file_name) {
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	IMGImage  image_return = NULL;
	FILE* file = NULL;

    if (file_name && (file = fopen(file_name, "rb"))) {
	    cinfo.err = jpeg_std_error(&jerr);
	    jpeg_create_decompress(&cinfo);
	    jpeg_stdio_src(&cinfo, file);

	    image_return = (IMGImage) malloc(sizeof(struct IMGImage_ST));

	    decode(&cinfo, image_return);
	    jpeg_destroy_decompress(&cinfo);
	    fclose(file);
	} else img_current_error = IMG_INVALID_ARGUMENT;

	return image_return;
}

IMGImage imgAlloc (
    unsigned width, unsigned height,
    RGBColor_ST background
) {
    int i, j;
    IMGImage image_return = (IMGImage) malloc(sizeof(struct IMGImage_ST));

    if (image_return) {
        image_return->data = (unsigned char*) malloc(
            sizeof(unsigned char*) * height * width * IMG_NUM_COMPONENTS
        );
        image_return->width = width;
        image_return->height = height;

        for (i = 0; i < image_return->width; i ++)
            for (j = 0; j < image_return->height; j ++)
                imgSetColorPixel(image_return, i, j, background);
    } else img_current_error = IMG_NOT_ENOUGH_SPACE;

    return image_return;
}

IMGImage imgCopy (const IMGImage self) {
    int i, j;
    IMGImage copy = imgAlloc(self->width, self->height, RGB_BLACK);
    RGBColor_ST curr_color;

    if (copy)
        for (i = 0; i < self->width; i ++)
            for (j = 0; j < self->height; j ++) {
                curr_color = imgGetColorPixel(self, i, j);
                imgSetColorPixel(copy, i, j, curr_color);
            }

    return copy;
}

void imgDestroy (IMGImage* image_p) {
    if (image_p && *image_p) {
        clear(*image_p);
        free(*image_p);
        *image_p = NULL;
    } else img_current_error = IMG_INVALID_ARGUMENT;
}


void imgSetColorPixel (
    IMGImage self,
    unsigned x, unsigned y,
    RGBColor_ST color
) {
    if (self && self->data) {
        unsigned char* pixel = self->data;

        pixel += (y * self->width + x) * self->components;
        pixel[0] = color.red;
        pixel[1] = color.green;
        pixel[2] = color.blue;
    } else img_current_error = IMG_INVALID_ARGUMENT;
}

RGBColor_ST imgGetColorPixel (
    const IMGImage self,
    unsigned x, unsigned y
) {
    if (self && self->data) {
        unsigned char* pixel = self->data;

        if (x < self->width && y < self->height) {
            pixel += (y * self->width + x) * self->components;

            return rgbSet(pixel[IMG_RED], pixel[IMG_GREEN], pixel[IMG_BLUE]);
        }
    } else img_current_error = IMG_INVALID_ARGUMENT;

    return RGB_BLACK;
}

int imgGetWidth (const IMGImage self) {
    if (self) return self->width;
    img_current_error = IMG_INVALID_ARGUMENT;

    return -1;
}

int imgGetHeight (const IMGImage self) {
    if (self) return self->height;
    img_current_error = IMG_INVALID_ARGUMENT;

    return -1;
}

int imgGetNumPixels (const IMGImage self) {
    if (self) return self->height * self->width;
    img_current_error = IMG_INVALID_ARGUMENT;

    return -1;
}

int* imgGetHistogram (const IMGImage self, int mode) {
    if (self && self->data) {
        int i, j;
        int* histogram = (int*) malloc(sizeof(int) * 256);

        if (histogram) {
            for (i = 0; i < 256; i ++) histogram[i] = 0;

            switch (mode) {
            case IMG_RED_CHANNEL:
                for (i = 0; i < self->width; i ++)
                    for (j = 0; j < self->height; j ++)
                        histogram[imgGetColorPixel(self, i, j).red] ++;

                break;
            case IMG_GREEN_CHANNEL:
                for (i = 0; i < self->width; i ++)
                    for (j = 0; j < self->height; j ++)
                        histogram[imgGetColorPixel(self, i, j).green] ++;

                break;
            case IMG_BLUE_CHANNEL:
                for (i = 0; i < self->width; i ++)
                    for (j = 0; j < self->height; j ++)
                        histogram[imgGetColorPixel(self, i, j).blue] ++;

                break;
            case IMG_MEAN_CHANNEL:
                for (i = 0; i < self->width; i ++)
                    for (j = 0; j < self->height; j ++)
                        histogram[
                            rgbGetAverage(imgGetColorPixel(self, i, j))
                        ] ++;

                break;
            }

            return histogram;
        } img_current_error = IMG_NOT_ENOUGH_SPACE;
    } else img_current_error = IMG_INVALID_ARGUMENT;

    return NULL;
}

LSTList imgGetJaxColors (
    const IMGImage self,
    unsigned jump_size,
    double tolerance
) {
    int i, j;
    RGBColor_ST curr_color;
    LSTList colors = lstNew();

    lstAppend(colors, rgbAlloc(imgGetColorPixel(self, 0, 0)));

    for (i = 0; i < self->width; i += jump_size)
        for (j = 0; j < self->height; j += jump_size) {
            curr_color = imgGetColorPixel(self, i, j);

            if (rgbGetSimilarityOnColorsSet(curr_color, colors, NULL)
                < tolerance) lstAppend(colors, rgbAlloc(curr_color));
        }

    return colors;
}

LSTList imgGetKmeansColors (
    const IMGImage self,
    unsigned num_colors,
    unsigned max_iter,
    double tolerance
) {
    int i, j;
    double similarity_aux = 0.0;
    LSTList cluster = NULL, curr_centers = NULL;
    LSTList curr_cluster = NULL, next_centers = NULL;

    curr_centers = initCenters(self, num_colors);
    cluster = setColorsCluster(self, curr_centers);
    next_centers = getCenters(cluster);

    for (i = 0; i < max_iter; i ++) {
        similarity_aux = similarity(curr_centers, next_centers);
        if (similarity_aux <= tolerance) return curr_centers;

        lstDestroy(&curr_centers);
        curr_centers = lstCopy(next_centers);
        lstDestroy(&next_centers);

        for (j = 0; j < lstGetLength(cluster); j ++) {
            curr_cluster = (LSTList) lstGetFirstElement(cluster);
            lstDestroy(&curr_cluster);
        }

        lstDestroy(&cluster);
        cluster = setColorsCluster(self, curr_centers);
        next_centers = getCenters(cluster);
    }

    return curr_centers;
}

IMGImage imgGetSector (
    const IMGImage self,
    int x, int y,
    unsigned width, unsigned height
) {
    int i, j, a, b, i_offset = x + width, j_offset = y + height;
    IMGImage sector = imgAlloc(width, height, RGB_BLACK);

    if (sector && self && self->data)
        for (i = x, a = 0; i < i_offset; i ++, a ++)
            for (j = y, b = 0; j < j_offset; j ++, b ++)
                if (i >= 0 && j >= 0 && i < self->width && j < self->height)
                    imgSetColorPixel(
                        sector, a, b, imgGetColorPixel(self, i, j)
                    );
                else imgSetColorPixel(sector, a, b, RGB_BLACK);

    return sector;
}

void imgSubtract (IMGImage self, const IMGImage other_image) {
    int i, j;
    int width = self->width < other_image->width ?
        self->width : other_image->width;
    int height = self->height < other_image->height ?
        self->height : other_image->height;
    RGBColor_ST curr_color;

    for (i = 0; i < width; i ++)
        for (j = 0; j < height; j ++) {
            curr_color = rgbSub(
                imgGetColorPixel(self, i, j),
                imgGetColorPixel(other_image, i, j)
            );
            imgSetColorPixel(self, i, j, curr_color);
        }
}

void imgDesaturate (IMGImage self) {
    if (self) {
        int i, j;
        unsigned average = 0;
        RGBColor_ST curr_pixel;

        for (i = 0; i < self->width; i ++)
            for (j = 0; j < self->height; j ++) {
                curr_pixel = imgGetColorPixel(self, i, j);
                average = rgbGetAverage(curr_pixel);
                imgSetColorPixel(
                    self, i, j, rgbSet(average, average, average)
                );
            }
    } else img_current_error = IMG_INVALID_ARGUMENT;
}

void imgQuantize (IMGImage self, const LSTList colors) {
    int i, j;
    RGBColor_ST curr_color, color;

    for (i = 0; i < self->width; i ++)
        for (j = 0; j < self->height; j ++) {
            curr_color = imgGetColorPixel(self, i, j);
            rgbGetSimilarityOnColorsSet(curr_color, colors, &color);
            imgSetColorPixel(self, i, j, color);
        }
}

unsigned char imgGetBHThreshold (int* histogram) {
    int l_limit = 0,  r_limit = 172;
    int l_weight = 0, r_weight = 0;
    int i, threshold = (r_limit + l_limit) / 2;

/*    for (i = 0; i < 256; i ++) printf("(%i %i)\n", i, histogram[i]);*/

    for (i = l_limit; i <= threshold; i ++) l_weight += histogram[i];
    for (i = threshold + 1; i < r_limit + 1; i ++) r_weight += histogram[i];

    while (l_limit <= r_limit) {
        printf("<<(%i %i %i) => %i %i >>\n", l_limit, threshold, r_limit, l_weight, r_weight);

        if (r_weight > l_weight) {
            r_weight -= histogram[r_limit --];

            if ((r_limit + l_limit) / 2 < threshold) {
                r_weight += histogram[threshold];
                l_weight -= histogram[threshold --];
            }
        } else {
            l_weight -= histogram[l_limit ++];

            if ((r_limit + l_limit) / 2 > threshold) {
                l_weight += histogram[threshold + 1];
                r_weight -= histogram[threshold + 1];
                threshold ++;
            }
        }
    }

    return threshold;
/*        int i, i_m, w_l = 0, w_r = 0, i_s = 0, i_e = 172;*/

/*       i_m = (int)((i_s + i_e) / 2.0f); // Base da balança I_m*/

/*       for (i = i_s; i <= i_m; i ++) w_l += histogram[i];*/
/*       for (i = i_m + 1; i <  i_e + 1; i ++) w_r += histogram[i];*/

/*       while (i_s <= i_e) {*/
/*               printf("<<(%i %i %i) => %i %i >>\n", i_s, i_m, i_e, w_l, w_r);*/
/*           if (w_r > w_l) { // mais peso à direita*/
/*               w_r -= histogram[i_e--];*/
/*               if (((i_s + i_e) / 2) < i_m) {*/
/*                   w_r += histogram[i_m];*/
/*                   w_l -= histogram[i_m--];*/
/*               }*/
/*           } else if (w_l >= w_r) { // mais peso à esquerda*/
/*               w_l -= histogram[i_s++];*/
/*               if (((i_s + i_e) / 2) > i_m) {*/
/*                   w_l += histogram[i_m + 1];*/
/*                   w_r -= histogram[i_m + 1];*/
/*                   i_m++;*/
/*               }*/
/*           }*/
/*       }*/
/*       return i_m;*/
}

void imgThresholded (IMGImage self, unsigned char threshold) {
    int i, j;
    RGBColor_ST curr_color;

    if (self && self->data) {
        for (i = 0; i < self->width; i ++)
            for (j = 0; j < self->height; j ++) {
                curr_color = imgGetColorPixel(self, i, j);

                if (rgbGetAverage(curr_color) < threshold)
                    imgSetColorPixel(self, i, j, RGB_WHITE);
                else
                    imgSetColorPixel(self, i, j, RGB_BLACK);
            }
    } else img_current_error = IMG_INVALID_ARGUMENT;
}

RGBColor_ST imgApplyConvolutionFilter (const IMGImage self, double** filter) {
    int i, j;
    double acc[] = { 0.0, 0.0, 0.0 };
    double factor = 0.0;
    unsigned char* curr_pixel = self->data;

    if (self && self->data && filter && *filter)
        for (i = 0; i < self->width; i ++)
            for (j = 0; j < self->height; j ++) {
                curr_pixel += (j * self->width + i) * self->components;
                acc[IMG_RED] += curr_pixel[IMG_RED] * filter[i][j];
                acc[IMG_GREEN] += curr_pixel[IMG_GREEN] * filter[i][j];
                acc[IMG_BLUE] += curr_pixel[IMG_BLUE] * filter[i][j];
                factor += fabs(filter[i][j]);
                curr_pixel = self->data;
            }
    else img_current_error = IMG_INVALID_ARGUMENT;

    return rgbSet(
        (unsigned char) fabs(acc[IMG_RED] / factor),
        (unsigned char) fabs(acc[IMG_GREEN] / factor),
        (unsigned char) fabs(acc[IMG_BLUE] / factor)
    );
}

void imgDraw (const IMGImage image, unsigned x, unsigned y) {
    if (image) {
        glRasterPos2i(0, 0);
        glDrawPixels(
            image->width, image->height, GL_RGB, GL_UNSIGNED_BYTE, image->data
        );
    } else img_current_error = IMG_INVALID_ARGUMENT;
}

/* ========================================================================== */

#endif

