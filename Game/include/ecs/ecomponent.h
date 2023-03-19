#pragma once

#include "utils/estring.h"
#include "eobject.h"

#include <string>

namespace ECS
{
    using ComponentIDType = size_t;

    class EComponentIDGenerator
    {
        inline static ComponentIDType counter;
    public:
        template<typename C>
        inline constexpr static ComponentIDType type = counter++;
    };

#define MAKE_COMPONENT(class_name)\
        public:\
                class_name(const class_name&) = delete;\
                class_name(class_name&&) = delete;\
                class_name& operator=(const class_name&) = delete;\
                class_name& operator=(class_name&&) = delete;\
        private:\
                static constexpr EString::StringId ComponentID = STRING_ID(#class_name);\
                friend class ECSRegistry;
}