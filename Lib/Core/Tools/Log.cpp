//
// Created by joao on 10/06/23.
//

#include "Log.h"

#include <typeinfo>
#include <iomanip>
#include <chrono>
#include <ctime>
#define GET_CLASS_NAME(obj) typeid(obj).name()


namespace Slab::Core {

    Log *Log::pMyInstance = nullptr;

    const Str Log::StartFormat = "\033[";
    const Str Log::SepFormat = ";";
    const Str Log::EndFormat = "m";

    const Str Log::FGBlack = "\033[30m";
    const Str Log::FGRed = "\033[31m";
    const Str Log::FGGreen = "\033[32m";
    const Str Log::FGYellow = "\033[33m";
    const Str Log::FGBlue = "\033[34m";
    const Str Log::FGMagenta = "\033[35m";
    const Str Log::FGCyan = "\033[36m";
    const Str Log::FGWhite = "\033[37m";

    const Str Log::BGBlack = "\033[40m";
    const Str Log::BGRed = "\033[41m";
    const Str Log::BGGreen = "\033[42m";
    const Str Log::BGYellow = "\033[43m";
    const Str Log::BGBlue = "\033[44m";
    const Str Log::BGMagenta = "\033[45m";
    const Str Log::BGCyan = "\033[46m";
    const Str Log::BGWhite = "\033[47m";

    const Str Log::ResetFormatting = "\033[0m";

    const Str Log::BoldFace = "\033[1m";
    const Str Log::Italic = "\033[3m"; // Might not work
    const Str Log::Underscore = "\033[4m"; // Might not work

    const Str Log::StatusFormat = Log::ResetFormatting + Log::FGBlue;
    const Str Log::InfoFormat = Log::ResetFormatting + Log::FGCyan;
    const Str Log::NoteFormat = Log::ResetFormatting + Log::FGWhite;
    const Str Log::AttentionFormat = Log::ResetFormatting + Log::FGMagenta;
    const Str Log::CriticalFormat = Log::ResetFormatting + Log::BGMagenta + Log::BoldFace;
    const Str Log::DebugFormat = Log::ResetFormatting + Log::FGMagenta + Log::Italic;
    const Str Log::SuccessFormat = Log::ResetFormatting + Log::FGGreen + Log::BoldFace;
    const Str Log::FailFormat = Log::ResetFormatting + Log::FGRed + Log::BoldFace;
    const Str Log::WarningFormat = Log::ResetFormatting + Log::FGYellow;
    const Str Log::WarningImportantFormat = Log::ResetFormatting + Log::FGBlack + Log::BGMagenta;
    const Str Log::ErrorFormat = Log::ResetFormatting + Log::FGRed;
    const Str Log::ErrorFatalFormat = Log::ResetFormatting + Log::FGRed + Log::BoldFace;

    Log::Log() : Singleton<Log>("Log"), FInterfaceOwner(true) {};

    auto Log::GetSingleton() -> Log & {
        if (Singleton::singleInstance == nullptr) {
            auto me = new Log;
            Log::pMyInstance = me;
            Singleton::singleInstance = Log::pMyInstance;

            Log::Note() << "Logging system initiated." << Log::Flush;

            me->LateStart("Log Manager", -10);
            me->Interface->AddParameters({me->LogDebug, me->LogNotes, me->Verbose});
        }

        return *Log::pMyInstance;
    }

    inline Str Log::Prefix() {
        StringStream ss;

        // Wall-clock timestamp (UTC) with milliseconds
        const auto now = std::chrono::system_clock::now();
        const auto tt = std::chrono::system_clock::to_time_t(now);
        std::tm tm{};
#if defined(_WIN32)
        gmtime_s(&tm, &tt);
#else
        gmtime_r(&tt, &tm);
#endif
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        ss << "\n" << ResetFormatting
           << std::put_time(&tm, "%FT%T") << '.'
           << std::setfill('0') << std::setw(3) << ms.count()
           << 'Z'
           << " [ ";

        return ss.str();
    };

    inline Str Log::Postfix() {
        return ResetFormatting + " ] ";
    };


    OStream &Log::Status() {
        auto &me = Log::GetSingleton();
        auto &stream = *me.pMainStream;
        stream << me.Prefix() << StatusFormat << "Status" << me.Postfix();
        return stream;
    }

    OStream &Log::Info() {
        auto &me = Log::GetSingleton();
        auto &stream = *me.pMainStream;
        stream << me.Prefix() << InfoFormat << "Info" << me.Postfix();
        return stream;
    }

    OStream &Log::Note() {
        auto &me = Log::GetSingleton();
        auto &stream = *me.pNotesStream;
        stream << me.Prefix() << NoteFormat << "Note" << me.Postfix();
        return stream;
    }

    OStream &Log::Attention() {
        auto &me = Log::GetSingleton();
        auto &stream = *me.pMainStream;
        stream << me.Prefix() << AttentionFormat << "Attention" << me.Postfix();
        return stream;
    }

    OStream &Log::Critical() {
        auto &me = Log::GetSingleton();
        auto &stream = *me.pMainStream;
        stream << me.Prefix() << CriticalFormat << "Critical" << me.Postfix();
        return stream;
    }

