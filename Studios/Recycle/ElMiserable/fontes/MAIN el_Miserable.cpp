// jogo_.cpp : Defines the entry COORD for the console application.
// 

#include "stdafx.h"
#include "main_header.h"

MAP mainWorldMap[6][6];

int _tmain(int argc, _TCHAR* argv[])
{	
	/* 
	-------------------------------------------------------------------------------------------
			CRIANDO E CARREGANDO IMAGENS E SPRITES DO JOGO.
	------------------------------------------------------------------------------------------- 
	*/
	TEXT_ gameFont("resource\\fonts\\alpha_min");
	{
		backcolor(BLACK);
		system ("cls");
		POINT_ atPos;
		atPos.X = 5;
		atPos.Y = DISPLAY_Y - 20;
		textcolor(WHITE);
		gameFont.write ("el miserable", atPos);
		atPos.Y += 10;
		gameFont.write ("cargando", atPos);
	}

	
	loadmap ("resource\\maps\\map_2_2", mainWorldMap[2][2]);
	loadmap ("resource\\maps\\map_3_1", mainWorldMap[3][1]);
	loadmap ("resource\\maps\\map_3_2", mainWorldMap[3][2]);
	loadmap ("resource\\maps\\map_3_3", mainWorldMap[3][3]);
	
	loadmap ("resource\\maps\\map_4_1", mainWorldMap[4][1]);
	loadmap ("resource\\maps\\map_4_2", mainWorldMap[4][2]);
	loadmap ("resource\\maps\\map_4_3", mainWorldMap[4][3]);
	
	BIG_SPRITE house;
	house.loadSpr ("resource\\sprites\\house1");
	BIG_SPRITE tree;
	tree.loadSpr ("resource\\sprites\\tree");
		
	SPRITE playerUp[8];
	playerUp[0].loadSpr ("resource\\hero\\up\\up1");
	playerUp[1].loadSpr ("resource\\hero\\up\\up2");
	playerUp[2].loadSpr ("resource\\hero\\up\\up3");
	playerUp[3].loadSpr ("resource\\hero\\up\\up4");
	playerUp[4].loadSpr ("resource\\hero\\up\\up5");
	playerUp[5].loadSpr ("resource\\hero\\up\\up6");
	playerUp[6].loadSpr ("resource\\hero\\up\\up7");
	playerUp[7].loadSpr ("resource\\hero\\up\\up8");
	SPRITE playerDown[8];
	playerDown[0].loadSpr ("resource\\hero\\down\\down1");
	playerDown[1].loadSpr ("resource\\hero\\down\\down2");
	playerDown[2].loadSpr ("resource\\hero\\down\\down3");
	playerDown[3].loadSpr ("resource\\hero\\down\\down4");
	playerDown[4].loadSpr ("resource\\hero\\down\\down5");
	playerDown[5].loadSpr ("resource\\hero\\down\\down6");
	playerDown[6].loadSpr ("resource\\hero\\down\\down7");
	playerDown[7].loadSpr ("resource\\hero\\down\\down8");
	SPRITE playerLeft[8];
	playerLeft[0].loadSpr ("resource\\hero\\left\\left1");
	playerLeft[1].loadSpr ("resource\\hero\\left\\left2");
	playerLeft[2].loadSpr ("resource\\hero\\left\\left3");
	playerLeft[3].loadSpr ("resource\\hero\\left\\left4");
	playerLeft[4].loadSpr ("resource\\hero\\left\\left5");
	playerLeft[5].loadSpr ("resource\\hero\\left\\left6");
	playerLeft[6].loadSpr ("resource\\hero\\left\\left7");
	playerLeft[7].loadSpr ("resource\\hero\\left\\left8");
	SPRITE playerRight[8];
	playerRight[0].loadSpr ("resource\\hero\\right\\right1");
	playerRight[1].loadSpr ("resource\\hero\\right\\right2");
	playerRight[2].loadSpr ("resource\\hero\\right\\right3");
	playerRight[3].loadSpr ("resource\\hero\\right\\right4");
	playerRight[4].loadSpr ("resource\\hero\\right\\right5");
	playerRight[5].loadSpr ("resource\\hero\\right\\right6");
	playerRight[6].loadSpr ("resource\\hero\\right\\right7");
	playerRight[7].loadSpr ("resource\\hero\\right\\right8");
	SPRITE playerUpAttack[9];
	playerUpAttack[0].loadSpr ("resource\\hero\\upAttack\\upAttack1");
	playerUpAttack[1].loadSpr ("resource\\hero\\upAttack\\upAttack2");
	playerUpAttack[2].loadSpr ("resource\\hero\\upAttack\\upAttack3");
	playerUpAttack[3].loadSpr ("resource\\hero\\upAttack\\upAttack4");
	playerUpAttack[4].loadSpr ("resource\\hero\\upAttack\\upAttack5");
	playerUpAttack[5].loadSpr ("resource\\hero\\upAttack\\upAttack6");
	playerUpAttack[6].loadSpr ("resource\\hero\\upAttack\\upAttack7");
	playerUpAttack[7].loadSpr ("resource\\hero\\upAttack\\upAttack8");
	playerUpAttack[8].loadSpr ("resource\\hero\\upAttack\\upAttack9");
	SPRITE playerDownAttack[9];
	SPRITE playerLeftAttack[9];
	SPRITE playerRightAttack[9];

	SPRITE tortugaUp[2];
	tortugaUp[0].loadSpr ("resource\\tortuga\\up1");
	tortugaUp[1].loadSpr ("resource\\tortuga\\up2");
	SPRITE tortugaDown[2];
	tortugaDown[0].loadSpr ("resource\\tortuga\\down1");
	tortugaDown[1].loadSpr ("resource\\tortuga\\down2");
	SPRITE tortugaLeft[2];
	tortugaLeft[0].loadSpr ("resource\\tortuga\\left1");
	tortugaLeft[1].loadSpr ("resource\\tortuga\\left2");
	SPRITE tortugaRight[2];
	tortugaRight[0].loadSpr ("resource\\tortuga\\right1");
	tortugaRight[1].loadSpr ("resource\\tortuga\\right2");

	
	
	
	/*
	---------------------------------------------------------------------------------------
			CRIANDO OS OBJETOS DO JOGO E INICIALIZANDO ELES COM SUAS RESPECTIVAS IMAGENS
		E ATRIBUTOS.
	---------------------------------------------------------------------------------------
	*/

	PLAYER player1;
	player1.attributes (6, 25, 30);
	player1.walkSprites = 8;
	player1.downWalkSprite (playerDown);
	player1.upWalkSprite   (playerUp);
	player1.leftWalkSprite (playerLeft);
	player1.rightWalkSprite(playerRight);
	player1.upAttackSprite (playerUpAttack);
	player1.attackSprites = 9;
	player1.placeAt(16, 95);
	
	BIG_OBJECT house1, tree1;
	house1.uses_sprite (house);
	tree1.uses_sprite (tree);

	TORTUGA tortuga[MAX_TORTUGAS];
	for (short i = 0; i < MAX_TORTUGAS; i++)
	{
		tortuga[i].walkSprites = 2;
		tortuga[i].attributes(2, 10, 5);
		tortuga[i].upWalkSprite	(tortugaUp);
		tortuga[i].downWalkSprite  (tortugaDown);
		tortuga[i].leftWalkSprite  (tortugaLeft);
		tortuga[i].rightWalkSprite (tortugaRight);
	}




	/*
	-------------------------------------------------------------------------------------------
			CRIANDO O JOGO, ATRIBUINDO OS OBJETOS E MAPAS A ELE E INICIALIZANDO ELE
	-------------------------------------------------------------------------------------------
	*/
	GAME jogo_ (3, 2);
	jogo_.useFont(gameFont);

	for (char i = 0; i < 6; i++)
		for (char j = 0; j < 6; j++)
			jogo_.useMap(mainWorldMap[i][j], i, j);

	/*
	jogo_.useMap(mainWorldMap[3][2], 3, 2);
	jogo_.useMap(mainWorldMap[4][2], 4, 2);
	jogo_.makeColorBasedCollisionMap();
	*/
	jogo_.useHouse1(house1);
	jogo_.useTree1(tree1);
	
	jogo_.player_is(player1);
	jogo_.useTORTUGA(tortuga);
	jogo_.makeRelationOfMapObjects();
	
	jogo_.updateBuffer();
	drawBuffer();
	jogo_.drawPlayer();
	jogo_.drawTORTUGAS();
	/*{
		backcolor(BLACK);
		textcolor(WHITE);
		jogo_.showCollisionMap();
	}*/




	/*
	-------------------------------------------------------------------------------------------
			RODANDO O JOGO
	-------------------------------------------------------------------------------------------
	*/
	char input = 0;
	BEHAVIOR playerBehavior;
	POINT_ statsPos;
	statsPos.X = 4;
	statsPos.Y = 2;
	bool keyboardHit = false;
	while (input != 27){
		// ------------- "VEZ" DO JOGADOR -------------------------------------------------
		static unsigned int sleepControl = 0;
		sleepControl++;
		if (_kbhit()){
			keyboardHit = true;
			sleepControl = 0;
			input = _getch();
			if (input == -32){
				input = _getch();				
				switch (input){
				case 72:	// UP
					player1.roundBehavior = UP;
					break;
				case 80:	// DOWN
					player1.roundBehavior = DOWN;
					break;
				case 75:	// LEFT
					player1.roundBehavior = LEFT;
					break;
				case 77:	// RIGHT
					player1.roundBehavior = RIGHT;
					break;
				}
			}
			else if (input == 32)	// ATAQUE
				jogo_.playerStartAttack();
		}
		jogo_.step(keyboardHit);
		keyboardHit = false;
		Sleep(1);
	}
	



	/*
	-------------------------------------------------------------------------------------------
			FINALIZANDO O JOGO
	-------------------------------------------------------------------------------------------
	*/
	POINT_ loc; loc.X = 130; loc.Y = 60;
	textcolor(WHITE);
	backcolor(GREEN);
	short x = 12345;
	//gameFont.write(x, loc);
	gameFont.write("adios kabron", loc);
	loc.X = 5; loc.Y = DISPLAY_Y - 8;
	gameFont.write("pressiona qualquier tiecla para continuar", loc);
	gotoxy(227, DISPLAY_Y - 3);
	cout << (char)219; 
	gotoxy(233, DISPLAY_Y - 3);
	cout << (char)219; 
	gotoxy(240, DISPLAY_Y - 3);
	cout << (char)219; 
	
	gotoxy(0,DISPLAY_Y);
	system ("pause");
	
	return 0;
}

