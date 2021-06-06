#pragma once
#include <Windows.h>
#include<direct.h>
#include<math.h>
#include"GUI.h"
#pragma warning
EXTERN_BOB_OBJECTS()
EXTERN_INPUT_DATA()
struct POINT_double
{
	double x, y;
};
class Elem
{
public:
	int x, y; //Œª÷√
	double q; //µÁ∫…¡ø
};
struct Circle
{
	int x, y, r;
};
class Core
{
public:
	void Init();
	void Main();
	void Flush();
	~Core();
public:
	void ProcessButtonMsg();
	void ShowInterface();
	void GetCurMsg();
	void InfoToBuffer(int fill=1);
	void Load();
	void DrawField();
	POINT_double ComputeDoubleForce(int x, int y, int q);
	inline void SetWindowHandle(HWND hwnd);
	POINT ComputeForce(int x,int y,int q);
private:
	vector<Elem> elem_in;
	vector<Elem> elem_out;
	vector<Circle> circles;
	HWND m_hWnd;
	unsigned int buffer[SCREEN_HEIGHT][SCREEN_WIDTH];
	int info[SCREEN_HEIGHT][SCREEN_WIDTH];
	void DrawFromStart(double x, double y,int sign);
};