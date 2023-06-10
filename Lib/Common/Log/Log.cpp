//
// Created by joao on 10/06/23.
//

#include "Log.h"

#include <typeinfo>
#define GET_CLASS_NAME(obj) typeid(obj).name()

Log* Log::myInstance = nullptr;
const String Log::InfoColor =             Log::ResetFormatting + Log::ForegroundCyan;
const String Log::NoteColor =             Log::ResetFormatting + Log::ForegroundBlue;
const String Log::AttentionColor =        Log::ResetFormatting + Log::ForegroundMagenta;
const String Log::CriticalColor =         Log::ResetFormatting + Log::BoldFace + Log::ForegroundMagenta;
const String Log::DebugColor =            Log::ResetFormatting + Log::Italic + Log::ForegroundMagenta;
const String Log::SuccessColor =          Log::ResetFormatting + Log::ForegroundGreen;
const String Log::WarningColor =          Log::ResetFormatting + Log::ForegroundYellow;
const String Log::WarningImportantColor = Log::ResetFormatting + Log::ForegroundBlack + BackgroundMagenta;
const String Log::ErrorColor =            Log::ResetFormatting + Log::ForegroundRed;
const String Log::ErrorFatalColor =       Log::ResetFormatting + Log::BoldFace + Log::ForegroundRed;

String Log::prefix;
String Log::postfix;

const Log::FlushClass Log::Flush;

Log::Log() : myStream(std::cout) {
    StringStream ss;

    ss << "\n" << ResetFormatting << " [";
    prefix = ss.str();

    ss.str("");
    ss << ResetFormatting << "]  ";
    postfix = ss.str();


}

auto Log::GetSingleton() -> Log & {
    if(Log::myInstance == nullptr) {
        Log::myInstance = new Log;

        Log::Note() << "Logging system initiated." << Log::Flush;
    }

    return *Log::myInstance;
}


OStream &Log::Info()                { auto &stream = Log::GetSingleton().myStream; stream << prefix << InfoColor                << "Info"      << postfix; return stream; }
OStream &Log::Note()                { auto &stream = Log::GetSingleton().myStream; stream << prefix << NoteColor                << "Note"      << postfix; return stream; }
OStream &Log::Attention()           { auto &stream = Log::GetSingleton().myStream; stream << prefix << AttentionColor           << "Attention" << postfix; return stream; }
OStream &Log::Critical()            { auto &stream = Log::GetSingleton().myStream; stream << prefix << CriticalColor            << "Critical"  << postfix; return stream; }
OStream &Log::Debug()               { auto &stream = Log::GetSingleton().myStream; stream << prefix << DebugColor               << "Debug"     << postfix; return stream; }
OStream &Log::Success()             { auto &stream = Log::GetSingleton().myStream; stream << prefix << SuccessColor             << "Success"   << postfix; return stream; }
OStream &Log::Warning()             { auto &stream = Log::GetSingleton().myStream; stream << prefix << WarningColor             << "Warning"   << postfix; return stream; }
OStream &Log::WarningImportant()    { auto &stream = Log::GetSingleton().myStream; stream << prefix << WarningImportantColor    << "Warning!"  << postfix; return stream; }
OStream &Log::Error()               { auto &stream = Log::GetSingleton().myStream; stream << prefix << ErrorColor               << "Error"     << postfix; return stream; }
OStream &Log::ErrorFatal()          { auto &stream = Log::GetSingleton().myStream; stream << prefix << ErrorFatalColor          << "Crash"     << postfix; return stream; }

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
    Log::GetSingleton().myStream << Log::Flush;
}

std::ostream &operator<<(std::ostream &os, const Log::FlushClass &flush) {
    os.flush();
    return os;
}

void Log::FlushClass::operator()() const {
//    Log::GetSingleton().myStream.flush();
}
