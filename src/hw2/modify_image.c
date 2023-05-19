#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853

/******************************** Resizing *****************************
  To resize we'll need some interpolation methods and a function to create
  a new image and fill it in with our interpolation methods.
************************************************************************/

float nn_interpolate(image im, float x, float y, int c)
{
    // TODO
    int row,col;
    row=(int)round(y);
    col=(int)round(x);
    /***********************************************************************
      This function performs nearest-neighbor interpolation on image "im"
      given a floating column value "x", row value "y" and integer channel "c",
      and returns the interpolated value.
    ************************************************************************/
    return get_pixel(im, col, row, c);
}

image nn_resize(image im, int w, int h)
{
    // TODO Fill in (also fix the return line)
    image new_img=make_image(w,h,im.c);
    float new_col,new_row;
    float ratio_c,ratio_r,value;
    ratio_c=(float)im.w/w;
    ratio_r=(float)im.h/h;
    for(int k=0;k<im.c;k++){
      for(int i=0;i<w;i++){
        for(int j=0;j<h;j++){
          new_col=(i+0.5)*ratio_c-0.5;
          new_row=(j+0.5)*ratio_r-0.5;
         value=nn_interpolate(im,new_col,new_row,k);
          set_pixel(new_img,i,j,k,value);
       }
      }
    }
    /***********************************************************************
      This function uses nearest-neighbor interpolation on image "im" to a new
      image of size "w x h"
    ************************************************************************/
    return new_img;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    // TODO
    /***********************************************************************
      This function performs bilinear interpolation on image "im" given
      a floating column value "x", row value "y" and integer channel "c".
      It interpolates and returns the interpolated value.
    ************************************************************************/
   int x1,x2,y1,y2;
   x1=(int)floor(x);
   y1=(int)floor(y);
   x2=x1+1;
   y2=y1+1;
   float q1,q2,q3,q4,value;
   q1=get_pixel(im,x1,y1,c);
   q2=get_pixel(im,x2,y1,c);
   q3=get_pixel(im,x1,y2,c);
   q4=get_pixel(im,x2,y2,c);
   value=((y2-y)*((x2-x)*q1+q2*(x-x1)))+((y-y1)*(q3*(x2-x)+q4*(x-x1)));

    return value;
}

image bilinear_resize(image im, int w, int h)
{
    // TODO
    /***********************************************************************
      This function uses bilinear interpolation on image "im" to a new image
      of size "w x h". Algorithm is same as nearest-neighbor interpolation.
    ************************************************************************/
    image new_img=make_image(w,h,im.c);
    float ratio_c,ratio_r,value;
    ratio_c=im.w/(float)w;
    ratio_r=im.h/(float)h;
    float new_col,new_row;
    for(int k=0;k<im.c;k++){
    for(int i=0;i<w;i++){
      for(int j=0;j<h;j++){
        new_col=(i+0.5)*ratio_c-0.5;
        new_row=(j+0.5)*ratio_r-0.5;
        value=bilinear_interpolate(im,new_col,new_row,k);
        set_pixel(new_img,i,j,k,value);
      }
    }
    }

    return new_img;
}


/********************** Filtering: Box filter ***************************
  We want to create a box filter. We will only use square box filters.
************************************************************************/

void l1_normalize(image im)
{
    // TODO
    /***********************************************************************
      This function divides each value in image "im" by the sum of all the
      values in the image and modifies the image in place.
    ************************************************************************/
   float total=0;
       for(int k=0;k<im.c;k++){
    for(int i=0;i<im.w;i++){
      for(int j=0;j<im.h;j++){
       total+=get_pixel(im,i,j,k);
      }
    }
    }
    for(int k=0;k<im.c;k++){
    for(int i=0;i<im.w;i++){
      for(int j=0;j<im.h;j++){
       set_pixel(im,i,j,k,get_pixel(im,i,j,k)/total);
      }
    }
    }
}

image make_box_filter(int w)
{
    // TODO
    /***********************************************************************
      This function makes a square filter of size "w x w". Make an image of
      width = height = w and number of channels = 1, with all entries equal
      to 1. Then use "l1_normalize" to normalize your filter.
    ************************************************************************/
   image box_filter=make_image(w,w,1);
   for(int i=0;i<w*w;i++){
       box_filter.data[i]=1;
   }
   l1_normalize(box_filter);
    return box_filter;
}

