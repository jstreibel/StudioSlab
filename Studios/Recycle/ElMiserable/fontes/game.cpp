#include "stdafx.h"
#include "game.h"

// ------------- INICIALIZACAO ----------------------------------------------------------------------
GAME::GAME(short linha, short coluna)
{
	zone_i = linha;
	zone_j = coluna;
	numberOfTORTUGAS = 0;
	zeroCollisionMap();
	mapChange = false;
}

// ------------- FONTE (TEXTO) ----------------------------------------------------------------------
void GAME::useFont(TEXT_ &temp){gameFont = &temp;}
// ------------- MAPA -------------------------------------------------------------------------------
void GAME::useMap(MAP &temp, short i, short j)
{
	worldMap[i][j] = &temp;

	
	// ------------------- TESTE DE FUNDO VERDE MULTI-TOM -----------------------
	MAP *change = worldMap[i][j];
	srand(time(NULL));
	for (short i = 0; i < DISPLAY_Y; i++)
		for (short j = 0; j < DISPLAY_X; j++)
		{
			short choice;
			/*
			if (change->text_color[i][j] == 2 && change->back_color[i][j] == 2 && change->map[i][j] == 220)
			{
				choice = rand() % 2;
#define DARKEN_GRASS 50
				for (short lowTone = 0; lowTone < DARKEN_GRASS; lowTone++)
				{
					if (choice == 1) choice = rand() % 2;
					else lowTone = DARKEN_GRASS;
				}
				char useBackColor = 2;
				switch (choice)
				{
				case 0:
					useBackColor = 2;
					break;
				case 1:
					useBackColor = 10;
					break;
				case 2:
					useBackColor = 3;
					break;
				}

				choice = rand() % 2;
				for (short lowTone = 0; lowTone < DARKEN_GRASS; lowTone++)
				{
					if (choice == 2) choice = rand() % 2;
					else lowTone = DARKEN_GRASS;
				}
				char useTextColor = 2;
				switch (choice)
				{
				case 0:
					useTextColor = 2;
					break;
				case 1:
					useTextColor = 10;
					break;
				case 2:
					useTextColor = 3;
					break;
				}

				choice = rand() % 3;
				char useTone = 220;
				switch (choice)
				{
				case 0:
					useTone = 219;
					break;
				case 1:
					useTone = 176;
					break;
				case 2:
					useTone = 177;
					break;
				case 3:
					useTone = 178;
					break;
				} // 2 - verde ;;; 3 - cyan ;;; 10 - verde claro
				
				change->map[i][j] = useTone;				
				change->text_color[i][j] = useTextColor;
				change->back_color[i][j] = useBackColor;
			}*/
		
			if (change->text_color[i][j] == 1 && change->back_color[i][j] == 1 && change->map[i][j] == 220)
				{
					choice = rand() % 3;
#define DARKEN_WATER 3  // 1 - azul, 3 - ciano, 9 - azul claro
					for (short lowTone = 0; lowTone < DARKEN_WATER; lowTone++)
					{
						if (choice == 1) choice = rand() % 3;
						else lowTone = DARKEN_WATER;
					}

					char useBackColor = 1;
					switch (choice)
					{
					case 0:
						useBackColor = 1;
						break;
					case 1:
						useBackColor = 3;
						break;
					case 2:
						useBackColor = 9;
						break;
					}

					choice = rand() % 2;
					for (short lowTone = 0; lowTone < DARKEN_WATER; lowTone++)
					{
						if (choice == 1) choice = rand() % 2;
						else lowTone = DARKEN_WATER;
					}
					char useTextColor = 1;
					switch (choice)
					{
					case 0:
						useTextColor = 1;
						break;
					case 1:
						useTextColor = 3;
						break;
					case 2:
						useTextColor = 9;
						break;
					}

					choice = rand() % 3;
					char useTone = 220;
					switch (choice)
					{
					case 0:
						useTone = 219;
						break;
					case 1:
						useTone = 176;
						break;
					case 2:
						useTone = 177;
						break;
					case 3:
						useTone = 178;
						break;
					} // 2 - verde ;;; 3 - cyan ;;; 10 - verde claro
				
					change->map[i][j] = useTone;				
					change->text_color[i][j] = useTextColor;
					change->back_color[i][j] = useBackColor;
				}
			}
}

