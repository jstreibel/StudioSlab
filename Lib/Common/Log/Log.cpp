//
// Created by joao on 10/06/23.
//

#include "Log.h"

#include <typeinfo>
#define GET_CLASS_NAME(obj) typeid(obj).name()

Log* Log::myInstance = nullptr;
const String Log::ForegroundBlack   = "\033[0;30m";
const String Log::ForegroundRed     = "\033[0;31m";
const String Log::ForegroundGreen   = "\033[0;32m";
const String Log::ForegroundYellow  = "\033[0;33m";
const String Log::ForegroundBlue    = "\033[0;34m";
const String Log::ForegroundMagenta = "\033[0;35m";
const String Log::ForegroundCyan    = "\033[0;36m";
const String Log::ForegroundWhite   = "\033[0;37m";

const String Log::BackgroundBlack   = "\033[0;40m" ;
const String Log::BackgroundRed     = "\033[0;41m" ;
const String Log::BackgroundGreen   = "\033[0;42m" ;
const String Log::BackgroundYellow  = "\033[0;43m";
const String Log::BackgroundBlue    = "\033[0;44m" ;
const String Log::BackgroundMagenta = "\033[0;45m";
const String Log::BackgroundCyan    = "\033[0;46m" ;
const String Log::BackgroundWhite   = "\033[0;47m" ;

const String Log::ResetFormatting   = "\033[0m";

const String Log::BoldFace          = "\033[1m";
const String Log::Italic            = "\033[3m"; // Might not work
const String Log::Underscore        = "\033[4m"; // Might not work

const String Log::InfoFormat = Log::ResetFormatting + Log::ForegroundCyan;
const String Log::NoteFormat = Log::ResetFormatting + Log::ForegroundWhite;
const String Log::AttentionFormat = Log::ResetFormatting + Log::ForegroundMagenta;
const String Log::CriticalFormat = Log::ResetFormatting + Log::BoldFace + Log::ForegroundMagenta;
const String Log::DebugFormat = Log::ResetFormatting + Log::Italic + Log::ForegroundMagenta;
const String Log::SuccessFormat = Log::ResetFormatting + Log::ForegroundGreen;
const String Log::WarningFormat = Log::ResetFormatting + Log::ForegroundYellow;
const String Log::WarningImportantFormat = Log::ResetFormatting + Log::ForegroundBlack + Log::BackgroundMagenta;
const String Log::ErrorFormat = Log::ResetFormatting + Log::ForegroundRed;
const String Log::ErrorFatalFormat = Log::ResetFormatting + Log::BoldFace + Log::ForegroundRed;

const Log::FlushClass Log::Flush;

Log::Log() : InterfaceOwner (true) { };

auto Log::GetSingleton() -> Log & {
    if(Log::myInstance == nullptr) {
        auto me = new Log;
        Log::myInstance = me;

        Log::Note() << "Logging system initiated." << Log::Flush;

        me->LateStart("Log Manager", -10);
        me->interface->addParameters({me->logDebug, me->logNotes, me->verbose});
    }

    return *Log::myInstance;
}

inline String Log::prefix(){
    StringStream ss;

    auto time = timer.getElTime_msec();

    ss << String("\n") << ResetFormatting  << std::setw(10) << ToString(timer.getElTime_msec())
       << "ms  [  ";

    return ss.str();
};

inline String Log::postfix(){
    return ResetFormatting + "  ]  ";
};


OStream &Log::Info()                { auto me = Log::GetSingleton(); auto &stream = *me.mainStream;  stream << me.prefix() << InfoFormat              << "INFO"      << me.postfix(); return stream; }
OStream &Log::Note()                { auto me = Log::GetSingleton(); auto &stream = *me.notesStream; stream << me.prefix() << NoteFormat              << "note"      << me.postfix(); return stream; }
OStream &Log::Attention()           { auto me = Log::GetSingleton(); auto &stream = *me.mainStream;  stream << me.prefix() << AttentionFormat         << "Attention" << me.postfix(); return stream; }
OStream &Log::Critical()            { auto me = Log::GetSingleton(); auto &stream = *me.mainStream;  stream << me.prefix() << CriticalFormat          << "Critical"  << me.postfix(); return stream; }
OStream &Log::Debug()               { auto me = Log::GetSingleton(); auto &stream = *me.debugStream; stream << me.prefix() << DebugFormat             << "Debug"     << me.postfix(); return stream; }
OStream &Log::Success()             { auto me = Log::GetSingleton(); auto &stream = *me.mainStream;  stream << me.prefix() << SuccessFormat           << "Success"   << me.postfix(); return stream; }
OStream &Log::Warning()             { auto me = Log::GetSingleton(); auto &stream = *me.mainStream;  stream << me.prefix() << WarningFormat           << "Warning"   << me.postfix(); return stream; }
OStream &Log::WarningImportant()    { auto me = Log::GetSingleton(); auto &stream = *me.mainStream;  stream << me.prefix() << WarningImportantFormat  << "Warning!"  << me.postfix(); return stream; }
OStream &Log::Error()               { auto me = Log::GetSingleton(); auto &stream = *me.mainStream;  stream << me.prefix() << ErrorFormat             << "Error"     << me.postfix(); return stream; }
OStream &Log::ErrorFatal()          { auto me = Log::GetSingleton(); auto &stream = *me.mainStream;  stream << me.prefix() << ErrorFatalFormat        << "Crash"     << me.postfix(); return stream; }

auto Log::Info(const String &str) -> void {
    Info() << str << Log::Flush;
}

auto Log::Note(const String &str) -> void {
    Note() << str << Log::Flush;
}

auto Log::Attention(const String &str) -> void {
    Attention() << str << Log::Flush;
}

auto Log::Critical(const String &str) -> void {
    Critical() << str << Log::Flush;
}

auto Log::Debug(const String &str) -> void {
    Debug() << str << Log::Flush;
}

auto Log::Success(const String &str) -> void {
    Success() << str << Log::Flush;
}

auto Log::Warning(const String &str) -> void {
    Info() << str << Log::Flush;
}

auto Log::WarningImportant(const String &str) -> void {
    Warning() << str << Log::Flush;
}

auto Log::Error(const String &str) -> void {
    Error() << str << Log::Flush;
}

auto Log::ErrorFatal(const String &str) -> void {
    ErrorFatal() << str << Log::Flush;
}

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
