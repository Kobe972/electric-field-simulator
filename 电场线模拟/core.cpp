#include"core.h"
extern CButton button[20];
EXTERN_INPUT_DATA()

inline void Core::SetWindowHandle(HWND hwnd)
{
    m_hWnd = hwnd;
}
POINT_double Core::ComputeDoubleForce(int x, int y,int q)
{
    POINT_double ret;
    double ret_x=0, ret_y=0;
    for (int i = 0; i < elem_in.size(); i++)
    {
        if (elem_in[i].x == x && elem_in[i].y == y) continue;
        ret_x += (x - elem_in[i].x) * elem_in[i].q * q / pow(sqrt((x - elem_in[i].x) * (x - elem_in[i].x) + (y - elem_in[i].y) * (y - elem_in[i].y)),3);
        ret_y += (y - elem_in[i].y) * elem_in[i].q * q / pow(sqrt((x - elem_in[i].x) * (x - elem_in[i].x) + (y - elem_in[i].y) * (y - elem_in[i].y)), 3);
    }
    for (int i = 0; i < elem_out.size(); i++)
    {
        if (elem_out[i].x == x && elem_out[i].y == y) continue;
        ret_x += (x - elem_out[i].x) * elem_out[i].q * q / pow(sqrt((x - elem_out[i].x) * (x - elem_out[i].x) + (y - elem_out[i].y) * (y - elem_out[i].y)), 3);
        ret_y += (y - elem_out[i].y) * elem_out[i].q * q / pow(sqrt((x - elem_out[i].x) * (x - elem_out[i].x) + (y - elem_out[i].y) * (y - elem_out[i].y)), 3);
    }
    double r = sqrt(ret_x * ret_x + ret_y * ret_y);
    if (r != 0)
    {
        ret_x /= r;
        ret_y /= r;
    }
    ret.x = ret_x;
    ret.y = ret_y;
    return ret;
}
POINT Core::ComputeForce(int x, int y, int q)
{
    POINT ret;
    double ret_x = 0, ret_y = 0;
    for (int i = 0; i < elem_in.size(); i++)
    {
        if (elem_in[i].x == x && elem_in[i].y == y) continue;
        ret_x += (x - elem_in[i].x) * elem_in[i].q * q / pow(sqrt((x - elem_in[i].x) * (x - elem_in[i].x) + (y - elem_in[i].y) * (y - elem_in[i].y)), 3);
        ret_y += (y - elem_in[i].y) * elem_in[i].q * q / pow(sqrt((x - elem_in[i].x) * (x - elem_in[i].x) + (y - elem_in[i].y) * (y - elem_in[i].y)), 3);
    }
    for (int i = 0; i < elem_out.size(); i++)
    {
        if (elem_out[i].x == x && elem_out[i].y == y) continue;
        ret_x += (x - elem_out[i].x) * elem_out[i].q * q / pow(sqrt((x - elem_out[i].x) * (x - elem_out[i].x) + (y - elem_out[i].y) * (y - elem_out[i].y)), 3);
        ret_y += (y - elem_out[i].y) * elem_out[i].q * q / pow(sqrt((x - elem_out[i].x) * (x - elem_out[i].x) + (y - elem_out[i].y) * (y - elem_out[i].y)), 3);
    }
    if (abs(ret_x) <= abs(ret_y) && ret_x != 0)
    {
        ret_y /= abs(ret_x);
        ret_x /= abs(ret_x);
    }
    if (abs(ret_x) > abs(ret_y) && ret_y != 0)
    {
        ret_x /= abs(ret_y);
        ret_y /= abs(ret_y);
    }
    ret.x = ret_x;
    ret.y = ret_y;
    return ret;
}
void Core::DrawFromStart(double x, double y,int sign)
{
    if ((x < 0 || y < 0 || x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) && x < 10 * SCREEN_WIDTH && y < 10 * SCREEN_HEIGHT&&x>-9* SCREEN_WIDTH&&y>-9* SCREEN_HEIGHT)
    {
        POINT_double force = ComputeDoubleForce(x, y, sign);
        if (force.x == 0 && force.y == 0)
            return;
        DrawFromStart(x + force.x, y + force.y, sign);
        return;
    }
    if (x < 0 || y < 0 || x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT)
    {
        return;
    }
    if (buffer[int(y)][int(x)] != RGBBIT(0, 255, 255, 255)&& buffer[int(y)][int(x)] != RGBBIT(0, 0, 0, 0))
        return;
    /*
    for (int i = 0; i < circles.size(); i++)
    {
        if (sqrt((x - circles[i].x) * (x - circles[i].x) + (y - circles[i].y) * (y - circles[i].y)) <= circles[i].r) return;
    }*/
    buffer[int(y)][int(x)] = 0;
    POINT_double force = ComputeDoubleForce(x, y, sign);
    if (force.x == 0 && force.y == 0)
    {
        return;
    }
    DrawFromStart(x + force.x, y + force.y,sign);
}
void Core::Init()
{
    Flush();
    button[IFLUSH].Create(IFLUSH, 180, 60, 310, 10, "flush");
    DInput_Init();
    DInput_Init_Mouse();
}
void Core::Main()
{
    ShowInterface();
    HWND hw;
    hw = GetForegroundWindow();
    if (hw == main_window_handle)
    {
        GetCurMsg();//include mouse and keyboard.understand "cur" as "current",not "cursor"
        ProcessButtonMsg();
    }
}
void Core::Flush()
{
    elem_in.clear();
    elem_out.clear();
    circles.clear();
    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < SCREEN_WIDTH; j++)
        {
            buffer[i][j] = RGBBIT(0, 255, 255, 255);
            info[i][j] = 0;
        }
    }
    Load();
    int deb;
    InfoToBuffer(0);
    DrawField();
    InfoToBuffer();
    return;
}
Core::~Core()
{
    DInput_Read_Mouse();
    DInput_Shutdown();
    DDraw_Shutdown();
}
void Core::GetCurMsg()
{
    GetCursorPos(&pos);
    ScreenToClient(main_window_handle, &pos);
    DInput_Read_Mouse();
    return;
}
void Core::InfoToBuffer(int fill)
{
    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < SCREEN_WIDTH; j++)
        {
            if (info[i][j] > 0 && info[i][j] < 10000)
            {
                for (int i2 = i - 4; i2 <= i + 4; i2++)
                {
                    for (int j2 = j - 4; j2 <= j + 4; j2++)
                    {
                        if (i2 >= 0 && j2 >= 0 && i2 < SCREEN_HEIGHT && j2 < SCREEN_WIDTH && sqrt((i2 - i) * (i2 - i) + (j2 - j) * (j2 - j)) < 4)
                        {
                            buffer[i2][j2] = RGBBIT(0, 255, 0, 0);
                        }
                    }
                }
            }
            if (info[i][j] < 0 && info[i][j] > -10000)
            {
                for (int i2 = i - 4; i2 <= i + 4; i2++)
                {
                    for (int j2 = j - 4; j2 <= j + 4; j2++)
                    {
                        if (i2 >= 0 && j2 >= 0 && i2 < SCREEN_HEIGHT && j2 < SCREEN_WIDTH && sqrt((i2 - i) * (i2 - i) + (j2 - j) * (j2 - j)) < 4)
                        {
                            buffer[i2][j2] = RGBBIT(0, 0, 0, 255);
                        }
                    }
                }
            }
            if (fill)
            {
                if (info[i][j] == 10000) buffer[i][j] = RGBBIT(0, 0, 0, 0); //导体边缘
                if (info[i][j] == -10000||info[i][j]>10000) buffer[i][j] = RGBBIT(0, 0, 220, 255); //导体内部
            }
            else
            {
                if (info[i][j] >= 20000)
                {
                    for (int i2 = i - 1; i2 <= i + 1; i2++)
                    {
                        for (int j2 = j - 1; j2 <= j + 1; j2++)
                        {
                            if (i2 >= 0 && j2 >= 0 && i2 < SCREEN_HEIGHT && j2 < SCREEN_WIDTH)
                            {
                                buffer[i2][j2] = RGBBIT(0, 255, 0, 0);
                            }
                        }
                    }
                }
                if (info[i][j] < 20000 && info[i][j]>10000)
                {
                    for (int i2 = i - 1; i2 <= i + 1; i2++)
                    {
                        for (int j2 = j - 1; j2 <= j + 1; j2++)
                        {
                            if (i2 >= 0 && j2 >= 0 && i2 < SCREEN_HEIGHT && j2 < SCREEN_WIDTH)
                            {
                                buffer[i2][j2] = RGBBIT(0, 0, 0, 255);
                            }
                        }
                    }
                }
            }
        }
    }
}
void Core::Load()
{
    FILE* input;
    if (_access("input.txt", 0) == -1)
    {
        MessageBox(main_window_handle, "Please put your input.txt here to give us information.", "Attention", MB_OK);
        return;
    }
    input = fopen("input.txt", "r");
    while (!feof(input))
    {
        char type[20];
        fscanf(input,"%s", type);
        if (strlen(type) == 0) continue;
        if (strcmp(_strlwr(type), "charge") == 0)
        {
            int x, y;
            int q;
            Elem tmp;
            fscanf(input,"%d %d %d", &x, &y, &q);
            tmp.x = x;
            tmp.y = y;
            tmp.q = q;
            info[y][x] = q;
            elem_out.push_back(tmp);
        }
        else if (strcmp(_strlwr(type), "circle") == 0)
        {
            int x, y, r, q;
            Elem tmp;
            fscanf(input, "%d %d %d", &x, &y, &r);
            circles.push_back({ x,y,r });
            for (int i = y - r - 1; i <= y + r + 1; i++)
            {
                for (int j = x - r - 1; j <= x + r + 1; j++)
                {
                    if (i<0||j < 0 || i >= SCREEN_HEIGHT || j >= SCREEN_WIDTH)
                    {
                        continue;
                    }
                    if(sqrt((i - y) * (i - y) + (j - x) * (j - x)) >= r && (sqrt((i - y + 1) * (i - y + 1) + (j - x) * (j - x))) < r || (sqrt((i - y - 1) * (i - y - 1) + (j - x) * (j - x))) < r || (sqrt((i - y) * (i - y) + (j - x + 1) * (j - x + 1))) < r || (sqrt((i - y) * (i - y) + (j - x - 1) * (j - x - 1))< r)) info[i][j]=10000;
                    if (sqrt((i - y) * (i - y) + (j - x) * (j - x)) < r) info[i][j] = -10000;
                }
            }
        }
    }
    if (circles.size() > 1) MessageBox(main_window_handle, "You can't involve more than 1 circles in your graph!", "Warning", MB_OK);
    if (circles.size() == 1)
    {
        double a = circles[0].r;
        for (int i = 0; i < elem_out.size(); i++)
        {
            double d = sqrt((circles[0].x - elem_out[i].x) * (circles[0].x - elem_out[i].x) + (circles[0].y - elem_out[i].y) * (circles[0].y - elem_out[i].y));
            double lambda = a * a / d / ( d - a * a / d );
            Elem tmp;
            tmp.x = (circles[0].x + lambda * elem_out[i].x) / (1 + lambda);
            tmp.y= (circles[0].y + lambda * elem_out[i].y) / (1 + lambda);
            tmp.q = -elem_out[i].q * a / d;
            info[tmp.y][tmp.x] = 20000 + tmp.q;
            elem_in.push_back(tmp);
            tmp.x = circles[0].x;
            tmp.y = circles[0].y;
            tmp.q = a / d * elem_out[i].q;
            info[tmp.y][tmp.x] = 20000 + tmp.q;
            elem_in.push_back(tmp);
        }
    }
}
void Core::ProcessButtonMsg()
{
    button[IFLUSH].Check();
    if (button[IFLUSH].m_state == BSTATEUP)
    {
        Flush();
        button[IFLUSH].m_state = BSTATENORMAL;
    }
    return;
}

