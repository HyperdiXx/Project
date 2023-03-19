#pragma once

#include "eecs.h"

#include <string>
#include <assert.h>

namespace ECS
{
    constexpr static int sINVALID_ENTITY_ID = -1;

    class EEntity
    {
    private:
        using EntityHandle = int;
    public:
        explicit EEntity(EntityHandle id) : mId(id) {}
        
        static EEntity Create(const std::string& name = std::string());

        bool IsValid() const { return mId != sINVALID_ENTITY_ID; };

        template<typename T, typename... Args>
        void AddComponent(Args&&... args)
        {
            //static_assert(!std::is_base_of<T, EComponent>::value, "T is not base of EComponent!");
            mRegistry.CreateComponent<T>(mId, std::forward<Args>(args)...);
        }

        template<typename T>
        auto GetComponent() const
        {            
            return mRegistry.GetComponent<T>(mId);
        }

    private:
        EntityHandle mId = sINVALID_ENTITY_ID;
        static ECSRegistry mRegistry;
    };
}
