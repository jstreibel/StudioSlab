//
// Created by joao on 10/06/23.
//

#include "Log.h"

#include <typeinfo>
#define GET_CLASS_NAME(obj) typeid(obj).name()

Log* Log::myInstance = nullptr;

const Str Log::ForegroundBlack   = "\033[0;30m";
const Str Log::ForegroundRed     = "\033[0;31m";
const Str Log::ForegroundGreen   = "\033[0;32m";
const Str Log::ForegroundYellow  = "\033[0;33m";
const Str Log::ForegroundBlue    = "\033[0;34m";
const Str Log::ForegroundMagenta = "\033[0;35m";
const Str Log::ForegroundCyan    = "\033[0;36m";
const Str Log::ForegroundWhite   = "\033[0;37m";

const Str Log::BackgroundBlack   = "\033[0;40m" ;
const Str Log::BackgroundRed     = "\033[0;41m" ;
const Str Log::BackgroundGreen   = "\033[0;42m" ;
const Str Log::BackgroundYellow  = "\033[0;43m";
const Str Log::BackgroundBlue    = "\033[0;44m" ;
const Str Log::BackgroundMagenta = "\033[0;45m";
const Str Log::BackgroundCyan    = "\033[0;46m" ;
const Str Log::BackgroundWhite   = "\033[0;47m" ;

const Str Log::ResetFormatting   = "\033[0m";

const Str Log::BoldFace          = "\033[1m";
const Str Log::Italic            = "\033[3m"; // Might not work
const Str Log::Underscore        = "\033[4m"; // Might not work

const Str Log::InfoFormat = Log::ResetFormatting + Log::ForegroundCyan;
const Str Log::NoteFormat = Log::ResetFormatting + Log::ForegroundWhite;
const Str Log::AttentionFormat = Log::ResetFormatting + Log::ForegroundMagenta;
const Str Log::CriticalFormat = Log::ResetFormatting + Log::BoldFace + Log::ForegroundMagenta;
const Str Log::DebugFormat = Log::ResetFormatting + Log::Italic + Log::ForegroundMagenta;
const Str Log::SuccessFormat = Log::ResetFormatting + Log::ForegroundGreen;
const Str Log::WarningFormat = Log::ResetFormatting + Log::ForegroundYellow;
const Str Log::WarningImportantFormat = Log::ResetFormatting + Log::ForegroundBlack + Log::BackgroundMagenta;
const Str Log::ErrorFormat = Log::ResetFormatting + Log::ForegroundRed;
const Str Log::ErrorFatalFormat = Log::ResetFormatting + Log::BoldFace + Log::ForegroundRed;

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

    ss << Str("\n") << ResetFormatting << std::setw(10) << ToStr(timer.getElTime_msec())
       << "ms  [ ";

    return ss.str();
};

inline Str Log::postfix(){
    return ResetFormatting + " ]  ";
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
OStream &Log::ErrorFatal()          { auto &me = Log::GetSingleton(); auto &stream = *me.mainStream;  stream << me.prefix() << ErrorFatalFormat        << "Crash"     << me.postfix(); return stream; }

auto Log::Info             (const Str &str) -> OStream& { return Info() << str << Log::Flush;}
auto Log::Note             (const Str &str) -> OStream& { return Note() << str << Log::Flush;}
auto Log::Attention        (const Str &str) -> OStream& { return Attention() << str << Log::Flush;}
auto Log::Critical         (const Str &str) -> OStream& { return Critical() << str << Log::Flush;}
auto Log::Debug            (const Str &str) -> OStream& { return Debug() << str << Log::Flush;}
auto Log::Success          (const Str &str) -> OStream& { return Success() << str << Log::Flush;}
auto Log::Warning          (const Str &str) -> OStream& { return Info() << str << Log::Flush;}
auto Log::WarningImportant (const Str &str) -> OStream& { return Warning() << str << Log::Flush;}
auto Log::Error            (const Str &str) -> OStream& { return Error() << str << Log::Flush;}
auto Log::ErrorFatal       (const Str &str) -> OStream& { return ErrorFatal() << str << Log::Flush;}

auto Log::FlushAll() -> void {
    *Log::GetSingleton().mainStream << Log::Flush;
}

OStream &operator<<(OStream &os, const Log::FlushClass &flush) {
    os.flush();
    return os;
}

void Log::notifyCLArgsSetupFinished() {
    InterfaceOwner::notifyCLArgsSetupFinished();



    if(**logDebug || **verbose){
        std::cout << ForegroundBlue << BoldFace << "\n\n --- SOME LATE DEBUG MESSAGES --- \n";

        *mainStream << debugStream->rdbuf();
        if(manageDebugStream)
            delete debugStream;

        manageDebugStream = false;
        debugStream = &std::cout;

        std::cout << ForegroundBlue << BoldFace << "\n\n --- END LATE DEBUG MESSAGES --- \n";
    }

    if(**logNotes || **verbose){
        std::cout << ForegroundBlue << BoldFace << "\n\n --- SOME LATE NOTES MESSAGES --- \n";
        *mainStream << notesStream->rdbuf();

        if(manageNotesStream)
            delete notesStream;

        manageNotesStream = false;
        notesStream = &std::cout;

        std::cout << ForegroundBlue << BoldFace << "\n\n --- END LATE NOTES MESSAGES --- \n";
    }


}

void Log::FlushClass::operator()() const {
//    Log::GetSingleton().myStream.flush();
}
