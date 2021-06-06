#pragma once

// DEFINES ////////////////////////////////////////////////

// MACROS /////////////////////////////////////////////////



// TYPES //////////////////////////////////////////////////



// PROTOTYPES /////////////////////////////////////////////

// input
int DInput_Init(void);
void DInput_Shutdown(void);

int DInput_Init_Mouse(void);
int DInput_Read_Mouse(void);
void DInput_Release_Mouse(void);

// GLOBALS ////////////////////////////////////////////////


// EXTERNALS //////////////////////////////////////////////

extern HWND main_window_handle; // save the window handle
extern HINSTANCE main_instance; // save the instance

extern DIMOUSESTATE mouse_state;  // contains state of mouse
extern POINT pos;

#define MOUSE_LEFT_BUTTON 0