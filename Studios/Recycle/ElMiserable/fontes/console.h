// console.h
// 
#if !defined CONSOLE_H
#define CONSOLE_H

#include <iostream>
#include <string>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <windows.h>
#include <string>
using namespace std;

// Função para limpar a tela no modo console windows
void clrscr();

// Função para posicionar o cursos na coordenada (x,y) na console
void gotoxy(int coluna, int linha);

//função para alterar a janela console para para tela-cheia (full screen) e vice-versa
void FullScreen();

// emite um som pelo auto-falante com a frequencia e duracao especificada
void SomBeep(DWORD frequencia, DWORD duracao);

// toca um som em arquivo WAV pela placa de som
// Se bEmLoop for true, o som é executado de modo contínuoo até
// a função SomPararWAV ser chamada.
void SomTocarWAV(char *nomeArquivoWAV, bool bEmLoop = false);

// interrompe a execucao do som que esta sendo tocado
void SomPararWAV();

// versao da biblioteca
const string LibVersion();

// informacoes sobre a biblioteca
const string LibDisclaimer();

// data da disponibilizacao da versao da biblioteca
const string LibReleaseDate();

// Cores possíveis de utilizar com as funções textcolor e backcolor
enum DOS_COLORS {
    BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN,
    LIGHT_GRAY, DARK_GRAY, LIGHT_BLUE, LIGHT_GREEN, LIGHT_CYAN,
    LIGHT_RED, LIGHT_MAGENTA, YELLOW, WHITE };

// Define a cor do caracter
void textcolor (DOS_COLORS iColor);

// Define a cor de fundo do caracter
void backcolor (DOS_COLORS iColor);

// Executa a leitura do teclado de um string (vetor de char) que contém espaços.
// A string lida está no argumento buffer.
void LerStringComEspacos(char buffer[], int tamanho_buffer);
#endif
