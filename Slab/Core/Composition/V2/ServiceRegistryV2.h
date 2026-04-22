#ifndef STUDIOSLAB_SERVICE_REGISTRY_V2_H
#define STUDIOSLAB_SERVICE_REGISTRY_V2_H

#include "Utils/Pointer.h"
#include "Utils/String.h"

#include <any>
#include <map>
#include <tuple>
#include <typeindex>

namespace Slab::Core::Composition::V2 {

    struct FServiceKeyV2 {
        std::type_index ServiceType = std::type_index(typeid(void));
        Str InstanceId;

        [[nodiscard]] auto operator<(const FServiceKeyV2 &rhs) const -> bool {
            return std::tie(ServiceType, InstanceId) < std::tie(rhs.ServiceType, rhs.InstanceId);
        }
    };

    class FServiceRegistryV2 {
    public:
        template<typename TService>
        auto Register(const TPointer<TService> &service, Str instanceId = {}) -> void {
            Services[MakeKey<TService>(std::move(instanceId))] = service;
        }

        template<typename TService>
        [[nodiscard]] auto Resolve(const Str &instanceId = {}) const -> TPointer<TService> {
            const auto it = Services.find(MakeKey<TService>(instanceId));
            if (it == Services.end()) return nullptr;

            const auto *service = std::any_cast<TPointer<TService>>(&it->second);
            if (service == nullptr) return nullptr;
            return *service;
        }

        template<typename TService>
        [[nodiscard]] auto Has(const Str &instanceId = {}) const -> bool {
            return Services.contains(MakeKey<TService>(instanceId));
        }

        auto Clear() -> void;
        [[nodiscard]] auto Count() const -> std::size_t;

    private:
        template<typename TService>
        [[nodiscard]] static auto MakeKey(const Str &instanceId) -> FServiceKeyV2 {
            return FServiceKeyV2{std::type_index(typeid(TService)), instanceId};
        }

        std::map<FServiceKeyV2, std::any> Services;
    };

} // namespace Slab::Core::Composition::V2

#endif // STUDIOSLAB_SERVICE_REGISTRY_V2_H
