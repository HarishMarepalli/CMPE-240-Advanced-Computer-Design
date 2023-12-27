/*
===============================================================================
 Name        : Harish_Marepalli_016707314_3D_Shading_DiffuseReflection.c
 Author      : Harish Marepalli
 Version     :
 Copyright   : $(copyright)
 Description : 1. This program is used for creating and displaying the world coordinate system, a 3D cube and a half sphere object based on the transformation pipeline.
 	 	 	   2. It is also used to rotate the cube w.r.t an arbitrary vector.
 	 	 	   3. Used to compute diffuse reflection on the top surface of the cube.
 	 	 	   4. Used to place a point light source in the world coordinate system and produce a shadow of dark blue by plotting a polygon.
 	 	 	   5. Used to scale up the diffuse reflection color by using linear equation.
 	 	 	   6. Used to place a tree on one of the 2 frontal surfaces of the cube.
 	 	 	   7. Used to compute diffuse reflection on visible part of the half sphere.
===============================================================================
*/

#include <cr_section_macros.h>
#include <NXP/crp.h>
#include "LPC17xx.h"                        /* LPC17xx definitions */
#include "ssp.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

/* Be careful with the port number and location number, because some of the locations may not exist in that port. */

#define PORT_NUM            0

uint8_t src_addr[SSP_BUFSIZE];
uint8_t dest_addr[SSP_BUFSIZE];

#define ST7735_TFTWIDTH 127
#define ST7735_TFTHEIGHT 159

#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_RAMWR 0x2C
#define ST7735_SLPOUT 0x11
#define ST7735_DISPON 0x29

#define swap(x, y) {x = x + y; y = x - y; x = x - y ;}

// Defining Color Values
#define LIGHTBLUE 0x00FFE0
#define GREEN 0x00FF00
#define DARKBLUE 0x000033
#define BLACK 0x000000
#define BLUE 0x0007FF
#define RED 0xFF0000
#define MAGENTA 0x00F81F
#define WHITE 0xFFFFFF
#define PURPLE 0xCC33FF

// Custom Defined Colors
#define BROWN 0xA52A2A
#define YELLOW 0xFFFE00

// Custom Defined GREEN Shades
#define GREEN1 0x38761D
#define GREEN2 0x002200
#define GREEN3 0x00FC7C
#define GREEN4 0x32CD32
#define GREEN5 0x228B22
#define GREEN6 0x006400

// Custom Defined RED Shades
#define RED1 0xE61C1C
#define RED2 0xEE3B3B
#define RED3 0xEF4D4D
#define RED4 0xE88080

#define GREY1 0x7A7C7B

int _height = ST7735_TFTHEIGHT;
int _width = ST7735_TFTWIDTH;

#define pi 3.1416

typedef struct Point
{
	float x;
	float y;
}Point;

// Structure for 2D
typedef struct
{
	float x; float y;
}Pts2D;

// Structure for 3D
typedef struct
{
	float x_value; float y_value; float z_value;
}Pts3D;

void spiwrite(uint8_t c)
{
	 int pnum = 0;

	 src_addr[0] = c;

	 SSP_SSELToggle( pnum, 0 );

	 SSPSend( pnum, (uint8_t *)src_addr, 1 );

	 SSP_SSELToggle( pnum, 1 );
}

void writecommand(uint8_t c)
{
	 LPC_GPIO0->FIOCLR |= (0x1<<2);

	 spiwrite(c);
}

void writedata(uint8_t c)
{
	 LPC_GPIO0->FIOSET |= (0x1<<2);

	 spiwrite(c);
}

void writeword(uint16_t c)
{
	 uint8_t d;

	 d = c >> 8;

	 writedata(d);

	 d = c & 0xFF;

	 writedata(d);
}

void write888(uint32_t color, uint32_t repeat)
{
	 uint8_t red, green, blue;

	 int i;

	 red = (color >> 16);

	 green = (color >> 8) & 0xFF;

	 blue = color & 0xFF;

	 for (i = 0; i< repeat; i++)
	 {
		  writedata(red);

		  writedata(green);

		  writedata(blue);
	 }
}

void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	 writecommand(ST7735_CASET);

	 writeword(x0);

	 writeword(x1);

	 writecommand(ST7735_RASET);

	 writeword(y0);

	 writeword(y1);
}

void fillrect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color)
{
	 //int16_t i;

	 int16_t width, height;

	 width = x1-x0+1;

	 height = y1-y0+1;

	 setAddrWindow(x0,y0,x1,y1);

	 writecommand(ST7735_RAMWR);

	 write888(color,width*height);
}

void lcddelay(int ms)
{
	 int count = 24000;

	 int i;

	 for ( i = count*ms; i > 0; i--);
}

