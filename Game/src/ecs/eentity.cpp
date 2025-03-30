#include "ecs/eentity.h"

#include "world/ecomponents.h"

namespace ECS
{
    ECSRegistry EEntity::mRegistry = {};

    EEntity EEntity::Create(const std::string& name)
    {
        static int entID = 0;
        
        EEntity result(entID++);

        if (!name.empty())
        {
            result.AddComponent<TagComponent>(name);
        }

        return result;
    }
}

