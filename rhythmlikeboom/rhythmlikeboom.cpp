#define _CRT_SECURE_NO_WARNINGS
#define BUFFSIZE 768
#define WIDTH 1280
#define HEIGHT 720


#include <iostream>
#include <fstream>
#include <cstring>
#include <windows.h>
#include <string.h>
#include <vector>
#include <string>

using namespace std;


// global variables
char* screen;
int columns, rows;
int windowSize[2];

// global utils
HWND GetConsoleHandle() {
    HWND hwnd;
    char pszWindowTitle[BUFFSIZE];

    GetConsoleTitleA(pszWindowTitle, BUFFSIZE);
    int i = strlen(pszWindowTitle);
    _itoa(GetTickCount(), &pszWindowTitle[i], 16);
    _itoa(GetCurrentProcessId(), &pszWindowTitle[strlen(pszWindowTitle)], 16);
    SetConsoleTitleA(pszWindowTitle);
    Sleep(50);
    hwnd = FindWindowA(NULL, pszWindowTitle);
    pszWindowTitle[i] = 0;
    SetConsoleTitleA(pszWindowTitle);
    return(hwnd);
}

int* getWindowBorderSize() {
    HWND consoleWindow = GetConsoleWindow();
    RECT wrect;
    GetWindowRect(consoleWindow, &wrect);
    RECT crect;
    GetClientRect(consoleWindow, &crect);
    POINT lefttop = { crect.left, crect.top };
    ClientToScreen(consoleWindow, &lefttop);
    POINT rightbottom = { crect.right, crect.bottom };
    ClientToScreen(consoleWindow, &rightbottom);

    int left_border = lefttop.x - wrect.left;
    int right_border = wrect.right - rightbottom.x;
    int bottom_border = wrect.bottom - rightbottom.y;
    int top_border_with_title_bar = lefttop.y - wrect.top;

	int borders[4] = { top_border_with_title_bar, left_border, right_border, bottom_border  };
    return borders;
}

int* getRealWindowSize() {
	int size[2];
	HWND consoleWindow = GetConsoleWindow();
	RECT rect;
    if (GetWindowRect(consoleWindow, &rect)) {
		int* borders = getWindowBorderSize();
        size[0] = WIDTH - borders[1] - borders[2];
        size[1] = HEIGHT - borders[0] - borders[3];
    }
	return size;
}

POINT getMouseClickPosition() {
    POINT p;
    if (GetCursorPos(&p)) {
        RECT rect;
        if (ScreenToClient(GetConsoleWindow(), &p)) {
            return p;
        }
    }
    p.x = -1;
    p.y = -1;
    return p;
}

int* getPosByPersentage(int x, int y) {
    int pos[2];
    pos[0] = (int)((float)columns * ((float)x / (float)100)) + 1;
    pos[1] = (int)((float)rows * ((float)y / (float)100)) + 1;
    return pos;
}

vector<string> getFile(string filename) {
    ifstream file(("./resources/text/" + filename + ".txt").c_str());
    vector<string> lines;
    string line;
    while (getline(file, line)) {
        lines.push_back(line);
    }
    file.close();

    return lines;
}

void concatVector(vector<string>& a, const vector<string>& b) {
    for (int i = 0; i < b.size(); i++) {
        a.push_back(b.at(i));
    }
}

// global actions
void rewrite() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord;
    coord.X = 0;
    coord.Y = 0;
    SetConsoleCursorPosition(hConsole, coord);
}

void clear() {
    for (int i = 0; i < columns * rows; i++)
        screen[i] = ' ';
}

void clearScreen() {
    COORD coord;
    coord.X = 0;
    coord.Y = 0;
    DWORD numOfChar;
    FillConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), ' ', 10000, coord, &numOfChar);
}

void printString(vector<string> data, int center) {
    if (center) {
        int space = (rows - data.size()) / 2;
        for (int j = 0; j < space; j++) printf("\n");
        for (int i = 0; i < data.size(); i++) {
            int pos = (columns - data.at(i).length()) / 2;
            for (int i = 0; i < pos; i++) printf(" ");
            printf("%s\n", data.at(i).c_str());
        }
    }else {
        for (int i = 0; i < data.size(); i++) {
            int pos = (columns - data.at(i).length()) / 2;
            for (int i = 0; i < pos; i++) printf(" ");
            printf("%s\n", data.at(i).c_str());
        }
    }
}