void lcd_init()
{
	 int i;
	 printf("3D Graphics Processing Engine Design: Shading and Diffuse Reflection Project\n");
	 // Set pins P0.16, P0.2, P0.22 as output
	 LPC_GPIO0->FIODIR |= (0x1<<16);

	 LPC_GPIO0->FIODIR |= (0x1<<2);

	 LPC_GPIO0->FIODIR |= (0x1<<22);

	 // Hardware Reset Sequence
	 LPC_GPIO0->FIOSET |= (0x1<<22);
	 lcddelay(500);

	 LPC_GPIO0->FIOCLR |= (0x1<<22);
	 lcddelay(500);

	 LPC_GPIO0->FIOSET |= (0x1<<22);
	 lcddelay(500);

	 // initialize buffers
	 for ( i = 0; i < SSP_BUFSIZE; i++ )
	 {
		   src_addr[i] = 0;
		   dest_addr[i] = 0;
	 }

	 // Take LCD display out of sleep mode
	 writecommand(ST7735_SLPOUT);
	 lcddelay(200);

	 // Turn LCD display on
	 writecommand(ST7735_DISPON);
	 lcddelay(200);
}

// Coordinate to Cartesian (Converting virtual X-Coordinate to physical X-Coordinate)
int16_t xCartesian(int16_t x)
{
	x = x + (_width>>1);
	return x;
}

// Converting virtual Y-Coordinate to physical Y-Coordinate
int16_t yCartesian(int16_t y)
{
	y = (_height>>1) - y;
	return y;
}

void drawPixel(int16_t x, int16_t y, uint32_t color)
{
	 // Cartesian Implementation
	 x=xCartesian(x); y=yCartesian(y);

	 if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height))

	 return;

	 setAddrWindow(x, y, x + 1, y + 1);

	 writecommand(ST7735_RAMWR);

	 write888(color, 1);
}

/*****************************************************************************


** Descriptions:        Draw line function

**

** parameters:           Starting point (x0,y0), Ending point(x1,y1) and color

** Returned value:        None

**

*****************************************************************************/

void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color)
{
	 int16_t slope = abs(y1 - y0) > abs(x1 - x0);

	 if (slope)
	 {
		  swap(x0, y0);

		  swap(x1, y1);
	 }

	 if (x0 > x1)
	 {
		  swap(x0, x1);

		  swap(y0, y1);
	 }

	 int16_t dx, dy;

	 dx = x1 - x0;

	 dy = abs(y1 - y0);

	 int16_t err = dx / 2;

	 int16_t ystep;

	 if (y0 < y1)
	 {
		 ystep = 1;
	 }

	 else
	 {
		 ystep = -1;
	 }

	 for (; x0 <= x1; x0++)
	 {
		  if (slope)
		  {
			  drawPixel(y0, x0, color);
		  }

		  else
		  {
			  drawPixel(x0, y0, color);
		  }

		  err -= dy;

		  if (err < 0)
		  {
			   y0 += ystep;

			   err += dx;
		  }
	 }
}

float Lambda3D(float Zi,float Zs)
{
	float lambda;
	lambda = -Zi/(Zs-Zi);
	return lambda;
}

Pts3D ShadowPoint3D(Pts3D Pi, Pts3D Ps, float lambda)
{
	Pts3D pt;
	pt.x_value = (Pi.x_value + (lambda*(Ps.x_value-Pi.x_value)));
	pt.y_value = (Pi.y_value + (lambda*(Ps.y_value-Pi.y_value)));
	pt.z_value = (Pi.z_value + (lambda*(Ps.z_value-Pi.z_value)));
	return pt;
}

int getDiffuseColor(Pts3D Pi)
{
	uint32_t print_diffuse_color;
	Pts3D Ps = {-20,-20,220};
	int diff_red, diff_green, diff_blue;
	float new_red, new_green, new_blue, r1=0.8, r2=0.0, r3=0.0, scaling = 16000;

	float_t temp = ((Ps.z_value - Pi.z_value)/sqrt(pow((Ps.x_value - Pi.x_value),2) + pow((Ps.y_value - Pi.y_value),2) + pow((Ps.z_value - Pi.z_value),2))) / (pow((Ps.x_value - Pi.x_value),2) + pow((Ps.y_value - Pi.y_value),2) + pow((Ps.z_value - Pi.z_value),2));

	temp *= scaling;
	diff_red = r1 * temp * 255;
	diff_green = r2 * temp;
	diff_blue  = r3 * temp;
	new_red = (diff_red << 16);
	new_green = (diff_green << 8);
	new_blue = (diff_blue);
	print_diffuse_color = new_red + new_green +new_blue;
	return print_diffuse_color;
}

