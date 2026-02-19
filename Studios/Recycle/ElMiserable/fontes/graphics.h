// screen resolution = 340 X 143
// resolution ratio = 3 X 5

#include <conio.h>
#include <iostream>
#include <fstream>
#include "..\jedilib\console.h"

#define BIG_SPRITE_X 100
#define BIG_SPRITE_Y 50
#define SPRITE_X 40
#define SPRITE_Y 20
#define MAP_X 400
#define MAP_Y 400
#define DISPLAY_X 340
#define DISPLAY_Y 140
#define MAX_TEXT 24
#define NULL_CHAR (unsigned char)215

struct POINT_{short X; short Y;};
class BIG_SPRITE{
public:
	void drawSpr (POINT_);						//desenha um SPRITE na posição POINT_
	void drawSpr (POINT_, 
		short transparenceText[BIG_SPRITE_Y][BIG_SPRITE_X], 
		short transparenceBack[BIG_SPRITE_Y][BIG_SPRITE_X]);//igual ao de cima mas com um mapa de cor de fundo para desenhar
	void loadSpr (char[]);						// carrega um arquivo para dentro de um SPRITE já criado
	short int sizeX, sizeY;						// dimensão do SPRITE
	unsigned char map[BIG_SPRITE_Y][BIG_SPRITE_X];		// mapa de desenho do SPRITE
	short int back_color[BIG_SPRITE_Y][BIG_SPRITE_X];	// mapa de cores de fundo do SPRITE
	short int text_color[BIG_SPRITE_Y][BIG_SPRITE_X];	// mapa de cores de texto do SPRITE	
	bool is_transparent;						// com ou sem transparência. A cor de transparência é a do canto esquerdo SUPERIOR.
};

class SPRITE{
public:
	void drawSpr (POINT_);						//desenha um SPRITE na posição POINT_
	void drawSpr (POINT_, short transparenceText[SPRITE_Y][SPRITE_X], short transparenceBack[SPRITE_Y][SPRITE_X]);//igual ao de cima mas com um mapa de cor de fundo para desenhar
	void loadSpr (char[]);						// carrega um arquivo para dentro de um SPRITE já criado
	short int sizeX, sizeY;						// dimensão do SPRITE
	unsigned char map[SPRITE_Y][SPRITE_X];		// mapa de desenho do SPRITE
	short int back_color[SPRITE_Y][SPRITE_X];	// mapa de cores de fundo do SPRITE
	short int text_color[SPRITE_Y][SPRITE_X];	// mapa de cores de texto do SPRITE	
	bool is_transparent;						// com ou sem transparência. A cor de transparência é a do canto esquerdo SUPERIOR.
};

struct MAP{
	unsigned char map[MAP_Y][MAP_X];			// mapa de desenho do mapa
	char back_color[MAP_Y][MAP_X];				// mapa de cores de fundo do SPRITE
	char text_color[MAP_Y][MAP_X];				// mapa de cores de texto do SPRITE	
	short int sizeX, sizeY;						// dimensão do mapa
};

class SCREEN_BUFFER{
public:
	void get(SPRITE &copySprite);
	void get(MAP &copyMap); 
private:
	unsigned char map[MAP_Y][MAP_X];			// mapa de desenho do mapa
	char back_color[MAP_Y][MAP_X];				// mapa de cores de fundo do SPRITE
	char text_color[MAP_Y][MAP_X];				// mapa de cores de texto do SPRITE	
};

class TEXT_
{
public:
	TEXT_(char [256]);
	void write (char sentence[MAX_TEXT], POINT_);// funcao para escrever uma string
	void write (int number, POINT_); // funcao para escrever um valor inteiro
private:
	unsigned char alphabet[212][5];				// matriz que armazena a fonte
	short int sizeX, sizeY;						

};

void draw_(MAP &, POINT_);					//desenha um mapa com a matriz centralizada em COORD, desde o canto esquerdo superior da tela até o canto inferior direito da tela
void loadmap (char[], MAP &);				// carrega um arquivo para dentro de um mapa já criado

void bufferGets(SPRITE &, POINT_);
void bufferGets(MAP &, POINT_, POINT_);
void drawBuffer();
void drawBuffer(POINT_, POINT_);

