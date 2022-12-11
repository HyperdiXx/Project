#pragma once

#include "ecs/ecomponent.h"

#include "emath.h"

using namespace ECS;

class TagComponent final : public EComponent
{
public:

    explicit TagComponent(const std::string& tag) : mTag(tag) {}

private:
    std::string mTag;
};

class TransformComponent final : public EComponent
{
public:
    glm::vec3 mPos;
    glm::quat mRot;
    glm::vec3 mScale;
};
