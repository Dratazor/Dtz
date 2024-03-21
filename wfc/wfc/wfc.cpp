#include <iostream>
#include <vector>
#include <Windows.h>
#include <ctime>
#include <fstream>

#include "dpc.h"

std::ofstream file;

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
HWND consoleWindow = GetConsoleWindow();
HDC consoleDC = GetDC(consoleWindow);

void ShowConsoleCursor(bool showFlag)
{
    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = showFlag;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

static constexpr int screenH = 20;
static constexpr int screenW = 50;
static constexpr int adjacencyCount = 4;

bool zeroEntropy = false;

int characters[charNr] = {
    179, 180, 195, 197, 192, 217, 191, 218, 193, 194, 196, 32
};

int adjacency[charNr][adjacencyCount] = {
    //do this procedurally  {sus, stanga, jos, dreapta}
    {1, 0, 1, 0}, {1, 1, 1, 0}, {1, 0, 1, 1}, {1, 1, 1, 1},
    {1, 0, 0, 1}, {1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 1},
    {1, 1, 0, 1}, {0, 1, 1, 1}, {0, 1, 0, 1}, {0, 0, 0, 0},
};

cell collapse[screenH][screenW];
int screen[screenH][screenW];

void sendToFile()
{
    file.open("err.txt");
    for (int i = 0; i < screenH; i++)
    {
        for (int j = 0; j < screenW; j++)
            file<<screen[i][j]<<" ";
        file<<std::endl;
    }
    file.close();
}

int collapsePoint(point collapsePoint)
{
    std::vector<int> possibleChars;

    for (int i = 0; i < charNr; i++)
        if (collapse[collapsePoint.x][collapsePoint.y].possibilities[i])
            possibleChars.push_back(characters[i]);

    unsigned long long const randomChar = rand() % possibleChars.size();
    int const collapsedCharacter = possibleChars[randomChar];
    collapse[collapsePoint.x][collapsePoint.y].collapsed = true;
    collapse[collapsePoint.x][collapsePoint.y].collapsedChar = collapsedCharacter;

    possibleChars.clear();

    //printf("\nCollapsed char:%c\n", collapsedCharacter);

    return collapsedCharacter;
}

point findLowestEntropy()
{
    std::vector<point> lowestEntropies;

    int minEntropy = charNr + 1;

    for (int i = 0; i < screenH; i++)
        for (int j = 0; j < screenW; j++)
            if (!collapse[i][j].collapsed)
            {
                if (minEntropy > collapse[i][j].calculateEntropy() && collapse[i][j].entropy > 0)
                {
                    //printf("new min entropy: %i, at point: %i, %i\n", collapse[i][j].entropy, i, j);
                    minEntropy = collapse[i][j].entropy;
                    lowestEntropies.clear();
                }
                if (minEntropy == collapse[i][j].entropy)
                    lowestEntropies.push_back({i, j});
            }


    //printf("Size of entropy vector: %i", lowestEntropies.size());

    if(lowestEntropies.size() == 0)
        {
            zeroEntropy = true;
            sendToFile();
            system("PAUSE");
            return {-1, -1};
        }
    
    point const ret = lowestEntropies[rand() % lowestEntropies.size()];
    lowestEntropies.clear();

    //printf("\nLowest entropy at:%i, %i\n", ret.x, ret.y);

    return ret;
}

void lowerEntropy(point recentlyCollapsed, int collapsedCharacter)
{
    point const neighbours[4]
    {
        recentlyCollapsed + point{-1, 0} % point{screenH, screenW}, //up
        recentlyCollapsed + point{0, -1} % point{screenH, screenW}, //right
        recentlyCollapsed + point{1, 0} % point{screenH, screenW}, //down
        recentlyCollapsed + point{0, 1} % point{screenH, screenW}, //left
    };

    int index = 0;
    while (characters[index] != collapsedCharacter)
        index++;
    for (int i = 0; i < adjacencyCount; i++)
    {
        int const connection = adjacency[index][i];
        if (!collapse[neighbours[i].x][neighbours[i].y].collapsed)
        {
            for (int j = 0; j < charNr; j++)
                if (adjacency[j][(i + adjacencyCount / 2) % adjacencyCount] != connection)
                    collapse[neighbours[i].x][neighbours[i].y].possibilities[j] = false;
        }
    }
}

bool done()
{
    for (int i = 0; i < screenH; i++)
        for (int j = 0; j < screenW; j++)
            if (!collapse[i][j].collapsed)
                return false;
    return true;
}

void copy()
{
    for (int i = 0; i < screenH; i++)
        for (int j = 0; j < screenW; j++)
            if (collapse[i][j].collapsedChar != 33)
                screen[i][j] = collapse[i][j].collapsedChar;
            else screen[i][j] = 32;
}

void display()
{
    for (int i = 0; i < screenH; i++)
    {
        for (int j = 0; j < screenW; j++)
            printf("%c", screen[i][j]);
        printf("\n");
    }
}

void reset()
{
    for (int i = 0; i < screenH; i++)
        for (int j = 0; j < screenW; j++)
            collapse[i][j].reset();
}

int main()
{
    system("color 02");
    ShowConsoleCursor(false);
    srand(static_cast<unsigned int>(time(nullptr)));

    bool keepGoing = true;

    while (keepGoing)
    {
        if (GetAsyncKeyState(VK_SPACE))
        {
            system("cls");
            reset();

            

            while (!done())
            {
                
                
                point const lowestEntropy = findLowestEntropy();
                int const collapsedChar = collapsePoint(lowestEntropy);

                lowerEntropy(lowestEntropy, collapsedChar);

                
            }

            copy();
            display();

            
        }
        if (GetAsyncKeyState(VK_ESCAPE))
            keepGoing = false;
        Sleep(200);
    }

    return 0;
}
