// DEMO8_12_16b.CPP - Sparse universe scrolling demo
// 16-bit version

// INCLUDES ///////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN  

// you must #define INITGUID if not done elsewhere
#define INITGUID
#include <winsock2.h>
#include "core.h"
#include "pch.h"


EXTERN_BOB_OBJECTS()

// defines for windows 
#define WINDOW_CLASS_NAME "WINCLASS1"

// setup a 640x480 16-bit windowed mode example
#define WINDOW_TITLE      "电场线模拟"
#define WINDOW_WIDTH      800   // size of window
#define WINDOW_HEIGHT     600
#define WINDOWED_APP      1     // 0 not windowed, 1 windowed

#define BITMAP_ID            0x4D42 // universal id for a bitmap
#define MAX_COLORS_PALETTE   256

// size of universe, could be anything
#define MAX_UNIVERSE_X 6400
#define MAX_UNIVERSE_Y 4800

#define NUM_OBJECTS_UNIVERSE 256 // number of objects in the universe

// used to flag type of object
#define GENERATOR_OBJ   0
#define BEACON_OBJ      1  
#define ALIEN_OBJ       2  



// PROTOTYPES /////////////////////////////////////////////

// game console
int Game_Init(void* parms = NULL, int num_parms = 0);
int Game_Shutdown(void* parms = NULL, int num_parms = 0);
int Game_Main(void* parms = NULL, int num_parms = 0);

// GLOBALS ////////////////////////////////////////////////
// windows vars
int       window_closed = 0;    // tracks if window is closed

char buffer[80];                // used to print text

// demo globals
Core core;
// PROTOTYPES //////////////////////////////////////////////

LRESULT CALLBACK WindowProc(HWND hwnd,
    UINT msg,
    WPARAM wparam,
    LPARAM lparam)
{
    // this is the main message handler of the system
    PAINTSTRUCT		ps;		// used in WM_PAINT
    HDC				hdc;	// handle to a device context
    char buffer[80];        // used to print strings

    // what is the message 
    switch (msg)
    {
    case WM_CREATE:
    {
        // do initialization stuff here
        // return success
        return(0);
    } break;

    case WM_PAINT:
    {
        // simply validate the window 
        hdc = BeginPaint(hwnd, &ps);
        // end painting
        EndPaint(hwnd, &ps);

        // return success
        return(0);
    } break;
    case WM_DESTROY:
    {

        // kill the application, this sends a WM_QUIT message 
        PostQuitMessage(0);

        // return success
        return(0);
    } break;
    case WM_CLOSE:
        Game_Shutdown();
        break;

    default:break;

    } // end switch

// process any messages that we didn't take care of 
    return (DefWindowProc(hwnd, msg, wparam, lparam));

} // end WinProc

// WINMAIN ////////////////////////////////////////////////

int WINAPI WinMain(HINSTANCE hinstance,
    HINSTANCE hprevinstance,
    LPSTR lpcmdline,
    int ncmdshow)
{

    srand(time(0));
    WNDCLASSEX winclass; // this will hold the class we create
    HWND	   hwnd;	 // generic window handle
    MSG		   msg;		 // generic message
    HDC        hdc;      // graphics device context

    // first fill in the window class stucture
    winclass.cbSize = sizeof(WNDCLASSEX);
    winclass.style = CS_DBLCLKS | CS_OWNDC |
        CS_HREDRAW | CS_VREDRAW;
    winclass.lpfnWndProc = WindowProc;
    winclass.cbClsExtra = 0;
    winclass.cbWndExtra = 0;
    winclass.hInstance = hinstance;
    winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    winclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    winclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    winclass.lpszMenuName = NULL;
    winclass.lpszClassName = WINDOW_CLASS_NAME;
    winclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    // save hinstance in global
    main_instance = hinstance;

    // register the window class
    if (!RegisterClassEx(&winclass))
        return(0);

    /// create the window
    if (!(hwnd = CreateWindowEx(NULL,                  // extended style
        WINDOW_CLASS_NAME,     // class
        WINDOW_TITLE, // title
        (WINDOWED_APP ? (WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION) : (WS_POPUP | WS_VISIBLE)),
        0, 0,	  // initial x,y
        WINDOW_WIDTH, WINDOW_HEIGHT,  // initial width, height
        NULL,	  // handle to parent 
        NULL,	  // handle to menu
        hinstance,// instance of this application
        NULL)))	// extra creation parms
        return(0);

    // save main window handle
    main_window_handle = hwnd;

    if (WINDOWED_APP)
    {
        // now resize the window, so the client area is the actual size requested
        // since there may be borders and controls if this is going to be a windowed app
        // if the app is not windowed then it won't matter
        RECT window_rect = { 0,0,WINDOW_WIDTH - 1,WINDOW_HEIGHT - 1 };

        // make the call to adjust window_rect
        AdjustWindowRectEx(&window_rect,
            GetWindowStyle(main_window_handle),
            GetMenu(main_window_handle) != NULL,
            GetWindowExStyle(main_window_handle));

        // save the global client offsets, they are needed in DDraw_Flip()
        window_client_x0 = -window_rect.left;
        window_client_y0 = -window_rect.top;

        // now resize the window with a call to MoveWindow()
        MoveWindow(main_window_handle,
            0, // x position
            0, // y position
            window_rect.right - window_rect.left, // width
            window_rect.bottom - window_rect.top, // height
            TRUE);

        // show the window, so there's no garbage on first render
        ShowWindow(main_window_handle, SW_SHOW);
    } // end if windowed
    // initialize game here
    Game_Init();

    // enter main event loop
    while (TRUE)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } // end if

        // main game processing goes here
        Game_Main();

    } // end while

// closedown game here
    Game_Shutdown();

    // return to Windows like this
    return(msg.wParam);

} // end WinMain

// GAME PROGRAMMING CONSOLE FUNCTIONS ////////////////

int Game_Init(void* parms, int num_parms)
{
    // this function is where you do all the initialization 

    // start up DirectDraw (replace the parms as you desire)
    DDraw_Init(WINDOW_WIDTH, WINDOW_HEIGHT);
    core.Init();
    return(1);

} // end Game_Init

///////////////////////////////////////////////////////////

int Game_Shutdown(void* parms, int num_parms)
{
    return(1);
}

///////////////////////////////////////////////////////////

int Game_Main(void* parms, int num_parms)
{
    // this is the workhorse of your game it will be called
    // continuously in real-time this is like main() in C
    // all the calls for you game go here!
    core.Main();
    DDraw_Flip();
    return(1);

}

//////////////////////////////////////////////////////////