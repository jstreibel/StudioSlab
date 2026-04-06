// API_gráfica.cpp : 
//

#include "stdafx.h"
#include "graphics.h"

MAP screenBuffer;
void bufferGets(SPRITE &copySprite, POINT_ position)
{
	short maxY = position.Y + copySprite.sizeY;
	short maxX = position.X + copySprite.sizeX;
	
	for (short y = position.Y, i = 0; y < maxY; y++, i++)
	{
		for (short x = position.X, j = 0; x < maxX; x++, j++)
		{
			screenBuffer.map[y][x] = copySprite.map[i][j];						
			screenBuffer.text_color[y][x] = copySprite.text_color[i][j];
			screenBuffer.back_color[y][x] = copySprite.back_color[i][j];
		}		
	}
}

void bufferGets(MAP &copyMap, POINT_ start, POINT_ end)
{
	for (short i = start.Y; i < end.Y; i++)
		for (short j = start.X; j < end.X; j++)
		{
			screenBuffer.map[i][j] = copyMap.map[i][j];						
			screenBuffer.text_color[i][j] = copyMap.text_color[i][j];
			screenBuffer.back_color[i][j] = copyMap.back_color[i][j];
		}		
}
void drawBuffer()
{
	for (short i = 0; i < DISPLAY_Y; i++)
		for (short j = 0; j < DISPLAY_X; j++)
		{
			if (i == DISPLAY_Y && j == DISPLAY_X) break;
			gotoxy(j, i);
			textcolor(DOS_COLORS(screenBuffer.text_color[i][j]));
			backcolor(DOS_COLORS(screenBuffer.back_color[i][j]));
			cout << screenBuffer.map[i][j];
		}
}
void drawBuffer(POINT_ start, POINT_ end)
{
	for (short i = start.Y; i < end.Y; i++)
		for (short j = start.X; j < end.X; j++)
		{
			if (i == DISPLAY_Y && j == DISPLAY_X) break;
			gotoxy(j, i);
			textcolor(DOS_COLORS(screenBuffer.text_color[i][j]));
			backcolor(DOS_COLORS(screenBuffer.back_color[i][j]));
			cout << screenBuffer.map[i][j];
		}
}

TEXT_::TEXT_(char file[256])
{
	unsigned char thru;
	ifstream load_ (file, ios::in);
	load_ >> sizeX >> sizeY;
	for (short int i = 0; i < sizeY; i++)
	{
		for (short int j = 0; j < sizeX; j++)
		{
			load_ >> thru;
			switch (thru)
			{
			case NULL_CHAR:
				alphabet[i][j] = ' ';
				break;
			default:
				alphabet[i][j] = thru;
				break;
			}
		}
	}
	sizeX = 5;
	sizeY = 7;

};
void TEXT_::write (char sentence[/*MAX_TEXT*/], POINT_ position)
{
	// 97 = a ;; 122 = z
	// 5 x 7 --- 5 x 212
	bool found = false;

	//PASSA LETRA POR LETRA DA SENTENCA
	for (short int letter = 0; sentence[letter] != 0; letter++)
	{
		found = false;
		if (sentence[letter] == 32)
			continue;

		// PROCURA A LETRA NA FONTE (DESENHADA) CORRESPONDENTE A LETRA DA SENTENCA
		else for (short int write = 0; !found; write += 8)
		{	
			// TESTA SE A LETRA DA FONTE CORRESPONDE A LETRA DA SENTENCA
				if (alphabet[write][0] == sentence[letter])
				{
					// FOUND!
					found = true;		

					// FINALMENTE DESENHA A LETRA!
					for (short int i = (write + 1), y = position.Y; i < write + 8; i++)
					{
						for (short int j = 0, x = position.X + (letter * 5); j < 5; j++)
						{
							if (alphabet[i][j] == 32) 
							{
								x++;
								continue;
							}
							else
							{
								gotoxy(x, y);
								cout << alphabet[i][j];
								x++;
							}							
						}
						y++;
					}				
				}
			}
		
	}
};
void TEXT_::write(int number, POINT_ position)
{	
#define MAX 6
	char convert[MAX];
	for (short i = 0; i < MAX; i++) convert[i] = 32;
	short index = MAX - 2;
	convert[MAX - 1] = 0;
	short test;
	
	bool converting = true;
	while (converting) // 65 A em ASCII -- 48 -> 0 em ascii -- 97 -> a em ascii
	{
		convert[index] = (number % 10) + 97;
		index--;
		if (number < 10) converting = false;
		number = number / 10;
	}
	write (convert, position);
}
void BIG_SPRITE::drawSpr(POINT_ start){
	for (short int y = 0; y < sizeY; y++)
	{
		for (short int x = 0; x < sizeX; x++)
		{
			if (is_transparent && 
				back_color[y][x] == back_color[0][0] &&
				text_color[y][x] == back_color[0][0])
				continue;
			else
			{
				gotoxy(x + start.X, y + start.Y);
				textcolor (DOS_COLORS (text_color[y][x]));
				backcolor (DOS_COLORS (back_color[y][x]));
				cout << map[y][x];
			}
		}
	}
};

