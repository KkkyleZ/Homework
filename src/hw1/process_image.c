#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    // TODO Fill this in
    int ch,row,col,value;
    float pixel;
    if(x<0)                        //bounded check for input data
        x=0;
    if(y<0)
        y=0;
    if(c>=im.c)
        c=im.c-1;
    if(x>=im.w)
        x=im.w-1;
    if(y>=im.h)
        y=im.h-1;

    ch= im.w*im.h*c;            //channel,row,columns  expression
    row=im.w*y;
    col=x;
    value=col+row+ch;
    pixel= im.data[value];
    return pixel;
}


void set_pixel(image im, int x, int y, int c, float v)
{
    // TODO Fill this in
    int ch,row,col,value;
    if(x<0)                            //bounded check for input data
        x=0;
    if(y<0)
        y=0;
    if(c>=im.c)
        c=im.c-1;
    if(x>=im.w)
        x=im.w-1;
    if(y>=im.h)
        y=im.h-1;
    ch= im.w*im.h*c;                  //channel, row column expression
    row=im.w*y;
    col=x;
    value = ch+row+col;
    im.data[value] = v;
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    // TODO Fill this in
    size_t newM=im.w*im.h*im.c*sizeof(float);              //use memcpy to copy data
    memcpy(copy.data, im.data, newM);
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    // TODO Fill this in
    int r,g,b;
    r=0*im.w*im.h;                                              //rgb expression
    g=1*im.w*im.h;
    b=2*im.w*im.h;
    for(int i = 0; i < im.w * im.h; i++){                      //greyscale formula
        gray.data[i] = 0.299 * im.data[i+r] + 0.587 * im.data[i + g] + 0.114 * im.data[i + b];
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    // TODO Fill this in
    assert(c >= 0 && c < im.c);
    for (int i = 0; i < im.w * im.h; i++) {                    //Iterate over the width and height to manipulate all pixel points for shift
        im.data[i + c * im.w * im.h]=im.data[i + c * im.w * im.h]+v;
    }
}

void clamp_image(image im)
{
    // TODO Fill this in
    for (int i = 0; i < im.w * im.h * im.c; i++)
    {
        if(im.data[i]<0){                   //make sure the pixel between [0,1]
            im.data[i]=0;
        }
        else if(im.data[i]>1){
            im.data[i]=1;
        }
    }
}



// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    // TODO Fill this in
    float R,G,B,C,V,m,S,H;
    for (int i=0;i<im.w;i++){
        for(int j=0;j<im.h;j++){
           R=get_pixel(im,i,j,0);                  //use get_pixel we can easier to operate the target channel
           G=get_pixel(im,i,j,1);                  //what we want to operate
           B=get_pixel(im,i,j,2);

           V=three_way_max(R,G,B);               //handy function
           m=three_way_min(R,G,B);
           C=V-m;

           if(V==0){                           //rgb to hsv formula
               S=0;
           }else
           {
               S=C/V;
           }
           if(C==0){
               H=0;
           }else if(V==R){
               H=(G-B)/C;
           }else if(V==G){
               H=(B-R)/C+2;
           }else if(V==B){
               H=(R-G)/C+4;
           }
           if(H<0){
               H=H/6+1;
           }else{
               H=H/6;
           }
            set_pixel(im, i, j, 0, H);            //reset the hsv for data
            set_pixel(im, i, j, 1, S);
            set_pixel(im, i, j, 2, V);
        }
    }
}



void hsv_to_rgb(image im)
{
    // TODO Fill this in
    float H,S,V,R,G,B,Hi,F,P,Q,T;
    for (int i=0;i<im.w;i++){
        for(int j=0;j<im.h;j++){
           H=get_pixel(im,i,j,0);                //get hsv's data
           S=get_pixel(im,i,j,1);
           V=get_pixel(im,i,j,2);

           if(S==0){
                set_pixel(im, i, j, 0, V);         //if S==0 then we don't need consider H and S it's grey
                set_pixel(im, i, j, 1, V);
                set_pixel(im, i, j, 2, V);
           }else{
           if(H==1){                              //hsv to rgb formula
               H=0;
           }else{
               H=H*6;
           }
           Hi=(int)H;
           F=H-Hi;
           P=V*(1-S);
           T=V*(1-(1-F)*S);
           Q=V*(1-F*S);
           if(Hi==0){
               R=V;
               G=T;
               B=P;
           }else if(Hi==1){
               R=Q;
               G=V;
               B=P;
           }else if(Hi==2){
               R=P;
               G=V;
               B=T;
           }else if(Hi==3){
               R=P;
               G=Q;
               B=V;
           }else if(Hi==4){
               R=T;
               G=P;
               B=V;
           }else if(Hi==5){
               R=V;
               G=P;
               B=Q;
           }
           set_pixel(im, i, j, 0, R);            //reset to rgb
           set_pixel(im, i, j, 1, G);
           set_pixel(im, i, j, 2, B);
           }
        }
    }
}

void scale_image(image im, int c, float v){
    assert(c >= 0 && c < im.c);
    for (int i = 0; i < im.w * im.h; i++) {
        im.data[i + c * im.w * im.h]=im.data[i + c * im.w * im.h]*v;   //scale the data
    }
}