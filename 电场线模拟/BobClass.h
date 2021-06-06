#pragma once
#include <windows.h>   // include important windows stuff
#include <windowsx.h> 
#include <mmsystem.h>
#include <iostream> // include important C/C++ stuff
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <io.h>
#include <fcntl.h>
#include <sys/timeb.h>
#include <time.h>
#include<iostream>
using namespace std;

#include "pch.h"
EXTERN_BOB_OBJECTS()

#define BITMAP_ID            0x4D42 // universal id for a bitmap
#define BITMAP_STATE_DEAD    0
#define BITMAP_STATE_ALIVE   1
#define BITMAP_STATE_DYING   2 
#define BITMAP_ATTR_LOADED   128

#define BITMAP_EXTRACT_MODE_CELL  0
#define BITMAP_EXTRACT_MODE_ABS   1

#define WINDOW_WIDTH      800   // size of window
#define WINDOW_HEIGHT     600

UINT RGBBIT(UCHAR a, UCHAR r, UCHAR g, UCHAR b);
int DDraw_Init(int width, int height);
int DDraw_Shutdown(void);
LPDIRECTDRAWCLIPPER DDraw_Attach_Clipper(LPDIRECTDRAWSURFACE7 lpdds, int num_rects, LPRECT clip_list);
LPDIRECTDRAWSURFACE7 DDraw_Create_Surface(int width, int height, int mem_flags = 0,
    UINT color_key_low = RGBBIT(0, 0, 0, 0),
    UINT color_key_high = RGBBIT(0, 0, 0, 0));
int DDraw_Fill_Surface(LPDIRECTDRAWSURFACE7 lpdds, USHORT color, RECT* client = NULL);
int DDraw_Flip(void);
void Draw_Array(unsigned int* buffer, LPDIRECTDRAWSURFACE7 lpdds);
int Draw_Line(int x0, int y0, int x1, int y1, int color, UINT* vb_start, int lpitch);
void Change_To_Client_Rect(LPRECT rect);
typedef class BITMAP_FILE_TAG
{
public:
    BITMAPFILEHEADER bitmapfileheader;  // this contains the bitmapfile header
    BITMAPINFOHEADER bitmapinfoheader;  // this is all the info including the palette
    UINT* buffer;           // this is a pointer to the data
    int Flip_Bitmap(UINT* image, int width, int height);
    int Load_File(const char* filename);
    int Unload_File();
} BITMAP_FILE, * BITMAP_FILE_PTR;
void DDraw_Draw_Bitmap(BITMAP_FILE_PTR bitmap, LPDIRECTDRAWSURFACE7 lpdds, POINT coor);
void DDraw_SetColorKey(LPDIRECTDRAWSURFACE7 lpdds, UINT color_key_low = RGBBIT(0, 0, 0, 0),
    UINT color_key_high = RGBBIT(0, 0, 0, 0));

class Clock {
private:
    ULONGLONG m_dwStartTime;
public:
    void Start_Clock() { m_dwStartTime = GetTickCount64(); }
    void Wait_Clock(DWORD dwcount) { while (GetTickCount64() - m_dwStartTime <= dwcount); }
    int Get_Time() { return (GetTickCount64() - m_dwStartTime) / 1000; }
};
