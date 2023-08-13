//
// Created by joao on 10/06/23.
//

#include "Log.h"

#include <typeinfo>
#define GET_CLASS_NAME(obj) typeid(obj).name()

Log* Log::myInstance = nullptr;

const Str Log::StartFormat = "\033[";
const Str Log::SepFormat = ";";
const Str Log::EndFormat = "m";

const Str Log::FGBlack   = "\033[30m";
const Str Log::FGRed     = "\033[31m";
const Str Log::FGGreen   = "\033[32m";
const Str Log::FGYellow  = "\033[33m";
const Str Log::FGBlue    = "\033[34m";
const Str Log::FGMagenta = "\033[35m";
const Str Log::FGCyan    = "\033[36m";
const Str Log::FGWhite   = "\033[37m";

const Str Log::BGBlack   = "\033[40m" ;
const Str Log::BGRed     = "\033[41m" ;
const Str Log::BGGreen   = "\033[42m" ;
const Str Log::BGYellow  = "\033[43m";
const Str Log::BGBlue    = "\033[44m" ;
const Str Log::BGMagenta = "\033[45m";
const Str Log::BGCyan    = "\033[46m" ;
const Str Log::BGWhite   = "\033[47m" ;

const Str Log::ResetFormatting   = "\033[0m";

const Str Log::BoldFace          = "\033[1m";
const Str Log::Italic            = "\033[3m"; // Might not work
const Str Log::Underscore        = "\033[4m"; // Might not work

const Str Log::InfoFormat =             Log::ResetFormatting + Log::FGCyan;
const Str Log::NoteFormat =             Log::ResetFormatting + Log::FGWhite;
const Str Log::AttentionFormat =        Log::ResetFormatting + Log::FGMagenta;
const Str Log::CriticalFormat =         Log::ResetFormatting                  + Log::BGMagenta + Log::BoldFace  ;
const Str Log::DebugFormat =            Log::ResetFormatting + Log::FGMagenta                  + Log::Italic ;
const Str Log::SuccessFormat =          Log::ResetFormatting + Log::FGGreen                    + Log::BoldFace ;
const Str Log::WarningFormat =          Log::ResetFormatting + Log::FGYellow;
const Str Log::WarningImportantFormat = Log::ResetFormatting + Log::FGBlack   + Log::BGMagenta;
const Str Log::ErrorFormat =            Log::ResetFormatting + Log::FGRed;
const Str Log::ErrorFatalFormat =       Log::ResetFormatting + Log::FGRed                      + Log::BoldFace;

const Log::FlushClass Log::Flush;

Log::Log() : Singleton<Log>("Log"), InterfaceOwner (true) { };

auto Log::GetSingleton() -> Log & {
    if(Singleton::singleInstance == nullptr) {
        auto me = new Log;
        Log::myInstance = me;
        Singleton::singleInstance = Log::myInstance;

        Log::Note() << "Logging system initiated." << Log::Flush;

        me->LateStart("Log Manager", -10);
        me->interface->addParameters({me->logDebug, me->logNotes, me->verbose});
    }

    return *Log::myInstance;
}

inline Str Log::prefix(){
    StringStream ss;

    auto time = timer.getElTime_msec();

    ss << Str("\n") << ResetFormatting << std::setw(10) << ToStr(time)
       << "ms [ ";

    return ss.str();
};

inline Str Log::postfix(){
    return ResetFormatting + " ] ";
};


