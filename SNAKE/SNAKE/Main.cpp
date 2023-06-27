#include <iostream>
#include <vector>
#include <random>
#include <string>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include "resource.h"

using namespace std;

//-------------------------------------------------------------------------------------------------------
// Constantes e Variáveis Globais

//DISPLAY
const char windowClassName[] = "GameWindowClass";
int windowWidth = 500;
int windowHeight = 500;
RECT winRect                  = { 0, 0, windowWidth, windowHeight };
HWND WindowHandler;
HDC hdc;

PAINTSTRUCT ps;

//INPUT
bool keyPressed[256]          = { 0 };


//-------------------------------------------------------------------------------------------------------
// Game Functions

class SnakePart {
public:
    int X;
    int Y;
    int OLDX;
    int OLDY;
};

class Point {
public:
    int X;
    int Y;
};

enum class direction { UP, DOWN, LEFT, RIGHT};
direction dir;
vector<SnakePart> Snake;

int y = 240;
int x = 240;
int SizeY = 10;
int SizeX = 10;

HDC playerHDC;
RECT playerRect;
static HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
static HBRUSH background = CreateSolidBrush(RGB(0, 0, 0));
static HBRUSH greenBrush = CreateSolidBrush(RGB(0, 255, 0));

bool SnakeCollided;

Point Apple;
string pointCount;
int pointNumberCount = 0;

bool gameBegin;


void AddPart() {
    SnakePart tempSnakePart;
    tempSnakePart.X = Snake.back().X;
    tempSnakePart.Y = Snake.back().Y;
    Snake.push_back(tempSnakePart);
}

void ShowPoint() {
    HDC ShowScore = GetDC(WindowHandler);
    SetTextColor(ShowScore, RGB(255, 255, 255));
    SetBkColor(ShowScore, RGB(0, 0, 0));
    SetBkMode(ShowScore, TRANSPARENT);
    pointCount = "Score: " + to_string(pointNumberCount);
    TextOut(ShowScore, 240, 0, pointCount.c_str(), strlen(pointCount.c_str()));
    ReleaseDC(WindowHandler, ShowScore);
}

void OnPointCollision() {
    AddPart();
    AddPart();
    AddPart();

    int TempX = (rand() % 50) * 10;
    int TempY = (rand() % 50) * 10;
    Apple.X = TempX;
    Apple.Y = TempY;

    pointNumberCount++;
    //MessageBeep(0xFFFFFFFF);
}

void GameInit() {
    hdc = GetDC(WindowHandler);
    playerHDC = GetDC(WindowHandler);
    playerRect = { 0, 0, 20, 20 };
    y = 240;
    x = 240;
    
    SnakePart tempSnakePart;
    tempSnakePart.X = 0;
    tempSnakePart.Y = 0;
    Snake.push_back(tempSnakePart);
    AddPart();
    AddPart();
    AddPart();

    SnakeCollided = false;

    int RandomX  = (rand() % 50) * 10;
    int RandomY  = (rand() % 50) * 10;
    Apple.X = RandomX;
    Apple.Y = RandomY;

    SnakeCollided = false;
    pointNumberCount = 0;
    gameBegin = false;
}


void GameUpdate() {
    if (keyPressed[68] && dir != direction::LEFT) {
        dir = direction::RIGHT;
    }
    else if (keyPressed[65] && dir != direction::RIGHT) {
        dir = direction::LEFT;
    }
    else if (keyPressed[87] && dir != direction::DOWN) {
        dir = direction::UP;
    }
    else if (keyPressed[83] && dir != direction::UP) {
        dir = direction::DOWN;
    }

    switch (dir)
    {
    case direction::RIGHT:
        x += 10;
        break;
    case direction::LEFT:
        x -= 10;
        break;
    case direction::UP:
        y -= 10;
        break;
    case direction::DOWN:
        y += 10;
        break;
    default:
        break;
    }
}

void GameDraw() {
    if (x > 490)
    {
        x = 0;
    }
    if (x < 0)
    {
        x = 490;
    }
    if (y > 490)
    {
        y = 0;
    }
    if (y < 0)
    {
        y = 490;
    }

    FillRect(hdc, &winRect, background);

    Snake[0].X = x;
    Snake[0].Y = y;

    playerRect = { x, y, (x + SizeX), (y + SizeY) };
    FillRect(playerHDC, &playerRect, greenBrush);

    for (int i = 1; i < Snake.size(); i++) {                                        // Checks if snake's head is colliding with body or a point by checking X and Y positions
        if ((Snake[0].X == Snake[i].OLDX) && (Snake[0].Y == Snake[i].OLDY))
        {
            SnakeCollided = true;
        }
        if ((Snake[0].X == Apple.X) && (Snake[0].Y == Apple.Y))
        {
            OnPointCollision();
        }
    }

    for(int i = 1; i < Snake.size(); i++) {
        RECT tempRect;

        Snake[i].X = Snake[i - 1].OLDX;
        Snake[i].Y = Snake[i - 1].OLDY;
       
        tempRect = { Snake[i].X, Snake[i].Y, (Snake[i].X + SizeX), (Snake[i].Y + SizeY) };

        FillRect(playerHDC, &tempRect, greenBrush);
    }

    for(int i = 0; i < Snake.size(); i++){
        Snake[i].OLDX = Snake[i].X;
        Snake[i].OLDY = Snake[i].Y;
    }

    // Points Render
    RECT pointRect = { Apple.X, Apple.Y, (Apple.X + SizeX), (Apple.Y + SizeY) };
    FillRect(playerHDC, &pointRect, redBrush);
    ShowPoint();
}

