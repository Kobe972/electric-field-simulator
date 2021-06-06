#include "BobClass.h"
#include "pch.h"

CREATE_BOB_OBJECTS()
// DEFINES ////////////////////////////////////////////////
UINT RGBBIT(UCHAR a, UCHAR r, UCHAR g, UCHAR b) { return b + (g << 8) + (r << 16) + (a << 24); }

int DDraw_Init(int width, int height)
{
    if (FAILED(DirectDrawCreateEx(NULL, (void**)&lpdd, IID_IDirectDraw7, NULL)))
        return(0);

    if (FAILED(lpdd->SetCooperativeLevel(main_window_handle, DDSCL_NORMAL)))
        return(0);

    // set globals
    screen_height = height;
    screen_width = width;

    // Create the primary surface
    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);

    // windowed mode
    ddsd.dwFlags = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    // set the backbuffer count to 0 for windowed mode
    ddsd.dwBackBufferCount = 0;

    // create the primary surface
    lpdd->CreateSurface(&ddsd, &lpddsprimary, NULL);

    // get the pixel format of the primary surface
    DDPIXELFORMAT ddpf; // used to get pixel format

    // initialize structure
    DDRAW_INIT_STRUCT(ddpf);

    // query the format from primary surface
    lpddsprimary->GetPixelFormat(&ddpf);

    // must be windowed, so create a double buffer that will be blitted
    // rather than flipped as in full screen mode
    lpddsback = DDraw_Create_Surface(width, height); // int mem_flags, USHORT color_key_flag);
    // clear out both primary and secondary surfaces
    DDraw_Fill_Surface(lpddsback, 0);

    // set software algorithmic clipping region
    min_clip_x = 0;
    max_clip_x = screen_width - 1;
    min_clip_y = 0;
    max_clip_y = screen_height - 1;

    // setup backbuffer clipper always
    RECT screen_rect = { 0,0,screen_width,screen_height };
    lpddclipper = DDraw_Attach_Clipper(lpddsback, 1, &screen_rect);

    // set windowed clipper
    if (FAILED(lpdd->CreateClipper(0, &lpddclipperwin, NULL)))
        return(0);

    if (FAILED(lpddclipperwin->SetHWnd(0, main_window_handle)))
        return(0);

    if (FAILED(lpddsprimary->SetClipper(lpddclipperwin)))
        return(0);

    // return success
    return(1);
} // end DDraw_Init

///////////////////////////////////////////////////////////

LPDIRECTDRAWSURFACE7 DDraw_Create_Surface(int width,
    int height,
    int mem_flags,
    UINT color_key_low,
    UINT color_key_high)
{
    // this function creates an offscreen plain surface

    DDSURFACEDESC2 ddsd;         // working description
    LPDIRECTDRAWSURFACE7 lpdds;  // temporary surface

    // set to access caps, width, and height
    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CKSRCBLT;

    // set dimensions of the new bitmap surface
    ddsd.dwWidth = width;
    ddsd.dwHeight = height;

    // set surface to offscreen plain
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | mem_flags;

    // create the surface
    if (FAILED(lpdd->CreateSurface(&ddsd, &lpdds, NULL)))
        return(NULL);

    DDraw_SetColorKey(lpdds, color_key_low, color_key_high);

    // return surface
    return(lpdds);
} // end DDraw_Create_Surface

///////////////////////////////////////////////////////////

int DDraw_Fill_Surface(LPDIRECTDRAWSURFACE7 lpdds, USHORT color, RECT* client)
{
    DDBLTFX ddbltfx; // this contains the DDBLTFX structure

    // clear out the structure and set the size field 
    DDRAW_INIT_STRUCT(ddbltfx);

    // set the dwfillcolor field to the desired color
    ddbltfx.dwFillColor = color;

    // ready to blt to surface
    lpdds->Blt(client,     // ptr to dest rectangle
        NULL,       // ptr to source surface, NA            
        NULL,       // ptr to source rectangle, NA
        DDBLT_COLORFILL | DDBLT_WAIT,   // fill and wait                   
        &ddbltfx);  // ptr to DDBLTFX structure

// return success
    return(1);
} // end DDraw_Fill_Surface

