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
    BoolParameter::Ptr verbose = BoolParameter::New(false, "v,verbose", "Show note and debug messages.");

    Log();

    String prefix();
    String postfix();

    static auto GetSingleton() -> Log&;

    OStream *mainStream  = &std::cout;       bool manageMainStream  = false;
    OStream *notesStream = new StringStream; bool manageNotesStream = true;
    OStream *debugStream = new StringStream; bool manageDebugStream = true;

    Timer timer;

public:
    const static String ForegroundBlack;
    const static String ForegroundRed;
    const static String ForegroundGreen;
    const static String ForegroundYellow;
    const static String ForegroundBlue;
    const static String ForegroundMagenta;
    const static String ForegroundCyan;
    const static String ForegroundWhite;

    const static String BackgroundBlack;
    const static String BackgroundRed;
    const static String BackgroundGreen;
    const static String BackgroundYellow;
    const static String BackgroundBlue;
    const static String BackgroundMagenta;
    const static String BackgroundCyan;
    const static String BackgroundWhite;

    const static String ResetFormatting;

    const static String BoldFace;
    const static String Italic;     // Might not work
    const static String Underscore; // Might not work

    const static String InfoFormat;
    const static String NoteFormat;
    const static String SuccessFormat;
    const static String AttentionFormat;
    const static String CriticalFormat;
    const static String DebugFormat;
    const static String WarningFormat;
    const static String WarningImportantFormat;
    const static String ErrorFormat;
    const static String ErrorFatalFormat;

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