void GAME::drawMap()
{
	POINT_ usingCenterXY = player->returnPosition();
	draw_(*worldMap[zone_i][zone_j], usingCenterXY);
}
void GAME::makeRelationOfMapObjects()
{
	// --- zera a relação de objetos na tela ---
	for (short i = 0; i < 64; i++)
		mapObjects[i].type = 0;


	// --- cria a relação de objetos da zona atual (zone_i, zone_j) do mapa do mundo --- 
	unsigned char read = 0;
	short objectNumber = 0;
	for (short i = 0; i < DISPLAY_Y; i++)
		for (short j = 0; j < DISPLAY_X; j++)
		{
			read = worldMap[zone_i][zone_j]->map[i][j];
			if ((read >= 65 && read <= 90) ||
				(read >= 97 && read <= 122))
			{
				//worldMap[zone_i][zone_j]->map[i][j] = (char)220;
				worldMap[zone_i][zone_j]->text_color[i][j] = worldMap[zone_i][zone_j]->back_color[0][0];
				worldMap[zone_i][zone_j]->back_color[i][j] = worldMap[zone_i][zone_j]->back_color[0][0];
				mapObjects[objectNumber].type = read;
				mapObjects[objectNumber].location.X = j;
				mapObjects[objectNumber].location.Y = i;
				objectNumber++;
		}
	}


	// ------------------ ATUALIZA OS OBJETOS COM SUAS RESPECTIVAS POSIÇÕES ------------------------------
	
	numberOfTORTUGAS = 0;
	for (short i = 0; i < MAX_TORTUGAS; i++)
		tortuga[i].zeroPathNodes();

	MAP *zoneMap = worldMap[zone_i][zone_j];
	
	{
		for (short i = 0; i < 64; i++)
		{
			if (mapObjects[i].type == 0) continue;
			switch (mapObjects[i].type)
			{
			case 'A':   // CASA
				house1->location = mapObjects[i].location;
				drawOnMap (*house1);
				putInCollisionMap (*house1);
				break;
			case 't':	// ARVORE
				tree1->location = mapObjects[i].location;
				drawOnMap (*tree1);
				putInCollisionMap (*tree1);
				break;
			case 'T':	// TORTUGA
				short X = mapObjects[i].location.X;
				short Y = mapObjects[i].location.Y;
				
				unsigned char number = zoneMap->map[Y][X + 1] - 48;
				//zoneMap->map[Y][X + 1] = (char)220;
				zoneMap->text_color[Y][X + 1] = zoneMap->back_color[0][0];
				zoneMap->back_color[Y][X + 1] = zoneMap->back_color[0][0];
				
				unsigned char wayPoint = zoneMap->map[Y][X + 2] - 48;
				//zoneMap->map[Y][X + 2] = (char)220;
				zoneMap->text_color[Y][X + 2] = zoneMap->back_color[0][0];
				zoneMap->back_color[Y][X + 2] = zoneMap->back_color[0][0];
				
				if (wayPoint == 0)
				{
					tortuga[number].placeAt (mapObjects[i].location.X, mapObjects[i].location.Y);
					tortuga[number].getPathPoint(wayPoint, mapObjects[i].location);
					tortuga[numberOfTORTUGAS].isOnMap(true);
					numberOfTORTUGAS++;
				}
				else 
					tortuga[number].getPathPoint(wayPoint, mapObjects[i].location);
				break;
			}
		}
	}		
}


// ------------- JOGADOR ----------------------------------------------------------------------------
void GAME::player_is(PLAYER &temp){player = &temp;}
void GAME::drawPlayer()	
{
	SPRITE colorMap;
	POINT_ size = player->returnSize();
	colorMap.sizeX = size.X;
	colorMap.sizeY = size.Y;

	POINT_ start = player->returnPosition();
	POINT_ end;
	end.X = start.X + size.X;
	end.Y = start.Y + size.Y;
	
	for (short iMap = (start.Y), i = 0; iMap < end.Y; iMap++)
	{
		for (short jMap = (start.X), j= 0; jMap < end.X; jMap++)
		{
			colorMap.back_color[i][j] = worldMap[zone_i][zone_j]->back_color[iMap][jMap];
			colorMap.text_color[i][j] = worldMap[zone_i][zone_j]->text_color[iMap][jMap];
			j++;
		}
		i++;
	}

	player->draw(colorMap);	
	
	//player->draw();
}

