#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#include "matrix.h"
#include <time.h>

// Frees an array of descriptors.
// descriptor *d: the array.
// int n: number of elements in array.
void free_descriptors(descriptor *d, int n)
{
    int i;
    for(i = 0; i < n; ++i){
        free(d[i].data);
    }
    free(d);
}

// Create a feature descriptor for an index in an image.
// image im: source image.
// int i: index in image for the pixel we want to describe.
// returns: descriptor for that index.
descriptor describe_index(image im, int i)
{
    int w = 5;
    descriptor d;
    d.p.x = i%im.w;
    d.p.y = i/im.w;
    d.data = calloc(w*w*im.c, sizeof(float));
    d.n = w*w*im.c;
    int c, dx, dy;
    int count = 0;
    // If you want you can experiment with other descriptors
    // This subtracts the central value from neighbors
    // to compensate some for exposure/lighting changes.
    for(c = 0; c < im.c; ++c){
        float cval = im.data[c*im.w*im.h + i];
        for(dx = -w/2; dx < (w+1)/2; ++dx){
            for(dy = -w/2; dy < (w+1)/2; ++dy){
                float val = get_pixel(im, i%im.w+dx, i/im.w+dy, c);
                d.data[count++] = cval - val;
            }
        }
    }
    return d;
}

// Marks the spot of a point in an image.
// image im: image to mark.
// ponit p: spot to mark in the image.
void mark_spot(image im, point p)
{
    int x = p.x;
    int y = p.y;
    int i;
    for(i = -9; i < 10; ++i){
        set_pixel(im, x+i, y, 0, 1);
        set_pixel(im, x, y+i, 0, 1);
        set_pixel(im, x+i, y, 1, 0);
        set_pixel(im, x, y+i, 1, 0);
        set_pixel(im, x+i, y, 2, 1);
        set_pixel(im, x, y+i, 2, 1);
    }
}

// Marks corners denoted by an array of descriptors.
// image im: image to mark.
// descriptor *d: corners in the image.
// int n: number of descriptors to mark.
void mark_corners(image im, descriptor *d, int n)
{
    int i;
    for(i = 0; i < n; ++i){
        mark_spot(im, d[i].p);
    }
}

// Creates a 1d Gaussian filter.
// float sigma: standard deviation of Gaussian.
// returns: single row image of the filter.
image make_1d_gaussian(float sigma)
{
    // TODO: make separable 1d Gaussian.
    return make_image(1,1,1);
}

// Smooths an image using separable Gaussian filter.
// image im: image to smooth.
// float sigma: std dev. for Gaussian.
// returns: smoothed image.
image smooth_image(image im, float sigma)
{
    // TODO: use two convolutions with 1d gaussian filter.
    image filter = make_gaussian_filter(sigma);
    image smoothed_img = convolve_image(im, filter, 1);
    free_image(filter);
    return smoothed_img;

}

// Calculate the structure matrix of an image.
// image im: the input image.
// float sigma: std dev. to use for weighted sum.
// returns: structure matrix. 1st channel is Ix^2, 2nd channel is Iy^2,
//          third channel is IxIy.
image structure_matrix(image im, float sigma)
{   
    // TODO: calculate structure matrix for im.
    // 1. You can use Sobel filter and associated functions from HW2 to calculate the derivatives.
    image gx_filter = make_gx_filter();
    image gy_filter = make_gy_filter();
    image Ix = convolve_image(im, gx_filter,0);
    image Iy = convolve_image(im, gy_filter,0);
    free_image(gx_filter);
    free_image(gy_filter);
    // 2. The measures are element-wise multiplications. 
    image S = make_image(im.w, im.h, 3);
    int r,g,b;
    r=0*im.w*im.h;                                             
    g=1*im.w*im.h;
    b=2*im.w*im.h;
    for (int i = 0; i < im.w * im.h; i++) {  
        S.data[i+r] = Ix.data[i] * Ix.data[i];
        S.data[i+g] = Iy.data[i] * Iy.data[i];
        S.data[i+b] = Ix.data[i] * Iy.data[i];
    }
    free_image(Ix);
    free_image(Iy);
    // 3. The weighted sum can be easily computed with a Gaussian blur as discussed in class. 
    image filter = make_gaussian_filter(sigma);
    S = convolve_image(S, filter, 1);
    free_image(filter);

    return S;
}

