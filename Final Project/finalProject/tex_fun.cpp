/* Texture functions for cs580 GzLib	*/
#include    "stdafx.h" 
#include	"stdio.h"
#include	"Gz.h"
#include	"math.h"

///////// Perlin Noise function
float Noise(int x, int y);
float SmoothNoise(int x, int y);
float Linear_Interpolate(float a, float b, float x);
float InterpolatedNoise(float x, float y);
float PerlinNoise_2D(float x, float y);

/////////


GzColor	*image;
int xs, ys;
int reset = 1;

/* Image texture function */
int tex_fun(float u, float v, GzColor color)
{
  unsigned char		pixel[3];
  unsigned char     dummy;
  char  		foo[8];
  int   		i, j;
  FILE			*fd;

  if (reset) {          /* open and load texture file */
    fd = fopen ("texture", "rb");
    if (fd == NULL) {
      fprintf (stderr, "texture file not found\n");
      exit(-1);
    }
    fscanf (fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
    image = (GzColor*)malloc(sizeof(GzColor)*(xs+1)*(ys+1));
    if (image == NULL) {
      fprintf (stderr, "malloc for texture image failed\n");
      exit(-1);
    }

    for (i = 0; i < xs*ys; i++) {	/* create array of GzColor values */
      fread(pixel, sizeof(pixel), 1, fd);
      image[i][RED] = (float)((int)pixel[RED]) * (1.0 / 255.0);
      image[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0 / 255.0);
      image[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0 / 255.0);
      }

    reset = 0;          /* init is done */
	fclose(fd);
  }

/* bounds-test u,v to make sure nothing will overflow image array bounds */
  if(u<0){u=0;}
  else if(u>1){u=1;}

  if(v<0){v=0;}
  else if(v>1){v=1;}	
	
/* determine texture cell corner values and perform bilinear interpolation */

u=u*(xs-1);
v=v*(ys-1);

int uf,uc,vf,vc;
float s,t;
GzColor exactcolor;

uf=floor(u);
uc=ceil(u);
vf=floor(v);
vc=ceil(v);

s=u-uf;
t=v-vf;




exactcolor[RED]=s*t*image[uc+(vc*xs)][RED]+(1-s)*t*image[uf+(vc*xs)][RED]+s*(1-t)*image[uc+(vf*xs)][RED]+(1-s)*(1-t)*image[uf+(vf*xs)][RED];
exactcolor[GREEN]=s*t*image[uc+(vc*xs)][GREEN]+(1-s)*t*image[uf+(vc*xs)][GREEN]+s*(1-t)*image[uc+(vf*xs)][GREEN]+(1-s)*(1-t)*image[uf+(vf*xs)][GREEN];
exactcolor[BLUE]=s*t*image[uc+(vc*xs)][BLUE]+(1-s)*t*image[uf+(vc*xs)][BLUE]+s*(1-t)*image[uc+(vf*xs)][BLUE]+(1-s)*(1-t)*image[uf+(vf*xs)][BLUE];


/* set color to interpolated GzColor value and return */

color[RED]=exactcolor[RED];
color[GREEN]=exactcolor[GREEN];
color[BLUE]=exactcolor[BLUE];

  return GZ_SUCCESS;// write by me
}


/* Procedural texture function */
int ptex_fun(float u, float v, GzColor color)
{

int xs=64;
int ys=64;
int i,j;

 if (reset) 
 { 
 image = (GzColor*)malloc(sizeof(GzColor)*(xs+1)*(ys+1));

 for(j=0;j<ys;j++)
 {
	for(i=0;i<xs;i++)
	{
//	image[i+j*xs][RED]=(PerlinNoise_2D(i*3,j)+1)/2;
//	image[i+j*xs][GREEN]=(PerlinNoise_2D(2*i,3*j)+1)/2;
//	image[i+j*xs][BLUE]=(PerlinNoise_2D(i+3*j,j+j)+1)/2;

//	image[i+j*xs][RED]=2*PerlinNoise_2D(i*3,j)+3;
//	image[i+j*xs][GREEN]=0.5*PerlinNoise_2D(2*i,3*j)+i;
//	image[i+j*xs][BLUE]=1.3*PerlinNoise_2D(i+3*j,j+j);

	image[i+j*xs][RED]=((2*PerlinNoise_2D(i*3,j)+3)-1)/4;
	image[i+j*xs][GREEN]=((0.5*PerlinNoise_2D(2*i,3*j)+i)-i+0.5);
	image[i+j*xs][BLUE]=(1.3*PerlinNoise_2D(i+3*j,j+j)+1.3)/2.6;
 
	}
 }
 reset = 0;          
 }/* init is done */



/* bounds-test u,v to make sure nothing will overflow image array bounds */
  if(u<0){u=0;}
  else if(u>1){u=1;}

  if(v<0){v=0;}
  else if(v>1){v=1;}	
	
/* determine texture cell corner values and perform bilinear interpolation */

u=u*(xs-1);
v=v*(ys-1);

int uf,uc,vf,vc;
float s,t;
GzColor exactcolor;

uf=floor(u);
uc=ceil(u);
vf=floor(v);
vc=ceil(v);

s=u-uf;
t=v-vf;


exactcolor[RED]=s*t*image[uc+(vc*xs)][RED]+(1-s)*t*image[uf+(vc*xs)][RED]+s*(1-t)*image[uc+(vf*xs)][RED]+(1-s)*(1-t)*image[uf+(vf*xs)][RED];
exactcolor[GREEN]=s*t*image[uc+(vc*xs)][GREEN]+(1-s)*t*image[uf+(vc*xs)][GREEN]+s*(1-t)*image[uc+(vf*xs)][GREEN]+(1-s)*(1-t)*image[uf+(vf*xs)][GREEN];
exactcolor[BLUE]=s*t*image[uc+(vc*xs)][BLUE]+(1-s)*t*image[uf+(vc*xs)][BLUE]+s*(1-t)*image[uc+(vf*xs)][BLUE]+(1-s)*(1-t)*image[uf+(vf*xs)][BLUE];


/* set color to interpolated GzColor value and return */
/*
 if(exactcolor[RED]<0){exactcolor[RED]=0;}
  else if(exactcolor[RED]>1){exactcolor[RED]=1;}

  if(exactcolor[GREEN]<0){exactcolor[GREEN]=0;}
  else if(exactcolor[GREEN]>1){exactcolor[GREEN]=1;}

 if(exactcolor[BLUE]<0){exactcolor[BLUE]=0;}
  else if(exactcolor[BLUE]>1){exactcolor[BLUE]=1;}
 */

color[RED]=exactcolor[RED];
color[GREEN]=exactcolor[GREEN];
color[BLUE]=exactcolor[BLUE];


return GZ_SUCCESS;// write by me
}


////////////////////////////////////////////////////// Nosie Procedural
float Linear_Interpolate(float a, float b, float x)
{
	return  a*(1-x) + b*x;
}

float Noise(int x, int y)
{
	int n;
    n = x + y * 57;
	n = (n << 13) ^ n;
	

	return (1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
   
}
float SmoothNoise(int x, int y)
{
    float corners = ( Noise(x-1, y-1)+Noise(x+1, y-1)+Noise(x-1, y+1)+Noise(x+1, y+1) ) / 16;
    float sides   = ( Noise(x-1, y)  +Noise(x+1, y)  +Noise(x, y-1)  +Noise(x, y+1) ) /  8;
    float center  =  Noise(x, y) / 4;
    return corners + sides + center;
}
float InterpolatedNoise(float x, float y)
{
      int integer_X=int(x);
      float fractional_X=x - integer_X;

      int integer_Y    = int(y);
      float fractional_Y = y - integer_Y;

     float v1 = SmoothNoise(integer_X,     integer_Y);
     float v2 = SmoothNoise(integer_X + 1, integer_Y);
     float v3 = SmoothNoise(integer_X,     integer_Y + 1);
     float v4 = SmoothNoise(integer_X + 1, integer_Y + 1);

      float i1 = Linear_Interpolate(v1 , v2 , fractional_X);
      float i2 = Linear_Interpolate(v3 , v4 , fractional_X);

      return Linear_Interpolate(i1 , i2 , fractional_Y);

}
 float PerlinNoise_2D(float x, float y)
 {
      float total = 0;
      float p = (1.0/2.0) ;
      int n = 6 - 1;
	  int i;

     
   	for(i=0;i<=n;i++)
	{

         float frequency = pow(2.0,i);
         float amplitude = pow(p,i);

         total = total + InterpolatedNoise(x * frequency, y * frequency) * amplitude;

	}

      return total;

 }