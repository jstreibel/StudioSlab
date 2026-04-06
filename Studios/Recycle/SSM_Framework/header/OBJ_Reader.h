/* 
 *	Unisinos - São Leopoldo
 *	GT - JEDi
 *	Computação Gráfica - 2008/2
 *	Prof. Leandro Tonietto
 *	Alunos: João Saldanha Streibel
 *			Deividson Muller Goulart
 *
 *
 *		ARQUIVO OBJ_Reader.h - Definição e declaração da classe OBJ_Reader: carregamento de vértices,
 *	faces, normais, coordenadas textura e grupos de um arquivo Wavefront OBJ.
 *
 *	Input:
 *		- Referência para um objeto vazio do tipo Mesh: pode ser passado pelo construtor ou pelo
 *			método OBJ_Reader::setMeshToLoad( Mesh *toLoad );
 *		- Nome do arquivo a ser carregado: pode ser passado pelo construtor ou pelo método
 *			OBJ_Reader::setFileName( char *file_name ).
 *
 *	Output:
 *		- Inunda o objeto Mesh passado por referência com vértices, faces, normais e coorde-
 *			nadas de textura, de acordo com o que houver disponível no arquivo .obj .
 *
 *	Funcionamento:
 *			Depois de passados o nome do arquivo e o Mesh para dentro do qual o arquivo deverá
 *		ser carregado, é preciso chamar o método OBJ_Reader::Load( ), que irá efetuar o carregamento em si.
 *		Se necessário, é possível pegar a referência para o Mesh carregado através do método
 *		OBJ_Reader::getMesh( ), que retorna a posição de memória do Mesh carregado.
 *
 *
 *	Unisinos - São Leopoldo
 *	GT - JEDi
 *	Computação Gráfica - 2008/2
 *	Prof. Leandro Tonietto
 *	Alunos: João Saldanha Streibel
 *			Deividson Muller Goulart
 */




/*	***************** ATENÇÃO *****************************************************
		TO DO
	As coordenadas de textura podem vir como três coordenadas ou duas coordenadas.
	O tratamento adequado ainda não foi implementado.
	
	[ 09/09/2008 ]

	******************************************************************************* */
#pragma once

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "MeshStructs.h"
#include "conversor.h"

using namespace std;

#define COMENTARIO		'#'
#define QUEBRA_DE_LINHA	'\n'
#define VERTICE			'v'
#define TEXTURA			't'
#define NORMAL			'n'
#define FACE			'f'
#define GRUPO			'g'
#define ONE_BYTE		1

