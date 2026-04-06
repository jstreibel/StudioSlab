#include "..\graphics\graphics.h"
#include <cstdlib>
#include <time.h>

#define DEFAULT_SPEED 6 // 6 = velocidade lateral sincronizada com a animacao do SPRITE do ZELDA

enum BEHAVIOR {UP, DOWN, LEFT, RIGHT, DO_NOTHING, ATTACK_UP, ATTACK_DOWN, ATTACK_LEFT, ATTACK_RIGHT};

int square(int num);
int sqRoot(int num);
int returnDistanceBetween (POINT_ tPos, POINT_ pPos);
struct COLLISION_AREA{
	POINT_ start;
	POINT_ end;
};
class BIG_OBJECT
{
public:
	void uses_sprite(BIG_SPRITE &temp);
	POINT_ location;
	BIG_SPRITE *bitmap;
private:	
};

class OBJECT
{
	// --------------- SPRITES, IMAGENS E FUNCOES DE DESENHO ---------------------------
public:
	void uses_sprite(SPRITE &temp);		//qual o SPRITE que este objeto usa
	void draw();						//desenha esse objeto
	void draw(SPRITE &colorMap);		//desenha esse objeto com uma mapa de fundo para a transparencia
	SPRITE returnSprite();
private:
	SPRITE *bitmap;						//endereço do SPRITE q este objeto usa
	// --------------- LOCALIZACAO, AREA, TAMANHO, ETC ----------------------------------
public:
	POINT_ returnSize();				//retorna o limite direito inferior do objeto
	POINT_ location;					//localização na tela deste objeto
	POINT_ returnPosition();			//retorna o limite esquerdo superior do objeto
	void placeAt(short x, short y);		//coloca este objeto para a posição x y
	COLLISION_AREA collisionArea;
	void isSolid (bool tellMe);			//diz se o obj eh solido ou n (NAO UTILIZADO)
	void isFixed (bool tellMe);			//diz se o obj eh fixo na tela ou n (NAO UTILIZADO)
private:
	bool fixed;							//(NAO UTILIZADO)
	bool solid;							//(NAO UTILIZADO)
};

class CREATURE:public OBJECT
{
public:
	// ---------------- INICIALIZACAO ---------------
	CREATURE();

	// ---------------- SPRITES ---------------------
public:
	void upWalkSprite(SPRITE []);		// sprites de caminhada para cima
	void downWalkSprite(SPRITE []);		// sprites de caminhada para baixo
	void leftWalkSprite(SPRITE []);		// sprites de caminhada para a esquerda
	void rightWalkSprite(SPRITE []);	// sprites de caminhada para a direita
		
	void upAttackSprite(SPRITE []);		// sprites de ataque para cima
	void downAttackSprite(SPRITE []);	// sprites de ataque para baixo
	void leftAttackSprite(SPRITE []);	// sprites de ataque para a esquerda	
	void rightAttackSprite(SPRITE []);	// sprites de ataque para a direita

	void swapSpriteCorrection();		// corrige diferencas de tamanho entre sprites (na posicao do jogador)

	short walkSprites;					// numero de sprites utilizados na movimentação
	short attackSprites;				// numero de sprites utilizados no ataque
	short attackIndex;					// quadro do sprite de ataque utilizado no momento
private:
	POINT_ difference;					// diferenca de tamanhos para coordenar a posicao do jogador na troca de sprites
	short spriteIndex;					// quadro do sprite de caminhada utilizado no momento
	SPRITE *upWalk;	
	SPRITE *upAttack;
	SPRITE *downWalk;
	SPRITE *downAttack;
	SPRITE *leftWalk;
	SPRITE *leftAttack;
	SPRITE *rightWalk;
	SPRITE *rightAttack;

	// -------------- ATRIBUTOS --------------------
public:
	void attributes (short spd, short enrgy, short mana_);
	void takeDamage (short);
	short returnLife ();
	short returnSpeed();
	short delaySpeed;
	short delayCorrection;
private:
	short int speed, energy, mana;   //atributos basicos (???)
	short int		 energyMax, manaMax;
	COLLISION_AREA playerCollisionArea;

	// --------------- MOVIMENTACAO E ACOES ---------
public:
	void move (BEHAVIOR &go);			// move este objeto na direção go
	short attack ();					// atacar, retorna em qual momento do ataque a criatura esta
	void stopAttack ();
	BEHAVIOR roundBehavior;				// comportamento da criatura na rodada
	void relativeMove(short x, short y);// move este objeto x, y unidades
};


// ___________________________________________________________________________________________
// ________________ TORTUGA __________________________________________________________________
class TORTUGA:public CREATURE
{
public:
	// --------------- INICIALIZACAO ------------------------------------------
	TORTUGA();
	// --------------- SISTEMA DE ROTA E I.A. DA TORTUGA ----------------------
public:
	void getPathPoint (short, POINT_);	// ENVIA PARA A TORTUGA UMA POSICAO E O SEU INDICE PARA SEREM ADICIONADOS A SUA ROTA
	void zeroPathNodes ();				// ZERA O CAMINHO TA TORTUGA
	BEHAVIOR think(POINT_ pPos);					// Faz a TORTUGA decidir o que fazer na rodada
private:
	short choice;						// pequena variável que recebe valor aleatorio para ajudar a TORTUGA em suas decisoes
	short isGoingToNode;				// o noh para o qual a TORTUGA esta indo no momento
	short pathNodes;					// numero total de nós no caminho da tortuga
	POINT_ waypoint[10];				// os nós da rota da TORTUGA
	// --------------- QUESTOES EXISTENCIAIS DA TORTUGA -----------------------
public:
	bool isOnMap();						// retorna verdadeiro ou falso, dependendo de a tartaruga existir ou nao
	void isOnMap(bool);					// recebe um valor que vai dizer se a tortuga existe ou nao
private:	
	bool exists;						// armazena verdadeiro ou falso, dependendo de a tartaruga existir ou nao
};

class PLAYER:public CREATURE		//sujeito à criatividade dos programadores
{
public:
	PLAYER();
	
private: 
};

class npc:public CREATURE			//sujeito à criatividade dos programadores
{};