// Estimate the cornerness of each pixel given a structure matrix S.
// image S: structure matrix for an image.
// returns: a response map of cornerness calculations.
image cornerness_response(image S)
{
    // TODO: fill in R, "cornerness" for each pixel using the structure matrix.
    // We'll use formulation det(S) - alpha * trace(S)^2, alpha = .06.
    image R = make_image(S.w, S.h, 1);
    float alpha = 0.06;
    image detS = make_image(S.w, S.h, 1);
    image traceS2 = make_image(S.w, S.h, 1);

    int xx,yy,xy;
    xx=0*S.w*S.h;                                             
    yy=1*S.w*S.h;
    xy=2*S.w*S.h;
    for(int i = 0; i < S.w * S.h; i++){     
        detS.data[i] = S.data[i+xx] * S.data[i+yy] - pow(S.data[i+xy], 2);
        traceS2.data[i] = alpha * pow(S.data[i+xx] + S.data[i+yy], 2);
    }
    R = sub_image(detS, traceS2);
    return R;
}

// Perform non-max supression on an image of feature responses.
// image im: 1-channel image of feature responses.
// int w: distance to look for larger responses.
// returns: image with only local-maxima responses within w pixels.
image nms_image(image im, int w)
{
    image r = copy_image(im);
    // TODO: perform NMS on the response map.
    // for every pixel in the image:
    //     for neighbors within w:
    //         if neighbor response greater than pixel response:
    //             set response to be very low (I use -999999 [why not 0??])
    int row = 0;
    while(row < im.h){
        int col = 0;
        while(col < im.w){
            int cur_index = row * im.w + col;
            int max_index = -1;
            for(int i = -w; i < w+1; i++){
                for(int j = -w; j < w+1; j++){
                    int temp_index =  (row+i) * im.w + (col+j);
                    if(temp_index < im.w*im.h)
                        if(im.data[max_index] < im.data[temp_index])
                            max_index = temp_index;
                }
            }
            if(im.data[cur_index] < im.data[max_index])
                r.data[cur_index] = -999999;
            
            col += 1;
        }
        row += 1;
    }
    return r;
}

// Perform harris corner detection and extract features from the corners.
// image im: input image.
// float sigma: std. dev for harris.
// float thresh: threshold for cornerness.
// int nms: distance to look for local-maxes in response map.
// int *n: pointer to number of corners detected, should fill in.
// returns: array of descriptors of the corners in the image.
descriptor *harris_corner_detector(image im, float sigma, float thresh, int nms, int *n)
{
    // Calculate structure matrix
    image S = structure_matrix(im, sigma);

    // Estimate cornerness
    image R = cornerness_response(S);

    // Run NMS on the responses
    image Rnms = nms_image(R, nms);

    //TODO: count number of responses over threshold
    int count = 0; // change this
    for(int i = 0; i < Rnms.w*Rnms.h; i ++){
        if(Rnms.data[i] != -999999 && Rnms.data[i] > thresh){
            count++;
        }
    }
    // printf("count %d\n", count);
    *n = count; // <- set *n equal to number of corners in image.
    descriptor *d = calloc(count, sizeof(descriptor));
    //TODO: fill in array *d with descriptors of corners, use describe_index.
    int num = 0;
    for(int i = 0; i < Rnms.w*Rnms.h; i ++){
        if(Rnms.data[i] != -999999 && num < count && Rnms.data[i] > thresh){
            d[num] = describe_index(im, i);
            num++;
        }
    }
      
    // printf("=========================\n");
    free_image(S);
    free_image(R);
    free_image(Rnms);
    return d;
}

// Find and draw corners on an image.
// image im: input image.
// float sigma: std. dev for harris.
// float thresh: threshold for cornerness.
// int nms: distance to look for local-maxes in response map.
void detect_and_draw_corners(image im, float sigma, float thresh, int nms)
{
    int n = 0;
    descriptor *d = harris_corner_detector(im, sigma, thresh, nms, &n);
    mark_corners(im, d, n);
}
