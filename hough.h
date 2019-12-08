#ifndef HOUGH_H
#define HOUGH_H

#include "image.h"

Img *hough(Img *source);
void hough_process_point(Img *source, int *hist, int x, int y);
void draw_line(Img *source, int *hist, double r, double theta);
int grosso_merdo(int a, int b);
int angle_to_index(double theta);
double index_to_angle(int index);
double get_max(int *hist);
Img *apply_angle(Img *source, double angle);
double get_median(int *hist);

#endif
