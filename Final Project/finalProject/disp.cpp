/*   CS580 HW   */
#include    "stdafx.h"  
#include	"Gz.h"
#include	"disp.h"
#include <math.h>

double round(double d)
{
  return floor(d + 0.5);
}


int GzNewFrameBuffer(char** framebuffer, int width, int height)
{
/* create a framebuffer:
 -- allocate memory for framebuffer : (sizeof)GzPixel x width x height
 -- pass back pointer 
 */
 	*framebuffer= new char[sizeof(GzPixel)*width*height];

	return GZ_SUCCESS;
}

int GzNewDisplay(GzDisplay	**display, GzDisplayClass dispClass, int xRes, int yRes)
{

/* create a display:
  -- allocate memory for indicated class and resolution
  -- pass back pointer to GzDisplay object in display
*/


  *display =new GzDisplay;
  (*display)->fbuf = new GzPixel[xRes*yRes];
  (*display)->dispClass=dispClass;
  (*display)->xres=xRes;
  (*display)->yres=yRes;
  
	return GZ_SUCCESS;
}


int GzFreeDisplay(GzDisplay	*display)
{
/* clean up, free memory */
	delete []display;

	return GZ_SUCCESS;
}


int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes, GzDisplayClass	*dispClass)
{
/* pass back values for an open display */
     
	 int x=display->xres;
	 int y=display->yres;
	 GzDisplayClass disp=display->dispClass;

	 *xRes=x;
	 *yRes=y;
	 *dispClass=disp;
     
	return GZ_SUCCESS;
}


int GzInitDisplay(GzDisplay	*display)
{
/* set everything to some default values - start a new frame */
	int index;

   for(int i=0;i<display->xres;i++){
    for(int j=0;j<display->yres;j++){
	
       index = ARRAY(j,i);
	display->fbuf[index].red=128<<4;
    display->fbuf[index].green=112<<4;
    display->fbuf[index].blue=96<<4;

//	 display->fbuf[index].z=128<<4;
	display->fbuf[index].z=INT_MAX;
	
									}
								}
   /*
   display->fbuf[index].red=short(128.0/255.0*global_max);
    display->fbuf[index].green=short(112.0/255.0*global_max);
    display->fbuf[index].blue=short(96.0/255.0*global_max);
*/


	return GZ_SUCCESS;
}


int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
/* write pixel values into the display */

bool cando;

cando=1;
if(i<0){cando=0;} else if (i>=display->xres){cando=0;}
if(j<0){cando=0;} else if (j>=display->yres){cando=0;}


if(cando){

if(r>4095){r=4095;}else if(r<0){r=0;}
if(g>4095){g=4095;}else if(g<0){r=0;}
if(b>4095){b=4095;}else if(b<0){r=0;}



    display->fbuf[ARRAY(i,j)].red=r;
	display->fbuf[ARRAY(i,j)].green=g;
	display->fbuf[ARRAY(i,j)].blue=b;
	display->fbuf[ARRAY(i,j)].alpha=a;
	display->fbuf[ARRAY(i,j)].z=z;


		}

	return GZ_SUCCESS;
}


int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
	/* pass back pixel value in the display */
	/* check display class to see what vars are valid */
   
bool cando;

cando=1;
if(i<0){cando=0;} else if (i>=display->xres){cando=0;}
if(j<0){cando=0;} else if (j>=display->yres){cando=0;}


if(cando){



    *r=display->fbuf[ARRAY(i,j)].red;
	*g=display->fbuf[ARRAY(i,j)].green;
	*b=display->fbuf[ARRAY(i,j)].blue;
	*a=display->fbuf[ARRAY(i,j)].alpha;
	*z=display->fbuf[ARRAY(i,j)].z;

		}

	return GZ_SUCCESS;
}


int GzFlushDisplay2File(FILE* outfile, GzDisplay *display)
{ 
 
      fprintf (outfile, "P6 %d %d 255\r",display->xres,display->yres);
     
	  int index;
	  int red,green,blue;
	 
   for(int i=0;i<display->xres;i++){
      for(int j=0;j<display->yres;j++){
       index = ARRAY(j,i);
        red=display->fbuf[index].red>>4;
	 
		green=display->fbuf[index].green>>4;
        blue=display->fbuf[index].blue>>4;

/*
		 red=round(display->fbuf[index].red/global_max*255.0);
	 
		green=round(display->fbuf[index].green/global_max*255.0);
        blue=round(display->fbuf[index].blue/global_max*255.0);
*/
         fprintf (outfile,"%c%c%c",red,green,blue);
	  }
   }
	/* write pixels to ppm file based on display class -- "P6 %d %d 255\r" */
   
   
	return GZ_SUCCESS;
}

int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay *display)
{
    
	/* write pixels to framebuffer: 
		- Put the pixels into the frame buffer
		- Caution: store the pixel to the frame buffer as the order of blue, green, and red 
		- Not red, green, and blue !!!

  */
	GzPixel pixel;
    int index;

    for(int i=0;i<display->xres;i++){
    for(int j=0;j<display->yres;j++){
	
        index = ARRAY(j,i);
    	pixel.blue=display->fbuf[index].blue;
		pixel.green=display->fbuf[index].green;
        pixel.red=display->fbuf[index].red;

	framebuffer[index*3]= pixel.blue>>4;
    framebuffer[index*3+1] = pixel.green>>4;
    framebuffer[index*3+2] = pixel.red>>4;
											
	}
	}
	/*
	framebuffer[index*3]= pixel.blue/global_max*255.0;
    framebuffer[index*3+1] = pixel.green/global_max*255.0;
    framebuffer[index*3+2] = pixel.red/global_max*255.0;


  */

	return GZ_SUCCESS;
}