int getDiffuseColorGreen(Pts3D Pi)
{
	uint32_t print_diffuse_color;
	Pts3D Ps = {-20,-20,220};
	int diff_red, diff_green, diff_blue;
	float new_red, new_green, new_blue, r1=0.0, r2=1.0, r3=0.0, scaling = 8000;

	float_t temp = ((Ps.z_value - Pi.z_value)/sqrt(pow((Ps.x_value - Pi.x_value),2) + pow((Ps.y_value - Pi.y_value),2) + pow((Ps.z_value - Pi.z_value),2))) / (pow((Ps.x_value - Pi.x_value),2) + pow((Ps.y_value - Pi.y_value),2) + pow((Ps.z_value - Pi.z_value),2));

	temp *= scaling;
	diff_red = r1 * temp * 255;
	diff_green = r2 * temp * 255;
	diff_blue  = r3 * temp * 255;
	new_red = (diff_red << 16);
	new_green = (diff_green << 8);
	new_blue = (diff_blue);
	print_diffuse_color = new_red + new_green +new_blue;
	return print_diffuse_color;
}

Pts2D get3DTransform(Pts3D Pi)
{
	float Xe=150, Ye=150, Ze=100;
	float Rho=sqrt(pow(Xe,2)+pow(Ye,2)+pow(Xe,2));
	float_t D_focal=120;

	typedef struct{
		float X; float Y; float Z;
	}pworld;

	typedef struct{
		float X; float Y; float Z;

	}pviewer;

	typedef struct{
		float X; float Y;
	}pperspective;

	pworld world;
	pviewer viewer;
	pperspective perspective;

	world.X = Pi.x_value;
	world.Y = Pi.y_value;
	world.Z = Pi.z_value;

	float sPheta = Ye/sqrt(pow(Xe,2)+pow(Ye,2));
	float cPheta = Xe/sqrt(pow(Xe,2)+pow(Ye,2));
	float sPhi = sqrt(pow(Xe,2)+pow(Ye,2))/Rho;
	float cPhi = Ze/Rho;

	viewer.X=-sPheta*world.X+cPheta*world.Y;
	viewer.Y = -cPheta * cPhi * world.X - cPhi * sPheta * world.Y + sPhi * world.Z;
	viewer.Z = -sPhi * cPheta * world.X - sPhi * cPheta * world.Y-cPheta * world.Z + Rho;

	perspective.X=D_focal*viewer.X/viewer.Z;
	perspective.Y=D_focal*viewer.Y/viewer.Z;

	Pts2D pt;
	pt.x=perspective.X;
	pt.y=perspective.Y;
	return pt;

}

/* Rotate point p with respect to o and angle <angle> */
Pts3D rotate_pointIn3D(Pts3D p, Pts3D o, float angle)
{
	Pts3D rt, t, new;

	float s = sin(angle);
	float c = cos(angle);

	//translate point to origin
	/*t.x_value = 105.00;
	t.y_value = p.y_value - o.y_value;
	t.z_value = p.z_value - o.z_value;

	rt.x_value = 105.00;
	rt.y_value = t.y_value * c - t.z_value * s;
	rt.z_value = t.y_value * s + t.z_value * c;

	//translate point back
	new.x_value = 105.00;
	new.y_value = rt.y_value + o.y_value;
	new.z_value = rt.z_value + o.z_value;*/

	//translate point to origin
	t.x_value = p.x_value - o.x_value;
	t.y_value = o.y_value;
	t.z_value = p.z_value - o.z_value;

	rt.x_value = t.x_value * c - t.z_value * s;
	rt.y_value = o.y_value;
	rt.z_value = t.x_value * s + t.z_value * c;

	//translate point back
	new.x_value = rt.x_value + o.x_value;
	new.y_value = o.y_value;
	new.z_value = rt.z_value + o.z_value;

	return new;
}

void drawTree(float xstart, float ystart, float zstart, int cube_side)
{
	Pts2D lcd, start, end, c;
	Pts3D start3D, end3D, c3D;

	double lambda = 0.6;

	/*start3D.x_value = xstart + cube_side;
	start3D.y_value = ystart + (cube_side/2);
	start3D.z_value = zstart;

	end3D.x_value = xstart + cube_side;
	end3D.y_value = ystart + (cube_side/2);
	end3D.z_value = zstart + (cube_side/2);*/

	start3D.x_value = xstart + (cube_side/2);
	start3D.y_value = ystart + cube_side;
	start3D.z_value = zstart;

	end3D.x_value = xstart + (cube_side/2);
	end3D.y_value = ystart + cube_side;
	end3D.z_value = zstart + (cube_side/2);

	designTreeTrunkIn3D(start3D, end3D, GREEN, 1);
	designTreeIn3D(start3D, end3D, 2, lambda);
}