void Core::ShowInterface()
{
    Draw_Array(buffer[0], lpddsback);
    button[IFLUSH].Draw();
    return;
}

void Core::DrawField()
{
    POINT start[12] = { {1,4},{3,3},{4,1},{-1,4},{-3,3},{-4,1},{1,-4},{3,-3},{4,-1},{-1,-4},{-3,-3},{-4,-1} };
    for (int i = 0; i < elem_out.size(); i++)
    {
        for (int j = 0; j < 12; j++)
        {
            DrawFromStart(elem_out[i].x + start[j].x, elem_out[i].y + start[j].y,elem_out[i].q>0?1:-1);
        }
    }
    for (int i = 0; i < elem_in.size(); i++)
    {
        for (int j = 0; j < 12; j++)
        {
            DrawFromStart(elem_in[i].x + start[j].x, elem_in[i].y + start[j].y, elem_in[i].q > 0 ? 1 : -1);
        }
    }/*
    for (int i = 0; i < circles.size(); i++)
    {
        for (int j = 0; j < 12; j++)
        {
            double offset_x = circles[i].r * cos(2 * j * 3.1415926 / 12);
            double offset_y = circles[i].r * sin(2 * j * 3.1415926 / 12);
            DrawFromStart(ceil(circles[i].x+offset_x), ceil(circles[i].y + offset_y), 1);
            DrawFromStart(ceil(circles[i].x + offset_x), ceil(circles[i].y + offset_y), -1);
        }
    }*/
}