image convolve_image(image im, image filter, int preserve)
{
    // TODO
    /***********************************************************************
      This function convolves the image "im" with the "filter". The value
      of preserve is 1 if the number of input image channels need to be
      preserved. Check the detailed algorithm given in the README.
    ************************************************************************/
   assert(im.c==filter.c||filter.c==1);
   image new_img;
   if(preserve==1){
     new_img=make_image(im.w,im.h,im.c);
   }else{
     new_img=make_image(im.w,im.h,1);
   }
   float sum;
   for(int i=0;i<im.w;i++){
     for(int j=0;j<im.h;j++){
       for(int k=0;k<im.c;k++){
       sum=0;
       for(int m=0;m<filter.w;m++){
         for(int n=0;n<filter.h;n++){
          int col_index=i+m-(int)filter.w/2;
          int row_index=j+n-(int)filter.h/2;
           float value = get_pixel(im,col_index,row_index,k);
          if(filter.c==1){
            sum=sum+value*get_pixel(filter,m,n,0);
          }else{
            sum=sum+value*get_pixel(filter,m,n,k);
          }
         }
       }
       if(preserve==1){
       set_pixel(new_img,i,j,k,sum);
       }else{
         set_pixel(new_img, i, j, 0, get_pixel(new_img,i,j,0)+sum);
       }
     }
     }
   }

    return new_img;
}

image make_highpass_filter()
{
    // TODO
    /***********************************************************************
      Create a 3x3 filter with highpass filter values using image.data[]
    ************************************************************************/
   image high_filter=make_image(3,3,1);
    set_pixel(high_filter, 0, 0, 0, 0);
    set_pixel(high_filter, 1, 0, 0, -1);
    set_pixel(high_filter, 2, 0, 0, 0);
    set_pixel(high_filter, 0, 1, 0, -1);
    set_pixel(high_filter, 1, 1, 0, 4);
    set_pixel(high_filter, 2, 1, 0, -1);
    set_pixel(high_filter, 0, 2, 0, 0);
    set_pixel(high_filter, 1, 2, 0, -1);
    set_pixel(high_filter, 2, 2, 0, 0);
    return high_filter;
}

image make_sharpen_filter()
{
    // TODO
    /***********************************************************************
      Create a 3x3 filter with sharpen filter values using image.data[]
    ************************************************************************/
    image sharpen_filter=make_image(3,3,1);
    set_pixel(sharpen_filter, 0, 0, 0, 0);
    set_pixel(sharpen_filter, 1, 0, 0, -1);
    set_pixel(sharpen_filter, 2, 0, 0, 0);
    set_pixel(sharpen_filter, 0, 1, 0, -1);
    set_pixel(sharpen_filter, 1, 1, 0, 5);
    set_pixel(sharpen_filter, 2, 1, 0, -1);
    set_pixel(sharpen_filter, 0, 2, 0, 0);
    set_pixel(sharpen_filter, 1, 2, 0, -1);
    set_pixel(sharpen_filter, 2, 2, 0, 0);
    return sharpen_filter;
}

image make_emboss_filter()
{
    // TODO
    /***********************************************************************
      Create a 3x3 filter with emboss filter values using image.data[]
    ************************************************************************/
    image emboss_filter=make_image(3,3,1);
    set_pixel(emboss_filter, 0, 0, 0, -2);
    set_pixel(emboss_filter, 1, 0, 0, -1);
    set_pixel(emboss_filter, 2, 0, 0, 0);
    set_pixel(emboss_filter, 0, 1, 0, -1);
    set_pixel(emboss_filter, 1, 1, 0, 1);
    set_pixel(emboss_filter, 2, 1, 0, 1);
    set_pixel(emboss_filter, 0, 2, 0, 0);
    set_pixel(emboss_filter, 1, 2, 0, 1);
    set_pixel(emboss_filter, 2, 2, 0, 2);
    return emboss_filter;
}