    OStream &Log::Debug() {
        auto &me = Log::GetSingleton();
        auto &stream = *me.pDebugStream;
        stream << me.Prefix() << DebugFormat << "Debug" << me.Postfix();
        return stream;
    }

    OStream &Log::Success() {
        auto &me = Log::GetSingleton();
        auto &stream = *me.pMainStream;
        stream << me.Prefix() << SuccessFormat << "Success" << me.Postfix();
        return stream;
    }

    OStream &Log::Fail() {
        auto &me = Log::GetSingleton();
        auto &stream = *me.pMainStream;
        stream << me.Prefix() << FailFormat << "Fail" << me.Postfix();
        return stream;
    }

    OStream &Log::Warning() {
        auto &me = Log::GetSingleton();
        auto &stream = *me.pMainStream;
        stream << me.Prefix() << WarningFormat << "Warning" << me.Postfix();
        return stream;
    }

    OStream &Log::WarningImportant() {
        auto &me = Log::GetSingleton();
        auto &stream = *me.pMainStream;
        stream << me.Prefix() << WarningImportantFormat << "Warning!" << me.Postfix();
        return stream;
    }

    OStream &Log::Error() {
        auto &me = Log::GetSingleton();
        auto &stream = *me.pMainStream;
        stream << me.Prefix() << ErrorFormat << "Error" << me.Postfix();
        return stream;
    }

    OStream &Log::ErrorFatal() {
        auto &me = Log::GetSingleton();
        auto &stream = *me.pMainStream;
        stream << me.Prefix() << ErrorFatalFormat << "Crash" << me.Postfix();
        return stream;
    }

    auto Log::Status(const Str &str) -> OStream & { return Status() << str << Log::Flush; }

    auto Log::Info(const Str &str) -> OStream & { return Info() << str << Log::Flush; }

    auto Log::Note(const Str &str) -> OStream & { return Note() << str << Log::Flush; }

    auto Log::Attention(const Str &str) -> OStream & { return Attention() << str << Log::Flush; }

    auto Log::Critical(const Str &str) -> OStream & { return Critical() << str << Log::Flush; }

    auto Log::Debug(const Str &str) -> OStream & { return Debug() << str << Log::Flush; }

    auto Log::Success(const Str &str) -> OStream & { return Success() << str << Log::Flush; }

    auto Log::Fail(const Str &str) -> OStream & { return Fail() << str << Log::Flush; }

    auto Log::Warning(const Str &str) -> OStream & { return Warning() << str << Log::Flush; }

    auto Log::WarningImportant(const Str &str) -> OStream & { return Warning() << str << Log::Flush; }

    auto Log::Error(const Str &str) -> OStream & { return Error() << str << Log::Flush; }

    auto Log::ErrorFatal(const Str &str) -> OStream & { return ErrorFatal() << str << Log::Flush; }

    auto Log::FlushAll() -> void {
        *Log::GetSingleton().pMainStream << Log::Flush;
    }

    void Log::NotifyInterfaceSetupIsFinished() {
        FInterfaceOwner::NotifyInterfaceSetupIsFinished();


#if !FORCE_VERBOSE
        if (**LogDebug || **Verbose) {
            std::cout << FGBlue << BoldFace << "\n\n --- SOME LATE DEBUG MESSAGES --- \n";

            if (pDebugStream->good()) {
                StringStream ss;
                ss << pDebugStream->rdbuf();
                *pMainStream << ss.str();
            }

            if (bManageDebugStream) delete pDebugStream;

            bManageDebugStream = false;
            pDebugStream = &std::cout;

            std::cout << FGBlue << BoldFace << "\n\n --- END LATE DEBUG MESSAGES --- \n";
        }

        if (**LogNotes || **Verbose) {
            std::cout << FGBlue << BoldFace << ((**LogDebug || **Verbose) ? "" : "\n")
                      << "\n --- SOME LATE NOTES MESSAGES --- \n";

            if (pNotesStream->good()) {
                StringStream ss;
                ss << pNotesStream->rdbuf();
                *pMainStream << ss.str();
            }

            if (bManageNotesStream) delete pNotesStream;

            bManageNotesStream = false;
            pNotesStream = &std::cout;

            std::cout << FGBlue << BoldFace << "\n\n --- END LATE NOTES MESSAGES --- \n";
        }
#endif


    }


    const Log::FlushClass Log::Flush;

    OStream &operator<<(OStream &os, const Log::FlushClass &flush) {
        os.flush();
        return os;
    }

    void Log::FlushClass::operator()() const {}


    Log::FormattingClass Log::Format;

    OStream &operator<<(OStream &OS, const Log::FormattingClass &width) {
        OS << std::setw((int) width.Len);

        switch (width.TextPosition) {
            case Log::Left:
                OS << std::left;
                break;
            case Log::Right:
                OS << std::right;
                break;
        }

        return OS;
    }

    auto Log::FormattingClass::operator()(CountType _len) -> const FormattingClass & {
        this->Len = _len;
        return *this;
    }

    auto Log::FormattingClass::operator()(ETextPosition pos) -> const FormattingClass & {
        this->TextPosition = pos;
        return *this;
    }


}