void BIG_SPRITE::loadSpr(char nome[])
{
	ifstream load_ (nome, ios::in);
	unsigned char thru;
	load_ >> sizeX;
	load_ >> sizeY;
	for (int i = 0; i < sizeY; i++)
	{
		for (int j = 0; j < sizeX; j++)	
		{
			load_ >> thru;
			switch (thru)
			{
			case NULL_CHAR:
				map[i][j] = ' ';
				break;
			default:
				map[i][j] = thru;
				break;
			}			
		}
	}

	for (int i = 0; i < sizeY; i++)
	{
		for (int j = 0; j < sizeX; j++)	
		{
			load_ >> text_color[i][j];			
		}
	}
	for (int i = 0; i < sizeY; i++)
	{
		for (int j = 0; j < sizeX; j++)	
		{
			load_ >> back_color[i][j];				
		}
	}
	is_transparent = true;
};

void BIG_SPRITE::drawSpr(POINT_ start, short transparenceText[BIG_SPRITE_Y][BIG_SPRITE_X], short transparenceBack[BIG_SPRITE_Y][BIG_SPRITE_X])
{
	for (short int y = 0; y < sizeY; y++)
	{
		for (short int x = 0; x < sizeX; x++)
		{
			if (back_color[y][x] == back_color[0][0])
				backcolor (DOS_COLORS(transparenceBack[y][x]));
			else backcolor (DOS_COLORS (back_color[y][x]));

			if (text_color[y][x] == back_color[0][0])
				textcolor (DOS_COLORS (transparenceText[y][x]));
			else textcolor (DOS_COLORS (text_color[y][x]));
						
			gotoxy(x + start.X, y + start.Y);
			//backcolor (DOS_COLORS (back_color[y][x]));
			cout << map[y][x];
		}
	}	
};

void SPRITE::drawSpr(POINT_ start){
	for (short int y = 0; y < sizeY; y++)
	{
		for (short int x = 0; x < sizeX; x++)
		{
			if (is_transparent && 
				back_color[y][x] == back_color[0][0] &&
				text_color[y][x] == back_color[0][0])
				continue;
			else
			{
				gotoxy(x + start.X, y + start.Y);
				textcolor (DOS_COLORS (text_color[y][x]));
				backcolor (DOS_COLORS (back_color[y][x]));
				cout << map[y][x];
			}
		}
	}
};

void SPRITE::loadSpr(char nome[])
{
	ifstream load_ (nome, ios::in);
	unsigned char thru;
	load_ >> sizeX;
	load_ >> sizeY;
	for (int i = 0; i < sizeY; i++)
	{
		for (int j = 0; j < sizeX; j++)	
		{
			load_ >> thru;
			switch (thru)
			{
			case NULL_CHAR:
				map[i][j] = ' ';
				break;
			default:
				map[i][j] = thru;
				break;
			}			
		}
	}

	for (int i = 0; i < sizeY; i++)
	{
		for (int j = 0; j < sizeX; j++)	
		{
			load_ >> text_color[i][j];			
		}
	}
	for (int i = 0; i < sizeY; i++)
	{
		for (int j = 0; j < sizeX; j++)	
		{
			load_ >> back_color[i][j];				
		}
	}
	is_transparent = true;
};

