#ifndef USERINPUT_H
#define USERINPUT_H


#include "SFML/Graphics/RenderWindow.hpp"
#include "KeyPressListener.h"
#include "KeyReleaseListener.h"


class UserInput
{
private:
    static UserInput *myInstance;
	sf::RenderWindow *mWindow;
private:
	explicit UserInput(sf::RenderWindow *window);

public:
    static void Initialize(sf::RenderWindow *window);
    static UserInput* getInstance();

public:
	void treatEvents();
	bool isFinished() const;
	void addKeyPressListener(KeyPressListener *keyPressListener);

private:
	bool mFinished;
	std::vector<KeyPressListener*> keyPressListeners;
};

#endif // USERINPUT_H