///////////////////////////////////////////////////////////

LPDIRECTDRAWCLIPPER DDraw_Attach_Clipper(LPDIRECTDRAWSURFACE7 lpdds,
    int num_rects,
    LPRECT clip_list)

{
    // this function creates a clipper from the sent clip list and attaches
    // it to the sent surface

    int index;                         // looping var
    LPDIRECTDRAWCLIPPER lpddclipper;   // pointer to the newly created dd clipper
    LPRGNDATA region_data;             // pointer to the region data that contains
                                       // the header and clip list

    // first create the direct draw clipper
    if (FAILED(lpdd->CreateClipper(0, &lpddclipper, NULL)))
        return(NULL);

    // now create the clip list from the sent data

    // first allocate memory for region data
    region_data = (LPRGNDATA)malloc(sizeof(RGNDATAHEADER) + num_rects * sizeof(RECT));

    // now copy the rects into region data
    memcpy(region_data->Buffer, clip_list, sizeof(RECT) * num_rects);

    // set up fields of header
    region_data->rdh.dwSize = sizeof(RGNDATAHEADER);
    region_data->rdh.iType = RDH_RECTANGLES;
    region_data->rdh.nCount = num_rects;
    region_data->rdh.nRgnSize = num_rects * sizeof(RECT);

    region_data->rdh.rcBound.left = 64000;
    region_data->rdh.rcBound.top = 64000;
    region_data->rdh.rcBound.right = -64000;
    region_data->rdh.rcBound.bottom = -64000;

    // find bounds of all clipping regions
    for (index = 0; index < num_rects; index++)
    {
        // test if the next rectangle unioned with the current bound is larger
        if (clip_list[index].left < region_data->rdh.rcBound.left)
            region_data->rdh.rcBound.left = clip_list[index].left;

        if (clip_list[index].right > region_data->rdh.rcBound.right)
            region_data->rdh.rcBound.right = clip_list[index].right;

        if (clip_list[index].top < region_data->rdh.rcBound.top)
            region_data->rdh.rcBound.top = clip_list[index].top;

        if (clip_list[index].bottom > region_data->rdh.rcBound.bottom)
            region_data->rdh.rcBound.bottom = clip_list[index].bottom;

    } // end for index

// now we have computed the bounding rectangle region and set up the data
// now let's set the clipping list

    if (FAILED(lpddclipper->SetClipList(region_data, 0)))
    {
        // release memory and return error
        free(region_data);
        return(NULL);
    } // end if

 // now attach the clipper to the surface
    if (FAILED(lpdds->SetClipper(lpddclipper)))
    {
        // release memory and return error
        free(region_data);
        return(NULL);
    } // end if

 // all is well, so release memory and send back the pointer to the new clipper
    free(region_data);
    return(lpddclipper);

} // end DDraw_Attach_Clipper

///////////////////////////////////////////////////////////

int DDraw_Shutdown()
{
    // this function release all the resources directdraw
    // allocated, mainly to com objects

    // release the clippers first
    if (lpddclipper)
        lpddclipper->Release();

    if (lpddclipperwin)
        lpddclipperwin->Release();

    // release the secondary surface
    if (lpddsback)
        lpddsback->Release();

    // release the primary surface
    if (lpddsprimary)
        lpddsprimary->Release();

    // finally, the main dd object
    if (lpdd)
        lpdd->Release();

    // return success
    return(1);
} // end DDraw_Shutdown

///////////////////////////////////////////////////////////


void DDraw_SetColorKey(LPDIRECTDRAWSURFACE7 lpdds, UINT color_key_low, UINT color_key_high)
{
    DDCOLORKEY color_key; // used to set color key
    color_key.dwColorSpaceLowValue = color_key_low;
    color_key.dwColorSpaceHighValue = color_key_high;

    // now set the color key for source blitting
    lpdds->SetColorKey(DDCKEY_SRCBLT, &color_key);
}


