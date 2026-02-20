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

    FLog *FLog::pMyInstance = nullptr;

    const Str FLog::StartFormat = "\033[";
    const Str FLog::SepFormat = ";";
    const Str FLog::EndFormat = "m";

    const Str FLog::FGBlack = "\033[30m";
    const Str FLog::FGRed = "\033[31m";
    const Str FLog::FGGreen = "\033[32m";
    const Str FLog::FGYellow = "\033[33m";
    const Str FLog::FGBlue = "\033[34m";
    const Str FLog::FGMagenta = "\033[35m";
    const Str FLog::FGCyan = "\033[36m";
    const Str FLog::FGWhite = "\033[37m";

    const Str FLog::BGBlack = "\033[40m";
    const Str FLog::BGRed = "\033[41m";
    const Str FLog::BGGreen = "\033[42m";
    const Str FLog::BGYellow = "\033[43m";
    const Str FLog::BGBlue = "\033[44m";
    const Str FLog::BGMagenta = "\033[45m";
    const Str FLog::BGCyan = "\033[46m";
    const Str FLog::BGWhite = "\033[47m";

    const Str FLog::ResetFormatting = "\033[0m";

    const Str FLog::BoldFace = "\033[1m";
    const Str FLog::Italic = "\033[3m"; // Might not work
    const Str FLog::Underscore = "\033[4m"; // Might not work

    const Str FLog::StatusFormat = FLog::ResetFormatting + FLog::FGBlue;
    const Str FLog::InfoFormat = FLog::ResetFormatting + FLog::FGCyan;
    const Str FLog::NoteFormat = FLog::ResetFormatting + FLog::FGWhite;
    const Str FLog::AttentionFormat = FLog::ResetFormatting + FLog::FGMagenta;
    const Str FLog::CriticalFormat = FLog::ResetFormatting + FLog::BGMagenta + FLog::BoldFace;
    const Str FLog::DebugFormat = FLog::ResetFormatting + FLog::FGMagenta + FLog::Italic;
    const Str FLog::SuccessFormat = FLog::ResetFormatting + FLog::FGGreen + FLog::BoldFace;
    const Str FLog::FailFormat = FLog::ResetFormatting + FLog::FGRed + FLog::BoldFace;
    const Str FLog::WarningFormat = FLog::ResetFormatting + FLog::FGYellow;
    const Str FLog::WarningImportantFormat = FLog::ResetFormatting + FLog::FGBlack + FLog::BGMagenta;
    const Str FLog::ErrorFormat = FLog::ResetFormatting + FLog::FGRed;
    const Str FLog::ErrorFatalFormat = FLog::ResetFormatting + FLog::FGRed + FLog::BoldFace;

    FLog::FLog() : FSingleton<FLog>("Log"), FInterfaceOwner(true) {};

    auto FLog::GetSingleton() -> FLog & {
        if (FSingleton<FLog>::singleInstance == nullptr) {
            auto me = new FLog;
            FLog::pMyInstance = me;
            FSingleton<FLog>::singleInstance = FLog::pMyInstance;

            FLog::Note() << "Logging system initiated." << FLog::Flush;

            me->LateStart("Log Manager", -10);
            me->Interface->AddParameters({me->LogDebug, me->LogNotes, me->Verbose});
        }

        return *FLog::pMyInstance;
    }

    inline Str FLog::Prefix() {
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

    inline Str FLog::Postfix() {
        return ResetFormatting + " ] ";
    };


    OStream &FLog::Status() {
        auto &me = FLog::GetSingleton();
        auto &stream = *me.pMainStream;
        stream << me.Prefix() << StatusFormat << "Status" << me.Postfix();
        return stream;
    }

    OStream &FLog::Info() {
        auto &me = FLog::GetSingleton();
        auto &stream = *me.pMainStream;
        stream << me.Prefix() << InfoFormat << "Info" << me.Postfix();
        return stream;
    }

    OStream &FLog::Note() {
        auto &me = FLog::GetSingleton();
        auto &stream = *me.pNotesStream;
        stream << me.Prefix() << NoteFormat << "Note" << me.Postfix();
        return stream;
    }

    OStream &FLog::Attention() {
        auto &me = FLog::GetSingleton();
        auto &stream = *me.pMainStream;
        stream << me.Prefix() << AttentionFormat << "Attention" << me.Postfix();
        return stream;
    }

    OStream &FLog::Critical() {
        auto &me = FLog::GetSingleton();
        auto &stream = *me.pMainStream;
        stream << me.Prefix() << CriticalFormat << "Critical" << me.Postfix();
        return stream;
    }

    OStream &FLog::Debug() {
        auto &me = FLog::GetSingleton();
        auto &stream = *me.pDebugStream;
        stream << me.Prefix() << DebugFormat << "Debug" << me.Postfix();
        return stream;
    }

    OStream &FLog::Success() {
        auto &me = FLog::GetSingleton();
        auto &stream = *me.pMainStream;
        stream << me.Prefix() << SuccessFormat << "Success" << me.Postfix();
        return stream;
    }

    OStream &FLog::Fail() {
        auto &me = FLog::GetSingleton();
        auto &stream = *me.pMainStream;
        stream << me.Prefix() << FailFormat << "Fail" << me.Postfix();
        return stream;
    }

    OStream &FLog::Warning() {
        auto &me = FLog::GetSingleton();
        auto &stream = *me.pMainStream;
        stream << me.Prefix() << WarningFormat << "Warning" << me.Postfix();
        return stream;
    }

    OStream &FLog::WarningImportant() {
        auto &me = FLog::GetSingleton();
        auto &stream = *me.pMainStream;
        stream << me.Prefix() << WarningImportantFormat << "Warning!" << me.Postfix();
        return stream;
    }

    OStream &FLog::Error() {
        auto &me = FLog::GetSingleton();
        auto &stream = *me.pMainStream;
        stream << me.Prefix() << ErrorFormat << "Error" << me.Postfix();
        return stream;
    }

    OStream &FLog::ErrorFatal() {
        auto &me = FLog::GetSingleton();
        auto &stream = *me.pMainStream;
        stream << me.Prefix() << ErrorFatalFormat << "Crash" << me.Postfix();
        return stream;
    }

    auto FLog::Status(const Str &str) -> OStream & { return Status() << str << FLog::Flush; }

    auto FLog::Info(const Str &str) -> OStream & { return Info() << str << FLog::Flush; }

    auto FLog::Note(const Str &str) -> OStream & { return Note() << str << FLog::Flush; }

    auto FLog::Attention(const Str &str) -> OStream & { return Attention() << str << FLog::Flush; }

    auto FLog::Critical(const Str &str) -> OStream & { return Critical() << str << FLog::Flush; }

    auto FLog::Debug(const Str &str) -> OStream & { return Debug() << str << FLog::Flush; }

    auto FLog::Success(const Str &str) -> OStream & { return Success() << str << FLog::Flush; }

    auto FLog::Fail(const Str &str) -> OStream & { return Fail() << str << FLog::Flush; }

    auto FLog::Warning(const Str &str) -> OStream & { return Warning() << str << FLog::Flush; }

    auto FLog::WarningImportant(const Str &str) -> OStream & { return Warning() << str << FLog::Flush; }

    auto FLog::Error(const Str &str) -> OStream & { return Error() << str << FLog::Flush; }

    auto FLog::ErrorFatal(const Str &str) -> OStream & { return ErrorFatal() << str << FLog::Flush; }

    auto FLog::FlushAll() -> void {
        *FLog::GetSingleton().pMainStream << FLog::Flush;
    }

    void FLog::NotifyInterfaceSetupIsFinished() {
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


    const FLog::FlushClass FLog::Flush;

    OStream &operator<<(OStream &os, const FLog::FlushClass &flush) {
        os.flush();
        return os;
    }

    void FLog::FlushClass::operator()() const {}


    FLog::FormattingClass FLog::Format;

    OStream &operator<<(OStream &OS, const FLog::FormattingClass &width) {
        OS << std::setw((int) width.Len);

        switch (width.TextPosition) {
            case FLog::Left:
                OS << std::left;
                break;
            case FLog::Right:
                OS << std::right;
                break;
        }

        return OS;
    }

    auto FLog::FormattingClass::operator()(CountType _len) -> const FormattingClass & {
        this->Len = _len;
        return *this;
    }

    auto FLog::FormattingClass::operator()(ETextPosition pos) -> const FormattingClass & {
        this->TextPosition = pos;
        return *this;
    }


}
