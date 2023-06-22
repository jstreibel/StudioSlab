//
// Created by joao on 10/06/23.
//

#ifndef STUDIOSLAB_LOG_H
#define STUDIOSLAB_LOG_H


#include "Common/Types.h"
#include "Common/Utils.h"
#include "Common/Timer.h"

#include "Base/Controller/Interface/CommonParameters.h"
#include "Base/Controller/Interface/InterfaceOwner.h"

class Log : public InterfaceOwner
{
    static Log *myInstance;

    BoolParameter::Ptr logDebug = BoolParameter::New(false, "log_debug", "Show debug messages.");
    BoolParameter::Ptr logNotes = BoolParameter::New(false, "log_notes", "Show note messages.");
    BoolParameter::Ptr verbose = BoolParameter::New(false, "verbose,vv", "Show note and debug messages.");

    Log();

    String prefix();
    String postfix();

    static auto GetSingleton() -> Log&;

    OStream *mainStream  = &std::cout;       bool manageMainStream  = false;
    OStream *notesStream = new StringStream; bool manageNotesStream = true;
    OStream *debugStream = new StringStream; bool manageDebugStream = true;

    Timer timer;

public:
    constexpr const static String ForegroundBlack   = "\033[0;30m";
    constexpr const static String ForegroundRed     = "\033[0;31m";
    constexpr const static String ForegroundGreen   = "\033[0;32m";
    constexpr const static String ForegroundYellow  = "\033[0;33m";
    constexpr const static String ForegroundBlue    = "\033[0;34m";
    constexpr const static String ForegroundMagenta = "\033[0;35m";
    constexpr const static String ForegroundCyan    = "\033[0;36m";
    constexpr const static String ForegroundWhite   = "\033[0;37m";

    constexpr const static String BackgroundBlack   = "\033[0;40m" ;
    constexpr const static String BackgroundRed     = "\033[0;41m" ;
    constexpr const static String BackgroundGreen   = "\033[0;42m" ;
    constexpr const static String BackgroundYellow  = "\033[0;43m";
    constexpr const static String BackgroundBlue    = "\033[0;44m" ;
    constexpr const static String BackgroundMagenta = "\033[0;45m";
    constexpr const static String BackgroundCyan    = "\033[0;46m" ;
    constexpr const static String BackgroundWhite   = "\033[0;47m" ;

    constexpr const static String ResetFormatting   = "\033[0m";

    constexpr const static String BoldFace          = "\033[1m";
    constexpr const static String Italic            = "\033[3m"; // Might not work
    constexpr const static String Underscore        = "\033[4m"; // Might not work

    const static String InfoColor;
    const static String NoteColor;
    const static String SuccessColor;
    const static String AttentionColor;
    const static String CriticalColor;
    const static String DebugColor;
    const static String WarningColor;
    const static String WarningImportantColor;
    const static String ErrorColor;
    const static String ErrorFatalColor;

    constexpr const static char Return = '\r';

    static auto Info             ()                  -> OStream&;
    static auto Note             ()                  -> OStream&;
    static auto Attention        ()                  -> OStream&;
    static auto Critical         ()                  -> OStream&;
    static auto Debug            ()                  -> OStream&;
    static auto Success          ()                  -> OStream&;
    static auto Warning          ()                  -> OStream&;
    static auto WarningImportant ()                  -> OStream&;
    static auto Error            ()                  -> OStream&;
    static auto ErrorFatal       ()                  -> OStream&;

    static auto Info             (const String& str) -> void;
    static auto Note             (const String& str) -> void;
    static auto Attention        (const String& str) -> void;
    static auto Critical         (const String& str) -> void;
    static auto Debug            (const String& str) -> void;
    static auto Success          (const String& str) -> void;
    static auto Warning          (const String& str) -> void;
    static auto WarningImportant (const String& str) -> void;
    static auto Error            (const String& str) -> void;
    static auto ErrorFatal       (const String& str) -> void;

    class FlushClass { public: void operator()() const; };
    friend FlushClass;
    static const FlushClass Flush;
    friend OStream& operator<<(OStream& os, const FlushClass& flush);

    static auto FlushAll() -> void;

    auto notifyCLArgsSetupFinished() -> void override;

};


#endif //STUDIOSLAB_LOG_H