// Question 2.3.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: TODO
//It is desired to convolve using box filters and high-pass filters to prevent unwanted brightness or contrast changes.
//For sharpening and embossing filters, we do not need to retain them because the purpose of these filters is to change
//the brightness and contrast of the image to achieve their desired effect.

// Question 2.3.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: TODO
//post-processing may be required for highpass, sharpen, and emboss filters to reduce artifacts,
//

image make_gaussian_filter(float sigma)
{
    // TODO
    /***********************************************************************
      sigma: a float number for the Gaussian.
      Create a Gaussian filter with the given sigma. Note that the kernel size
      is the next highest odd integer from 6 x sigma. Return the Gaussian filter.
    ************************************************************************/
   int size=(int)ceilf(6*sigma);
   if(size%2==0){
     size++;
   }
   float value;
   image gaussian_filter=make_image(size,size,1);
   int h_size=size/2;
   for(int i=0;i<size;i++){
     for(int j=0;j<size;j++){
       value=(1/(2*M_PI*sigma*sigma))*exp(-1*(pow(i-h_size,2)+pow(j-h_size,2))/(2*pow(sigma,2)));
       set_pixel(gaussian_filter,i,j,0,value);
     }
   }
   l1_normalize(gaussian_filter);
   return gaussian_filter;
}

image add_image(image a, image b)
{
    // TODO
    /***********************************************************************
      The input images a and image b have the same height, width, and channels.
      Sum the given two images and return the result, which should also have
      the same height, width, and channels as the inputs. Do necessary checks.
    ************************************************************************/
    assert(a.w == b.w && a.h == b.h && a.c == b.c);
    image new_img=make_image(a.w,a.h,a.c);
    for(int i=0;i<a.w*a.h*a.c;i++){
      new_img.data[i]=a.data[i]+b.data[i];
    }
    return new_img;
}

image sub_image(image a, image b)
{
    // TODO
    /***********************************************************************
      The input image a and image b have the same height, width, and channels.
      Subtract the given two images and return the result, which should have
      the same height, width, and channels as the inputs. Do necessary checks.
    ************************************************************************/
    assert(a.w == b.w && a.h == b.h && a.c == b.c);
    image new_img=make_image(a.w,a.h,a.c);
    for(int i=0;i<a.w*a.h*a.c;i++){
      new_img.data[i]=a.data[i]-b.data[i];
    }
    return new_img;
}

image make_gx_filter()
{
    // TODO
    /***********************************************************************
      Create a 3x3 Sobel Gx filter and return it
    ************************************************************************/
  image gx_filter = make_image(3, 3, 1);
  set_pixel(gx_filter, 0, 0, 0, -1);
  set_pixel(gx_filter, 1, 0, 0, 0);
  set_pixel(gx_filter, 2, 0, 0, 1);
  set_pixel(gx_filter, 0, 1, 0, -2);
  set_pixel(gx_filter, 1, 1, 0, 0);
  set_pixel(gx_filter, 2, 1, 0, 2);
  set_pixel(gx_filter, 0, 2, 0, -1);
  set_pixel(gx_filter, 1, 2, 0, 0);
  set_pixel(gx_filter, 2, 2, 0, 1);

    return gx_filter;
}

image make_gy_filter()
{
    // TODO
    /***********************************************************************
      Create a 3x3 Sobel Gy filter and return it
    ************************************************************************/
  image gy_filter = make_image(3, 3, 1);
  set_pixel(gy_filter, 0, 0, 0, -1);
  set_pixel(gy_filter, 1, 0, 0, -2);
  set_pixel(gy_filter, 2, 0, 0, -1);
  set_pixel(gy_filter, 0, 1, 0, 0);
  set_pixel(gy_filter, 1, 1, 0, 0);
  set_pixel(gy_filter, 2, 1, 0, 0);
  set_pixel(gy_filter, 0, 2, 0, 1);
  set_pixel(gy_filter, 1, 2, 0, 2);
  set_pixel(gy_filter, 2, 2, 0, 1);

    return gy_filter;
}