/* Design the trunk of a tree */
void designTreeTrunkIn3D(Pts3D start3D, Pts3D end3D, uint32_t color, uint8_t thickness)
{
	Pts2D lcd, start, end;

	lcd = get3DTransform(start3D);

	start.x = lcd.x;
	start.y = lcd.y;

	lcd = get3DTransform(end3D);

	end.x = lcd.x;
	end.y = lcd.y;

	int i;
	for(i = 0; i < thickness; i++)
		drawLine(start.x + i, start.y, end.x + i, end.y, color);
}

/* Design a Tree using drawline method and rotatepoint method */
void designTreeIn3D(Pts3D start3D, Pts3D end3D, int level, double lambda)
{
	Pts2D lcd, c, start, end;
	Pts3D c3D, rtl3D, rtr3D;

	if(level == 0)
		return;

	lcd = get3DTransform(start3D);

	start.x = lcd.x;
	start.y = lcd.y;

	lcd = get3DTransform(end3D);

	end.x = lcd.x;
	end.y = lcd.y;

	/*c3D.x_value = start3D.x_value;
	c3D.y_value = end3D.y_value + (lambda*(end3D.y_value - start3D.y_value));
	c3D.z_value = end3D.z_value + (lambda*(end3D.z_value - start3D.z_value));*/

	c3D.x_value = end3D.x_value + (lambda*(end3D.x_value - start3D.x_value));
	c3D.y_value = start3D.y_value;
	c3D.z_value = end3D.z_value + (lambda*(end3D.z_value - start3D.z_value));

	lcd = get3DTransform(c3D);

	c.x = lcd.x;
	c.y = lcd.y;

	drawLine(c.x, c.y, end.x, end.y, GREEN);
	designTreeIn3D(end3D, c3D, level - 1, lambda);

	rtl3D = rotate_pointIn3D(c3D, end3D, pi/6);

	lcd = get3DTransform(rtl3D);

	drawLine(lcd.x, lcd.y, end.x, end.y, GREEN);
	designTreeIn3D(end3D, rtl3D, level - 1, lambda);

	rtr3D = rotate_pointIn3D(c3D, end3D, -pi/6);

	lcd = get3DTransform(rtr3D);

	drawLine(lcd.x, lcd.y, end.x, end.y, GREEN);
	designTreeIn3D(end3D, rtr3D, level - 1, lambda);
}

