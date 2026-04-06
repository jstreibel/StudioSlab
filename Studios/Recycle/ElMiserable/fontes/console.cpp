// console.cpp
//

#pragma comment(lib, "user32.lib")
#include "stdafx.h"
#include "console.h"
#include "Winable.h"

const char LIB_VERSION_MAJOR[3] = "01";
const char LIB_VERSION_MINOR[3] = "02";
const char LIB_VERSION_QUICKFIX[5] = "0000";

//
// Clears the screen
//
/*void clrscr()
{
	COORD coordScreen = { 0, 0 }; 
	DWORD cCharsWritten; 
	CONSOLE_SCREEN_BUFFER_INFO csbi; 
	DWORD dwConSize; 
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); 

	GetConsoleScreenBufferInfo(hConsole, &csbi); 
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y; 
	FillConsoleOutputCharacter(hConsole, text_(' '), dwConSize, coordScreen, &cCharsWritten); 
	GetConsoleScreenBufferInfo(hConsole, &csbi); 
	FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten); 
	SetConsoleCursorPosition(hConsole, coordScreen); 
}*/

//
// Moves the cursor to x, y in console window
// ie x=left\right y=top\bottom
//
void gotoxy(int coluna, int linha)
{
	COORD POINT_;
	POINT_.X = coluna; POINT_.Y = linha;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), POINT_);
}

void FullScreen()
{
	keybd_event(VK_MENU, 0x38, 0, 0);
	keybd_event(VK_RETURN, 0x1c, 0, 0);
	keybd_event(VK_RETURN, 0x1c, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_MENU, 0x38, KEYEVENTF_KEYUP, 0);
}
void SomBeep(DWORD frequencia, DWORD duracao)
{
	Beep(frequencia, duracao);
}
/*void SomTocarWAV(char *nomeArquivoWAV, bool bEmLoop)
{
    DWORD fdwSound = SND_ASYNC|SND_FILENAME;
    if(bEmLoop)
        fdwSound |= SND_LOOP;
	PlaySound(nomeArquivoWAV, NULL, fdwSound);
}
void SomPararWAV()
{
	PlaySound(NULL, NULL, SND_ASYNC);
}*/

const string LibVersion()
{
	string str(LIB_VERSION_MAJOR + string(".") + LIB_VERSION_MINOR + string(".") + LIB_VERSION_QUICKFIX);
	return str;
}
const string LibReleaseDate()
{
	string str("19 - Abril - 2005");
	return str;
}
const string LibDisclaimer()
{
	string strL1("Biblioteca de funcoes para Modo Console.\n");
	string strL2("Desenvolvida pelos alunos dos cursos de graduacao:\n");
	string strL3("\tJogos e Entretenimento Digital - GT-JEDi\n");
	string strL4("\tEngenharia da Computacao\n");
	string strL5("UNISINOS - Universidade do Vale do Rio dos Sinos\n");

	string srtDisclaimer(strL1 + strL2 + strL3 + strL4 + strL5);
	return srtDisclaimer;
}
void textcolor (DOS_COLORS iColor)
{
	HANDLE hl = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
	BOOL b = GetConsoleScreenBufferInfo(hl, &bufferInfo);
	bufferInfo.wAttributes &= 0x00F0;
	SetConsoleTextAttribute (hl, bufferInfo.wAttributes |= iColor);
}

// -------------------------------------------------------------------------
void backcolor (DOS_COLORS iColor)
{
	HANDLE hl = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
	BOOL b = GetConsoleScreenBufferInfo(hl, &bufferInfo);
	bufferInfo.wAttributes &= 0x000F;
	SetConsoleTextAttribute (hl, bufferInfo.wAttributes |= (iColor << 4));
}

void LerStringComEspacos(char buffer[], int tamanho_buffer)
{
	char ch = cin.peek();
	while(ch == '\n')
	{
		if(ch == '\n')
			cin.ignore();
		ch = cin.peek();
	}
	cin.clear();
	cin.getline(buffer, tamanho_buffer);
}