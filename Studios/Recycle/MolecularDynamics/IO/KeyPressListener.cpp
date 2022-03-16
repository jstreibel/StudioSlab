//
// Created by joao on 16/05/2020.
//

#include "KeyPressListener.h"
#include "UserInput.h"

KeyPressListener::KeyPressListener() {
    UserInput::getInstance()->addKeyPressListener(this);
}