void feature_normalize(image im)
{
    // TODO
    /***********************************************************************
      Calculate minimum and maximum pixel values. Normalize the image by
      subtracting the minimum and dividing by the max-min difference.
    ************************************************************************/
   float value,min,max;
   min=0;
   max=im.data[0];
   for(int i=0;i<im.w;i++){
     for(int j=0;j<im.h;j++){
       for(int k=0;k<im.c;k++){
         value=get_pixel(im,i,j,k);
         if(value<min){
           min=value;
         }
         if(value>max){
           max=value;
         }
       }
     }
   }
    for(int i=0;i<im.w;i++){
     for(int j=0;j<im.h;j++){
       for(int k=0;k<im.c;k++){
         value=get_pixel(im,i,j,k);
         set_pixel(im,i,j,k,(value-min)/(max-min));
         }
       }
     }
}

image *sobel_image(image im)
{
    // TODO
    /***********************************************************************
      Apply Sobel filter to the input image "im", get the magnitude as sobelimg[0]
      and gradient as sobelimg[1], and return the result.
    ************************************************************************/
    image *sobelimg = calloc(2, sizeof(image));
    sobelimg[0]=make_image(im.w,im.h,1);
    sobelimg[1]=make_image(im.w,im.h,1);
    image gx_filter=make_gx_filter();
    image gy_filter=make_gy_filter();
    image gx=convolve_image(im,gx_filter,0);
    image gy=convolve_image(im,gy_filter,0);
    float mag,grad;
    for(int i=0;i<im.w;i++){
      for(int j=0;j<im.h;j++){
        mag=sqrt(get_pixel(gx,i,j,0)*get_pixel(gx,i,j,0)+get_pixel(gy,i,j,0)*get_pixel(gy,i,j,0));
        grad=atan2(get_pixel(gy,i,j,0),get_pixel(gx,i,j,0));
        set_pixel(sobelimg[0],i,j,0,mag);
        set_pixel(sobelimg[1],i,j,0,grad);
      }
    }
    return sobelimg;
}

image colorize_sobel(image im)
{
  // TODO
  /***********************************************************************
    Create a colorized version of the edges in image "im" using the
    algorithm described in the README.
  ************************************************************************/
 image gaussian_filter=make_gaussian_filter(4);
 image smooth_image=convolve_image(im,gaussian_filter,1);
 image *sobelimg = sobel_image(smooth_image);
 image mag=sobelimg[0];
 image grad=sobelimg[1];
 feature_normalize(mag);
 feature_normalize(grad);
 image colorize_img=make_image(im.w,im.h,im.c);
 for(int i=0;i<im.w;i++){
   for(int j=0;j<im.h;j++){
     set_pixel(colorize_img,i,j,0,get_pixel(grad,i,j,0));
     set_pixel(colorize_img,i,j,1,get_pixel(mag,i,j,0));
     set_pixel(colorize_img,i,j,2,get_pixel(mag,i,j,0));
   }
 }
 hsv_to_rgb(colorize_img);
  return colorize_img;
}

// EXTRA CREDIT: Median filter

int compare(const void* a, const void* b){
    float compare_a=*(const float*)a;
    float compare_b=*(const float*)b;
    return (compare_a>compare_b) - (compare_a<compare_b);
}

image apply_median_filter(image im, int kernel_size)
{
  int edgex=floor(kernel_size/2);
  int edgey=floor(kernel_size/2);
  float window[kernel_size*kernel_size];
  int count;
  image new_img=make_image(im.w,im.h,im.c);
  for(int k=0;k<im.c;k++){
  for(int i=edgex;i<im.w-edgex;i++){
    for(int j=edgey;j<im.h-edgey;j++){
      count=0;
      for(int m=0;m<kernel_size;m++){
        for(int n=0;n<kernel_size;n++){
          window[count]=get_pixel(im,i+m-edgex,j+n-edgey,0);
          count++;
        }
      }
      qsort(window,kernel_size*kernel_size,sizeof(float),compare);
      set_pixel(new_img,i,j,k,window[kernel_size*kernel_size/2]);
    }
  }
  }
  return new_img;
}




// SUPER EXTRA CREDIT: Bilateral filter

/*
image apply_bilateral_filter(image im, float sigma1, float sigma2)
{
  return make_image(1,1,1);
}
*/