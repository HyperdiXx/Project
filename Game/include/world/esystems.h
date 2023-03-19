#pragma once

#include <egraphics.h>
#include <entt/fwd.hpp>

class World;

namespace RenderMeshSystem
{
    void update(EProject::Render3D* render3D, entt::registry& reg);
};

namespace CanvasSystem
{
    void update(World* wrld, EProject::Render2D* render2d, entt::registry& reg);
}
