#include <world/esystems.h>

#include <world/ecomponents.h>
#include <world/eworld.h>

#include <entt/entt.hpp>

void RenderMeshSystem::update(EProject::Render3D* render3D, entt::registry& reg)
{
    auto gr = reg.group<TransformComponent>(entt::get<StaticMeshComponent>);

    for (auto ent : gr)
    {
        const auto& [tr, mc] = gr.get<TransformComponent, StaticMeshComponent>(ent);
        
        render3D->drawMeshModel(mc, tr);
    }
}

void CanvasSystem::update(World* wrld, EProject::Render2D* render2D, entt::registry& reg)
{
    const auto& view = reg.view<SpriteComponent>();

    for (auto ent : view)
    {
        const auto& sprite = view.get<SpriteComponent>(ent);        
        const auto& pos = sprite.mPos;
        const auto& col = sprite.mColor;

        render2D->drawQuad(pos, col);
    }
}