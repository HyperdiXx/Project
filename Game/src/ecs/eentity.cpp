#include "ecs/eentity.h"

#include "world/ecomponents.h"

namespace ECS
{
    ECSRegistry EEntity::mRegistry = {};

    EEntity EEntity::Create(const std::string& name)
    {
        static int entID = 0;
        
        EEntity result(entID++);

        result.AddComponent<TagComponent>("Cat");

        return result;
    }
}