class OBJ_Reader
{
public:
	static bool load(Mesh *current_mesh, char *obj_filename){
		ifstream obj_file( obj_filename, ios::in );
		if( !obj_file.is_open( ) ){
			cout << "OBJ_Loader ERRO: o arquivo \"" << obj_filename << "\" não pode ser aberto. Abortando operação de carregamento.\n";
			return false;
		}

		// char leitura[] --> é um array temporário para dentro donde será feita a leitura do arquivo passo-a-passo.
		char leitura[1024];	// 1024 bytes é um exagero pra esse caso, sim eu sei, mas é só um KB pra garantir...
		for(int i=0; i<1024; ++i) leitura[i] = '\0';
//[dmg]leitura é só pra uma linha pro vez	
		//	bool efetua_leitura --> a cada ciclo do while de leitura do arquivo (que inicia logo abaixo),
		// é checado se é necessário fazer a leitura da próxima palavra do arquivo. Isso acontece porque em
		// alguns casos uma face pode ter mais de três vértices, e na parte de leitura de faces é feita uma leitura
		// temporária pra dentro do char leitura[] . Uma vez que essa leitura foi feita, é checado se o que foi
		// lido trata-se de um valor numérico ou de outro valor qualquer. Caso seja valor numérico, o array é 
		// convertido para um valor inteiro (ou float), e a leitura do arquivo segue normal. No entanto, se é
		// detectado que foi lido um valor não-numérico (a próxima linha), então o bool efetua_leitura é setado
		// pra false , pq o que seria lido no ciclo seguinte do 'for' já foi lido.
		bool efetua_leitura = true;

		Vertex v_temp;
		TextureCoord tex_coord;
		Group *group_temp = NULL;
		Face *face_temp = NULL;

		/*	Toda a leitura do arquivo .obj é feita dentro do loop abaixo.
		 *	São lidos caracteres até o espaço ou quebra de linha seguinte.
		 *	Se uma linha inicia por '#', o cursor é colocado na linha seguinte no arquivo,
		 * o que é feito pelo método nextLine(...) desta mesma classe.
		 * 
		 *	A checagem acerca da natureza da linha (vértice, coordenada de textura, normal, face, etc. )
		 * é feita em diversos 'if', sendo que cada um termina em um 'continue', de forma que não
		 * são feitos os 'if' seguintes.
		 *
		 *	Quando a leitura é deparada com um novo grupo (linha iniciando por 'g'), então é feita criado
		 * um novo 'Group' e o anterior é deletado caso existisse.
		 *
		 *	Quando é encontrada uma face, esta face é jogada dentro do grupo atual, e caso este não exista,
		 * é então criado um novo grupo.
		 */
//[dmg]==============================================================
		
		while( 1 ){
			/*
			 *	Quando for detectado que o arquivo sendo lido chegou ao fim, então
			 * o último grupo criado é mandado para o current_mesh e depois é destruído.
			 */
			if( obj_file.eof( ) ){//[dmg]EOF acho q é fim de arquivo
				if( group_temp ){//[dmg]quando termina o arq ele passa o ultimo group pro mesh, e o deleta
					current_mesh->addGroup( *group_temp );
					delete group_temp;
				}
				if( face_temp )
					delete face_temp;

				/*dance*/break;
			}

			if( efetua_leitura )//[dmg]
				obj_file >> leitura;
			else efetua_leitura = true;

			if( leitura[0] == COMENTARIO ){
				nextLine( obj_file );
				continue;//[dmg]volta pro inicio do while
			}

			// v = vértice
			// vt = coordenada de textura
			// vn = normal
			// f = face
			// g = grupo
			if( leitura[0] == VERTICE ){
				// se for 'vt' --> coordenada de textura
				if( leitura[1] == TEXTURA ){//[dmg]no caso de duas letras juntas, vt o 't' fica no leitura[1]
					obj_file >> tex_coord.U >> tex_coord.V;
					//	As próximas linhas são para checar se existe uma terceira coordenada
					// textura no .obj (o 3DS Max, por exemplo, salva três coordenadas).
					obj_file >> leitura;
					if( !isNumber( leitura ) ){
						tex_coord.W = 0.f;
						// bool efetua_leitura = false : no próximo loop do while principal
						// desta funcao não será lido para dentro do char leitura[], pq já foi
						// lido (na linha acima, para testar se havia uma terceira coordenada
						// de textura.
						efetua_leitura = false;//[dmg] se nao for numero, coloca false p nao ler de novo no proximo loop
					}
					else tex_coord.W = convertStringToFloat( leitura );
					
					current_mesh->addTextureCoordinate( tex_coord );
					continue;
				}
				// se for 'vn' --> normal
				if( leitura[1] == NORMAL ){
					obj_file >> v_temp.X >> v_temp.Y >> v_temp.Z;
					current_mesh->addNormal( v_temp );
					continue;
				}
				// se for só 'v' --> vértice
				obj_file >> v_temp.X >> v_temp.Y >> v_temp.Z;
				current_mesh->addVertex( v_temp );
				continue;
			}
			/*
			 *	Se for detectada a existência de um novo grupo, este é criado. 
			 *	No momento em que o novo grupo é criado, é checado se havia outro grupo
			 * já criado. Se sim, este grupo é passado para dentro do Mesh *current_mesh
			 * e então deletado (porque a classe Mesh recebe cópias).
			 */
			if( leitura[0] == GRUPO ){
				if( group_temp ){
					current_mesh->addGroup( *group_temp );
					delete group_temp;
				}
				group_temp = new Group;//[dmg]esse new deixa NULL no ponteiro pra ele conferir no proximo loop???ou teria q ter group_temp=NULL; aqui?
				nextLine( obj_file );
				continue;
			}
			if( leitura[0] == FACE ){
				if( !group_temp ) group_temp = new Group;
				if( face_temp ) delete face_temp;
				face_temp = new Face;

				int int_vert, int_tex_coord, int_normal;

				// Esse 'for' bizarro eu vi os profissionais fazendo!
				for(;;){
					obj_file >> leitura;
					// Essa checagem abaixo é feita porque nunca se sabe quantas arestas terá a face...
					if( isNumber( leitura ) ){
						int_vert		= findVertexIndex( leitura );
						int_tex_coord	= findTextureCoordIndex( leitura );
						int_normal		= findNormalIndex( leitura );
						face_temp->addVertexIndex( int_vert );
						if( int_tex_coord != -1 )
							face_temp->addTextureCoordinateIndex( int_tex_coord );
						if( int_normal != -1 )
							face_temp->addNormalIndex( int_normal );
						continue;
					}
					group_temp->addFace( *face_temp );
					efetua_leitura = false;
					break;
				}
				continue; // o while principal.
			}
		}	
		obj_file.close();

		return true;
	};

private:
	static inline void nextLine( ifstream &obj_file ){
		/*char procura;
		do{
			obj_file.read( &procura, ONE_BYTE );//[dmg]lê(passa pro procura)byte a byte, até chegar no '/n', ou terminar o arquivo
		}while( procura != QUEBRA_DE_LINHA && !(obj_file.eof()) );*/

		char buffer[2048];
		obj_file.getline(buffer, 2048);
	}
	/*
	 *	As funções internas (métodos privados se preferir) abaixo recebem por parâmetro
	 * um vetor de caracteres do tipo face de um arquivo .obj (formato padrão v/t/n),
	 * e extrai deste vetor somente um dos valores (ou vértice, ou textura, ou normal)
	 * contidos nele.
	 *
	 *	Por ser possível que o arquivo não contenha coordenadas de textura ou mesmo normais,
	 * então os métodos que procuram estes valores irão retornar -1 se não os encontrarem.
	 *
	 */
	static inline int findVertexIndex( char face_string[] ){
		static char vertex_index[32];
		static int i;
		i=0;
		while( (face_string[i] != '/') && (face_string[i] != '\0') ){
			vertex_index[i] = face_string[i++];
		}
		vertex_index[i] = '\0';
		
		return convertStringToInt( vertex_index );
	}
	static inline int findTextureCoordIndex( char face_string[] ){
		static char tex_coord_index[32];
		static int i;
		i=0;
		/*	Procura o início do valor, que é depois da primeira barra. Se não houver essa
		 * primeira barra ou se o caractere seguinte à ela for outra barra, então retorna
		 * uma negativa indicando inexistência deste valor.
		 */

		// sobre esse 'for' bizarro aih embaixo: eu vi os profissionais fazendo e fiz igual.
		for(;;){
			if( face_string[i] == '/' ){
				if( face_string[i+1] == '/' )
					return -1;
				++i;
				break;
			}
			if ( (face_string[i] == '\0') )
				return -1;
			++i;
		}

		int j=0;
		while( (face_string[i] != '/') && (face_string[i] != '\0') )
			tex_coord_index[j++] = face_string[i++];

		tex_coord_index[j] = '\0';

		return convertStringToInt( tex_coord_index );
	}
	static inline int findNormalIndex( char face_string[] ){
		static char normal_index[32];
		static int i;
		i=0;
		bool already_found_a_slash = false;
		/*	No 'for' abaixo, o 'int i' é setado com o valor correspondente ao caractere
		 * anterior àquele que representa o valor inteiro que contém o índice do vetor 
		 * de normais correspondente.
		 */
		for(;;){
			if( face_string[i] == '\0' )
				return -1;
			if( face_string[i++] == '/' ){
				if( already_found_a_slash )
					break;
				already_found_a_slash = true;
			}
		}
		
		int j=0;
		for(;;){
			normal_index[j++] = face_string[i++];
			// Ponto de parada é o fim da string.
			if( face_string[i] == '\0' )
			{
				normal_index[j] = '\0';
				break;
			}
		}
		return convertStringToInt( normal_index );
	}
	/*
	 *	bool isFaceSet(...)
	 * retorna verdadeiro se a string passada iniciar com um caractere que represente
	 * um número e falso se iniciar com qualquer outro caractere.
	 */
	static inline bool isNumber( char chars[] ){
		// (char)48 = '0'
		// (char)57 = '9'
		if( ((chars[0] > 47) && (chars[0] < 58)) || chars[0] == '-')
			return true;
		else return false;
	}

};