bool GAME::movePlayer()
{
	BEHAVIOR dir = player->roundBehavior;
	// -------------------------- PARTE I ------------------------------ //
	// determina a área atrás do PLAYER que será apagada, para não deixar rastro
	// posicao minima para apagar na tela o rastro do PLAYER(canto esquerdo superior)
	POINT_ eraseMin;	
	eraseMin = player->returnPosition();
	//posicao maxima para apagar na tela o rastro do PLAYER(canto direito inferior)
	POINT_ eraseMax = player->returnSize(); 
	eraseMax.X += eraseMin.X;
	eraseMax.Y += eraseMin.Y;
	// definindo a posição e o tamanho a apagar na tela o rastro do PLAYER
	//eraseAmount: dimensão da caixa para apagar
	short eraseAmount = player->returnSpeed();
	switch (dir){
	case UP:
		eraseMin.Y = eraseMax.Y - eraseAmount / 2;
		break;
	case DOWN:
		eraseMax.Y = eraseMin.Y + eraseAmount / 2;
		break;
	case LEFT:
		eraseMin.X = eraseMax.X - eraseAmount;
		break;
	case RIGHT:
		eraseMax.X = eraseMin.X + eraseAmount;		
		break;
	}

	// ---------------------- PARTE II ------------------------- //
	// colide (ou não) o PLAYER
	bool playerHasMoved = true;
	bool collision = collide(dir);
	POINT_ playerPositionBefore = player->returnPosition();
	if (!collision){
		player->move(dir);
		playerHasMoved = true;
	}

	// ----------------------- PARTE III -------------------------- //
	// testando se o PLAYER chegou ao fim da tela, se sim, 
	// desloca ele para o outro lado da tela
	if (!collision) checkPlayerMapChange();

	if (mapChange){
		eraseMin = playerPositionBefore;
		eraseMax = player->returnSize();
		eraseMax.X += eraseMin.X;
		eraseMax.Y += eraseMin.Y;
	}

	if (playerHasMoved && !mapChange)
	erase (eraseMin, eraseMax);	
	return (mapChange);	
}
bool GAME::checkPlayerMapChange(){
	POINT_ playerPositionAfter = player->returnPosition();
	POINT_ playerSize = player->returnSize();
	switch (player->roundBehavior){
	case UP:
		if		(playerPositionAfter.Y < 0)	{
			player->placeAt(playerPositionAfter.X, (DISPLAY_Y - playerSize.Y));
			mapChange = true;
		}	
		break;
	case DOWN:
		if (playerPositionAfter.Y > (DISPLAY_Y - playerSize.Y))	{
			player->placeAt(playerPositionAfter.X, 0);
			mapChange = true;
		}	
		break;
	case LEFT:
		if (playerPositionAfter.X < 0)	{
			player->placeAt((DISPLAY_X - playerSize.X), playerPositionAfter.Y);
			mapChange = true;
		}	
		break;
	case RIGHT:
		if (playerPositionAfter.X > (DISPLAY_X - playerSize.X))	{
			player->placeAt(0, playerPositionAfter.Y);
			mapChange = true;
		}
		break;
	}

	if (mapChange)
	{
		switch (player->roundBehavior)
		{
		case UP:
			zone_i--;
			break;
		case DOWN:
			zone_i++;
			break;
		case LEFT:
			zone_j--;
			break;
		case RIGHT:
			zone_j++;
		break;
		}
	}
	return (mapChange);
}
void GAME::playerAttack() 
{
	short timing = 1;
	timing = player->attack();
	if (!timing)
	{
		switch (player->roundBehavior){
		case ATTACK_UP:
			player->roundBehavior = DOWN;
			player->move(player->roundBehavior);
			player->roundBehavior = UP;
			break;
		case ATTACK_DOWN:
			player->roundBehavior = DOWN;
			break;
		case ATTACK_LEFT:
			player->roundBehavior = LEFT;
			break;
		case ATTACK_RIGHT:
			player->roundBehavior = RIGHT;
			break;
		}
		POINT_ startErase = player->returnPosition();
		POINT_ endErase;
		POINT_ sum = player->returnSize();
		endErase.X = startErase.X + sum.X + 20;
		endErase.Y = startErase.Y + sum.Y + 1;
		erase (startErase, endErase);
		player->swapSpriteCorrection();
		movePlayer();
		drawPlayer();
	}
}
void GAME::playerStartAttack()
{		
	switch (player->roundBehavior){
	case UP:
		player->roundBehavior = ATTACK_UP;
		break;
	case DOWN:
		//player->roundBehavior = ATTACK_DOWN;
		break;
	case LEFT:
		//player->roundBehavior = ATTACK_LEFT;
		break;
	case RIGHT:
		//player->roundBehavior = ATTACK_RIGHT;
		break;
	}
	player->swapSpriteCorrection();
	player->attackIndex = 0;
}
void GAME::playerCollideWithTortuga(short i)
{
	player->roundBehavior = tortuga[i].roundBehavior;
	for (short run = 0; run < 3; run++){
		mapChange = false;
		movePlayer();
		if (!mapChange)
			drawPlayer();
		else break;
	}
	player->takeDamage(5);
}
void GAME::showPlayerStats(POINT_ place)
{
	short life = player->returnLife();
	gameFont->write("life ", place);
	place.X += 25;
	gameFont->write(life, place);
}
// ------------- INIMIGOS ---------------------------------------------------------------------------
void GAME::useTORTUGA (TORTUGA temp[]){tortuga = temp;}
void GAME::moveTORTUGA(short i)
{
	BEHAVIOR behavior = tortuga[i].roundBehavior;

	// caso a TORTUGA tenha se movido
	if (behavior >= 0 && behavior < 4)
	{
		// determina a área atrás da TORTUGA que será apagada, para não deixar rastro
		// posicao minima para apagar na tela o rastro da TORTUGA(canto esquerdo superior)

		POINT_ startErase;	
		startErase = tortuga[i].returnPosition();
		//posicao maxima para apagar na tela o rastro do PLAYER(canto direito inferior)
		POINT_ endErase = tortuga[i].returnSize(); 
		endErase.X += startErase.X;
		endErase.Y += startErase.Y;
		// definindo a posição e o tamanho a apagar na tela o rastro do PLAYER
		// eraseAmount: dimensão da caixa para apagar
		short eraseAmount = tortuga[i].returnSpeed();
		switch (behavior)
		{
		case UP:
			startErase.Y = endErase.Y - eraseAmount / 2;
			break;
		case DOWN:
			endErase.Y = startErase.Y + eraseAmount / 2;
			break;
		case LEFT:
			startErase.X = endErase.X - eraseAmount;
			break;
		case RIGHT:
			endErase.X = startErase.X + eraseAmount;		
			break;
		}
		tortuga[i].move(behavior);
		erase (startErase, endErase);
	}	
}
void GAME::behaveTORTUGA ()
{
	bool playerOnSight = false;
	BEHAVIOR behavior;
	POINT_ pPos;
	pPos.X = player->location.X;
	pPos.Y = player->location.Y;
	
	for (short i = 0; i < numberOfTORTUGAS; i++)
	{		
		behavior = tortuga[i].think(pPos);
		// caso a TORTUGA tenha se movido
		if (behavior >= 0 && behavior < 4)
		{
			// determina a área atrás da TORTUGA que será apagada, para não deixar rastro
			// posicao minima para apagar na tela o rastro da TORTUGA(canto esquerdo superior)
	
			POINT_ startErase;	
			startErase = tortuga[i].returnPosition();
			//posicao maxima para apagar na tela o rastro do PLAYER(canto direito inferior)
			POINT_ endErase = tortuga[i].returnSize(); 
			endErase.X += startErase.X;
			endErase.Y += startErase.Y;
			// definindo a posição e o tamanho a apagar na tela o rastro do PLAYER
			// eraseAmount: dimensão da caixa para apagar
			short eraseAmount = tortuga[i].returnSpeed();
			switch (behavior)
			{
			case UP:
				startErase.Y = endErase.Y - eraseAmount / 2;
				break;
			case DOWN:
				endErase.Y = startErase.Y + eraseAmount / 2;
				break;
			case LEFT:
				startErase.X = endErase.X - eraseAmount;
				break;
			case RIGHT:
				endErase.X = startErase.X + eraseAmount;		
				break;
			}
			tortuga[i].move(behavior);
			erase (startErase, endErase);
		}	
	}
};
void GAME::drawTORTUGAS()
{
	bool exists;
	for (short i = 0; i < numberOfTORTUGAS; i++)
	{
		exists = tortuga[i].isOnMap();
		if (exists)
		{
			SPRITE colorMap;
			POINT_ size = tortuga[i].returnSize();
			colorMap.sizeX = size.X;
			colorMap.sizeY = size.Y;

			POINT_ start = tortuga[i].returnPosition();
			POINT_ end;
			end.X = start.X + size.X;
			end.Y = start.Y + size.Y;
	
			for (short iMap = (start.Y), i_ = 0; iMap < end.Y; iMap++)
			{
				for (short jMap = (start.X), j_ = 0; jMap < end.X; jMap++)
				{
					colorMap.back_color[i_][j_] = worldMap[zone_i][zone_j]->back_color[iMap][jMap];
					colorMap.text_color[i_][j_] = worldMap[zone_i][zone_j]->text_color[iMap][jMap];
					j_++;
				}
				i_++;
			}
			tortuga[i].draw(colorMap);
		}
	}

	//tortuga[i]->draw(colorMap);	
	//player->draw();
}
// ------------- OUTROS OBJETOS ---------------------------------------------------------------------
void GAME::drawOnMap(BIG_OBJECT &temp)
{
	short x = 0;
	short y = 0;

	short nullColor = temp.bitmap->back_color[0][0];
	POINT_ atPosition = temp.location;

	for (short i = atPosition.Y; i < atPosition.Y + temp.bitmap->sizeY; i++)
	{
		x = 0;
		for (short j = atPosition.X; j < atPosition.X + temp.bitmap->sizeX; j++)
		{
			worldMap[zone_i][zone_j]->map[i][j] = temp.bitmap->map[y][x];
			if (temp.bitmap->back_color[y][x] != nullColor)
				worldMap[zone_i][zone_j]->back_color[i][j] = temp.bitmap->back_color[y][x];
			 
			if (temp.bitmap->text_color[y][x] != nullColor)
				worldMap[zone_i][zone_j]->text_color[i][j] = temp.bitmap->text_color[y][x];
			x++;
		}
		y++;
	}
};
void GAME::useHouse1(BIG_OBJECT &temp){house1 = &temp;}
void GAME::useTree1(BIG_OBJECT &temp)
{
	tree1 = &temp;
	short x = tree1->bitmap->sizeX;
	short y = tree1->bitmap->sizeY;
	BIG_SPRITE *change = tree1->bitmap;
	short choice;
	
	for (short i = 0; i < y; i++)
		for (short j = 0; j < x; j++)
		{
			if (change->text_color[i][j] == 2 && change->back_color[i][j] == 2 && change->map[i][j] == 220)
			{
				choice = rand() % 2;
#define DARKEN_GRASS 50
				for (short lowTone = 0; lowTone < DARKEN_GRASS; lowTone++)
				{
					if (choice == 1) choice = rand() % 2;
					else lowTone = DARKEN_GRASS;
				}
				char useBackColor = 2;
				switch (choice)
				{
				case 0:
					useBackColor = 2;
					break;
				case 1:
					useBackColor = 10;
					break;
				case 2:
					useBackColor = 3;
					break;
				}

				choice = rand() % 2;
				for (short lowTone = 0; lowTone < DARKEN_GRASS; lowTone++)
				{
					if (choice == 2) choice = rand() % 2;
				else lowTone = DARKEN_GRASS;
				}
				char useTextColor = 2;
				switch (choice)
				{
				case 0:
					useTextColor = 2;
					break;
				case 1:
					useTextColor = 10;
					break;
				case 2:
					useTextColor = 3;
					break;
				}
					choice = rand() % 3;
				char useTone = 220;
				switch (choice)
				{
				case 0:
					useTone = 219;
					break;
				case 1:
					useTone = 176;
					break;
				case 2:
					useTone = 177;
					break;
				case 3:
					useTone = 178;
					break;
				} // 2 - verde ;;; 3 - cyan ;;; 10 - verde claro
				
				change->map[i][j] = useTone;				
				change->text_color[i][j] = useTextColor;
				change->back_color[i][j] = useBackColor;
			}
		}
}
// ------------- COLISOES ---------------------------------------------------------------------------
bool GAME::collide(BEHAVIOR dir)
{
	bool collision = false;
	short maxDistance = player->returnSpeed();
	short distance = 0;
	
	POINT_ posMin = player->returnPosition();
	POINT_ posMax = player->returnSize();
	posMax.X += posMin.X;
	posMax.Y += posMin.Y;
	
	switch (dir)
	{
	case UP:
		for (short y = posMin.Y - 1; y >= posMin.Y - (maxDistance / 2); y--)
		{
			for (short x = posMin.X; x < posMax.X; x++)
			{
				if (collisionMap[y][x] == 1)
				{
					player->relativeMove(0, (0 - distance));
					collision = true;
					break;
				}
				
			}
			if (collision) break;
			distance++;
		}
		break;
	case DOWN:
		for (short y = posMax.Y; y < posMax.Y + (maxDistance / 2); y++)
		{
			for (short x = posMin.X; x < posMax.X; x++)
			{
				if (collisionMap[y][x] == 1)
				{
					player->relativeMove(0, distance);
					collision = true;
					break;
				}
			}
			if (collision) break;
			distance++;
		}
		break;
	case LEFT:
		for (short x = posMin.X - 1; x >= posMin.X - maxDistance; x--)
		{
			for (short y = posMin.Y; y < posMax.Y; y++)
			{
				if (collisionMap[y][x] == 1)
				{
					player->relativeMove((0 - distance), 0);
					collision = true;
					break;
				}
			}
			if (collision) break;
			distance++;
		}
		break;
	case RIGHT:
		for (short x = posMax.X; x < posMax.X + maxDistance; x++)
		{
			for (short y = posMin.Y; y < posMax.Y; y++)
			{
				if (collisionMap[y][x] == 1)
				{
					player->relativeMove(distance, 0);
					collision = true;
					break;
				}
				
			}
			if (collision) break;
			distance++;
		}
		break;
	}	
	

	return (collision);
}

