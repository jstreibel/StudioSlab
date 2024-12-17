//
// Created by joao on 11/8/24.
//

#include "Slab-ImGui-Interop.h"

namespace Slab::Graphics {

    ImGuiKey SlabToImGuiTranslate(KeyMap slab_key) {
        switch (slab_key) {
            case Key_UNKNOWN:           return ImGuiKey_None;
            case Key_SPACE:             return ImGuiKey_Space;
            case Key_APOSTROPHE:        return ImGuiKey_Apostrophe;
            case Ket_LEFT_PARENTHESIS:  return ImGuiKey_0;
            case Key_RIGHT_PARENTHESIS: return ImGuiKey_9;
            case Key_ASTERISK:          return ImGuiKey_8;
            case Key_PLUS:              return ImGuiKey_Equal;
            case Key_COMMA:             return ImGuiKey_Comma;
            case Key_MINUS:             return ImGuiKey_Minus;
            case Key_PERIOD:            return ImGuiKey_Period;
            case Key_SLASH:             return ImGuiKey_Slash;
            case Key_0:                 return ImGuiKey_0;
            case Key_1:                 return ImGuiKey_1;
            case Key_2:                 return ImGuiKey_2;
            case Key_3:                 return ImGuiKey_3;
            case Key_4:                 return ImGuiKey_4;
            case Key_5:                 return ImGuiKey_5;
            case Key_6:                 return ImGuiKey_6;
            case Key_7:                 return ImGuiKey_7;
            case Key_8:                 return ImGuiKey_8;
            case Key_9:                 return ImGuiKey_9;
            case Key_SEMICOLON:         return ImGuiKey_Semicolon;
            case Key_EQUAL:             return ImGuiKey_Equal;
            case Key_A:                 return ImGuiKey_A;
            case Key_B:                 return ImGuiKey_B;
            case Key_C:                 return ImGuiKey_C;
            case Key_D:                 return ImGuiKey_D;
            case Key_E:                 return ImGuiKey_E;
            case Key_F:                 return ImGuiKey_F;
            case Key_G:                 return ImGuiKey_G;
            case Key_H:                 return ImGuiKey_H;
            case Key_I:                 return ImGuiKey_I;
            case Key_J:                 return ImGuiKey_J;
            case Key_K:                 return ImGuiKey_K;
            case Key_L:                 return ImGuiKey_L;
            case Key_M:                 return ImGuiKey_M;
            case Key_N:                 return ImGuiKey_N;
            case Key_O:                 return ImGuiKey_O;
            case Key_P:                 return ImGuiKey_P;
            case Key_Q:                 return ImGuiKey_Q;
            case Key_R:                 return ImGuiKey_R;
            case Key_S:                 return ImGuiKey_S;
            case Key_T:                 return ImGuiKey_T;
            case Key_U:                 return ImGuiKey_U;
            case Key_V:                 return ImGuiKey_V;
            case Key_W:                 return ImGuiKey_W;
            case Key_X:                 return ImGuiKey_X;
            case Key_Y:                 return ImGuiKey_Y;
            case Key_Z:                 return ImGuiKey_Z;
            case Key_LEFT_BRACKET:      return ImGuiKey_LeftBracket;
            case Key_BACKSLASH:         return ImGuiKey_Backslash;
            case Key_RIGHT_BRACKET:     return ImGuiKey_RightBracket;
            case Key_GRAVE_ACCENT:      return ImGuiKey_GraveAccent;
            case Key_a:                 return ImGuiKey_A;
            case Key_b:                 return ImGuiKey_B;
            case Key_c:                 return ImGuiKey_C;
            case Key_d:                 return ImGuiKey_D;
            case Key_e:                 return ImGuiKey_E;
            case Key_f:                 return ImGuiKey_F;
            case Key_g:                 return ImGuiKey_G;
            case Key_h:                 return ImGuiKey_H;
            case Key_i:                 return ImGuiKey_I;
            case Key_j:                 return ImGuiKey_J;
            case Key_k:                 return ImGuiKey_K;
            case Key_l:                 return ImGuiKey_L;
            case Key_m:                 return ImGuiKey_M;
            case Key_n:                 return ImGuiKey_N;
            case Key_o:                 return ImGuiKey_O;
            case Key_p:                 return ImGuiKey_P;
            case Key_q:                 return ImGuiKey_Q;
            case Key_r:                 return ImGuiKey_R;
            case Key_s:                 return ImGuiKey_S;
            case Key_t:                 return ImGuiKey_T;
            case Key_u:                 return ImGuiKey_U;
            case Key_v:                 return ImGuiKey_V;
            case Key_w:                 return ImGuiKey_W;
            case Key_x:                 return ImGuiKey_X;
            case Key_y:                 return ImGuiKey_Y;
            case Key_z:                 return ImGuiKey_Z;
            // case Key_WORLD_1:           return ImGuiKey_;
            // case Key_WORLD_2:           return ImGuiKey_;
            case Key_ESCAPE:            return ImGuiKey_Escape;
            case Key_ENTER:             return ImGuiKey_Enter;
            case Key_TAB:               return ImGuiKey_Tab;
            case Key_BACKSPACE:         return ImGuiKey_Backspace;
            case Key_INSERT:            return ImGuiKey_Insert;
            case Key_DELETE:            return ImGuiKey_Delete;
            case Key_RIGHT:             return ImGuiKey_RightArrow;
            case Key_LEFT:              return ImGuiKey_LeftArrow;
            case Key_DOWN:              return ImGuiKey_DownArrow;
            case Key_UP:                return ImGuiKey_UpArrow;
            case Key_PAGE_UP:           return ImGuiKey_PageUp;
            case Key_PAGE_DOWN:         return ImGuiKey_PageDown;
            case Key_HOME:              return ImGuiKey_Home;
            case Key_END:               return ImGuiKey_End;
            case Key_CAPS_LOCK:         return ImGuiKey_CapsLock;
            case Key_SCROLL_LOCK:       return ImGuiKey_ScrollLock;
            case Key_NUM_LOCK:          return ImGuiKey_NumLock;
            case Key_PRINT_SCREEN:      return ImGuiKey_PrintScreen;
            case Key_PAUSE:             return ImGuiKey_Pause;
            case Key_F1:                return ImGuiKey_F1;
            case Key_F2:                return ImGuiKey_F2;
            case Key_F3:                return ImGuiKey_F3;
            case Key_F4:                return ImGuiKey_F4;
            case Key_F5:                return ImGuiKey_F5;
            case Key_F6:                return ImGuiKey_F6;
            case Key_F7:                return ImGuiKey_F7;
            case Key_F8:                return ImGuiKey_F8;
            case Key_F9:                return ImGuiKey_F9;
            case Key_F10:               return ImGuiKey_F10;
            case Key_F11:               return ImGuiKey_F11;
            case Key_F12:               return ImGuiKey_F12;
            case Key_F13:               return ImGuiKey_F13;
            case Key_F14:               return ImGuiKey_F14;
            case Key_F15:               return ImGuiKey_F15;
            case Key_F16:               return ImGuiKey_F16;
            case Key_F17:               return ImGuiKey_F17;
            case Key_F18:               return ImGuiKey_F18;
            case Key_F19:               return ImGuiKey_F19;
            case Key_F20:               return ImGuiKey_F20;
            case Key_F21:               return ImGuiKey_F21;
            case Key_F22:               return ImGuiKey_F22;
            case Key_F23:               return ImGuiKey_F23;
            case Key_F24:               return ImGuiKey_F24;
            // case Key_F25:               return ImGuiKey_;
            case Key_KP_0:              return ImGuiKey_Keypad0;
            case Key_KP_1:              return ImGuiKey_Keypad1;
            case Key_KP_2:              return ImGuiKey_Keypad2;
            case Key_KP_3:              return ImGuiKey_Keypad3;
            case Key_KP_4:              return ImGuiKey_Keypad4;
            case Key_KP_5:              return ImGuiKey_Keypad5;
            case Key_KP_6:              return ImGuiKey_Keypad6;
            case Key_KP_7:              return ImGuiKey_Keypad7;
            case Key_KP_8:              return ImGuiKey_Keypad8;
            case Key_KP_9:              return ImGuiKey_Keypad9;
            case Key_KP_DECIMAL:        return ImGuiKey_KeypadDecimal;
            case Key_KP_DIVIDE:         return ImGuiKey_KeypadDivide;
            case Key_KP_MULTIPLY:       return ImGuiKey_KeypadMultiply;
            case Key_KP_SUBTRACT:       return ImGuiKey_KeypadSubtract;
            case Key_KP_ADD:            return ImGuiKey_KeypadAdd;
            case Key_KP_ENTER:          return ImGuiKey_KeypadEnter;
            case Key_KP_EQUAL:          return ImGuiKey_KeypadEqual;
            case Key_LEFT_SHIFT:        return ImGuiKey_LeftShift;
            case Key_LEFT_CONTROL:      return ImGuiKey_LeftCtrl;
            case Key_LEFT_ALT:          return ImGuiKey_LeftAlt;
            case Key_LEFT_SUPER:        return ImGuiKey_LeftSuper;
            case Key_RIGHT_SHIFT:       return ImGuiKey_RightShift;
            case Key_RIGHT_CONTROL:     return ImGuiKey_RightCtrl;
            case Key_RIGHT_ALT:         return ImGuiKey_RightAlt;
            case Key_RIGHT_SUPER:       return ImGuiKey_RightSuper;
            case Key_MENU:              return ImGuiKey_Menu;
        }

        return ImGuiKey_None;
    }
}