#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

using namespace std;

const int pixelHeight = 1;
const int pixelWidth = 1;

const int mapOffsetH = 12;
const int mapOffsetW = 24;

const int screenHeight = 1800;
const int screenWidth = 900;

const int gradientLenght = 100;

int permutation[256];

float terrain[screenHeight][screenWidth];

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
HWND consoleWindow = GetConsoleWindow();
HDC consoleDC = GetDC(consoleWindow);

void ShowConsoleCursor(bool showFlag)
{
    CONSOLE_CURSOR_INFO     cursorInfo;

    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = showFlag;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void clear_screen()
{
    DWORD n;
    DWORD size;
    COORD coord = {0};
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(hConsole, &csbi);

    size = csbi.dwSize.X * csbi.dwSize.Y;

    FillConsoleOutputCharacter(hConsole, TEXT(' '), size, coord, &n);
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, size, coord, &n);

    SetConsoleCursorPosition(hConsole, coord);
}

class vector2
{
public:
    float x = 0;
    float y = 0;

    void create(float x1, float y1)
    {
        x = x1;
        y = y1;
    }

    float dot(vector2 other) { return x * other.x + y * other.y; }
};

class cursorInfo
{
public:
    vector2 pos = {mapOffsetH, mapOffsetW};
    static const int sizeX = 3;
    static const int sizeY = 3;

    float replaced[sizeX][sizeY];
    vector2 replacedCoord = {0, 0};

    COLORREF cursorColor = RGB(255, 0, 0);
} cursor;

struct vector3
{
    int x = 0;
    int y = 0;
    int z = 0;
} gradient[gradientLenght];

struct gradientChangePoint
{
    vector3 rgb;
    int percentage = 0;
};

void makePermutation()
{
    for (int i = 0; i < 256; i++)
        permutation[i] = i;
    for (int i = 255; i > 1; i--)
    {
        int index = rand() % (i - 1);
        swap(permutation[i], permutation[index]);
    }
}

vector2 GetConstantVector(int v)
{
    int h = v & 3;
    vector2 returnVector;
    if (h == 0)
    {
        returnVector.x = 1;
        returnVector.y = 1;
    }
    else if (h == 1)
    {
        returnVector.x = -1;
        returnVector.y = 1;
    }
    else if (h == 2)
    {
        returnVector.x = -1;
        returnVector.y = -1;
    }
    else
    {
        returnVector.x = 1;
        returnVector.y = -1;
    }
    return returnVector;
}

float fade(float x) { return ((6 * x - 15) * x + 10) * x * x * x; }

float lerp(float x, float a, float b) { return a + x * (b - a); }

float noise(float x, float y)
{
    int X = int(x) & 255;
    int Y = int(y) & 255;

    float xf = x - float(int(x));
    float yf = y - float(int(y));

    vector2 topRight, topLeft, bottomRight, bottomLeft;

    topRight.create(xf - 1, yf - 1);
    topLeft.create(xf, yf - 1);
    bottomRight.create(xf - 1, yf);
    bottomLeft.create(xf, yf);

    //printf("vectors done\n");

    int valueTopRight, valueTopLeft, valueBottomRight, valueBottomLeft;

    valueTopRight = permutation[permutation[X + 1] + Y + 1];
    valueTopLeft = permutation[permutation[X] + Y + 1];
    valueBottomRight = permutation[permutation[X + 1] + Y];
    valueBottomLeft = permutation[permutation[X] + Y];

    //printf("permutations done\n");

    float dotTopRight, dotTopLeft, dotBottomRight, dotBottomLeft;

    dotTopRight = topRight.dot(GetConstantVector(valueTopRight));
    dotTopLeft = topLeft.dot(GetConstantVector(valueTopLeft));
    dotBottomRight = bottomRight.dot(GetConstantVector(valueBottomRight));
    dotBottomLeft = bottomLeft.dot(GetConstantVector(valueBottomLeft));

    //printf("dot products done\n");

    float u = fade(xf);
    float v = fade(yf);

    //printf("fades done\n");

    return lerp(u, lerp(v, dotBottomLeft, dotTopLeft), lerp(v, dotBottomRight, dotTopRight));
}

float fractalBrownianMotion(int x, int y, int numOctaves)
{
    //printf("start ");
    float result = 0.0f;
    float amplitude = 1.0f;
    float frequency = 0.005f;

    for (int i = 0; i < numOctaves; i++)
    {
        float n = amplitude * noise(float(x) * frequency, float(y) * frequency);
        //printf("noise %i \n", i);
        result += n;

        amplitude *= 0.5;
        frequency *= 2.0;
    }

    return result;
}

void makeTerrain()
{
    int octaves = 13;

    makePermutation();

    for (int i = 0; i < screenHeight; i++)
        for (int j = 0; j < screenWidth; j++)
            terrain[i][j] = fractalBrownianMotion(i, j, octaves);
}

COLORREF colour(float height)
{
    height = (height + 1.0f) / 2.0f;
    COLORREF result;

    result = RGB(gradient[int(height * gradientLenght) % 100].x, gradient[int(height * gradientLenght) % 100].y,
                 gradient[int(height * gradientLenght) % 100].z);
    if (height < 0.0f)
        result = RGB(66, 130, 244);
    if (height > 1.0f)
        result = RGB(255, 255, 255);

    return result;
}

