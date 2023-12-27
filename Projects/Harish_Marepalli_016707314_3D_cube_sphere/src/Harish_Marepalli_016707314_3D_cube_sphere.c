/*
===============================================================================
 Name        : Harish_Marepalli_016707314_3D_cube_sphere.c
 Author      : Harish Marepalli
 Version     :
 Copyright   : $(copyright)
 Description : This program is used to for creating and displaying the world coordinate system, a 3D cube and a half sphere object based on the transformation pipeline.
===============================================================================
*/

#include <cr_section_macros.h>
#include <NXP/crp.h>
#include "LPC17xx.h"                        /* LPC17xx definitions */
#include "ssp.h"
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

int _height = ST7735_TFTHEIGHT;
int _width = ST7735_TFTWIDTH;

#define pi 3.1416

typedef struct Point
{
	float x;
	float y;
}Point;

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
	 printf("3D Graphics Processing Engine Design: A 3D Cube and a Half Sphere Homework\n");
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

// Coordinate to Cartesian
int16_t xCartesian(int16_t x)
{
	x = x + (_width>>1);
	return x;
}

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

void drawCube()
{
	#define UpperBD 52
	#define NumOfPts 10
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

	float Xe=200, Ye=200, Ze=200;

	float Rho=sqrt(pow(Xe,2)+pow(Ye,2)+pow(Xe,2));
	float D_focal=150; //For perspective projection

	pworld WCS;
	pviewer V;
	pperspective P;

	// X-Y-Z World Coordinate System with each of the axis equal to 150

	// Origin
	WCS.X[0]=0.0; WCS.Y[0]=0.0; WCS.Z[0]=0.0;
	WCS.X[1]=150.0; WCS.Y[1]=0.0; WCS.Z[1]=0.0;
	WCS.X[2]=0.0; WCS.Y[2]=150.0; WCS.Z[2]=0.0;
	WCS.X[3]=0.0; WCS.Y[3]=0.0;	WCS.Z[3]=150.0;

	// Elevate Cube along Z_w axis by 10

	WCS.X[4]=100.0; WCS.Y[4]=0; WCS.Z[4]=10.0;
	WCS.X[5]=0.0; WCS.Y[5]=100.0; WCS.Z[5]=10.0;
	WCS.X[6]=0.0; WCS.Y[6]=0.0; WCS.Z[6]=110.0;
	WCS.X[7]=100.0; WCS.Y[7]=0.0; WCS.Z[7]=110.0;
	WCS.X[8]=100.0; WCS.Y[8]=100.0; WCS.Z[8]=10.0;
	WCS.X[9]=0.0; WCS.Y[9]=100.0; WCS.Z[9]=110.0;
	WCS.X[10]=100.0; WCS.Y[10]=100.0; WCS.Z[10]=110.0;

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

	// CUBE DRAWLINES
	drawLine(P.X[0],P.Y[0],P.X[1],P.Y[1],RED);
	drawLine(P.X[0],P.Y[0],P.X[2],P.Y[2],0x00FF00);
	drawLine(P.X[0],P.Y[0],P.X[3],P.Y[3],0x0000FF);
	drawLine(P.X[7],P.Y[7],P.X[4],P.Y[4],WHITE);
	drawLine(P.X[7],P.Y[7],P.X[6],P.Y[6],WHITE);
	drawLine(P.X[7],P.Y[7],P.X[10],P.Y[10],WHITE);
	drawLine(P.X[8],P.Y[8],P.X[4],P.Y[4],WHITE);
	drawLine(P.X[8],P.Y[8],P.X[5],P.Y[5],WHITE);
	drawLine(P.X[8],P.Y[8],P.X[10],P.Y[10],WHITE);
	drawLine(P.X[9],P.Y[9],P.X[6],P.Y[6],WHITE);
	drawLine(P.X[9],P.Y[9],P.X[5],P.Y[5],WHITE);
	drawLine(P.X[9],P.Y[9],P.X[10],P.Y[10],WHITE);
}

void drawSphere()
{
	#define UpperBD 360
	#define UpperPCBD 200
	#define NumOfPts 10
	typedef struct{
			float X[UpperBD]; float Y[UpperBD]; float Z[UpperBD];
		}pworld;
	typedef struct{
			float X[UpperBD]; float Y[UpperBD]; float Z[UpperBD];
		}pviewer;
	typedef struct{
			float X[UpperBD]; float Y[UpperBD];
		}pperspective;
	typedef struct{
			float X[UpperPCBD]; float Y[UpperPCBD]; float Z[UpperPCBD];
			}pcontour;
	pworld WCS;
	pviewer V;
	pperspective P;
	pcontour PC;

	float Xe=200, Ye=200, Ze=200;

	float Rho=sqrt(pow(Xe,2)+pow(Ye,2)+pow(Xe,2));
	float D_focal=150;

	int i,level;
	float y;

	float angle_theta;
	int radius = 100;
	int delta = 10;
	int kx=0, ky=0, kz=0;

	for(level=0;level<=9;level++) //10 levels of cross section contours
	{
		for(i=0;i<360;i++)
		{
			angle_theta = i*3.142 /180;
			WCS.X[i] = 0 + radius*cos(angle_theta);
			WCS.Y[i] =  0 + radius*sin(angle_theta);
			WCS.Z[i] = 10*level; //Contours one above the other with the distance of 10.
		}

		float sPheta = Ye/sqrt(pow(Xe,2)+pow(Ye,2));
		float cPheta = Xe/sqrt(pow(Xe,2)+pow(Ye,2));
		float sPhi = sqrt(pow(Xe,2)+pow(Ye,2))/Rho;
		float cPhi = Ze/Rho;

		// WORLD TO VIEWER TRANSFROM

		for(i=0;i<360;i++)
		{
			V.X[i] = -sPheta * WCS.X[i] + cPheta * WCS.Y[i];
			V.Y[i] = -cPheta * cPhi * WCS.X[i] - cPhi * sPheta * WCS.Y[i] + sPhi * WCS.Z[i];
			V.Z[i] = -sPhi * cPheta * WCS.X[i] - sPhi * cPheta * WCS.Y[i] - cPhi * WCS.Z[i] + Rho;
		}

		for(i=0;i<360;i++)
		{
			P.X[i]=V.X[i]*(D_focal/V.Z[i]);
			P.Y[i]=V.Y[i]*(D_focal/V.Z[i]);
			drawPixel(P.X[i], P.Y[i], WHITE);
			if(i%18 == 0)
			{
				PC.X[kx++] = P.X[i];
				PC.Y[ky++] = P.Y[i];
			}
		}

		//radius-=10;
		radius -= (level + 1);
	}
	for(i=0;i<kx;i++)
	{
		if((i+20)<kx)
		drawLine(PC.X[i], PC.Y[i], PC.X[i+20], PC.Y[i+20], WHITE);
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

	 drawCube();

	 drawSphere();

	 return 0;
}
