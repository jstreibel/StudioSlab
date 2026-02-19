#include "stdafx.h"
#include "objects.h"

int square(int num){return (num * num);}
int sqRoot(int num){
   int i;
   int sqr;
   for(i = 0;;i++){
      if(i*i == num){
         sqr = i;
         break;
      }
      if(i*i < num)
         continue;
      if(i*i > num){
         sqr = i-1;
         break;
      }
   }
   return sqr;
}
int returnDistanceBetween (POINT_ tPos, POINT_ pPos){
	short x1, x2, y1, y2;
	short distance = 0;
	if (tPos.X <= pPos.X){
		x1 = tPos.X;
		x2 = pPos.X;
	}
	else if (pPos.X < tPos.X){
		x1 = pPos.X;
		x2 = tPos.X;
	}

	if (tPos.Y <= pPos.Y){
		y1 = tPos.Y;
		y2 = pPos.Y;
	}
	else if (pPos.Y < tPos.Y){
		y1 = pPos.Y; // / 2;
		y2 = tPos.Y; // / 2;
	}
	distance = sqRoot (square(x2 - x1) + square(y2 - y1));
	return (distance);
}
void BIG_OBJECT::uses_sprite(BIG_SPRITE &temp){bitmap = &temp;}
// _____________ OBJECT ________________________________________________|
// ------------- SPRITES, IMAGENS E FUNCOES DE DESENHO -------|
void OBJECT::uses_sprite(SPRITE &temp){bitmap = &temp;}
void OBJECT::draw(){bitmap->drawSpr(location);};
void OBJECT::draw(SPRITE &colorMap){bitmap->drawSpr(location, colorMap.text_color, colorMap.back_color);}
SPRITE OBJECT::returnSprite(){return (*bitmap);}
// ------------- LOCALIZACAO, AREA, TAMANHO, ETC. ------------|
POINT_ OBJECT::returnSize()
{
	POINT_ max;
	max.X = bitmap->sizeX;
	max.Y = bitmap->sizeY;
	return (max);
};
POINT_ OBJECT::returnPosition()
{
	POINT_ min;
	min.X = location.X;
	min.Y = location.Y;
	return (min);
};
void OBJECT::placeAt(short x, short y)
{
	location.X = x;
	location.Y = y;
}
// _____________ CREATURE ______________________________________________|
// ------------- INICIALIZACAO -------------------------------|
CREATURE::CREATURE()
{
	speed = DEFAULT_SPEED;
	uses_sprite(downWalk[0]);
	spriteIndex = 0;
	attackIndex = 0;
}
// ------------- SPRITES -------------------------------------|
void CREATURE::upWalkSprite(SPRITE up[]){upWalk = up;}
void CREATURE::upAttackSprite(SPRITE up[]){upAttack = up;}
void CREATURE::downWalkSprite(SPRITE down[]) 
{
	downWalk = down;
	uses_sprite(downWalk[0]);
}
void CREATURE::downAttackSprite(SPRITE down[]){downAttack = down;}
void CREATURE::leftWalkSprite(SPRITE left[]){leftWalk = left;}
void CREATURE::leftAttackSprite(SPRITE left[]){leftAttack = left;}
void CREATURE::rightWalkSprite(SPRITE right[]){rightWalk = right;}
void CREATURE::rightAttackSprite(SPRITE right[]){rightAttack = right;}
void CREATURE::swapSpriteCorrection(){
	switch (roundBehavior)
	{
	case UP:
		location.X += difference.X;
		location.Y += difference.Y;
		break;
	case DOWN:
		break;
	case LEFT:
		break;
	case RIGHT:
		break;
	case ATTACK_UP:
		difference.X = ((upAttack->sizeX - upWalk->sizeX) / 2) + 4;
		difference.Y = ((upAttack->sizeY - upWalk->sizeY) / 2) + 4;
		location.X -= difference.X;
		location.Y -= difference.Y;
		break;
	case ATTACK_DOWN:
		break;
	case ATTACK_LEFT:
		break;
	case ATTACK_RIGHT:
		break;
	}
}
// ------------- ATRIBUTOS -----------------------------------|
void CREATURE::attributes(short spd, short enrgy, short mana_)
{
	speed = spd;
	energy = enrgy;
	mana = mana_;
}
void CREATURE::takeDamage(short amount){energy -= amount;}
short CREATURE::returnSpeed(){return (speed);}
short CREATURE::returnLife(){return (energy);}
// ------------- MOVIMENTACAO E ACOES ------------------------|
void CREATURE::relativeMove(short x, short y)
{
	location.X += x;
	location.Y += y;
};
void CREATURE::move(BEHAVIOR &go)
{
	if (delayCorrection == delaySpeed)
	{
		switch (go)
		{
		case UP:
			uses_sprite(upWalk[spriteIndex]);
			location.Y -= speed / 2;
			break;
		case DOWN:
			uses_sprite(downWalk[spriteIndex]);
			location.Y += speed / 2;	
			break;
		case LEFT:
			uses_sprite(leftWalk[spriteIndex]);
			location.X -= speed;
			break;
		case RIGHT:
			uses_sprite(rightWalk[spriteIndex]);
			location.X += speed;
			break;
		}
	
	if (spriteIndex < walkSprites - 1)
	spriteIndex++;
	else spriteIndex = 0;
	roundBehavior = go;
	}
	if (delayCorrection < delaySpeed) delayCorrection++;
	else delayCorrection = 0;
};
short CREATURE::attack ()
{
	if (attackIndex < 9)
		switch (roundBehavior)
		{
		case ATTACK_UP:
			uses_sprite (upAttack[attackIndex]);
			break;
		case ATTACK_DOWN:
			uses_sprite (downAttack[attackIndex]);
			break;
		case ATTACK_LEFT:
			uses_sprite (leftAttack[attackIndex]);
			break;
		case ATTACK_RIGHT:
			uses_sprite (rightAttack[attackIndex]);
			break;
		}
	if (attackIndex < attackSprites) attackIndex++;
	else attackIndex = 0;
	return (attackIndex);
}
void CREATURE::stopAttack ()
{
	switch (roundBehavior)
	{

	}
}
// _____________ JOGADOR _______________________________________________|
// ------------- INICIALIZACAO -------------------------------|
PLAYER::PLAYER()
{
	delaySpeed = 0;
	delayCorrection = 0;
}
// _____________ TORTUGA _______________________________________________|
// ------------- INICIALIZACAO -------------------------------|
TORTUGA::TORTUGA()
{
	pathNodes = 0;
	isGoingToNode = 1;
	exists = false;
	choice = 0;
	delaySpeed = 1;
	delayCorrection = 0;
}
// ------------- SISTEMA DE ROTA E I.A. DA TORTUGA -----------|
void TORTUGA::getPathPoint(short number, POINT_ node)
{
	waypoint[number] = node;
	pathNodes++;
}
void TORTUGA::zeroPathNodes()
{
	pathNodes = 0;
	isGoingToNode = 1;
}
BEHAVIOR TORTUGA::think(POINT_ pPos)
{
	BEHAVIOR behavior = DO_NOTHING;
	pathNodes;

	POINT_ forcedLocation;
	forcedLocation.X = location.X;
	forcedLocation.Y = location.Y;
	if (returnDistanceBetween (forcedLocation, pPos) < 60){
		delaySpeed = 0;

		// Correção: para quando este objeto passa o seu ponto	por causa da velocidade
		POINT_ distance;	// distancia do Waypoint
		short minDistance = returnSpeed();	// distancia para se considerar a mudança de rumo
		if (location.X > pPos.X) distance.X = location.X - pPos.X;
		else if (location.X < pPos.X) distance.X = pPos.X - location.X;
		else distance.X = 0;

		if (location.Y > pPos.Y) distance.Y = location.Y - pPos.Y;
		else if (location.Y < pPos.Y) distance.Y = pPos.Y - location.Y;
		else distance.Y = 0;

		if (distance.X > minDistance){
			if (location.X < pPos.X)
				behavior = RIGHT;
			else if (location.X > pPos.X)
				behavior = LEFT;
		}
		
		if (distance.Y > minDistance){
			if (location.Y < pPos.Y)
				behavior = DOWN;
			else if (location.Y > pPos.Y)
				behavior = UP;
		}
	}
	/* ------- TENTATIVA FRUSTRADA DE FAZER A TARTARUGA ANDAR EM DIAGONAL
	if (choice < 9) choice++;
	else choice = 0;
	//choice = rand() % 2;
	
	if (location.X < waypoint[isGoingToNode].X)	{
		behavior = RIGHT;
		if (location.Y < waypoint[isGoingToNode].Y){
			if (choice >= 5 && choice <= 9)
				behavior = DOWN;
		}
		else if (location.Y > waypoint[isGoingToNode].Y)
			if (choice >= 5 && choice <= 9)
				behavior = UP;
	}
	else if (location.X > waypoint[isGoingToNode].X){
		behavior = LEFT;
		if (location.Y < waypoint[isGoingToNode].Y){
			if (choice >= 5 && choice <= 9)
				behavior = DOWN;
		}
		else if (location.Y > waypoint[isGoingToNode].Y)
			if (choice >= 5 && choice <= 9)
				behavior = UP;
	}
	*/
	else {
		delaySpeed = 1;
		// Correção: para quando este objeto passa o seu ponto	por causa da velocidade
		POINT_ distance;	// distancia do Waypoint
		short minDistance = returnSpeed();	// distancia para se considerar a mudança de rumo
		if (location.X > waypoint[isGoingToNode].X) distance.X = location.X - waypoint[isGoingToNode].X;
		else if (location.X < waypoint[isGoingToNode].X) distance.X = waypoint[isGoingToNode].X - location.X;
		else distance.X = 0;

		if (location.Y > waypoint[isGoingToNode].Y) distance.Y = location.Y - waypoint[isGoingToNode].Y;
		else if (location.Y < waypoint[isGoingToNode].Y) distance.Y = waypoint[isGoingToNode].Y - location.Y;
		else distance.Y = 0;


		if (distance.X > minDistance) 
		{
			if (location.X < waypoint[isGoingToNode].X)
				behavior = RIGHT;
			else if (location.X > waypoint[isGoingToNode].X)
				behavior = LEFT;
		}
		else if (distance.Y > (minDistance / 2))
		{
			if (location.Y < waypoint[isGoingToNode].Y)
				behavior = DOWN;
			else if (location.Y > waypoint[isGoingToNode].Y)
				behavior = UP;
		}
		else
		{
			if (isGoingToNode < pathNodes - 1) isGoingToNode++;
			else isGoingToNode = 0;
		}
	}
	
	
	/*
	if (distance.X < minDistance && distance.X > 0 - minDistance) 
		location.X = waypoint[isGoingToNode].X;
	if (distance.Y < (minDistance / 2) && distance.Y > (0 - (minDistance / 2)))
		location.Y = waypoint[isGoingToNode].Y;
	*/

	// trocando o waypoint

	return (behavior);
}
// ------------- QUESTOES EXISTENCIAIS DA TORTUGA ------------|
bool TORTUGA::isOnMap(void){return (exists);}
void TORTUGA::isOnMap(bool change){exists = change;}