int BITMAP_FILE::Load_File(const char* filename)
{
    FILE* fp;
    if ((fp = fopen(filename, "rb")) == NULL) return 0;

    //fseek(fp, 0, SEEK_SET);

    fread(&bitmapfileheader, 14, 1, fp);
    fread(&bitmapinfoheader, sizeof(BITMAPINFOHEADER), 1, fp);

    int Height = bitmapinfoheader.biHeight;
    int Width = bitmapinfoheader.biWidth;

    buffer = (UINT*)malloc(Height * Width * sizeof(UINT));
    if (buffer == nullptr) return 0;
    fread(buffer, sizeof(UINT), Height * Width, fp);

    fclose(fp);

    Flip_Bitmap(buffer, Width, Height);
    // return success
    return(1);
} // end Load_Bitmap_File

int BITMAP_FILE::Unload_File()
{
    // this function releases all memory associated with "bitmap"
    if (buffer)
    {
        // release memory
        free(buffer);

        // reset pointer
        buffer = NULL;

    } // end if

 // return success
    return(1);

} // end Unload_Bitmap_File

///////////////////////////////////////////////////////////

int BITMAP_FILE::Flip_Bitmap(UINT* image, int width, int height)
{
    // this function is used to flip bottom-up .BMP images
    UINT* buffer; // used to perform the image processing
    buffer = (UINT*)malloc(width * height * sizeof(UINT)); // allocate the temporary buffer
    memcpy(buffer, image, width * height * sizeof(UINT)); // copy image to work area

    // flip vertically
    for (int index = 0; index < height; index++)
        memcpy(image + ((height - 1) - index) * width,
            buffer + index * width, width * sizeof(UINT));

    // release the memory
    free(buffer);

    // return success
    return(1);

} // end Flip_Bitmap

