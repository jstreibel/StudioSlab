
#include <SFML/Graphics.hpp>

#include <iostream>

bool TestSFML() {
    // Setting GLIBCXX_USE_CXX11_ABI=0 will kind of solve CUDA incompatibility problem,
    // but then SFML gets weird, and the following line produces a alloc error.

    sf::RenderWindow window = sf::RenderWindow(
            sf::VideoMode(1920, 1080),
            "SFML Window");

    return true;
}

void ProduceError() {
    std::string hai = "hai";
    std::string ho = "ho";

    std::cout << hai.append(ho.begin(), ho.end()) << std::endl;
}

int main() {
    TestSFML();

    ProduceError();

    return 0;
}