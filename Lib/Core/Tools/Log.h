//
// Created by joao on 10/06/23.
//

#ifndef STUDIOSLAB_LOG_H
#define STUDIOSLAB_LOG_H


#include "Utils/Types.h"
#include "Utils/Utils.h"
#include "Utils/Timer.h"

#include "Core/Controller/CommandLine/CommonCLParameters.h"
#include "Core/Controller/CommandLine/CLInterfaceOwner.h"
#include "Utils/Singleton.h"

#include <iostream>

#define FORCE_VERBOSE false


namespace Slab::Core {

    class Log : public Singleton<Log>, CLInterfaceOwner {
        static Log *myInstance;

        BoolParameter::Ptr logDebug = BoolParameter::New(false, "log_debug", "Show debug messages.");
        BoolParameter::Ptr logNotes = BoolParameter::New(false, "log_notes", "Show note messages.");
        BoolParameter::Ptr verbose = BoolParameter::New(false, "verbose", "Show note and debug messages.");

        Log();

        Str prefix();

        Str postfix();

        OStream *mainStream = &std::cout;
        bool manageMainStream = false;
#if FORCE_VERBOSE
        OStream *notesStream = &std::cout; bool manageNotesStream = false;
        OStream *debugStream = &std::cout; bool manageDebugStream = false;
#else
        OStream *notesStream = new StringStream;
        bool manageNotesStream = true;
        OStream *debugStream = new StringStream;
        bool manageDebugStream = true;
#endif

        Timer timer;

    public:
        static auto GetSingleton() -> Log &;

        const static Str StartFormat;
        const static Str SepFormat;
        const static Str EndFormat;

        const static Str FGBlack;
        const static Str FGRed;
        const static Str FGGreen;
        const static Str FGYellow;
        const static Str FGBlue;
        const static Str FGMagenta;
        const static Str FGCyan;
        const static Str FGWhite;

        const static Str BGBlack;
        const static Str BGRed;
        const static Str BGGreen;
        const static Str BGYellow;
        const static Str BGBlue;
        const static Str BGMagenta;
        const static Str BGCyan;
        const static Str BGWhite;

        const static Str ResetFormatting;

        const static Str BoldFace;
        const static Str Italic;     // Might not work
        const static Str Underscore; // Might not work

        const static Str StatusFormat;
        const static Str InfoFormat;
        const static Str NoteFormat;
        const static Str SuccessFormat;
        const static Str FailFormat;
        const static Str AttentionFormat;
        const static Str CriticalFormat;
        const static Str DebugFormat;
        const static Str WarningFormat;
        const static Str WarningImportantFormat;
        const static Str ErrorFormat;
        const static Str ErrorFatalFormat;

        constexpr const static char Return = '\r';

        static auto Status() -> OStream &;

        static auto Info() -> OStream &;

        static auto Note() -> OStream &;

        static auto Attention() -> OStream &;

        static auto Critical() -> OStream &;

        static auto Debug() -> OStream &;

        static auto Success() -> OStream &;

        static auto Fail() -> OStream &;

        static auto Warning() -> OStream &;

        static auto WarningImportant() -> OStream &;

        static auto Error() -> OStream &;

        static auto ErrorFatal() -> OStream &;

        static auto Status(const Str &str) -> OStream &;

        static auto Info(const Str &str) -> OStream &;

        static auto Note(const Str &str) -> OStream &;

        static auto Attention(const Str &str) -> OStream &;

        static auto Critical(const Str &str) -> OStream &;

        static auto Debug(const Str &str) -> OStream &;

        static auto Success(const Str &str) -> OStream &;

        static auto Fail(const Str &str) -> OStream &;

        static auto Warning(const Str &str) -> OStream &;

        static auto WarningImportant(const Str &str) -> OStream &;

        static auto Error(const Str &str) -> OStream &;

        static auto ErrorFatal(const Str &str) -> OStream &;

        class FlushClass {
        public:
            void operator()() const;
        };

        friend FlushClass;
        static const FlushClass Flush;

        friend OStream &operator<<(OStream &os, const FlushClass &flush);

        enum TextPosition {
            Left, Right
        };

        class FormattingClass {
        public:
            Count len{};
            TextPosition textPosition = Left;

            const FormattingClass &operator()(Count len);

            const FormattingClass &operator()(TextPosition);
        };

        friend FormattingClass;
        static FormattingClass Format;

        friend OStream &operator<<(OStream &os, const FormattingClass &flush);

        static auto FlushAll() -> void;

        auto NotifyCLArgsSetupFinished() -> void override;

    };


}

#endif //STUDIOSLAB_LOG_H
