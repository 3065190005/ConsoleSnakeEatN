#pragma once
#define P1h 'A'
#define P2h 'B'
#define P1b 'l'
#define P2b 'c'
#define Fot 'o'
#define PtP 'd'

#define P1W 'W'
#define P1S 'S'
#define P1A 'A'
#define P1D 'D'

#define P2W 'I'
#define P2S 'K'
#define P2A 'J'
#define P2D 'L'

#define GameRectX 75
#define GameRectY 40

#define GameFrame 60

#define clearScreen() cls(hOutput);
#define keyDown(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)