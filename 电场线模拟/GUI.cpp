#include "GUI.h"
EXTERN_BOB_OBJECTS()
CButton button[20];
CButton::~CButton()
{
	for (int i = 0; i < 3; i++)
	{
		if (m_ButtonSur[i])
		{
			m_ButtonSur[i]->Release();
			m_ButtonSur[i] = NULL;
			m_bitmap[i].Unload_File();
		}
	}
}
void CButton::Create(int ID, int width, int height, int x, int y, std::string filename)
{
	m_ID = ID;
	POINT position_in_offscreen;
	position_in_offscreen.x = 0;
	position_in_offscreen.y = 0;
	BITMAP_FILE T_bitmap[3];
	for (int i = 0; i < 3; i++)
		m_ButtonSur[i] = DDraw_Create_Surface(width, height, 0, 0);
	m_Width = width;
	m_Height = height;
	boarder.left = x;
	boarder.top = y;
	boarder.bottom = boarder.top + m_Height;
	boarder.right = boarder.left + m_Width;
	T_bitmap[0].Load_File((".\\GUI\\button\\" + filename + ".bmp").c_str());
	T_bitmap[1].Load_File((".\\GUI\\button\\" + filename + "_on.bmp").c_str());
	T_bitmap[2].Load_File((".\\GUI\\button\\" + filename + "_down.bmp").c_str());
	for (int i = 0; i < 3; i++)
		DDraw_Draw_Bitmap(&T_bitmap[i], m_ButtonSur[i], position_in_offscreen);
	m_state = BSTATENORMAL;
	for (int i = 0; i < 3; ++i)
		T_bitmap[i].Unload_File();
}
void CButton::Draw()//在后备缓冲表面绘图
{
	lpddsback->Blt(&boarder, m_ButtonSur[m_state % 3], NULL, DDBLT_WAIT | DDBLT_KEYSRC, NULL);
}

void CButton::Check()
{
	if (m_clipped == false && m_state != BSTATEUP) m_state = BSTATENORMAL;
	if (pos.x >= boarder.left && pos.x <= boarder.right && pos.y >= boarder.top && pos.y <= boarder.bottom)
	{
		if (m_clipped == false) m_state = BSTATEON;
		else m_state = BSTATEUP;
		if (mouse_state.rgbButtons[MOUSE_LEFT_BUTTON] & 0x80)
		{
			m_state = BSTATEDOWN;
			m_clipped = true;
		}
		else m_clipped = false;
	}
	else
	{
		m_clipped = false;
	}
}
