#include <iostream>
#include <list>
#include <random>
#include <chrono>
#include <windows.h>
#include "define.h"

HANDLE hOutput;
HANDLE hOutBuf;
COORD coord = { 0,0 };

std::chrono::minutes randtime{ 3 };
std::chrono::steady_clock::time_point gamebegin;

const int gameFrame = GameFrame;

const int worldWidth = GameRectX;
const int worldHeight = GameRectY;

const char gameFlags[] = { P1h,P2h,P1b,P2h,PtP,Fot};
const char gameWallFlags[] = { ' ' ,'-','-','|' };

const char player1Head = P1h;
const char player2Head = P2h;

const char player1BodyFlag = P1b;
const char player2BodyFlag = P2h;

const char footFlag = Fot;

typedef struct tagPos {
	tagPos(int x, int y)
	{
		m_posX = x; m_posY = y;
	}
	tagPos()
	{
		m_posX = 0; m_posY = 0;
	};
	int m_posX;
	int m_posY;
} Pos;

typedef struct tagSnakeBody {
	int parent;
	Pos m_pos;
	char m_flag;
}SnakeBody;

Pos player1Motion;
Pos player2Motion;

Pos player1Pos;
Pos player2Pos;

Pos footPos;

int player1Score = 0;
int player2Score = 0;

std::list<SnakeBody> player1Body;
std::list<SnakeBody> player2Body;

void initPlayer1() {
	player1Body.clear();
	player1Pos.m_posY = 1;
	player1Pos.m_posX = 4;
	player1Score = 0;
	player1Motion = Pos(0, 0);
}

void initPlayer2() {
	player2Body.clear();
	player2Pos.m_posY = 2;
	player2Pos.m_posX = worldWidth-4;
	player2Score = 0;
	player2Motion = Pos(0, 0);
}

void initGame() {
	initPlayer1();
	initPlayer2();
	footPos = Pos(0, 0);
	gamebegin = std::chrono::steady_clock::now();
}

void updateEvent() {
	if (player1Motion.m_posX == 0 && player1Motion.m_posY == 0) { player1Motion.m_posX = 1; }
	if (player2Motion.m_posX == 0 && player2Motion.m_posY == 0) { player2Motion.m_posX = -1; }

	if (keyDown(P1W)) { 
		player1Motion = Pos(0, 0); player1Motion.m_posY = 1; }
	else if (keyDown(P1S)) {
		player1Motion = Pos(0, 0); player1Motion.m_posY = -1; }
	else if (keyDown(P1D)) { 
		player1Motion = Pos(0, 0); player1Motion.m_posX = 1; }
	else if (keyDown(P1A)) { 
		player1Motion = Pos(0, 0); player1Motion.m_posX = -1; }

	if (keyDown(P2W)) {
		player2Motion = Pos(0, 0); player2Motion.m_posY = 1; }
	else if (keyDown(P2S)) { 
		player2Motion = Pos(0, 0); player2Motion.m_posY = -1; }
	else if (keyDown(P2D)) { 
		player2Motion = Pos(0, 0); player2Motion.m_posX = 1; }
	else if (keyDown(P2A)) {
		player2Motion = Pos(0, 0); player2Motion.m_posX = -1; }

	if (player1Motion.m_posX == 1) { 
		player1Pos.m_posX++; }
	if (player1Motion.m_posX == -1) {
		player1Pos.m_posX--; }
	if (player1Motion.m_posY == 1) { 
		player1Pos.m_posY++; }
	if (player1Motion.m_posY == -1) {
		player1Pos.m_posY--; }

	if (player2Motion.m_posX == 1) { 
		player2Pos.m_posX++; }
	if (player2Motion.m_posX == -1) { 
		player2Pos.m_posX--; }
	if (player2Motion.m_posY == 1) { 
		player2Pos.m_posY++; }
	if (player2Motion.m_posY == -1) { 
		player2Pos.m_posY--; }
}

bool testToHitWall(Pos pos) {
	if (pos.m_posX == 0 || pos.m_posX == worldWidth - 1) { return true; }
	if (pos.m_posY == 0 || pos.m_posY == worldHeight - 1) { return true; }
	return false;
}

bool testForFoot(Pos pos) {
	if (pos.m_posX == footPos.m_posX && pos.m_posY == footPos.m_posY) {
		return true;
	}

	return false;
}

int testForPlayer() {
	if (player1Pos.m_posX == player2Pos.m_posX &&
		player1Pos.m_posY == player2Pos.m_posY) {
		return 0;
	}

	for (auto & i : player2Body) {
		if (i.m_pos.m_posX == player1Pos.m_posX && i.m_pos.m_posY == player1Pos.m_posY) {
			return 1;
		}
	}

	for (auto & i : player1Body) {
		if (i.m_pos.m_posX == player2Pos.m_posX && i.m_pos.m_posY == player2Pos.m_posY) {
			return 2;
		}
	}

	return -1;
}

void addPlayer1Body() {
	SnakeBody body;
	auto now = player1Body.rbegin();
	if (now != player1Body.rend()) {
		body.m_pos = now->m_pos;
	}
	else {
		body.m_pos = player1Pos;
	}
	body.parent = 1;
	body.m_flag = player1BodyFlag;
	player1Body.push_back(body);
	player1Score++;
}

void addPlayer2Body() {
	SnakeBody body;
	auto now = player2Body.rbegin();
	if (now != player2Body.rend()) {
		body.m_pos = now->m_pos;
	}
	else {
		body.m_pos = player2Pos;
	}
	body.parent = 2;
	body.m_flag = player2BodyFlag;
	player2Body.push_back(body);
	player2Score++;
}

