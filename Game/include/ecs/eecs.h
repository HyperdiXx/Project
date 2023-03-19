#pragma once

#include "eobject.h"
#include "esystem.h"
#include "ecomponent.h"

#include <unordered_map>
#include <array>
#include <memory>
#include <iostream>

namespace ECS
{
    template<typename T>
    class Factory
    {
    public:


    private:
        std::vector<T> mStorage;
    };

    class ECSRegistry
    {
        friend class EEntity;
        friend class EBaseObject;
        friend class ESystem;
    public:

        ECSRegistry() = default;      
        ECSRegistry(ECSRegistry&&) = default;
        ECSRegistry& operator=(ECSRegistry&&) = default;

        ~ECSRegistry()
        {

        }

    private:

        template<typename T, typename... Args>
        void CreateComponent(int ehandle, Args&&... args)
        {
            //UUID uuid = UUIDGenerator::Get();
            //auto& pool = GetPool<T>();
            //size_t index = pool.Allocate(uuid, std::forward<Args>(args)...);
            
            auto& compId = mEntCompStorage[ehandle];
            
            //if (auto it = mCompIdToStorage.find(T::ComponentID); it != mCompIdToStorage.end())
            //{
            //    auto& componentPool = it->second;
            //    
            //    //componentPool.emplace_back(component.get());
            //}
            //else
            //{
            //    auto component = std::make_shared<T>(std::forward<Args>(args)...);
            //    auto &components = mCompIdToStorage[T::ComponentID];
            //    components.emplace_back(component.get());
            //}
        }
        
        template<typename T>
        inline auto GetComponent(int ehandle) const
        {
            if (auto it = mEntCompStorage.find(ehandle); it != mEntCompStorage.end())
            {
                auto& componentPool = it->second;
             
                auto index_it = std::find(componentPool.begin(), componentPool.end(), T::ComponentID);

                if (index_it != componentPool.end())
                {
                    return static_cast<T>(componentPool[index_it]);
                }               
            }

            return T {};
        }

        void Clear()
        {

        }


    private:
        ECSRegistry(const ECSRegistry&) = delete;
        ECSRegistry& operator=(const ECSRegistry&) = delete;

    private:
        
        constexpr static int MAX_COMPONENTS = 30;
        constexpr static int MAX_SYSTEMS = 30;

        std::array<std::vector<std::shared_ptr<ESystem>>, MAX_SYSTEMS> mSystemStorage;
        
        //std::unordered_map<EString::StringId, std::vector<Factory>> mCompIdToStorage;

        std::unordered_map<int, std::vector<EString::StringId>> mEntCompStorage;
        std::unordered_map<int, std::vector<std::shared_ptr<ESystem>>> mEntSystemStorage;
    };

    using ECSRegistryPtr = std::shared_ptr<ECSRegistry>;
}
