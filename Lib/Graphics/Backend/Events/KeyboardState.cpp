//
// Created by joao on 11/11/24.
//

#include "KeyboardState.h"

namespace Slab::Graphics {

    const FKeyboardState::FKeyRecord FKeyboardState::DefaultKeyRecord{};

    bool FKeyboardState::NotifyKeyboard(EKeyMap key, EKeyState state, EModKeys modKeys) {
        mod_keys = modKeys;
        LastKey = key;
        SinceLastKeyEvent.reset();

        auto &record = KeyStates[key];

        if (record.State != state) {
            record.State = state;
            record.SinceStateChange.reset();
        } else if (state == Repeat) {
            record.SinceStateChange.reset();
        }

        return false;
    }

    bool FKeyboardState::NotifyCharacter(UInt codepoint) {
        LastCodepoint = codepoint;
        SinceLastCharacterEvent.reset();
        return false;
    }

    bool FKeyboardState::IsPressed(EKeyMap key) const {
        return GetState(key) != Release;
    }

    EKeyState FKeyboardState::GetState(EKeyMap key) const {
        if (const auto it = KeyStates.find(key); it != KeyStates.end()) {
            return it->second.State;
        }
        return Release;
    }

    const FKeyboardState::FKeyRecord& FKeyboardState::GetKeyRecord(EKeyMap key) const {
        if (const auto it = KeyStates.find(key); it != KeyStates.end()) {
            return it->second;
        }
        return DefaultKeyRecord;
    }

    bool FKeyboardState::AnyPressed() const {
        for (const auto& Entry : KeyStates) {
            if (Entry.second.State != Release) return true;
        }
        return false;
    }

    void FKeyboardState::Reset() {
        KeyStates.clear();
        mod_keys = {};
        LastKey = EKeyMap::Key_UNKNOWN;
        SinceLastKeyEvent.reset();
        LastCodepoint = 0;
        SinceLastCharacterEvent.reset();
    }

}
