#ifndef STUDIOSLAB_PLATFORM_HOST_V2_H
#define STUDIOSLAB_PLATFORM_HOST_V2_H

#include "Utils/Pointer.h"
#include "Utils/String.h"

#include <functional>

namespace Slab::Core::Platform::V2 {

    struct FPlatformHostDescriptorV2 {
        Str PlatformHostId;
        Str DisplayName;
        bool bHeadless = false;
        bool bProvidesGraphics = false;
        bool bProvidesSystemWindows = false;
    };

    class IPlatformHostV2 {
    public:
        virtual ~IPlatformHostV2() = default;

        [[nodiscard]] virtual auto GetDescriptor() const -> const FPlatformHostDescriptorV2 & = 0;

        virtual auto Startup() -> void = 0;
        virtual auto Run() -> void = 0;
        virtual auto RequestStop() -> void = 0;

        [[nodiscard]] auto GetPlatformHostId() const -> const Str & {
            return GetDescriptor().PlatformHostId;
        }

        [[nodiscard]] auto GetDisplayName() const -> const Str & {
            return GetDescriptor().DisplayName;
        }

        [[nodiscard]] auto IsHeadless() const -> bool {
            return GetDescriptor().bHeadless;
        }

        [[nodiscard]] auto ProvidesGraphics() const -> bool {
            return GetDescriptor().bProvidesGraphics;
        }

        [[nodiscard]] auto ProvidesSystemWindows() const -> bool {
            return GetDescriptor().bProvidesSystemWindows;
        }
    };

    DefinePointers(IPlatformHostV2)

    using FPlatformHostFactoryV2 = std::function<IPlatformHostV2_ptr()>;

} // namespace Slab::Core::Platform::V2

#endif // STUDIOSLAB_PLATFORM_HOST_V2_H
