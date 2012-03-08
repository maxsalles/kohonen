/*
** $ Herond Robaina Salles, Rafael Motta de Carvalho
** Prefix: img
*/

#ifndef _IMG_IMP
#define _IMG_IMP

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "GL/gl.h"

#include "../lst/list.h"
#include "../clr/color.h"
#include "image.h"

/* ========================================================================== */

static int img_current_error = IMG_NO_ERROR;

static int IMG_X_FILTER = 0;
static int IMG_Y_FILTER = 0;
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
	   } else current_error = IMG_NOT_ENOUGH_SPACE;
    } else current_error = IMG_INVALID_ARGUMENT;
}

static void clear (IMGImage self) {
    if (self && self->data) {
        free(self->data);
        self->data = NULL;
        self->width = 0;
        self->height = 0;
    } else lst_current_error = IMG_INVALID_ARGUMENT;
}

/*
** Kmeans
*/

static unsigned getPositionOfColorMoreSimilar (
    LSTList colors, unsigned char* pixel
) {
    int i, position_return = 0;
    double shortest_distance = 10000000.0000, current_distance;
    RGBAColor_ST color = pixelToColor(pixel), current_color;

    for (i = 0; i < clstGetLength(colors); i ++) {
        current_color = pixelToColor(
           (unsigned char*) clstLookElement(colors, i)
        );
        current_distance = rgbaGetDistance(color, current_color);

        if (current_distance < shortest_distance) {
            position_return = i;
            shortest_distance = current_distance;
        }
    }

    return position_return;
}

static LSTList initCenters (const IMGImage image, int num_colors) {
    int i, x, y;
    unsigned char* pixel = NULL;
    CLSTList colors_return = clstNew();

    srand(time(NULL));
    for (i = 0; i < num_colors; i ++) {
        x = rand() % image->width;
        y = rand() % image->height;

/*        pixel = getPixel(image,  i * image->width / num_colors, 0);*/
        pixel = getPixel(image,  x, y);
        if (!rgbaIsPresente(pixelToColor(pixel), colors_return))
            clstAppend(colors_return, pixel);
        else i --;
    }

    return colors_return;
}

static CLSTList setColorsCluster (
    const IMGImage image, const CLSTList centers
) {
    int i, j;
    unsigned char* pixel = NULL;
    CLSTList clusters = clstNew(), current_cluster = NULL;

    for (i = 0; i < clstGetLength(centers); i ++)
        clstAppend(clusters, clstNew());

    for (i = 0; i < image->width; i ++)
        for (j = 0; j < image->height; j ++) {
            pixel = getPixel(image, i, j);
            current_cluster = (CLSTList) clstLookElement(clusters,
                getPositionOfColorMoreSimilar(centers, pixel)
            );
            if (!current_cluster) printf("%p", current_cluster);
            clstAppend(current_cluster, pixel);
        }

    return clusters;
}

CLSTList getCenters (const CLSTList clusters) {
    int i, length = clstGetLength(clusters);
    RGBAColor_ST average_color;
    CLSTList centers = clstNew(), cluster = NULL;

    for (i = 0; i < length; i ++) {
        cluster = (CLSTList) clstLookElement(clusters, i);
        average_color = rgbaGetAverageColor(cluster);
        clstAppend(centers, rgbaAlloc(average_color));
    }

    return centers;
}

double similarity (CLSTList colors1, CLSTList colors2) {
    int i, length = clstGetLength(colors1);
    double similarity_average = 0.0;
    RGBAColor_ST color1, color2;

    for (i = 0; i < length; i ++) {
        color1 = pixelToColor((unsigned char*) clstLookElement(colors1, i));
        color2 = pixelToColor((unsigned char*) clstLookElement(colors2, i));
        similarity_average += rgbaGetDistance(color1, color2);
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
	} else current_error = IMG_INVALID_ARGUMENT;

	return image_return;
}

IMGImage imgAlloc (
    unsigned width, unsigned height,
    RGBColor_ST background
) {
    if (components) {
        int i, j;
        IMGImage image_return = (IMGImage) malloc(sizeof(struct IMGImage_ST));

        if (image_return) {
            image_return->data = (unsigned char*) malloc(
                sizeof(unsigned char*) * height * width * IMG_NUM_COMPONENTS
            );

            image_return->components = components;
            image_return->width = width;
            image_return->height = height;

            for (i = 0; i < image_return->width; i ++)
                for (j = 0; j < image_return->height; j ++)
                    imgSetColorPixel(image_return, i, j, background);
        } else current_error = IMG_NOT_ENOUGH_SPACE;

        return image_return;
    } current_error = IMG_INVALID_ARGUMENT;

    return NULL;
}

