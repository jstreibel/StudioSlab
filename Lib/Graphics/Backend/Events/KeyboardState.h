//
// Created by joao on 11/11/24.
//

#ifndef STUDIOSLAB_KEYBOARDSTATE_H
#define STUDIOSLAB_KEYBOARDSTATE_H

#include "KeyMap.h"
#include "SystemWindowEventListener.h"

#include "Utils/Map.h"
#include "Utils/Timer.h"

namespace Slab::Graphics {

    class FKeyboardState final : public FPlatformWindowEventListener {
    public:
        struct FKeyRecord {
            EKeyState State = Release;
            FTimer SinceStateChange;
        };

        FKeyboardState() = default;
        ~FKeyboardState() override = default;

        bool NotifyKeyboard(EKeyMap key, EKeyState state, EModKeys modKeys) override;
        bool NotifyCharacter(UInt codepoint) override;

        [[nodiscard]] bool IsPressed(EKeyMap key) const;
        [[nodiscard]] EKeyState GetState(EKeyMap key) const;
        [[nodiscard]] const FKeyRecord& GetKeyRecord(EKeyMap key) const;
        [[nodiscard]] bool AnyPressed() const;

        void Reset();

        EModKeys mod_keys{};

        EKeyMap LastKey = EKeyMap::Key_UNKNOWN;
        FTimer SinceLastKeyEvent;

        UInt LastCodepoint = 0;
        FTimer SinceLastCharacterEvent;

    private:
        using KeyMapStorage = HashMap<EKeyMap, FKeyRecord>;
        KeyMapStorage KeyStates{};

        static const FKeyRecord DefaultKeyRecord;
    };

}

#endif //STUDIOSLAB_KEYBOARDSTATE_H