void Restart() {
    Snake.erase(Snake.begin(), Snake.end());
    GameInit();
}

void GameOver(){
    //GetDC(WindowHandler);
    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkColor(hdc, RGB(0, 0, 0));
    SetBkMode(hdc, OPAQUE);
    TextOut(hdc, 210, 240, "Gameover", strlen("Gameover"));
    pointCount = "Score: " + to_string(pointNumberCount);
    TextOut(hdc, 220, 260, pointCount.c_str(), strlen(pointCount.c_str()));
    //TextOut(hdc, 130, 280, "Pressione a tecla 'R' para Reiniciar", strlen("Pressione a tecla 'R' para Reiniciar"));
    TextOut(hdc, 180, 280, "Press 'R' to Restart", strlen("Press 'R' to Restart"));
    //ReleaseDC(WindowHandler, hdc);
    if (keyPressed[82]) {   // R key in ASCII
        Restart();
    }
}


void Menu()
{
    //GetDC(WindowHandler);
    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkColor(hdc, RGB(0, 0, 0));
    SetBkMode(hdc, OPAQUE);
    TextOut(hdc, 210, 240, "Snake Game", strlen("Snake Game"));
    //TextOut(hdc, 100, 280, "Pressione as teclas 'W' 'A' 'S' ou 'D' para Iniciar", strlen("Pressione as teclas 'W' 'A' 'S' ou 'D' para Iniciar"));
    TextOut(hdc, 140, 280, "Press keys 'W' 'A' 'S' or 'D' to Start", strlen("Press keys 'W' 'A' 'S' or 'D' to Start"));
    
    if (keyPressed[68] && dir != direction::LEFT) {
        dir = direction::RIGHT;
        gameBegin = true;
    }
    else if (keyPressed[65] && dir != direction::RIGHT) {
        dir = direction::LEFT;
        gameBegin = true;
    }
    else if (keyPressed[87] && dir != direction::DOWN) {
        dir = direction::UP;
        gameBegin = true;
    }
    else if (keyPressed[83] && dir != direction::UP) {
        dir = direction::DOWN;
        gameBegin = true;
    }
}

void GameFinalize() {
    ReleaseDC(WindowHandler, hdc);
    ReleaseDC(WindowHandler, playerHDC);
}

//-------------------------------------------------------------------------------------------------------
// Windows
LRESULT CALLBACK WndProc(HWND WindowHandler, UINT Message, WPARAM WParam, LPARAM LParam)
{
    switch (Message) {
    case WM_KEYDOWN:
        keyPressed[WParam] = true;
        InvalidateRect(WindowHandler, NULL, TRUE);
        break;
    case WM_KEYUP:
        keyPressed[WParam] = false;
        InvalidateRect(WindowHandler, NULL, TRUE);
        break;
    case WM_CLOSE:
        DestroyWindow(WindowHandler);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(WindowHandler, Message, WParam, LParam);
}

//-------------------------------------------------------------------------------------------------------
// Windows

LRESULT CALLBACK MainWndProc(HWND WindowHandler,UINT Message,WPARAM WParam,LPARAM LParam){
    LRESULT Result = 0;
    switch (Message)
    {
        default:
            DefWindowProc(WindowHandler, Message, WParam, LParam);
    }
    return Result;
}

//-------------------------------------------------------------------------------------------------------
// Windows 

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR pCmdLine, _In_ int nCmdShow){
    WNDCLASSEX WindowClass;
    MSG msg = { };

    WindowClass.cbSize = sizeof(WNDCLASSEX);
    WindowClass.style = CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = WndProc;
    WindowClass.cbClsExtra = 0;
    WindowClass.cbWndExtra = 0;
    WindowClass.hInstance = hInstance;
    WindowClass.hIcon =   LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
    WindowClass.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR));
    WindowClass.hbrBackground = background;
    WindowClass.lpszMenuName = NULL;
    WindowClass.lpszClassName = windowClassName;
    WindowClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&WindowClass)){
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    
    WindowHandler = CreateWindowEx(
                        WS_EX_CLIENTEDGE,
                        windowClassName,
                        "Snake Game",
                        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                        CW_USEDEFAULT, CW_USEDEFAULT, 500, 500,
                        NULL, NULL, hInstance, NULL);

    if (WindowHandler == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error!",MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    AdjustWindowRectEx(&winRect, GetWindowStyle(WindowHandler), GetMenu(WindowHandler) != NULL, GetWindowExStyle(WindowHandler));

    int posX = GetSystemMetrics(SM_CXSCREEN) / 2 - (winRect.right - winRect.left) / 2;
    int posY = GetSystemMetrics(SM_CYSCREEN) / 2 - (winRect.bottom - winRect.top) / 2;

    MoveWindow(WindowHandler, posX, posY, winRect.right - winRect.left, winRect.bottom - winRect.top, TRUE);

    UpdateWindow(WindowHandler);
    GameInit();

    //-------------------------------------------------------------------------------------------------------
    // Game Loop
    do {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            if(!gameBegin) {
                Menu();
                Sleep(33);
            }
            else if (!SnakeCollided && gameBegin) {
                GameUpdate();
                GameDraw();
                Sleep(33);
            }
            else {
                GameOver();
                Sleep(33);
            }
        }
    }
    while (msg.message != WM_QUIT);

    GameFinalize();

    return msg.wParam;

    //-------------------------------------------------------------------------------------------------------
}