IMGImage imgCopy (const IMGImage self) {
    int i, j;
    IMGImage copy = imgAlloc(image->width, image->height, 3, RGB_BLACK);
    RGBColor_ST curr_color;

    if (copy)
        for (i = 0; i < image->width; i ++)
            for (j = 0; j < image->height; j ++) {
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
    if (image) return self->width;
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

LSTList imgGetJaxColors (
    const IMGImage self,
    unsigned jump_size,
    double tolerance
) {
    int i, j;
    RGBColor_ST curr_color;
    LSTList colors = clstNew();

    lstAppend(colors, rgbAlloc(imgGetColorPixel(self, 0, 0)));

    for (i = 0; i < image->width; i += jump_size)
        for (j = 0; j < image->height; j += jump_size) {
            curr_color = imgGetColorPixel(self, i, j);

            if (rgbGetSimilarityOnColorsSet(current_color, colors, NULL)
                < tolerance) lstAppend(colors, rgbaAlloc(current_color));
        }

    return colors;
}

CLSTList imgGetKmeansColors (
    const IMGImage self,
    unsigned num_colors,
    unsigned max_iter,
    double tolerance
) {
    int i, j;
    double similarity_aux = 0.0;
    LSTList cluster = NULL, curr_centers = NULL;
    LSTList curr_cluster = NULL, next_centers = NULL;

    curr_centers = initCenters(image, num_colors);
    cluster = setColorsCluster(image, current_centers);
    next_centers = getCenters(cluster);

    for (i = 0; i < max_iter; i ++) {
        similarity_aux = similarity(current_centers, next_centers);
        if (similarity_aux <= tolerance) return curr_centers;

        lstDestroy(&curr_centers);
        curr_centers = lstCopy(next_centers);
        lstDestroy(&next_centers);

        for (j = 0; j < lstGetLength(cluster); j ++) {
            curr_cluster = (LSTList) lstGetFirstElement(cluster);
            lstDestroy(&curr_cluster);
        }

        lstDestroy(&cluster);
        cluster = setColorsCluster(image, current_centers);
        next_centers = getCenters(cluster);
    }

    return current_centers;
}

IMGImage imgGetSector (
    const IMGImage self,
    int x, int y,
    unsigned width, unsigned height
) {
    int i, j, a, b, i_offset = x + width, j_offset = y + height;
    IMGImage sectot = imgAlloc(width, height, RGB_BLACK);

    if (sector && self && self->data)
        for (i = x, a = 0; i < i_offset; i ++, a ++)
            for (j = y, b = 0; j < j_offset; j ++, b ++)
                if (i >= 0 && j >= 0 && i < self->width && j < self->height)
                    imgSetColorPixel(
                        sectot, a, b, imgGetColorPixel(self, i, j)
                    );
                else imgSetColorPixel(image_return, a, b, RGB_BLACK);

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
                average = rgbaGetAverage(curr_pixel);
                imgSetColorPixel(
                    self, i, j, rgbSet(average, average, average)
                );
            }
    } else img_current_error = IMG_INVALID_ARGUMENT;
}

void imgQuantize (IMGImage self, const LSTList colors) {
    int i, j;
    RGBColor_ST curr_color, color;

    for (i = 0; i < image->width; i ++)
        for (j = 0; j < image->height; j ++) {
            curr_color = imgGetColorPixel(self, i, j);
            rgbGetSimilarityOnColorsSet(curr_color, colors, &color);
            imgSetColorPixel(self, i, j, color);
        }
}

void imgThresholded (IMGImage self, double threshold) {
    int i, j;
    RGBColor_ST curr_color;

    if (self && self->data && threshold >= 0.0 && threshold <= 1.0) {
        for (i = 0; i < self->width; i ++)
            for (j = 0; j < self->height; j ++) {
                curr_color = imgGetColorPixel(self, i, j);

                if ((double) rgbGetAverage(curr_pixel) / 255.0 < threshold)
                    imgSetColorPixel(image, i, j, RGB_WHITE);
                else
                    imgSetColorPixel(image, i, j, RGB_BLACK);
            }
    } else img_current_error = IMG_INVALID_ARGUMENT;
}

void imgApplyGaussianBlur (IMGImage self, unsigned width, unsigned height) {
    unsigned i, j;
    double** gaussian_matix = getGaussianMatrix(width, height);
    IMGImage sector = NULL, image_buffer = NULL;

    if (self && self->data && gaussian_matix && *gaussian_matix) {
        image_buffer = imgAlloc(image->width, image->height, RGB_BLACK);

        for (i = 0; i < self->width; i ++)
            for (j = 0; j < self->height; j ++) {
                sector = imgGetSector(
                    self, i - width / 2, j - height / 2, width, height
                );
                imgSetColorPixel(
                    image_buffer, i, j,
                    imgApplyConvolutionFilter(sector, gaussian_matix)
                );
                imgDestroy(&sector);
            }
    } else img_current_error = IMG_INVALID_ARGUMENT;

    free(*gaussian_matix);
    free(gaussian_matix);
    self->data = image_buffer->data;
}

