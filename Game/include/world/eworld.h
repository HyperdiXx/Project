#pragma once

#include <entt/entt.hpp>
#include <emath.h>

#include <world/esystems.h>
#include <eutils.h>

struct FrameInfo
{
    float dt = 0.0f;
    EProject::Render2D* render2DPtr = nullptr;
    EProject::Render3D* render3DPtr = nullptr;
};

class World
{
public:
    World() = default;
    ~World();

    World(const World& wrld) = delete;
    World& operator=(const World& wrld) = delete;
    
    void init(std::shared_ptr<EProject::AssetManager>& mng, const GDevicePtr& dev);

    entt::entity createObject(const std::string& tag = "");
    void destroyObject(const std::string& tag = "");
    
    template<typename T, typename... Args>
    auto& addComponent(const entt::entity ent, Args&&... args)
    {
        return m_registry.get_or_emplace<T>(ent, std::forward<Args>(args)...);
    }

    void update(const FrameInfo& fi);
    void updateFixed(const FrameInfo& fi);
    
    void draw(const FrameInfo& fi);

    glm::ivec2 screenToIso(int x, int y);
private:
    void preInit();

    void postInit();    

private:

    entt::registry m_registry;
    entt::organizer m_organizer;
    entt::dispatcher m_dispatcher;

    StaticMeshRenderablePtr helmetRenderable;
    StaticMeshRenderablePtr scifihelmetRenderable;
    //entt::observer m_renderSystem;
};


