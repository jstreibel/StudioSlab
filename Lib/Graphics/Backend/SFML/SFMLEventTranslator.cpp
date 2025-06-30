//
// Created by joao on 27/09/23.
//

#include "SFMLEventTranslator.h"
#include "Utils/ReferenceIterator.h"

#define For      for(auto &
#define Each     listener
#define Listener : guiListeners) listener
// #define Do (WHAT) {listener-> WHAT ;};

namespace Slab::Graphics {
    inline EKeyMap mapFromSFML(sf::Event::KeyEvent);

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

            EKeyMap key = mapFromSFML(inkey);
            EKeyState state = type == sf::Event::KeyPressed ? EKeyState::Press : EKeyState::Release;
            EModKeys modKeys{
                inkey.shift ? Press : Release,
                inkey.control ? Press : Release,
                inkey.alt ? Press : Release,
                inkey.system ? Press : Release,
                Release,
                Release};

            IterateReferences(SysWinListeners, Func(NotifyKeyboard, key, state, modKeys));
        }

        else if(isMouseButton) {
            EMouseButton button;
            switch (event.mouseButton.button) {
                case sf::Mouse::Left:           button = MouseButton_LEFT;  break;
                case sf::Mouse::Right:          button = MouseButton_RIGHT; break;
                case sf::Mouse::Middle:         button = MouseButton_MIDDLE; break;
                case sf::Mouse::XButton1:       button = MouseButton_4; break;
                case sf::Mouse::XButton2:       button = MouseButton_5; break;
                case sf::Mouse::ButtonCount:    button = MouseButton_LAST; break;
            }
            EKeyState state = type == sf::Event::MouseButtonPressed ? Press : Release;

            auto modKeys = EModKeys(
                (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) | sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))     ? Press : Release,
                (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) | sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) ? Press : Release,
                (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) | sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt))         ? Press : Release,
                (sf::Keyboard::isKeyPressed(sf::Keyboard::LSystem) | sf::Keyboard::isKeyPressed(sf::Keyboard::RSystem))   ? Press : Release,
                Release, Release);

            IterateReferences(SysWinListeners, Func(NotifyMouseButton, button, state, modKeys), StopOnFirstResponder);
        }

        else if(isMouseMovement) {
            static auto last_x = 0, last_y = 0;

            fix x = event.mouseMove.x;
            fix y = event.mouseMove.y;
            fix dx = x-last_x;
            fix dy = y-last_y;

            last_x = x;
            last_y = y;

            IterateReferences(SysWinListeners, Func(NotifyMouseMotion, x, y, dx, dy));
        }

        else if(isMouseWheel){
            fix x = event.mouseWheelScroll.x;
            fix y = event.mouseWheelScroll.y;
            IterateReferences(SysWinListeners, Func(NotifyMouseWheel, x, y));
        }

        else if(isResize) {
            fix w = event.size.width;
            fix h = event.size.height;
            IterateReferences(SysWinListeners, Func(NotifySystemWindowReshape, w, h));
        }
    }

    void SFMLEventTranslator::render(sf::RenderWindow *window) {
        IterateReferences(SysWinListeners, Func(NotifyRender));
    }


    inline EKeyMap mapFromSFML(sf::Event::KeyEvent keyEvent){
        auto shift = keyEvent.shift;

        switch (keyEvent.code) {
            case sf::Keyboard::Unknown:     return EKeyMap::Key_UNKNOWN;

            case sf::Keyboard::A:           return shift ? EKeyMap::Key_A : EKeyMap::Key_a;
            case sf::Keyboard::B:           return shift ? EKeyMap::Key_B : EKeyMap::Key_a;
            case sf::Keyboard::C:           return shift ? EKeyMap::Key_C : EKeyMap::Key_a;
            case sf::Keyboard::D:           return shift ? EKeyMap::Key_D : EKeyMap::Key_a;
            case sf::Keyboard::E:           return shift ? EKeyMap::Key_E: EKeyMap::Key_e;
            case sf::Keyboard::F:           return shift ? EKeyMap::Key_F: EKeyMap::Key_f;
            case sf::Keyboard::G:           return shift ? EKeyMap::Key_G: EKeyMap::Key_g;
            case sf::Keyboard::H:           return shift ? EKeyMap::Key_H: EKeyMap::Key_h;
            case sf::Keyboard::I:           return shift ? EKeyMap::Key_I: EKeyMap::Key_i;
            case sf::Keyboard::J:           return shift ? EKeyMap::Key_J: EKeyMap::Key_j;
            case sf::Keyboard::K:           return shift ? EKeyMap::Key_K: EKeyMap::Key_k;
            case sf::Keyboard::L:           return shift ? EKeyMap::Key_L: EKeyMap::Key_l;
            case sf::Keyboard::M:           return shift ? EKeyMap::Key_M: EKeyMap::Key_m;
            case sf::Keyboard::N:           return shift ? EKeyMap::Key_N: EKeyMap::Key_n;
            case sf::Keyboard::O:           return shift ? EKeyMap::Key_O: EKeyMap::Key_o;
            case sf::Keyboard::P:           return shift ? EKeyMap::Key_P: EKeyMap::Key_p;
            case sf::Keyboard::Q:           return shift ? EKeyMap::Key_Q: EKeyMap::Key_q;
            case sf::Keyboard::R:           return shift ? EKeyMap::Key_R: EKeyMap::Key_r;
            case sf::Keyboard::S:           return shift ? EKeyMap::Key_S: EKeyMap::Key_s;
            case sf::Keyboard::T:           return shift ? EKeyMap::Key_T: EKeyMap::Key_t;
            case sf::Keyboard::U:           return shift ? EKeyMap::Key_U: EKeyMap::Key_u;
            case sf::Keyboard::V:           return shift ? EKeyMap::Key_V: EKeyMap::Key_v;
            case sf::Keyboard::W:           return shift ? EKeyMap::Key_W: EKeyMap::Key_w;
            case sf::Keyboard::X:           return shift ? EKeyMap::Key_X: EKeyMap::Key_x;
            case sf::Keyboard::Y:           return shift ? EKeyMap::Key_Y: EKeyMap::Key_y;
            case sf::Keyboard::Z:           return shift ? EKeyMap::Key_Z: EKeyMap::Key_z;
            case sf::Keyboard::Num0:        return EKeyMap::Key_0;
            case sf::Keyboard::Num1:        return EKeyMap::Key_1;
            case sf::Keyboard::Num2:        return EKeyMap::Key_2;
            case sf::Keyboard::Num3:        return EKeyMap::Key_3;
            case sf::Keyboard::Num4:        return EKeyMap::Key_4;
            case sf::Keyboard::Num5:        return EKeyMap::Key_5;
            case sf::Keyboard::Num6:        return EKeyMap::Key_6;
            case sf::Keyboard::Num7:        return EKeyMap::Key_7;
            case sf::Keyboard::Num8:        return EKeyMap::Key_8;
            case sf::Keyboard::Num9:        return EKeyMap::Key_9;
            case sf::Keyboard::Escape:      return EKeyMap::Key_ESCAPE;
            case sf::Keyboard::LControl:    return EKeyMap::Key_LEFT_CONTROL;
            case sf::Keyboard::LShift:      return EKeyMap::Key_LEFT_SHIFT;
            case sf::Keyboard::LAlt:        return EKeyMap::Key_LEFT_ALT;
            case sf::Keyboard::LSystem:     return EKeyMap::Key_WORLD_1;
            case sf::Keyboard::RControl:    return EKeyMap::Key_RIGHT_CONTROL;
            case sf::Keyboard::RShift:      return EKeyMap::Key_RIGHT_SHIFT;
            case sf::Keyboard::RAlt:        return EKeyMap::Key_RIGHT_ALT;
            case sf::Keyboard::RSystem:     return EKeyMap::Key_WORLD_2;
            case sf::Keyboard::Menu:        return EKeyMap::Key_MENU;
            case sf::Keyboard::LBracket:    return EKeyMap::Key_LEFT_BRACKET;
            case sf::Keyboard::RBracket:    return EKeyMap::Key_RIGHT_BRACKET;
            case sf::Keyboard::Semicolon:   return EKeyMap::Key_SEMICOLON;
            case sf::Keyboard::Comma:       return EKeyMap::Key_COMMA;
            case sf::Keyboard::Period:      return EKeyMap::Key_PERIOD;
#if SFML_VERSION_MAJOR>=2 && SFML_VERSION_MINOR >=6
            case sf::Keyboard::Apostrophe:  return EKeyMap::Key_APOSTROPHE;
            case sf::Keyboard::Grave:       return EKeyMap::Key_GRAVE_ACCENT;
#endif
            case sf::Keyboard::Slash:       return EKeyMap::Key_SLASH;
            case sf::Keyboard::Backslash:   return EKeyMap::Key_BACKSLASH;
            case sf::Keyboard::Equal:       return EKeyMap::Key_EQUAL;
            case sf::Keyboard::Hyphen:      return EKeyMap::Key_MINUS;
            case sf::Keyboard::Space:       return EKeyMap::Key_SPACE;
            case sf::Keyboard::Enter:       return EKeyMap::Key_ENTER;
            case sf::Keyboard::Backspace:   return EKeyMap::Key_BACKSPACE;
            case sf::Keyboard::Tab:         return EKeyMap::Key_TAB;
            case sf::Keyboard::PageUp:      return EKeyMap::Key_PAGE_UP;
            case sf::Keyboard::PageDown:    return EKeyMap::Key_PAGE_DOWN;
            case sf::Keyboard::End:         return EKeyMap::Key_END;
            case sf::Keyboard::Home:        return EKeyMap::Key_HOME;
            case sf::Keyboard::Insert:      return EKeyMap::Key_INSERT;
            case sf::Keyboard::Delete:      return EKeyMap::Key_DELETE;
            case sf::Keyboard::Add:         return EKeyMap::Key_PLUS;
            case sf::Keyboard::Subtract:    return EKeyMap::Key_MINUS;
            case sf::Keyboard::Multiply:    return EKeyMap::Key_ASTERISK;
            case sf::Keyboard::Divide:      return EKeyMap::Key_SLASH;
            case sf::Keyboard::Left:        return EKeyMap::Key_LEFT;
            case sf::Keyboard::Right:       return EKeyMap::Key_RIGHT;
            case sf::Keyboard::Up:          return EKeyMap::Key_UP;
            case sf::Keyboard::Down:        return EKeyMap::Key_DOWN;
            case sf::Keyboard::Numpad0:     return EKeyMap::Key_KP_0;
            case sf::Keyboard::Numpad1:     return EKeyMap::Key_KP_1;
            case sf::Keyboard::Numpad2:     return EKeyMap::Key_KP_2;
            case sf::Keyboard::Numpad3:     return EKeyMap::Key_KP_3;
            case sf::Keyboard::Numpad4:     return EKeyMap::Key_KP_4;
            case sf::Keyboard::Numpad5:     return EKeyMap::Key_KP_5;
            case sf::Keyboard::Numpad6:     return EKeyMap::Key_KP_6;
            case sf::Keyboard::Numpad7:     return EKeyMap::Key_KP_7;
            case sf::Keyboard::Numpad8:     return EKeyMap::Key_KP_8;
            case sf::Keyboard::Numpad9:     return EKeyMap::Key_KP_9;
            case sf::Keyboard::F1:          return EKeyMap::Key_F1;
            case sf::Keyboard::F2:          return EKeyMap::Key_F2;
            case sf::Keyboard::F3:          return EKeyMap::Key_F23;
            case sf::Keyboard::F4:          return EKeyMap::Key_F24;
            case sf::Keyboard::F5:          return EKeyMap::Key_F25;
            case sf::Keyboard::F6:          return EKeyMap::Key_F6;
            case sf::Keyboard::F7:          return EKeyMap::Key_F7;
            case sf::Keyboard::F8:          return EKeyMap::Key_F8;
            case sf::Keyboard::F9:          return EKeyMap::Key_F9;
            case sf::Keyboard::F10:         return EKeyMap::Key_F10;
            case sf::Keyboard::F11:         return EKeyMap::Key_F11;
            case sf::Keyboard::F12:         return EKeyMap::Key_F12;
            case sf::Keyboard::F13:         return EKeyMap::Key_F13;
            case sf::Keyboard::F14:         return EKeyMap::Key_F14;
            case sf::Keyboard::F15:         return EKeyMap::Key_F15;
            case sf::Keyboard::Pause:       return EKeyMap::Key_PAUSE;

            case sf::Keyboard::KeyCount:    return EKeyMap::Key_UNKNOWN;

            default:                        return EKeyMap::Key_UNKNOWN;
        }


    }
} // Core