void initGradient()
{
    const int BreakPointNo = 10;

    gradientChangePoint points[BreakPointNo];

    points[0].rgb.x = 255;
    points[0].rgb.y = 255;
    points[0].rgb.z = 150;
    points[0].percentage = 0;

    points[1].rgb.x = 213;
    points[1].rgb.y = 255;
    points[1].rgb.z = 128;
    points[1].percentage = 5;

    points[2].rgb.x = 0;
    points[2].rgb.y = 175;
    points[2].rgb.z = 0;
    points[2].percentage = 10;

    //points[3].rgb.x = 48;
    //points[3].rgb.y = 92;
    //points[3].rgb.z = 4;
    //points[3].percentage = 30;

    //points[4].rgb.x = 50;
    //points[4].rgb.y = 111;
    //points[4].rgb.z = 24;
    //points[4].percentage = 50;

    points[3].rgb.x = 56;
    points[3].rgb.y = 89;
    points[3].rgb.z = 48;
    points[3].percentage = 58;

    points[4].rgb.x = 50;
    points[4].rgb.y = 87;
    points[4].rgb.z = 6;
    points[4].percentage = 60;
    
    points[5].rgb.x = 34;
    points[5].rgb.y = 52;
    points[5].rgb.z = 26;
    points[5].percentage = 70;

    points[6].rgb.x = 112;
    points[6].rgb.y = 88;
    points[6].rgb.z = 27;
    points[6].percentage = 75;
    
    points[7].rgb.x = 102;
    points[7].rgb.y = 63;
    points[7].rgb.z = 0;
    points[7].percentage = 86;

    points[8].rgb.x = 126;
    points[8].rgb.y = 126;
    points[8].rgb.z = 126;
    points[8].percentage = 90;

    points[9].rgb.x = 238;
    points[9].rgb.y = 238;
    points[9].rgb.z = 238;
    points[9].percentage = 100;

    for (int i = 0; i < BreakPointNo - 1; i++)
    {
        float deltaX = float(points[i + 1].rgb.x - points[i].rgb.x) / float(
            points[i + 1].percentage - points[i].percentage);
        float deltaY = float(points[i + 1].rgb.y - points[i].rgb.y) / float(
            points[i + 1].percentage - points[i].percentage);
        float deltaZ = float(points[i + 1].rgb.z - points[i].rgb.z) / float(
            points[i + 1].percentage - points[i].percentage);
        for (int j = points[i].percentage; j < points[i + 1].percentage; j++)
        {
            gradient[j].x = int(float(points[i].rgb.x) + deltaX * float(j - points[i].percentage));
            gradient[j].y = int(float(points[i].rgb.y) + deltaY * float(j - points[i].percentage));
            gradient[j].z = int(float(points[i].rgb.z) + deltaZ * float(j - points[i].percentage));
        }
    }
}

void initCursor()
{
    cursor.replacedCoord = cursor.pos;
    for (int i = 0; i < cursor.sizeX; i++)
        for (int j = 0; j < cursor.sizeY; j++)
        {
            cursor.replaced[i][j] = terrain[int(cursor.pos.x + i)][int(cursor.pos.y + j)];
        }
    for (int i = 0; i < cursor.sizeX; i++)
        for (int j = 0; j < cursor.sizeY; j++)
            SetPixel(consoleDC, (cursor.pos.x + mapOffsetH) + i, (cursor.pos.y + mapOffsetW) + j, cursor.cursorColor);
}

void moveCursor(int x, int y)
{
    for (int i = 0; i < cursor.sizeX; i++)
        for (int j = 0; j < cursor.sizeY; j++)
        {
            COLORREF color = colour(cursor.replaced[i][j]);
            SetPixel(consoleDC, (cursor.replacedCoord.x + mapOffsetH) + i, (cursor.replacedCoord.y + mapOffsetW) + j,
                     color);
        }
    cursor.pos.x = x;
    cursor.pos.y = y;

    initCursor();
}

int main()
{
    ShowConsoleCursor(false);
    
    srand(time(NULL));

    makeTerrain();

    initGradient();

    clear_screen();

    for (int i = 0; i < screenHeight; i ++)
        for (int j = 0; j < screenWidth; j++)
        {
            COLORREF color = colour(terrain[i][j]);
            for (int n = 0; n < pixelHeight; n++)
                for (int m = 0; m < pixelWidth; m++)
                    SetPixel(consoleDC, (i + mapOffsetH) * pixelHeight + n, (j + mapOffsetW) * pixelWidth + m,
                             color);
        }

    initCursor();

    while (true)
    {
        printf("%.0f\t", (float((terrain[int(cursor.pos.x)][int(cursor.pos.y)]) + 1.0f) / 2.0f) * 100);
        if (GetAsyncKeyState(VK_UP))
            moveCursor(cursor.pos.x, cursor.pos.y - cursor.sizeY);
        if (GetAsyncKeyState(VK_DOWN))
            moveCursor(cursor.pos.x, cursor.pos.y + cursor.sizeY);
        if (GetAsyncKeyState(VK_RIGHT))
            moveCursor(cursor.pos.x + cursor.sizeX, cursor.pos.y);
        if (GetAsyncKeyState(VK_LEFT))
            moveCursor(cursor.pos.x - cursor.sizeX, cursor.pos.y);
        Sleep(25);
        printf("\b\b\b\b\b\b\b\b\b");
    }
    return 0;
}