void GAME::putInCollisionMap (BIG_OBJECT &temp)
{
	short x = 0;
	short y = 0;

	POINT_ atPosition = temp.location;

	short nullColor = temp.bitmap->back_color[0][0];

	for (short i = atPosition.Y; i < atPosition.Y + temp.bitmap->sizeY; i++)	{
		x = 0;
		for (short j = atPosition.X; j < atPosition.X + temp.bitmap->sizeX; j++)		{
			if (temp.bitmap->back_color[y][x] != nullColor &&
				temp.bitmap->text_color[y][x] != nullColor)			{
				collisionMap[i][j] = 1;
			}
			x++;
		}
		y++;
	}
}

void GAME::zeroCollisionMap()
{
	for (short i = 0; i < DISPLAY_Y; i++)
		for (short j = 0; j < DISPLAY_X; j++)
			collisionMap[i][j] = 0;
}
void GAME::makeColorBasedCollisionMap()
{
	/*
	0 = BLACK		1 = BLUE		2 = GREEN		3 = CYAN		4 = RED
	5 = MAGENTA		6 = BROWN		7 = LIGHT_GRAY	8 = DARK_GRAY	9 = LIGHT_BLUE
	10 = LIGHT_GREEN	11 = LIGHT_CYAN		12 = LIGHT_RED		13 = LIGHT_MAGENTA
	14 = YELLOW		15 = WHITE
	*/
	char *textColor;
	char *backColor;
	short mainBackColor = worldMap[zone_i][zone_j]->back_color[0][0];
	for (short i = 0; i < DISPLAY_Y; i++)
	{
		for (short j = 0; j < DISPLAY_X; j++)
		{
			textColor = &worldMap[zone_i][zone_j]->text_color[i][j];
			backColor = &worldMap[zone_i][zone_j]->back_color[i][j];
			
			if (*textColor == 0) collisionMap[i][j] = 1;
			else collisionMap[i][j] = 0;
			// COR DE FUNDO E DE TEXTO = COR DE GRAMA E TERRA -> TRUE -> PASSAGEM LIVRE; FALSE -> COLISAO
			/*
			if ((*textColor == 14 ||
				 *textColor == 13 ||
				 *textColor == 3  ||
				 *textColor == 2  ||
				 *textColor == 10 ||
				 *textColor == 6) &&
				(*backColor == 14 ||
				 *backColor == 13 ||
				 *backColor == 3  ||
				 *backColor == 2  ||
				 *backColor == 10 ||
				 *backColor == 6))
				collisionMap[i][j] = 0;

			else 
				collisionMap[i][j] = 1;	
			*/

		}
	}
}

