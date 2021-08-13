#include "Viewer.h"

const int larguraDaJanelaEDoBitmap=600, alturaDaJanelaEDoBitmap=600;

int main() {
    Viewer janela1(larguraDaJanelaEDoBitmap, alturaDaJanelaEDoBitmap, "Fake wicked Ising");
    Viewer janela2(larguraDaJanelaEDoBitmap, alturaDaJanelaEDoBitmap, "Sierpinski retangulo");

    int numeroDeColores = 3;
    sf::Color algunasColores[] = {sf::Color::White, sf::Color::Red, sf::Color::Green};
    sf::Vector2f pontosDeUmTriangulo[] = {{0.0, 0.0},
                                          {1.0, 1.0},
                                          {0.0, 1.0}};
    sf::Vector2f ponto = {float(rand())/RAND_MAX, float(rand()/RAND_MAX)};

    while(true){
        janela1.trataEventos();
        janela2.trataEventos();


        /** AQUI VOCE PODE COLOCAR TODA A SIMULACAO E MALUQUICES **************************************
         *
         * Com o comando janela.setPixel, voce desenha o que quiser.
         */
        for(int i=0; i<10; i++) {
            sf::Color color = algunasColores[rand() % numeroDeColores];
            int x = rand() % larguraDaJanelaEDoBitmap, y = rand() % alturaDaJanelaEDoBitmap;
            janela1.setPixel(x, y, color);
        }

        for(int i=0; i<10; i++){
            sf::Vector2f pontoAleatorioDoTriangulo = pontosDeUmTriangulo[rand() % 3];
            ponto = (ponto+pontoAleatorioDoTriangulo)/2.0f;
            janela2.setPixelEntreZeroEUm(ponto.x, ponto.y, sf::Color::Yellow);
        }
        /*********************************************************************************************/


        janela1.desenhaTudo();
        janela2.desenhaTudo();
    }

    return 0;
}
