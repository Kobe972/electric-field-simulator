#pragma once
#include "pch.h"
class CPoint
{
	int m_x, m_y;
};
class CEntry
{
private:
	DWORD m_dwX, m_dwY;				//���Ͻ�����

	DWORD m_dwHeight, m_dwWidth;	//��ʽ��չ�ִ�С

public:
	virtual void Draw(void) = 0;//Draw function, must be completed.
	inline void SetLocation(DWORD x, DWORD y);
	inline void SetSize(DWORD height, DWORD width);

};