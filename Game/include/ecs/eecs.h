#pragma once

#include "esystem.h"
#include "ecomponent.h"

#include <unordered_map>
#include <array>
#include <memory>

namespace ECS
{

    class ECSRegistry
    {
        friend class EEntity;
        friend class EComponent;
        friend class ESystem;
    public:

        ECSRegistry() = default;
      
    private:

        template<typename T, typename... Args>
        void CreateComponent(int handle, Args&&... args)
        {
            //UUID uuid = UUIDGenerator::Get();
            //auto& pool = GetPool<T>();
            //size_t index = pool.Allocate(uuid, std::forward<Args>(args)...);
            

            auto& pool = mEntCompStorage[handle];
            
            auto comp = std::make_shared<T>(std::forward<Args>(args)...);            
            pool.emplace_back(comp);
        }
        
        template<typename T>
        T& GetComponent() const
        {
            
        }


    private:
        ECSRegistry(const ECSRegistry& reg) = delete;
        ECSRegistry& operator=(const ECSRegistry& reg) = delete;

    private:
        constexpr static int MAX_COMPONENTS = 30;
        constexpr static int MAX_SYSTEMS = 30;
        std::array<std::vector<std::shared_ptr<EComponent>>, MAX_COMPONENTS> mCompStorage;
        std::array<std::vector<std::shared_ptr<ESystem>>, MAX_SYSTEMS> mSystemStorage;
        
        std::unordered_map<int, std::vector<std::shared_ptr<EComponent>>> mEntCompStorage;
        std::unordered_map<int, std::vector<std::shared_ptr<ESystem>>> mEntSystemStorage;
    };

    using ECSRegistryPtr = std::shared_ptr<ECSRegistry>;
}