void GAME::showCollisionMap()
{
	gotoxy(0, 0);
	for (short i = 0; i < DISPLAY_Y; i++)
		for(short j = 0; j < DISPLAY_X; j++)
			if (!(i == DISPLAY_Y - 1 && j == DISPLAY_X - 1))
				cout << (int)collisionMap[i][j];
}
// ------------- OUTROS -----------------------------------------------------------------------------
void GAME::step(bool kbHit)
{
	mapChange = false;
	if (kbHit && player->roundBehavior < 4 && player->roundBehavior >= 0)
		movePlayer();
	
	creatureCollisionCheck();

	if (player->roundBehavior >= 5 && player->roundBehavior <= 8)
		playerAttack();
	
	if (mapChange){
		makeRelationOfMapObjects();
		zeroCollisionMap();
		makeColorBasedCollisionMap();
		updateBuffer();
		drawMap();
	}

	// --------------- "VEZ" DO COMPUTADOR ----------------------------------------------
	drawPlayer();
	behaveTORTUGA();
	drawTORTUGAS();

	// --------------- MOSTRAR STATS DO JOGADOR -----------------------------------------
	//jogo_.showPlayerStats(statsPos);
}
void GAME::erase (POINT_ start, POINT_ end)
{
	for (short i = start.Y; i <= end.Y - 1; i++)
	{
		for (short j = start.X; j <= end.X - 1; j++)
		{
			gotoxy(j, i);
			textcolor(DOS_COLORS(worldMap[zone_i][zone_j]->text_color[i][j]));
			backcolor(DOS_COLORS(worldMap[zone_i][zone_j]->back_color[i][j]));
			cout << worldMap[zone_i][zone_j]->map[i][j];
		}
	}
}
void GAME::updateBuffer()
{
		POINT_ start, end;
		start.X = 0; start.Y = 0;
		end.X = DISPLAY_X; end.Y = DISPLAY_Y;
		bufferGets (*worldMap[zone_i][zone_j], start, end);
}
// ------------- SCRIPT (MORTE, VIDA EXTRA, PEGAR OBJETOS, ACONTECIMENTOS, ETC) ---------------------
bool GAME::creatureCollisionCheck()
{
	POINT_ pPos = player->returnPosition();
	POINT_ pSize = player->returnSize();
	POINT_ pLim;
	pLim.X = pPos.X + pSize.X;
	pLim.Y = pPos.Y + pSize.Y;

	POINT_ tSize = tortuga[0].returnSize();
	POINT_ tLim;
	bool coughtAtAll = false;
	bool coughtByThisOne = false;
	for (short i = 0; i < numberOfTORTUGAS; i++)
	{
		coughtByThisOne = false;
		POINT_ tPos = tortuga[i].returnPosition();
		tLim.X = tPos.X + tSize.X;
		tLim.Y = tPos.Y + tSize.Y;

		if (// JOGADOR COM TORTUGA
			((pPos.X < tLim.X && pPos.X > tPos.X) &&	// PELA ESQUERDA E POR BAIXO
			(pPos.Y < tLim.Y && pPos.Y > tPos.Y)) ||

			((pLim.X > tPos.X && pLim.X < tLim.X) &&	// PELA DIREITA E POR CIMA
			(pLim.Y > tPos.Y && pLim.Y < tLim.Y)) ||	

			((pPos.X < tLim.X && pPos.X > tPos.X) &&	// PELA DIREITA E POR BAIXO
			(pLim.Y > tPos.Y && pLim.Y < tLim.Y)) ||

			((pLim.X > tPos.X && pLim.X < tLim.X) &&	// PELA ESQUERDA E POR BAIXO
			(pPos.Y < tLim.Y && pPos.Y > tPos.Y)) 
			||			
			// ------------ TORTUGA COM JOGADOR ------------------------------------

			((tPos.X < pLim.X && tPos.X > pPos.X) &&	// PELA ESQUERDA E POR BAIXO	
			(tPos.Y < pLim.Y && tPos.Y > pPos.Y)) ||

			((tLim.X > pPos.X && tLim.X < pLim.X) &&	// PELA DIREITA E POR CIMA
			(tLim.Y > pPos.Y && tLim.Y < pLim.Y)) ||	

			((tPos.X < pLim.X && tPos.X > pPos.X) &&	// PELA DIREITA E POR BAIXO
			(tLim.Y > pPos.Y && tLim.Y < pLim.Y)) ||

			((tLim.X > pPos.X && tLim.X < pLim.X) &&	// PELA ESQUERDA E POR BAIXO
			(tPos.Y < pLim.Y && tPos.Y > pPos.Y))) 
		{coughtByThisOne = true;}
		
		if (coughtByThisOne){
			playerCollideWithTortuga(i);
			coughtAtAll = true;
		}
	}

	return (coughtAtAll);
}

