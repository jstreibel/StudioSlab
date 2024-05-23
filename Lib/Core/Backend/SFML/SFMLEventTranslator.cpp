//
// Created by joao on 27/09/23.
//

#include "SFMLEventTranslator.h"

#define For      for(auto &
#define Each     listener
#define Listener : guiListeners) listener
// #define Do (WHAT) {listener-> WHAT ;};

namespace Slab::Core {
    inline KeyMap mapFromSFML(sf::Event::KeyEvent);

    void SFMLEventTranslator::event(const sf::Event &event) {
        fix type = event.type;
        bool isKey              = type == sf::Event::KeyPressed || type == sf::Event::KeyReleased;
        bool isMouseButton      = type == sf::Event::MouseButtonPressed
                               || type == sf::Event::MouseButtonReleased;
        bool isMouseMovement    = type == sf::Event::MouseMoved;
        bool isMouseWheel       = type == sf::Event::MouseWheelScrolled;
        bool isResize           = type == sf::Event::Resized;


        if(isKey) {
            auto inkey = event.key;

            KeyMap key = mapFromSFML(inkey);
            KeyState state = type == sf::Event::KeyPressed ? KeyState::Press : KeyState::Release;
            ModKeys modKeys{inkey.shift, inkey.control, inkey.alt, inkey.system, false, false };

            For Each Listener -> notifyKeyboard(key, state, modKeys);
        }

        else if(isMouseButton) {
            MouseButton button;
            switch (event.mouseButton.button) {
                case sf::Mouse::Left:           button = MouseButton_LEFT;  break;
                case sf::Mouse::Right:          button = MouseButton_RIGHT; break;
                case sf::Mouse::Middle:         button = MouseButton_MIDDLE; break;
                case sf::Mouse::XButton1:       button = MouseButton_4; break;
                case sf::Mouse::XButton2:       button = MouseButton_5; break;
                case sf::Mouse::ButtonCount:    button = MouseButton_LAST; break;
            }
            KeyState state = type == sf::Event::MouseButtonPressed ? Press : Release;

            auto modKeys = ModKeys(
            sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) | sf::Keyboard::isKeyPressed(sf::Keyboard::RShift),
            sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) | sf::Keyboard::isKeyPressed(sf::Keyboard::RControl),
            sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) | sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt),
            sf::Keyboard::isKeyPressed(sf::Keyboard::LSystem) | sf::Keyboard::isKeyPressed(sf::Keyboard::RSystem),
            false, false);

            For Each Listener ->notifyMouseButton(button, state, modKeys);
        }

        else if(isMouseMovement)
            For Each Listener ->notifyMouseMotion(event.mouseMove.x, event.mouseMove.y);

        else if(isMouseWheel)
            For Each Listener ->notifyMouseWheel(event.mouseWheelScroll.x, event.mouseWheelScroll.y);

        else if(isResize)
            For Each Listener ->notifyScreenReshape((int)event.size.width, (int)event.size.height);
    }

    void SFMLEventTranslator::render(sf::RenderWindow *window) {
        for(auto &listener : guiListeners) listener->notifyRender();
    }


    inline KeyMap mapFromSFML(sf::Event::KeyEvent keyEvent){
        auto shift = keyEvent.shift;

        switch (keyEvent.code) {
            case sf::Keyboard::Unknown:     return KeyMap::Key_UNKNOWN;

            case sf::Keyboard::A:           return shift ? KeyMap::Key_A : KeyMap::Key_a;
            case sf::Keyboard::B:           return shift ? KeyMap::Key_B : KeyMap::Key_a;
            case sf::Keyboard::C:           return shift ? KeyMap::Key_C : KeyMap::Key_a;
            case sf::Keyboard::D:           return shift ? KeyMap::Key_D : KeyMap::Key_a;
            case sf::Keyboard::E:           return shift ? KeyMap::Key_E: KeyMap::Key_e;
            case sf::Keyboard::F:           return shift ? KeyMap::Key_F: KeyMap::Key_f;
            case sf::Keyboard::G:           return shift ? KeyMap::Key_G: KeyMap::Key_g;
            case sf::Keyboard::H:           return shift ? KeyMap::Key_H: KeyMap::Key_h;
            case sf::Keyboard::I:           return shift ? KeyMap::Key_I: KeyMap::Key_i;
            case sf::Keyboard::J:           return shift ? KeyMap::Key_J: KeyMap::Key_j;
            case sf::Keyboard::K:           return shift ? KeyMap::Key_K: KeyMap::Key_k;
            case sf::Keyboard::L:           return shift ? KeyMap::Key_L: KeyMap::Key_l;
            case sf::Keyboard::M:           return shift ? KeyMap::Key_M: KeyMap::Key_m;
            case sf::Keyboard::N:           return shift ? KeyMap::Key_N: KeyMap::Key_n;
            case sf::Keyboard::O:           return shift ? KeyMap::Key_O: KeyMap::Key_o;
            case sf::Keyboard::P:           return shift ? KeyMap::Key_P: KeyMap::Key_p;
            case sf::Keyboard::Q:           return shift ? KeyMap::Key_Q: KeyMap::Key_q;
            case sf::Keyboard::R:           return shift ? KeyMap::Key_R: KeyMap::Key_r;
            case sf::Keyboard::S:           return shift ? KeyMap::Key_S: KeyMap::Key_s;
            case sf::Keyboard::T:           return shift ? KeyMap::Key_T: KeyMap::Key_t;
            case sf::Keyboard::U:           return shift ? KeyMap::Key_U: KeyMap::Key_u;
            case sf::Keyboard::V:           return shift ? KeyMap::Key_V: KeyMap::Key_v;
            case sf::Keyboard::W:           return shift ? KeyMap::Key_W: KeyMap::Key_w;
            case sf::Keyboard::X:           return shift ? KeyMap::Key_X: KeyMap::Key_x;
            case sf::Keyboard::Y:           return shift ? KeyMap::Key_Y: KeyMap::Key_y;
            case sf::Keyboard::Z:           return shift ? KeyMap::Key_Z: KeyMap::Key_z;
            case sf::Keyboard::Num0:        return KeyMap::Key_0;
            case sf::Keyboard::Num1:        return KeyMap::Key_1;
            case sf::Keyboard::Num2:        return KeyMap::Key_2;
            case sf::Keyboard::Num3:        return KeyMap::Key_3;
            case sf::Keyboard::Num4:        return KeyMap::Key_4;
            case sf::Keyboard::Num5:        return KeyMap::Key_5;
            case sf::Keyboard::Num6:        return KeyMap::Key_6;
            case sf::Keyboard::Num7:        return KeyMap::Key_7;
            case sf::Keyboard::Num8:        return KeyMap::Key_8;
            case sf::Keyboard::Num9:        return KeyMap::Key_9;
            case sf::Keyboard::Escape:      return KeyMap::Key_ESCAPE;
            case sf::Keyboard::LControl:    return KeyMap::Key_LEFT_CONTROL;
            case sf::Keyboard::LShift:      return KeyMap::Key_LEFT_SHIFT;
            case sf::Keyboard::LAlt:        return KeyMap::Key_LEFT_ALT;
            case sf::Keyboard::LSystem:     return KeyMap::Key_WORLD_1;
            case sf::Keyboard::RControl:    return KeyMap::Key_RIGHT_CONTROL;
            case sf::Keyboard::RShift:      return KeyMap::Key_RIGHT_SHIFT;
            case sf::Keyboard::RAlt:        return KeyMap::Key_RIGHT_ALT;
            case sf::Keyboard::RSystem:     return KeyMap::Key_WORLD_2;
            case sf::Keyboard::Menu:        return KeyMap::Key_MENU;
            case sf::Keyboard::LBracket:    return KeyMap::Key_LEFT_BRACKET;
            case sf::Keyboard::RBracket:    return KeyMap::Key_RIGHT_BRACKET;
            case sf::Keyboard::Semicolon:   return KeyMap::Key_SEMICOLON;
            case sf::Keyboard::Comma:       return KeyMap::Key_COMMA;
            case sf::Keyboard::Period:      return KeyMap::Key_PERIOD;
#if SFML_VERSION_MAJOR>=2 && SFML_VERSION_MINOR >=6
            case sf::Keyboard::Apostrophe:  return KeyMap::Key_APOSTROPHE;
            case sf::Keyboard::Grave:       return KeyMap::Key_GRAVE_ACCENT;
#endif
            case sf::Keyboard::Slash:       return KeyMap::Key_SLASH;
            case sf::Keyboard::Backslash:   return KeyMap::Key_BACKSLASH;
            case sf::Keyboard::Equal:       return KeyMap::Key_EQUAL;
            case sf::Keyboard::Hyphen:      return KeyMap::Key_MINUS;
            case sf::Keyboard::Space:       return KeyMap::Key_SPACE;
            case sf::Keyboard::Enter:       return KeyMap::Key_ENTER;
            case sf::Keyboard::Backspace:   return KeyMap::Key_BACKSPACE;
            case sf::Keyboard::Tab:         return KeyMap::Key_TAB;
            case sf::Keyboard::PageUp:      return KeyMap::Key_PAGE_UP;
            case sf::Keyboard::PageDown:    return KeyMap::Key_PAGE_DOWN;
            case sf::Keyboard::End:         return KeyMap::Key_END;
            case sf::Keyboard::Home:        return KeyMap::Key_HOME;
            case sf::Keyboard::Insert:      return KeyMap::Key_INSERT;
            case sf::Keyboard::Delete:      return KeyMap::Key_DELETE;
            case sf::Keyboard::Add:         return KeyMap::Key_PLUS;
            case sf::Keyboard::Subtract:    return KeyMap::Key_MINUS;
            case sf::Keyboard::Multiply:    return KeyMap::Key_ASTERISK;
            case sf::Keyboard::Divide:      return KeyMap::Key_SLASH;
            case sf::Keyboard::Left:        return KeyMap::Key_LEFT;
            case sf::Keyboard::Right:       return KeyMap::Key_RIGHT;
            case sf::Keyboard::Up:          return KeyMap::Key_UP;
            case sf::Keyboard::Down:        return KeyMap::Key_DOWN;
            case sf::Keyboard::Numpad0:     return KeyMap::Key_KP_0;
            case sf::Keyboard::Numpad1:     return KeyMap::Key_KP_1;
            case sf::Keyboard::Numpad2:     return KeyMap::Key_KP_2;
            case sf::Keyboard::Numpad3:     return KeyMap::Key_KP_3;
            case sf::Keyboard::Numpad4:     return KeyMap::Key_KP_4;
            case sf::Keyboard::Numpad5:     return KeyMap::Key_KP_5;
            case sf::Keyboard::Numpad6:     return KeyMap::Key_KP_6;
            case sf::Keyboard::Numpad7:     return KeyMap::Key_KP_7;
            case sf::Keyboard::Numpad8:     return KeyMap::Key_KP_8;
            case sf::Keyboard::Numpad9:     return KeyMap::Key_KP_9;
            case sf::Keyboard::F1:          return KeyMap::Key_F1;
            case sf::Keyboard::F2:          return KeyMap::Key_F2;
            case sf::Keyboard::F3:          return KeyMap::Key_F23;
            case sf::Keyboard::F4:          return KeyMap::Key_F24;
            case sf::Keyboard::F5:          return KeyMap::Key_F25;
            case sf::Keyboard::F6:          return KeyMap::Key_F6;
            case sf::Keyboard::F7:          return KeyMap::Key_F7;
            case sf::Keyboard::F8:          return KeyMap::Key_F8;
            case sf::Keyboard::F9:          return KeyMap::Key_F9;
            case sf::Keyboard::F10:         return KeyMap::Key_F10;
            case sf::Keyboard::F11:         return KeyMap::Key_F11;
            case sf::Keyboard::F12:         return KeyMap::Key_F12;
            case sf::Keyboard::F13:         return KeyMap::Key_F13;
            case sf::Keyboard::F14:         return KeyMap::Key_F14;
            case sf::Keyboard::F15:         return KeyMap::Key_F15;
            case sf::Keyboard::Pause:       return KeyMap::Key_PAUSE;

            case sf::Keyboard::KeyCount:    return KeyMap::Key_UNKNOWN;

            default:                        return KeyMap::Key_UNKNOWN;
        }


    }
} // Core