void SPRITE::drawSpr(POINT_ start, short transparenceText[SPRITE_Y][SPRITE_X], short transparenceBack[SPRITE_Y][SPRITE_X])
{
	for (short int y = 0; y < sizeY; y++)
	{
		for (short int x = 0; x < sizeX; x++)
		{
			if (back_color[y][x] == back_color[0][0])
				backcolor (DOS_COLORS(transparenceBack[y][x]));
			else backcolor (DOS_COLORS (back_color[y][x]));

			if (text_color[y][x] == back_color[0][0])
				textcolor (DOS_COLORS (transparenceText[y][x]));
			else textcolor (DOS_COLORS (text_color[y][x]));
			
			if (x + start.X < 0 ||
				x + start.X > DISPLAY_X - 1 ||
				y + start.Y < 0 ||
				y + start.Y > DISPLAY_Y - 1)
				continue;

			else
			{
				gotoxy(x + start.X, y + start.Y);
				//backcolor (DOS_COLORS (back_color[y][x]));
				//if (text_color[y][x] == back_color[0][0] && back_color[y][x] == back_color[0][0])
				//	continue;
				//else 
					cout << map[y][x];
			}
		}
	}	
};

void draw_(MAP &map, POINT_ center)
{	
	short int x_min = center.X - (DISPLAY_X / 2); // onde a matriz começa a ser impressa 
	short int y_min = center.Y - (DISPLAY_Y / 2); // onde a matriz começa a ser impressa

	if (x_min < 0) x_min = 0;
	if (y_min < 0) y_min = 0;

	if (x_min > ((map.sizeX) - (DISPLAY_X))) x_min = (map.sizeX) - (DISPLAY_X);
	if (y_min > ((map.sizeY) - (DISPLAY_Y))) y_min = (map.sizeY) - (DISPLAY_Y);
	
	// TESTANDO SE O TAMANHO DO MAPA EH MENOR QUE O BUFFER, TANTO EM X QUANTO EM Y
	short int x_max = x_min + DISPLAY_X;
	if (map.sizeX < DISPLAY_X)
	{
		x_max = map.sizeX;
		x_min = 0;
	}
	
	short int y_max = y_min + DISPLAY_Y;
	if (map.sizeY < DISPLAY_Y) 
	{
		y_max = map.sizeY;
		y_min = 0;
	}
	//REDESENHA O MAPA INTEIRO
	{
		gotoxy(0,0);
		for (short int y = y_min ; y < y_max ; y++)
		{
			for (short int x = x_min ; x < x_max ; x++)
			{
				if (y == (y_max - 1) && x == (x_max - 1)) continue;
				textcolor (DOS_COLORS (map.text_color[y][x]));
				backcolor (DOS_COLORS (map.back_color[y][x]));
				cout << map.map[y][x];
				/*if ((map.sizeX < DISPLAY_X) && (x == (x_max - 1))) 
					cout << endl;*/
			}
		}
	}
};
void loadmap(char nome [], MAP &map)
{
	//mapa01
	ifstream load_ (nome, ios::in);
	
	unsigned char thru;
	load_ >> map.sizeX;
	load_ >> map.sizeY;
	for (int i = 0; i < map.sizeY; i++)
	{
		for (int j = 0; j < map.sizeX; j++)	
		{
			load_ >> thru;
			switch (thru)
			{
			case NULL_CHAR:
				map.map[i][j] = ' ';
				break;
			default:
				map.map[i][j] = thru;
				break;
			}			
		}
	}

	short int color_;
	for (int i = 0; i < map.sizeY; i++)
	{
		for (int j = 0; j < map.sizeX; j++)	
		{
			load_ >> color_;
			map.text_color[i][j] = color_;			
		}
	}
	for (int i = 0; i < map.sizeY; i++)
	{
		for (int j = 0; j < map.sizeX; j++)	
		{
			load_ >> color_;
			map.back_color[i][j] = color_;				
		}
	}
};
/*
	0 = BLACK		1 = BLUE		2 = GREEN		3 = CYAN		4 = RED
	5 = MAGENTA		6 = BROWN		7 = LIGHT_GRAY	8 = DARK_GRAY	9 = LIGHT_BLUE
	10 = LIGHT_GREEN	11 = LIGHT_CYAN		12 = LIGHT_RED		13 = LIGHT_MAGENTA
	14 = YELLOW		15 = WHITE
*/