void setNewFoot() {
	std::default_random_engine rand;
	std::uniform_int_distribution<unsigned int> randomX(1, worldWidth - 1);
	std::uniform_int_distribution<unsigned int> randomY(1, worldHeight - 1);
	if (footPos.m_posX != 0 || footPos.m_posY != 0) { return; }
	footPos.m_posX = randomX(rand);
	footPos.m_posY = randomX(rand);
}

void updateGame() {
	setNewFoot();

	if (testToHitWall(player1Pos)) { 
		initPlayer1(); }
	if (testToHitWall(player2Pos)) { 
		initPlayer2(); }

	switch (testForPlayer()) {
	case 0:
		initPlayer1();
		initPlayer2();
		break;
	case 1:
		initPlayer1();
		break;
	case 2:
		initPlayer2();
		break;
	default:
		break;
	}

	if (testForFoot(player1Pos)) { addPlayer1Body(); footPos = Pos(0, 0); }
	if (testForFoot(player2Pos)) { addPlayer2Body(); footPos = Pos(0, 0); }
}

bool timeOver() {
	std::chrono::duration<double> duration_cost = std::chrono::duration_cast<
		std::chrono::duration<double>>((std::chrono::steady_clock::now() - gamebegin) - randtime);

	if (duration_cost.count() >= 0) { return true; }
	return false;
}

void initPrintBuffer() {
	hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	hOutBuf = CreateConsoleScreenBuffer(
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
	);

	SetConsoleActiveScreenBuffer(hOutBuf);
	CONSOLE_CURSOR_INFO cci;
	cci.bVisible = 0;
	cci.dwSize = 1;
	SetConsoleCursorInfo(hOutput, &cci);
	SetConsoleCursorInfo(hOutBuf, &cci);
}

void cls(HANDLE hConsole)
{
	COORD coordScreen = { 0, 0 };    // home for the cursor 
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
	if (!FillConsoleOutputCharacter(hConsole, (TCHAR)' ', dwConSize, coordScreen, &cCharsWritten)) return;
	SetConsoleCursorPosition(hConsole, coordScreen);
}

void printGameData() {
	int ref = 0;
	int count = 0;
	DWORD bytes;
	char mapBuffer[worldHeight][worldWidth] = { 0 };
	const int totalLen = ((worldHeight * worldWidth) * 4) + (worldHeight);
	char data[totalLen];
	clearScreen();
	for (auto y = 0; y < worldHeight; y++) {
		for (auto x = 0; x < worldWidth; x++) {
			char gamePrint;
			if (x == 0 || x == worldWidth - 1) { gamePrint = gameWallFlags[3]; }
			else if (y == 0) { gamePrint = gameWallFlags[2]; }
			else if (y == worldHeight - 1) { gamePrint = gameWallFlags[1]; }
			else { gamePrint = gameWallFlags[0]; }

			if (x == footPos.m_posX && y == footPos.m_posY)
			{ gamePrint = footFlag; }

			if (x == player1Pos.m_posX && y == player1Pos.m_posY)
			{ gamePrint = player1Head; }

			if (x == player2Pos.m_posX && y == player2Pos.m_posY)
			{ gamePrint = player2Head; }

			ref = 0;

			for (auto &i : player1Body) {
				if (i.m_pos.m_posX == x && i.m_pos.m_posY == y) { ref |= 0b01; }
				break;
			}

			for (auto &i : player2Body) {
				if (i.m_pos.m_posX == x && i.m_pos.m_posY == y) { ref++; ref |= 0b10; }
				break;
			}
			if (ref == 0b11) { gamePrint = gameFlags[4]; }
			else if (ref == 0xb01) { gamePrint = player1BodyFlag; }
			else if (ref == 0xb10) { gamePrint = player2BodyFlag; }
			mapBuffer[y][x] = gamePrint;
			std::cout << gamePrint;
			count++;
		}
		std::cout << std::endl;
		count++;
	}

	ReadConsoleOutputCharacterA(hOutput, data, totalLen, coord, &bytes);
	WriteConsoleOutputCharacterA(hOutBuf, data, totalLen, coord, &bytes);
}

void movePrintBuffer() {
	clearScreen();
	const int totalLen = 256;
	char data[totalLen];
	DWORD bytes;
	ReadConsoleOutputCharacterA(hOutput, data, totalLen, coord, &bytes);
	WriteConsoleOutputCharacterA(hOutBuf, data, totalLen, coord, &bytes);
}

void gameSuccess() {
	if (player1Score <= player2Score) { return; }
	clearScreen();
	std::cout << "u win" << std::endl;
	movePrintBuffer();
}

void gameFaild() {
	if (player1Score > player2Score) { return; }
	clearScreen();
	std::cout << "u lose" << std::endl;
	movePrintBuffer();
}

void endPrint() {
	std::cout << "type any key code to restart the game" << std::endl;
	movePrintBuffer();
	getchar();
}

int getGameFrame(int frame) {
	return 1 / frame;
}

int main() {
	initPrintBuffer();
	while (true) {
		initGame();
		while (true) {
			updateEvent();
			updateGame();
			printGameData();
			if (timeOver()) {
				break;
			}
			::Sleep(getGameFrame(gameFrame));
		}
		gameSuccess();
		gameFaild();
		endPrint();
	}
	return 0;
}