int DDraw_Flip(void)
{
    // this function flip the primary surface with the secondary surface

    // test if either of the buffers are locked
    if (primary_buffer || back_buffer)
        return(0);

    RECT    dest_rect;    // used to compute destination rectangle

    // get the window's rectangle in screen coordinates
    GetWindowRect(main_window_handle, &dest_rect);

    // compute the destination rectangle
    dest_rect.left += window_client_x0;
    dest_rect.top += window_client_y0;

    dest_rect.right = dest_rect.left + screen_width;
    dest_rect.bottom = dest_rect.top + screen_height;
    // clip the screen coords 
        // blit the entire back surface to the primary
    if (FAILED(lpddsprimary->Blt(&dest_rect, lpddsback, NULL, DDBLT_WAIT, NULL)))   return(0);
    // return success
    return(1);

} // end DDraw_Flip
void DDraw_Draw_Bitmap(BITMAP_FILE_PTR bitmap, LPDIRECTDRAWSURFACE7 lpdds, POINT coor)
{
    DDSURFACEDESC2 ddsd;  //  direct draw surface description 

    UINT* source_ptr,   // working pointers
        * dest_ptr;
    // extract bitmap data
    source_ptr = bitmap->buffer;
    // get the addr to destination surface memory

    // set size of the structure
    ddsd.dwSize = sizeof(ddsd);
    // lock the display surface
    if (FAILED((lpdds)->Lock(NULL,
        &ddsd,
        DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,
        NULL))) return;

    // assign a pointer to the memory surface for manipulation
    dest_ptr = (UINT*)ddsd.lpSurface;
    // iterate thru each scanline and copy bitmap
    int lpitch = (int)(ddsd.lPitch >> 2);
    dest_ptr += coor.x;
    dest_ptr += lpitch * coor.y;
    for (int index_y = 0; index_y < bitmap->bitmapinfoheader.biHeight; index_y++)
    {
        memcpy(dest_ptr, source_ptr, bitmap->bitmapinfoheader.biWidth * sizeof(UINT)); // copy next line of data to destination

        // advance pointers
        dest_ptr += lpitch;
        source_ptr += bitmap->bitmapinfoheader.biWidth;
    } // end for index_y

    (lpdds)->Unlock(NULL); // unlock the surface 
    return;
}
void Draw_Array(unsigned int* buffer, LPDIRECTDRAWSURFACE7 lpdds)
{
    DDSURFACEDESC2 ddsd;  //  direct draw surface description 

    UINT* source_ptr,   // working pointers
        * dest_ptr;
    // extract bitmap data
    source_ptr = buffer;
    // get the addr to destination surface memory

    // set size of the structure
    ddsd.dwSize = sizeof(ddsd);
    // lock the display surface
    if (FAILED((lpdds)->Lock(NULL,
        &ddsd,
        DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,
        NULL))) return;

    // assign a pointer to the memory surface for manipulation
    dest_ptr = (UINT*)ddsd.lpSurface;
    // iterate thru each scanline and copy bitmap
    int lpitch = (int)(ddsd.lPitch >> 2);
    for (int index_y = 0; index_y < SCREEN_HEIGHT; index_y++)
    {
        memcpy(dest_ptr, source_ptr, SCREEN_WIDTH * sizeof(UINT)); // copy next line of data to destination

        // advance pointers
        dest_ptr += lpitch;
        source_ptr += SCREEN_WIDTH;
    } // end for index_y

    (lpdds)->Unlock(NULL); // unlock the surface 
    return;
}
void Change_To_Client_Rect(LPRECT rect)
{
    RECT rect2;
    GetWindowRect(main_window_handle, &rect2);
    rect2.left += window_client_x0;
    rect2.top += window_client_y0;
    rect2.right = rect2.left + screen_width;
    rect2.bottom = rect2.top + screen_height;
    rect->left += rect2.left;
    rect->right += rect2.left;
    rect->top += rect2.top;
    rect->bottom += rect2.top;
    return;
}
int Draw_Line(int x0, int y0, // starting position 
    int x1, int y1, // ending position
    int color,     // color index
    UINT* vb_start, int lpitch) // video buffer and memory pitch
{
    // this function draws a line from xo,yo to x1,y1 using differential error
    // terms (based on Bresenahams work)
    int dx,             // difference in x's
        dy,             // difference in y's
        dx2,            // dx,dy * 2
        dy2,
        x_inc,          // amount in pixel space to move during drawing
        y_inc,          // amount in pixel space to move during drawing
        error,          // the discriminant i.e. error i.e. decision variable
        index;          // used for looping
    // pre-compute first pixel address in video buffer
    int clash = 0;
    vb_start = vb_start + x0 + y0 * lpitch;
    // compute horizontal and vertical deltas
    dx = x1 - x0;
    dy = y1 - y0;
    // test which direction the line is going in i.e. slope angle
    if (dx >= 0)
    {
        x_inc = 1;

    } // end if line is moving right
    else
    {
        x_inc = -1;
        dx = -dx;  // need absolute value

    } // end else moving left

 // test y component of slope

    if (dy >= 0)
    {
        y_inc = lpitch;
    } // end if line is moving down
    else
    {
        y_inc = -lpitch;
        dy = -dy;  // need absolute value
    } // end else moving up
 // compute (dx,dy) * 2
    dx2 = dx << 1;
    dy2 = dy << 1;
    // now based on which delta is greater we can draw the line
    if (dx > dy)
    {
        // initialize error term
        error = dy2 - dx;
        // draw the line
        for (index = 0; index <= dx; index++)
        {
            // set the pixel
            *vb_start = color;
            // test if error has overflowed
            if (error >= 0)
            {
                error -= dx2;
                // move to next line
                vb_start += y_inc;
            } // end if error overflowed
            // adjust the error term
            error += dy2;
            // move to the next pixel
            vb_start += x_inc;
        } // end for
    } // end if |slope| <= 1
    else
    {
        // initialize error term
        error = dx2 - dy;
        // draw the line
        for (index = 0; index <= dy; index++)
        {
            // set the pixel
            *vb_start = color;
            // test if error overflowed
            if (error >= 0)
            {
                error -= dy2;
                // move to next line
                vb_start += x_inc;
            } // end if error overflowed
         // adjust the error term
            error += dx2;
            // move to the next pixel
            vb_start += y_inc;
        } // end for
    } // end else |slope| > 1
 // return success
    return(1);
} // end Draw_Line