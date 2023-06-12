//
// Created by joao on 10/06/23.
//

#include "Log.h"

#include <typeinfo>
#define GET_CLASS_NAME(obj) typeid(obj).name()

Log* Log::myInstance = nullptr;
const String Log::InfoColor =             Log::ResetFormatting + Log::ForegroundCyan;
const String Log::NoteColor =             Log::ResetFormatting + Log::ForegroundWhite;
const String Log::AttentionColor =        Log::ResetFormatting + Log::ForegroundMagenta;
const String Log::CriticalColor =         Log::ResetFormatting + Log::BoldFace + Log::ForegroundMagenta;
const String Log::DebugColor =            Log::ResetFormatting + Log::Italic + Log::ForegroundMagenta;
const String Log::SuccessColor =          Log::ResetFormatting + Log::ForegroundGreen;
const String Log::WarningColor =          Log::ResetFormatting + Log::ForegroundYellow;
const String Log::WarningImportantColor = Log::ResetFormatting + Log::ForegroundBlack + BackgroundMagenta;
const String Log::ErrorColor =            Log::ResetFormatting + Log::ForegroundRed;
const String Log::ErrorFatalColor =       Log::ResetFormatting + Log::BoldFace + Log::ForegroundRed;

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
    return String("\n") + ResetFormatting + ToString(timer.getElTime_msec()) + "ms  [ ";
};

inline String Log::postfix(){
    return ResetFormatting + " ]  ";
};


OStream &Log::Info()                { auto me = Log::GetSingleton(); auto &stream = *me.mainStream;  stream <<  me.prefix() << InfoColor             << "INFO"      << me.postfix(); return stream; }
OStream &Log::Note()                { auto me = Log::GetSingleton(); auto &stream = *me.notesStream; stream <<  me.prefix() << NoteColor             << "note"      << me.postfix(); return stream; }
OStream &Log::Attention()           { auto me = Log::GetSingleton(); auto &stream = *me.mainStream;  stream <<  me.prefix() << AttentionColor        << "Attention" << me.postfix(); return stream; }
OStream &Log::Critical()            { auto me = Log::GetSingleton(); auto &stream = *me.mainStream;  stream <<  me.prefix() << CriticalColor         << "Critical"  << me.postfix(); return stream; }
OStream &Log::Debug()               { auto me = Log::GetSingleton(); auto &stream = *me.debugStream; stream <<  me.prefix() << DebugColor            << "Debug"     << me.postfix(); return stream; }
OStream &Log::Success()             { auto me = Log::GetSingleton(); auto &stream = *me.mainStream;  stream <<  me.prefix() << SuccessColor          << "Success"   << me.postfix(); return stream; }
OStream &Log::Warning()             { auto me = Log::GetSingleton(); auto &stream = *me.mainStream;  stream <<  me.prefix() << WarningColor          << "Warning"   << me.postfix(); return stream; }
OStream &Log::WarningImportant()    { auto me = Log::GetSingleton(); auto &stream = *me.mainStream;  stream <<  me.prefix() << WarningImportantColor << "Warning!"  << me.postfix(); return stream; }
OStream &Log::Error()               { auto me = Log::GetSingleton(); auto &stream = *me.mainStream;  stream <<  me.prefix() << ErrorColor            << "Error"     << me.postfix(); return stream; }
OStream &Log::ErrorFatal()          { auto me = Log::GetSingleton(); auto &stream = *me.mainStream;  stream <<  me.prefix() << ErrorFatalColor       << "Crash"     << me.postfix(); return stream; }

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

std::ostream &operator<<(std::ostream &os, const Log::FlushClass &flush) {
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