//Rotate cube with respect to the arbitrary vector
Pts3D rotateCoord3D(Pts3D ARBPi, Pts3D ARBPi1, int angle, float cube_x, float cube_y, float cube_z)
{
	Pts3D ARBPi1_T, ARBPi1_R;
	Pts3D cube_T, cube_Rzw, cube_Ryw, cube_Rzw_main, cube_Ryw_reverse, cube_Rzw_reverse, cube_Treverse;

	float cos_alpha_dash, sin_alpha_dash, cos_alpha_doubledash, sin_alpha_doubledash;

	float denom1, denom2;

	float angle_rad;

	//Step1: Translation
	//Find new Pi+1 point after Translation for calculating cos(alpha_dash) and sin(alpha_dash) values
	ARBPi1_T.x_value = ARBPi1.x_value - ARBPi.x_value;
	ARBPi1_T.y_value = ARBPi1.y_value - ARBPi.y_value;
	ARBPi1_T.z_value = ARBPi1.z_value - ARBPi.z_value;

	//Multiply Translation matrix with the given cube point
	cube_T.x_value = cube_x - ARBPi.x_value;
	cube_T.y_value = cube_y - ARBPi.y_value;
	cube_T.z_value = cube_z - ARBPi.z_value;

	//Step2: Rotation with respect to Zw axis (Note that here alpha_dash will be negative since rotation is clockwise => so this changes the rotation_Zw matrix)
	//Calculate cos(alpha_dash) and sin(alpha_dash) values
	denom1 = sqrt(pow(ARBPi1_T.x_value,2) + pow(ARBPi1_T.y_value,2));
	cos_alpha_dash = ARBPi1_T.x_value / denom1;
	sin_alpha_dash = ARBPi1_T.y_value / denom1;

	//Find new Pi+1 point after Rotation. This new point is used to calculate the cos(alpha_doubledash) and sin(alpha_doubledash) values
	ARBPi1_R.x_value = ARBPi1_T.x_value*cos_alpha_dash + ARBPi1_T.y_value*sin_alpha_dash;
	ARBPi1_R.y_value = ARBPi1_T.y_value*cos_alpha_dash - ARBPi1_T.x_value*sin_alpha_dash;
	ARBPi1_R.z_value = ARBPi1_T.z_value;

	//Multiply Rotation_Zw matrix with the translated cube point
	cube_Rzw.x_value = cube_T.x_value*cos_alpha_dash + cube_T.y_value*sin_alpha_dash;
	cube_Rzw.y_value = cube_T.y_value*cos_alpha_dash - cube_T.x_value*sin_alpha_dash;
	cube_Rzw.z_value = cube_T.z_value;

	//Step3: Rotation with respect to Yw axis (Note that here alpha_doubledash will be negative since rotation is clockwise => so this changes the rotation_Yw matrix)
	//Calculate cos(alpha_dash) and sin(alpha_dash) values
	denom2 = sqrt(pow(ARBPi1_R.x_value,2) + pow(ARBPi1_R.z_value,2));
	cos_alpha_doubledash = ARBPi1_R.z_value / denom2;
	sin_alpha_doubledash = ARBPi1_R.x_value / denom2;

	//Multiply Rotation_Yw matrix with the rotated_Zw cube point
	cube_Ryw.x_value = cube_Rzw.x_value*cos_alpha_doubledash - cube_Rzw.z_value*sin_alpha_doubledash;
	cube_Ryw.y_value = cube_Rzw.y_value;
	cube_Ryw.z_value = cube_Rzw.z_value*cos_alpha_doubledash + cube_Rzw.x_value*sin_alpha_doubledash;

	//Step4: Rotation with respect to Zw axis
	//change the given angle from degrees to radians
	angle_rad = (pi * angle) / 180;

	//Multiply Rotation_Yw matrix with the rotated_Yw cube point
	cube_Rzw_main.x_value = cube_Ryw.x_value*cos(angle_rad) - cube_Ryw.y_value*sin(angle_rad);
	cube_Rzw_main.y_value = cube_Ryw.y_value*cos(angle_rad) + cube_Ryw.x_value*sin(angle_rad);
	cube_Rzw_main.z_value = cube_Ryw.z_value;

	//Step5: Revert back the Rotation with respect to Yw axis
	//Multiply Rotation_Yw matrix with the rotated_Zw_main cube point
	cube_Ryw_reverse.x_value = cube_Rzw_main.x_value*cos_alpha_doubledash + cube_Rzw_main.z_value*sin_alpha_doubledash;
	cube_Ryw_reverse.y_value = cube_Rzw_main.y_value;
	cube_Ryw_reverse.z_value = cube_Rzw_main.z_value*cos_alpha_doubledash - cube_Rzw_main.x_value*sin_alpha_doubledash;

	//Step6: Revert back the Rotation with respect to Zw axis
	//Multiply Rotation_Yw matrix with the rotated_Yw_reverse cube point
	cube_Rzw_reverse.x_value = cube_Ryw_reverse.x_value*cos_alpha_dash - cube_Ryw_reverse.y_value*sin_alpha_dash;
	cube_Rzw_reverse.y_value = cube_Ryw_reverse.y_value*cos_alpha_dash + cube_Ryw_reverse.x_value*sin_alpha_dash;
	cube_Rzw_reverse.z_value = cube_Ryw_reverse.z_value;

	//Step7: Revert back the Translation
	//Multiply Translation matrix with the given cube point
	cube_Treverse.x_value = cube_Rzw_reverse.x_value + ARBPi.x_value;
	cube_Treverse.y_value = cube_Rzw_reverse.y_value + ARBPi.y_value;
	cube_Treverse.z_value = cube_Rzw_reverse.z_value + ARBPi.z_value;

	return cube_Treverse;
}