void setPixel(int* pos, char c) {
    screen[pos[1] * columns + pos[0]] = c;
}


void render() {
    rewrite();
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++)
            printf("%c", screen[i * columns + j]);
        if (i != rows-1)
            printf("\n");
    }
}

// intro actions
string drawLine(int loop, int LENGTH, int BAR_LENGTH) {
    string line;

    loop = LENGTH + BAR_LENGTH < loop ? (LENGTH + BAR_LENGTH) * 2 - loop : loop;
    loop = loop < 0 ? loop * -1 : loop;

    for (int i = 0; i < loop - BAR_LENGTH; i++) {
        if (BAR_LENGTH - (loop - LENGTH) < 0) break;
        line.append("=");
    }

    if (loop < BAR_LENGTH)
        for (int i = 0; i < loop; i++) line.append("#");
    else if (LENGTH < loop)
        for (int i = 0; i < BAR_LENGTH - (loop - LENGTH); i++) line.append("#");
    else
        for (int i = 0; i < BAR_LENGTH; i++) line.append("#");

    for (int i = 0; i < LENGTH - loop; i++) line.append("=");

    return line;
}

// animation
void introAnimation() {
    clearScreen();

    int loop = 0;
    while (true) {
        const int BAR_LENGTH = 10;
        const int LENGTH = 130;

        if (loop > (LENGTH + BAR_LENGTH) * 2) {
            loop = 0;
            clear();
        }
        if (GetKeyState(VK_RETURN) & 0x8000) {
            break;
        }

        rewrite();
        vector<string> data;
        data.push_back(drawLine(LENGTH - loop, LENGTH, BAR_LENGTH));
        concatVector(data, getFile("intro"));
        data.push_back(drawLine(loop, LENGTH, BAR_LENGTH));
        printString(data, 1);
        loop++;

        Sleep(1);
    }
}

void countdownAimation() {
    clearScreen();
    rewrite();
    printString(getFile("3"), 1);
    Sleep(1000);
    clearScreen();
    rewrite();
    printString(getFile("2"), 1);
    Sleep(1000);
    clearScreen();
    rewrite();
    printString(getFile("1"), 1);
    Sleep(1000);
}

void mainAimation() {
    clearScreen();
    while (true) {
        if (GetKeyState(VK_RETURN) & 0x8000) {
            //break;
        }

        if (GetKeyState(VK_LBUTTON) & 0x8000) {
            POINT p = getMouseClickPosition();
            if (p.x != -1 && p.y != -1) {
                int* pos = getPosByPersentage((int)(((float)p.x * (float)100) / (float)windowSize[0]), (int)(((float)p.y * (float)100) / (float)windowSize[1]));
                clear();
                setPixel(pos, '+');
                render();
            }
        }
    }
}


// initialize
void initScreen() {
    // window size
    HWND wh = GetConsoleHandle();
    MoveWindow(wh, 0, 0, WIDTH, HEIGHT, 1);
    cout << "\n";

    Sleep(100);

    // get console size
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    columns = csbi.srWindow.Right - csbi.srWindow.Left;
    rows = csbi.srWindow.Bottom - csbi.srWindow.Top;    

    // hide cursor
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);

    // disable selecting
    DWORD prev_mode;
    GetConsoleMode(consoleHandle, &prev_mode);
    SetConsoleMode(consoleHandle, ENABLE_EXTENDED_FLAGS | (prev_mode & ~ENABLE_QUICK_EDIT_MODE));

    // get window size
    windowSize[0] = getRealWindowSize()[0] - 10;
    windowSize[1] = getRealWindowSize()[1] - 10;

	// screen buffer
    screen = (char*)malloc(columns * rows * sizeof(char)); // 208 * 53

	// clear screen buffer
    clear();
	clearScreen();
}

int main() {
    initScreen();

    introAnimation();
    countdownAimation();

	mainAimation();

	//setPixel(getPosByPersentage(50, 50), '+');
 //   render();
}