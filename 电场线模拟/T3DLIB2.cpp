// T3DLIB2.CPP - Game Engine Part II

// INCLUDES ///////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN  
// #define INITGUID

#include <windows.h>   // include important windows stuff
#include <windowsx.h> 
#include <mmsystem.h>
#include <objbase.h>
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

#include <ddraw.h>  // directX includes
#include <dinput.h>

#include "T3DLIB2.H"

// DEFINES ////////////////////////////////////////////////

// TYPES //////////////////////////////////////////////////

// PROTOTYPES /////////////////////////////////////////////

// EXTERNALS /////////////////////////////////////////////

extern HWND main_window_handle;     // access to main window handle in main module
extern HINSTANCE main_instance; // save the instance

// GLOBALS ////////////////////////////////////////////////

// directinput globals
LPDIRECTINPUT8       lpdi = NULL;    // dinput object
LPDIRECTINPUTDEVICE8 lpdikey = NULL;    // dinput keyboard
LPDIRECTINPUTDEVICE8 lpdimouse = NULL;    // dinput mouse
// these contain the target records for all di input packets
UCHAR keyboard_state[256]; // contains keyboard state table
DIMOUSESTATE mouse_state;  // contains state of mouse
POINT pos;

// FUNCTIONS //////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////

int DInput_Init(void)
{
    // this function initializes directinput

    if (FAILED(DirectInput8Create(main_instance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&lpdi, NULL)))
        return(0);

    // return success
    return(1);

} // end DInput_Init

///////////////////////////////////////////////////////////

void DInput_Shutdown(void)
{
    // this function shuts down directinput

    if (lpdi)
        lpdi->Release();

} // end DInput_Shutdown

///////////////////////////////////////////////////////////

int DInput_Init_Mouse(void)
{
    // this function intializes the mouse

    // create a mouse device 
    if (lpdi->CreateDevice(GUID_SysMouse, &lpdimouse, NULL) != DI_OK)
        return(0);

    // set cooperation level
    // change to EXCLUSIVE FORGROUND for better control
    if (lpdimouse->SetCooperativeLevel(main_window_handle,
        DISCL_NONEXCLUSIVE | DISCL_BACKGROUND) != DI_OK)
        return(0);

    // set data format
    if (lpdimouse->SetDataFormat(&c_dfDIMouse) != DI_OK)
        return(0);

    // acquire the mouse
    if (lpdimouse->Acquire() != DI_OK)
        return(0);

    // return success
    return(1);

} // end DInput_Init_Mouse

int DInput_Read_Mouse(void)
{
    // this function reads  the mouse state

    if (lpdimouse)
    {
        if (lpdimouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouse_state) != DI_OK)
            return(0);
    }
    else
    {
        // mouse isn't plugged in, zero out state
        memset(&mouse_state, 0, sizeof(mouse_state));

        // return error
        return(0);
    } // end else

// return sucess
    return(1);

} // end DInput_Read_Mouse

void DInput_Release_Mouse(void)
{
    // this function unacquires and releases the mouse

    if (lpdimouse)
    {
        lpdimouse->Unacquire();
        lpdimouse->Release();
    } // end if

} // end DInput_Release_Mouse