void imgApplySobelEdgeDetection (IMGImage self) {
    unsigned i, j;
    double** sobel_filter_x = getSobelFilter(IMG_X_FILTER);
    double** sobel_filter_y = getSobelFilter(IMG_Y_FILTER);
    IMGImage sector = NULL, image_buffer = NULL;
    RGBColor_ST curr_color;

    if (self && self->data && sobel_filter_x && *sobel_filter_x &&
         sobel_filter_y && *sobel_filter_y) {
        image_buffer = imgAlloc(self->width, self->height, RGB_BLACK);

        if (image_buffer) {
            for (i = 0; i < self->width; i ++)
                for (j = 0; j < self->height; j ++) {
                    sector = imgGetSector(self, i - 1, j - 1, 3, 3);
                    curr_color = rgbSum(
                        imgApplyConvolutionFilter(sector, sobel_filter_x),
                        imgApplyConvolutionFilter(sector, sobel_filter_y)
                    );
                    imgSetColorPixel(image_buffer, i, j, curr_color);
                }
        } else em_current_error = IMG_NOT_ENOUGH_SPACE;

        free(*sobel_filter_x);
        free(sobel_filter_x);
        free(*sobel_filter_y);
        free(sobel_filter_y);
        self->data = image_buffer->data;
    }
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
        (unsigned char) fabs(accumulator[IMG_RED] / factor),
        (unsigned char) fabs(accumulator[IMG_GREEN] / factor),
        (unsigned char) fabs(accumulator[IMG_BLUE] / factor)
    );
}

void imgDraw (const IMGImage image, unsigned x, unsigned y) {
    if (image) {
        glRasterPos2i(0, 0);
        glDrawPixels(
            image->width, image->height, GL_RGB, GL_UNSIGNED_BYTE, image->data
        );
    } else current_error = IMG_INVALID_ARGUMENT;
}






unsigned char* getPixel (const IMGImage image, unsigned x, unsigned y) {
    if (image && image->data && x < image->width && y < image->height)
       return image->data + (y * image->width + x) * image->components;
    current_error = IMG_INVALID_ARGUMENT;

    return NULL;
}

RGBAColor_ST pixelToColor (const unsigned char* pixel) {
    if (pixel) {
        RGBAColor_ST color_return = { pixel[RED], pixel[GREEN], pixel[BLUE] };

        return color_return;
    }

    current_error = IMG_INVALID_ARGUMENT;

    return RGBA_BLACK;
}

static double** getGaussianMatrix (int width, int height) {
    int i, j, x, y;
    const double SIGMA_2 = 2.8 * 2.8;
    static double* matrix = NULL;
    static double** matrix_return = NULL;

    if (!matrix) matrix = (double*) malloc(sizeof(double) * width * height);
    if (!matrix_return) matrix_return = (double**) malloc(
        sizeof(double) * height
    );

    if (matrix && matrix_return) {
        for (i = 0; i < height; i ++) matrix_return[i] = matrix + (i * width);

        for (i = 0, x = -1 * width / 2; i < width; i ++, x ++)
            for (j = 0, y = -1 * height / 2; j < height; j ++, y ++)
                matrix_return[i][j] = (1.0 / (2.0 * M_PI * SIGMA_2)) * pow(
                    M_E, -1.0 * (double) (x * x + y * y) / (2.0 * SIGMA_2)
                );
    } else current_error = IMG_NOT_ENOUGH_SPACE;

    return matrix_return;
}

static double** getSobelFilter (int filter) {
    int i, j;
    static double* matrix = NULL;
    static double** matrix_return = NULL;
    static const double SOBEL_X_MATRIX[3][3] = {
        { -1.0, -2.0, -1.0 },
        {  0.0,  0.0,  0.0 },
        {  1.0,  2.0,  1.0 }
    };
    static const double SOBEL_Y_MATRIX[3][3] = {
        { -1.0,  0.0, 1.0 },
        { -2.0,  0.0, 2.0 },
        { -1.0,  0.0, 1.0 }
    };

    if (!matrix) matrix = (double*) malloc(sizeof(double) * 3 * 3);
    if (!matrix_return) matrix_return = (double**) malloc(sizeof(double) * 3);

    if (matrix && matrix_return) {
        for (i = 0; i < 3; i ++) matrix_return[i] = matrix + (i * 3);

        for (i = 0; i < 3; i ++)
            for (j = 0; j < 3; j ++) matrix_return[i][j] = filter == X_FILTER ?
                SOBEL_X_MATRIX[i][j] : SOBEL_Y_MATRIX[i][j];
    } else current_error = IMG_NOT_ENOUGH_SPACE;

    return matrix_return;
}

/* ========================================================================== */

#endif