OStream &Log::Info()                { auto &me = Log::GetSingleton(); auto &stream = *me.mainStream;  stream << me.prefix() << InfoFormat              << "INFO"      << me.postfix(); return stream; }
OStream &Log::Note()                { auto &me = Log::GetSingleton(); auto &stream = *me.notesStream; stream << me.prefix() << NoteFormat              << "note"      << me.postfix(); return stream; }
OStream &Log::Attention()           { auto &me = Log::GetSingleton(); auto &stream = *me.mainStream;  stream << me.prefix() << AttentionFormat         << "Attention" << me.postfix(); return stream; }
OStream &Log::Critical()            { auto &me = Log::GetSingleton(); auto &stream = *me.mainStream;  stream << me.prefix() << CriticalFormat          << "Critical"  << me.postfix(); return stream; }
OStream &Log::Debug()               { auto &me = Log::GetSingleton(); auto &stream = *me.debugStream; stream << me.prefix() << DebugFormat             << "Debug"     << me.postfix(); return stream; }
OStream &Log::Success()             { auto &me = Log::GetSingleton(); auto &stream = *me.mainStream;  stream << me.prefix() << SuccessFormat           << "Success"   << me.postfix(); return stream; }
OStream &Log::Warning()             { auto &me = Log::GetSingleton(); auto &stream = *me.mainStream;  stream << me.prefix() << WarningFormat           << "Warning"   << me.postfix(); return stream; }
OStream &Log::WarningImportant()    { auto &me = Log::GetSingleton(); auto &stream = *me.mainStream;  stream << me.prefix() << WarningImportantFormat  << "Warning!"  << me.postfix(); return stream; }
OStream &Log::Error()               { auto &me = Log::GetSingleton(); auto &stream = *me.mainStream;  stream << me.prefix() << ErrorFormat             << "Error"     << me.postfix(); return stream; }
OStream &Log::Fatal()               { auto &me = Log::GetSingleton(); auto &stream = *me.mainStream;  stream << me.prefix() << ErrorFatalFormat        << "Crash" << me.postfix(); return stream; }

auto Log::Info             (const Str &str) -> OStream& { return Info() << str << Log::Flush;}
auto Log::Note             (const Str &str) -> OStream& { return Note() << str << Log::Flush;}
auto Log::Attention        (const Str &str) -> OStream& { return Attention() << str << Log::Flush;}
auto Log::Critical         (const Str &str) -> OStream& { return Critical() << str << Log::Flush;}
auto Log::Debug            (const Str &str) -> OStream& { return Debug() << str << Log::Flush;}
auto Log::Success          (const Str &str) -> OStream& { return Success() << str << Log::Flush;}
auto Log::Warning          (const Str &str) -> OStream& { return Info() << str << Log::Flush;}
auto Log::WarningImportant (const Str &str) -> OStream& { return Warning() << str << Log::Flush;}
auto Log::Error            (const Str &str) -> OStream& { return Error() << str << Log::Flush;}
auto Log::ErrorFatal       (const Str &str) -> OStream& { return Fatal() << str << Log::Flush;}

auto Log::FlushAll() -> void {
    *Log::GetSingleton().mainStream << Log::Flush;
}

OStream &operator<<(OStream &os, const Log::FlushClass &flush) {
    os.flush();
    return os;
}

void Log::notifyCLArgsSetupFinished() {
    InterfaceOwner::notifyCLArgsSetupFinished();


    #if !FORCE_VERBOSE
    if(**logDebug || **verbose){
        std::cout << FGBlue << BoldFace << "\n\n --- SOME LATE DEBUG MESSAGES --- \n";

        if(debugStream->good()){
            StringStream ss; ss << debugStream->rdbuf();
            *mainStream << ss.str();
        }

        if(manageDebugStream) delete debugStream;

        manageDebugStream = false;
        debugStream = &std::cout;

        std::cout << FGBlue << BoldFace << "\n\n --- END LATE DEBUG MESSAGES --- \n";
    }

    if(**logNotes || **verbose){
        std::cout << FGBlue << BoldFace << ((**logDebug || **verbose) ? "" : "\n") << "\n --- SOME LATE NOTES MESSAGES --- \n";

        if(notesStream->good()){
            StringStream ss; ss << notesStream->rdbuf();
            *mainStream << ss.str();
        }

        if(manageNotesStream)    delete        notesStream;

        manageNotesStream = false;
        notesStream = &std::cout;

        std::cout << FGBlue << BoldFace << "\n\n --- END LATE NOTES MESSAGES --- \n";
    }
    #endif


}

void Log::FlushClass::operator()() const {
//    Log::GetSingleton().myStream.flush();
}
