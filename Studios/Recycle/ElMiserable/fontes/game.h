#include "..\objects & creatures\objects.h"

#define MAX_ZONES_I 6
#define MAX_ZONES_J 6
#define MAX_TORTUGAS 10 

struct RELATION{
	char type;
	POINT_ location;
};


class GAME
{
	//----------- INICIALIZACAO ----------------------------------------------------------------------------------------
public:
	GAME (short linha, short coluna);

	//----------- FONTE DO JOGO (TEXTO)---------------------------------------------------------------------------------
public:
	void useFont(TEXT_ &temp);	
private:
	TEXT_ *gameFont;

	//----------- MAPA -------------------------------------------------------------------------------------------------
public:
	void useMap(MAP &temp, short i, short j);
	void drawMap();
private:
	MAP *worldMap[6][6];
	short zone_i;
	short zone_j;
	bool mapChange;			// utilitário

	// ---------- JOGADOR ----------------------------------------------------------------------------------------------
public:
	void player_is(PLAYER &temp);
	void drawPlayer();
	bool movePlayer();			// testa colisão, testa se chegou no fim do mapa, se sim troca o mapa, move o PLAYER
	bool checkPlayerMapChange();
	void playerAttack();
	void playerStartAttack();
	bool collide(BEHAVIOR dir);			// se houve colisão, aproxima o objeto o máximo do ponto de colisão. Retorna se houve colisão.
	void playerCollideWithTortuga(short); // retorna se a colisao empurrou o jogador p o prox mapa ou nao
	void showPlayerStats(POINT_);
private:
	PLAYER *player;

	// ---------- INIMIGOS ---------------------------------------------------------------------------------------------
public:
	// ->TORTUGA
	void useTORTUGA (TORTUGA temp[]);
	void moveTORTUGA (short);
	short numberOfTORTUGAS;
	void behaveTORTUGA ();
	void drawTORTUGAS ();
private:
	TORTUGA *tortuga;
	// ---------- OUTROS OBJETOS ---------------------------------------------------------------------------------------
public:
	void drawOnMap(BIG_OBJECT &);
	void useHouse1(BIG_OBJECT &);
	void useTree1 (BIG_OBJECT &);
private:
	BIG_OBJECT *house1, *tree1;

	// ---------- COLISÕES ---------------------------------------------------------------------------------------------
public:
	void putInCollisionMap (BIG_OBJECT &);
	void zeroCollisionMap();
	void showCollisionMap();
	void makeColorBasedCollisionMap(void);
private:
	char collisionMap[160][360];

	// ---------- OUTROS -----------------------------------------------------------------------------------------------
public:
	void step (bool kbHit);								// atualiza o jogo
	void erase (POINT_, POINT_);				// apaga um quadrado (desenha o mapa de fundo)
	void updateBuffer();						// carrega o buffer com o mapa todo
	void makeRelationOfMapObjects(); // cria a relação de objetos no mapa e já apaga o que for referência
private:
	RELATION mapObjects[64];

	// ---------- SCRIPT (MORTE, VIDA EXTRA, PEGAR OBJETOS, ACONTECIMENTOS, ETC) ---------------------------------------
public:
	bool creatureCollisionCheck (); // checagem de rotina: se um inimigo atacou o jogador

private:
};