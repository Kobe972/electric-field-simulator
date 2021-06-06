#pragma once
#include "Entry.h"
#include"BobClass.h"
#include<vector>
using namespace std;
//button ID
#define IFLUSH 0

//button states
#define BSTATENORMAL 0
#define BSTATEON 1
#define BSTATEDOWN 2
#define BSTATEUP 3

class CButton : CEntry
{
public:
	~CButton();
	void Create(int ID, int width, int height, int x, int y, std::string filename);
	RECT boarder; //��ť�߽�
	int m_Width, m_Height;
	char* m_Style[3];//��״̬ʱ��ͼ���ַ��ƽʱ������ƹ������£�
	LPDIRECTDRAWSURFACE7 m_ButtonSur[3];
	BITMAP_FILE m_bitmap[3];
	int m_ID;
	int m_state;
	bool m_clipped = false;//֮ǰ����Ƿ���
public:
	void Draw();
	void Check();//process its reaction toward mouse
};