//Draw the cube
void drawCube()
{
	#define UpperBD 52
	#define NumOfPts 16
	typedef struct{
			float X[UpperBD]; float Y[UpperBD]; float Z[UpperBD];
		}pworld;
	typedef struct{
			float X[UpperBD]; float Y[UpperBD]; float Z[UpperBD];
		}pviewer;
	typedef struct{
			float X[UpperBD]; float Y[UpperBD];
		}pperspective;

	// EYE VIEW(200,200,200),

	float Xe=150, Ye=150, Ze=100;

	float Rho=sqrt(pow(Xe,2)+pow(Ye,2)+pow(Xe,2));
	float D_focal=120; //For perspective projection

	pworld WCS;
	pviewer V;
	pperspective P;
	Pts3D Vsingle, RWCS, ARBPi, ARBPi1;
	Pts2D Psingle;
	float_t L1,L2,L3,L4;
	int angle, i;

	angle = -5; //minus for clockwise

	// X-Y-Z World Coordinate System with each of the axis equal to 150

	// Origin
	WCS.X[0]=0.0; WCS.Y[0]=0.0; WCS.Z[0]=0.0;

	//Axes
	WCS.X[1]=150.0; WCS.Y[1]=0.0; WCS.Z[1]=0.0;
	WCS.X[2]=0.0; WCS.Y[2]=150.0; WCS.Z[2]=0.0;
	WCS.X[3]=0.0; WCS.Y[3]=0.0;	WCS.Z[3]=150.0;

	// Elevate Cube along Z_w axis by 10
	// New points to define the cube center as (80,80,35)
	WCS.X[4]=55.0; WCS.Y[4]=55.0; WCS.Z[4]=10.0;
	WCS.X[5]=55.0; WCS.Y[5]=55.0; WCS.Z[5]=60.0;
	WCS.X[6]=55.0; WCS.Y[6]=105.0; WCS.Z[6]=10.0;
	WCS.X[7]=55.0; WCS.Y[7]=105.0; WCS.Z[7]=60.0;
	WCS.X[8]=105.0; WCS.Y[8]=55.0; WCS.Z[8]=10.0;
	WCS.X[9]=105.0; WCS.Y[9]=55.0; WCS.Z[9]=60.0;
	WCS.X[10]=105.0; WCS.Y[10]=105.0; WCS.Z[10]=10.0;
	WCS.X[11]=105.0; WCS.Y[11]=105.0; WCS.Z[11]=60.0;

	//Define given Arbitrary vectors
	ARBPi.x_value=0.0; ARBPi.y_value=0.0; ARBPi.z_value=35.0;
	ARBPi1.x_value=200.0; ARBPi1.y_value=220.0; ARBPi1.z_value=40.0;

	for(i=4;i<12;i++)
	{
		RWCS = rotateCoord3D(ARBPi, ARBPi1, angle, WCS.X[i], WCS.Y[i], WCS.Z[i]);
		WCS.X[i] = RWCS.x_value;
		WCS.Y[i] = RWCS.y_value;
		WCS.Z[i] = RWCS.z_value;
	}

	//Shadow Calculation

	// Point of Light Source PS(-20, -20, 220)
	WCS.X[12]=-20.0; WCS.Y[12]=-20.0; WCS.Z[12]=220.0;

	Pts3D Ps;
	Ps.x_value = WCS.X[12]; Ps.y_value = WCS.Y[12]; Ps.z_value = WCS.Z[12];

	Pts3D P1;
	P1.x_value = WCS.X[5]; P1.y_value = WCS.Y[5]; P1.z_value = WCS.Z[5];

	Pts3D P2;
	P2.x_value = WCS.X[9]; P2.y_value = WCS.Y[9]; P2.z_value = WCS.Z[9];

	Pts3D P3;
	P3.x_value = WCS.X[11]; P3.y_value = WCS.Y[11]; P3.z_value = WCS.Z[11];

	Pts3D P4;
	P4.x_value = WCS.X[7]; P4.y_value = WCS.Y[7]; P4.z_value = WCS.Z[7];

	// Shadow Points

	Pts3D S1,S2,S3,S4;
	L1 = Lambda3D(WCS.Z[5],WCS.Z[12]);
	L2 = Lambda3D(WCS.Z[9],WCS.Z[12]);
	L3 = Lambda3D(WCS.Z[11],WCS.Z[12]);
	L4 = Lambda3D(WCS.Z[7],WCS.Z[12]);
	S1 = ShadowPoint3D(P1,Ps,L1);
	S2 = ShadowPoint3D(P2,Ps,L2);
	S3 = ShadowPoint3D(P3,Ps,L3);
	S4 = ShadowPoint3D(P4,Ps,L4);

	WCS.X[13]=S1.x_value;		WCS.Y[13]=S1.y_value;		WCS.Z[13]=S1.z_value; //S1
	WCS.X[14]=S2.x_value;		WCS.Y[14]=S2.y_value;		WCS.Z[14]=S2.z_value; //S2
	WCS.X[15]=S3.x_value;		WCS.Y[15]=S3.y_value;		WCS.Z[15]=S3.z_value; //S3
	WCS.X[16]=S4.x_value;		WCS.Y[16]=S4.y_value;		WCS.Z[16]=S4.z_value; //S4

	float sPheta = Ye/sqrt(pow(Xe,2)+pow(Ye,2));
	float cPheta = Xe/sqrt(pow(Xe,2)+pow(Ye,2));
	float sPhi = sqrt(pow(Xe,2)+pow(Ye,2))/Rho;
	float cPhi = Ze/Rho;

	// WORLD TO VIEWER TRANSFROM

	for(int i=0;i<=NumOfPts;i++)
	{
		V.X[i] = -sPheta * WCS.X[i] + cPheta * WCS.Y[i];
		V.Y[i] = -cPheta * cPhi * WCS.X[i] - cPhi * sPheta * WCS.Y[i] + sPhi * WCS.Z[i];
		V.Z[i] = -sPhi * cPheta * WCS.X[i] - sPhi * cPheta * WCS.Y[i] - cPhi * WCS.Z[i] + Rho;
	}

	for(int i=0;i<=NumOfPts;i++)
	{
		P.X[i]=V.X[i]*(D_focal/V.Z[i]);
		P.Y[i]=V.Y[i]*(D_focal/V.Z[i]);
	}

	Pts3D temp, temp1;

	temp.x_value = WCS.X[5]; temp.y_value = WCS.Y[5]; temp.z_value = WCS.Z[5];

	float temp_color, temp_color1;

	temp_color = getDiffuseColor(temp);

	// Draw Lines for all the edges of the cube
	drawLine(P.X[0],P.Y[0],P.X[1],P.Y[1],RED);
	drawLine(P.X[0],P.Y[0],P.X[2],P.Y[2],0x00FF00);
	drawLine(P.X[0],P.Y[0],P.X[3],P.Y[3],0x0000FF);

	//New Centered Cube DrawLines
	drawLine(P.X[6],P.Y[6],P.X[4],P.Y[4],WHITE);
	drawLine(P.X[10],P.Y[10],P.X[8],P.Y[8],WHITE);
	drawLine(P.X[6],P.Y[6],P.X[10],P.Y[10],BLUE);
	drawLine(P.X[8],P.Y[8],P.X[4],P.Y[4],WHITE);

	drawLine(P.X[7],P.Y[7],P.X[5],P.Y[5],temp_color);
	drawLine(P.X[7],P.Y[7],P.X[11],P.Y[11],WHITE);
	drawLine(P.X[9],P.Y[9],P.X[11],P.Y[11],WHITE);
	drawLine(P.X[9],P.Y[9],P.X[5],P.Y[5],temp_color);

	drawLine(P.X[9],P.Y[9],P.X[8],P.Y[8],temp_color);
	drawLine(P.X[11],P.Y[11],P.X[10],P.Y[10],WHITE);
	drawLine(P.X[5],P.Y[5],P.X[4],P.Y[4],temp_color);
	drawLine(P.X[7],P.Y[7],P.X[6],P.Y[6],BLUE);

	// Shadow Drawlines
	drawLine(P.X[13],P.Y[13],P.X[14],P.Y[14],DARKBLUE);
	drawLine(P.X[14],P.Y[14],P.X[15],P.Y[15],DARKBLUE);
	drawLine(P.X[16],P.Y[16],P.X[15],P.Y[15],DARKBLUE);
	drawLine(P.X[16],P.Y[16],P.X[13],P.Y[13],DARKBLUE);

	double xvaluestart = S1.x_value;
	double xvalueend = S2.x_value;
	double yvaluestart = S1.y_value;
	double yvalueend = S4.y_value;

	/*printf("%lf\n", xvaluestart);
	printf("%lf\n", xvalueend);
	printf("%lf\n", yvaluestart);
	printf("%lf\n", yvalueend);*/

	//Shadow fill
	for(double x=xvaluestart;x<=xvalueend;x+=1.234)
		for(double y=yvaluestart;y<=yvalueend;y+=1.234)
		{
			Pts3D pt; Pts2D pt_new;
			pt.x_value=x; pt.y_value=y; pt.z_value=0;
			pt_new = get3DTransform(pt);
			drawPixel(pt_new.x,pt_new.y,DARKBLUE);
		}

	//Red - top side Fill
	float diff_color;
	for(float y=WCS.Y[5];y<=WCS.Y[6];y+=0.9888)
		for(float x=WCS.X[4];x<=WCS.X[8];x+=0.9888)
		{
			Pts3D pt; Pts2D pt2d;
			pt.x_value=x; pt.y_value=y; pt.z_value=WCS.Z[5];
			pt2d = get3DTransform(pt);
			// RED DIFFUSE
			diff_color = getDiffuseColor(pt);
			drawPixel(pt2d.x,pt2d.y,diff_color);
		}
	//Green - left side Fill
	for(float y=WCS.Y[5];y<=WCS.Y[6];y+=0.988)
		for(float z=WCS.Z[5];z>=WCS.Z[4];z-=0.988)
		{
			Pts3D pt; Pts2D pt2d;
			pt.x_value=WCS.X[8]; pt.y_value=y; pt.z_value=z;
			pt2d = get3DTransform(pt);
			drawPixel(pt2d.x,pt2d.y,GREEN);
		}
	//Blue - right side Fill
	for(float z=WCS.Z[5];z>=WCS.Z[4];z-=0.988)
		for(float x=WCS.X[8];x>=WCS.X[4];x-=0.988)
		{
			Pts3D pt; Pts2D pt2d;
			pt.x_value=x; pt.y_value=WCS.Y[6]; pt.z_value=z;
			pt2d = get3DTransform(pt);
			drawPixel(pt2d.x,pt2d.y,BLUE);
		}

	int cube_side = 50;

	drawTree(WCS.X[4], WCS.Y[4], WCS.Z[4], cube_side);
}

