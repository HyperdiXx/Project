#include <world/eworld.h>
#include <world/ecomponents.h>


#include <graphics/emesh.h>

#include <eheader.h>

struct RenderMeshSubmitEvent final
{
    int m_ind = -1;
    int m_mesh = 1;
};

using namespace EProject;

World::~World()
{
}

void World::preInit()
{

}

void World::init(std::shared_ptr<AssetManager>& mng, const GDevicePtr& dev)
{
    preInit();

    const auto ent1 = createObject("Ent1");
    addComponent<TransformComponent>(ent1, glm::vec3(0.0f, 5.0f, 0.0f));

    int spriteCounter = 0;

    std::vector<glm::vec2> cache = {};

    for (int y = -30; y < 31; ++y)
    {
        for (int x = -30; x < 31; ++x)
        {
            const glm::ivec2 pos(x, y);
            const auto isoPos = screenToIso(x, y);
            const glm::vec2 pos2d(isoPos.x * 4.5f, isoPos.y * 4.5f);

            if (auto it = std::find(cache.begin(), cache.end(), pos2d); it == cache.end())
            {
                auto spriteEnt = createObject();
                auto& spriteComp = addComponent<SpriteComponent>(spriteEnt);
                spriteComp.mPos = pos2d;

                cache.emplace_back(pos2d);
                ++spriteCounter;
            }
        }
    }

    std::cout << "Sprite rendered: " << spriteCounter << "\n";

    const auto modelsDir = PathHandler::getModelsDir();

    PathKey helmetKey(modelsDir / "Helmet" / "DamagedHelmet.gltf");
    Asset<MeshInstance> helmetMesh = mng->getAsset<MeshInstance>(helmetKey.path);
    
    helmetRenderable = std::make_shared<StaticMeshRenderable>();
    helmetRenderable->setModelName("Helmet");
    helmetRenderable->createOnGPU(helmetMesh, dev, mng);
    
    PathKey SciFiHelmetKey(modelsDir / "SciFiHelmet" / "SciFiHelmet.gltf");
    Asset<MeshInstance> SciFiHelmetMesh = mng->getAsset<MeshInstance>(SciFiHelmetKey.path);

    scifihelmetRenderable = std::make_shared<StaticMeshRenderable>();
    scifihelmetRenderable->setModelName("SciFiHelmet");
    scifihelmetRenderable->createOnGPU(SciFiHelmetMesh, dev, mng);

    const auto ent2 = createObject("Ent2");

    addComponent<TransformComponent>(ent2, glm::vec3(5.0f, 0.0f, 0.0f), glm::quat(glm::vec3(0.0f, glm::radians(180.0f), 0.0f)));
    auto& ent2Mesh = addComponent<StaticMeshComponent>(ent2, helmetRenderable);

    const auto ent3 = createObject("Ent3");
    addComponent<TransformComponent>(ent3, glm::vec3(-5.0f, 0.0f, 0.0f));
    auto& ent3Mesh = addComponent<StaticMeshComponent>(ent3, scifihelmetRenderable);

    postInit();
}

void World::postInit()
{  
    //m_dispatcher.sink<RenderMeshSubmitEvent>().connect<&RenderMeshSystem::render>(m_renderSystem);
}

entt::entity World::createObject(const std::string& tag)
{
    auto ent = m_registry.create();

    if (!tag.empty())
    {
        m_registry.emplace<TagComponent>(ent, tag);
    }

    return ent;
}

void World::update(const FrameInfo& fi)
{    
    //m_dispatcher.enqueue<RenderMeshSubmitEvent>({5, 5});
     
    //m_renderSystem.render(m_registry);
    
    
    //m_dispatcher.update();
}

void World::draw(const FrameInfo& fi)
{    
    RenderMeshSystem::update(fi.render3DPtr, m_registry);
    CanvasSystem::update(this, fi.render2DPtr, m_registry);


}

glm::ivec2 World::screenToIso(int x, int y)
{
    return { (x - y) / 2, (x + y) / 4 };
}