// Method to draw the half sphere using contours
void drawSphere()
{
	#define UpperBD 360
	#define UpperPCBD 800
	#define TotalPts 360
	#define NumOfLevels 20
	typedef struct{
			float X[UpperBD]; float Y[UpperBD]; float Z[UpperBD];
		}pworld;
	typedef struct{
			float X[UpperBD]; float Y[UpperBD]; float Z[UpperBD];
		}pviewer;
	typedef struct{
			float X[UpperBD]; float Y[UpperBD];
		}pperspective;

	// Structure to hold the equi-distant points on each contour
	typedef struct{
			float X[UpperPCBD]; float Y[UpperPCBD]; float Z[UpperPCBD];
			}pcontour;
	pworld WCS;
	pviewer V;
	pperspective P;
	pcontour PC;
	Pts3D Vsingle, temp3D;
	Pts2D Psingle;

	float Xe=150, Ye=150, Ze=100;

	float Rho=sqrt(pow(Xe,2)+pow(Ye,2)+pow(Xe,2));
	float D_focal=120;

	int diffColor[UpperBD];
	int diffColorPC[UpperPCBD];

	int i,level;
	float y;

	float angle_theta;
	int radius = 100;
	int delta = 10;
	int kx=0, ky=0, kz=0;

	for(level=0;level<=NumOfLevels-1;level++) //20 levels of cross section contours
	{
		for(i=0;i<TotalPts;i++)
		{
			// Logic to draw a circle using angles
			angle_theta = i*3.142 /180;
			WCS.X[i] = 0 + radius*cos(angle_theta);
			WCS.Y[i] =  0 + radius*sin(angle_theta);
			WCS.Z[i] = 4*level; //Contours one above the other with the distance of 10.

			temp3D.x_value = WCS.X[i];
			temp3D.y_value = WCS.Y[i];
			temp3D.z_value = WCS.Z[i];
			diffColor[i] = getDiffuseColorGreen(temp3D);
		}

		float sPheta = Ye/sqrt(pow(Xe,2)+pow(Ye,2));
		float cPheta = Xe/sqrt(pow(Xe,2)+pow(Ye,2));
		float sPhi = sqrt(pow(Xe,2)+pow(Ye,2))/Rho;
		float cPhi = Ze/Rho;

		// WORLD TO VIEWER TRANSFROM

		for(i=0;i<TotalPts;i++)
		{
			V.X[i] = -sPheta * WCS.X[i] + cPheta * WCS.Y[i];
			V.Y[i] = -cPheta * cPhi * WCS.X[i] - cPhi * sPheta * WCS.Y[i] + sPhi * WCS.Z[i];
			V.Z[i] = -sPhi * cPheta * WCS.X[i] - sPhi * cPheta * WCS.Y[i] - cPhi * WCS.Z[i] + Rho;
		}

		// Viewer to perspective transform for each point
		for(i=0;i<TotalPts;i++)
		{
			P.X[i]=V.X[i]*(D_focal/V.Z[i]);
			P.Y[i]=V.Y[i]*(D_focal/V.Z[i]);
			drawPixel(P.X[i], P.Y[i], diffColor[i]);

			// Logic to store a selected number of equi-distant points on each contour into PC.
			// In this case, for each contour, 24 points are selected and later joined.
			if(i%(TotalPts/40) == 0)
			{
				PC.X[kx++] = P.X[i];
				PC.Y[ky++] = P.Y[i];

				diffColorPC[kz++] = diffColor[i];
			}
		}

		//radius-=10;
		//radius -= (level + 1);
		radius-=0.25*level;	// decrease radius of each contour when level increases and Z_w increases
	}

	// Logic to join the equi-distant points of one contour to the next contour
	for(i=0;i<kx;i++)
	{
		if((i+40)<kx)
		drawLine(PC.X[i], PC.Y[i], PC.X[i+40], PC.Y[i+40], diffColorPC[i]);
	}
}

int main (void)
{
	uint32_t pnum = 0 ;

	if ( pnum == 0 )
		SSP0Init();
	else
		 puts("Port number is incorrect");

	 lcd_init();

	 fillrect(0, 0, ST7735_TFTWIDTH, ST7735_TFTHEIGHT, BLACK);

	 drawSphere();

	 drawCube();

	 return 0;
}
