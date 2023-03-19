#pragma once

#include <eutils.h>

namespace EProject
{
    namespace fs = std::filesystem;
    
    struct PathHasher
    {
        std::size_t operator()(const fs::path& path) const 
        {
            return fs::hash_value(path);
        }
    };

    struct MeshVertex
    {
        glm::vec3 pos = { 0, 0, 0 };
        glm::vec3 normal = { 0, 0, 0 };
        glm::vec3 tangent = { 0, 0, 0 };
        glm::vec3 bitangent = { 0, 0, 0 };
        glm::vec2 uv = { 0, 0 };
   
        const static Layout* getLayout();
    };

    struct SkinnedMeshVertex
    {
        glm::vec3 pos = { 0, 0, 0 };
        glm::vec3 normal = { 0, 0, 0 };
        glm::vec3 tangent = { 0, 0, 0 };
        glm::vec3 bitangent = { 0, 0, 0 };
        glm::vec2 uv = { 0, 0 };

        glm::vec4 bone_idx = { 0, 0, 0, 0 };
        glm::vec4 bone_weight = { 0, 0, 0, 0 };

        const static Layout* getLayout();
    };

    struct Material
    {
        glm::vec4 albedo = { 0, 0, 0, 0 };
        glm::vec4 emission = { 0, 0, 0, 0 };

        float emission_strength = 0.0f;
        float metallic = 0.0f;
        float roughness = 0.0f;

        fs::path albedo_map;
        fs::path metallic_map;
        fs::path roughness_map;
        fs::path emission_map;
        fs::path normal_map;
    };

    class Mesh
    {
    public:
        Mesh() = default;

        size_t getIndicesCount() const;
        size_t getMaterialsCount() const;

        void setName(const std::string& mshName);
        const std::string& getName() const;

        void addIndex(int32_t index);
        void addMaterial(const Material& mat);

        const AABB& getAABB() const;

        const int32_t* getIndexData() const { return indices.data(); }
    protected:
        std::string name;
        std::vector<int32_t> indices;
        std::vector<std::string> vgroups;
        std::vector<Material> materials;
        AABB bbox;
    };

    class MeshData : public Mesh
    {
    public:
        MeshData() = default;

        void addVertex(const MeshVertex& mshVertex);

        const AABB& calculateAABB();
        
        const Material& getMaterial() const;
        size_t getVertexCount() const;

        const MeshVertex* getVertexData() const { return vertices.data(); }

    public:
        uint32_t startVertex = 0;
        uint32_t startIndex = 0;
        uint32_t indexCount = 0;
        uint32_t materialId = -1;
    private:
        std::vector<MeshVertex> vertices;
    };

    class SkinnedMeshData : public Mesh
    {
    public:
        SkinnedMeshData() = default;
        
        void addVertex(const SkinnedMeshVertex& mshVertex);

        const AABB& calculateAABB();

        size_t getVertexCount() const;

    private:
        std::vector<SkinnedMeshVertex> vertices;
    };

    class MeshInstance : public IAsset
    {
    public:
        MeshInstance() = default;
        ~MeshInstance() override;

        MeshInstance(const std::filesystem::path& _path);
        MeshInstance(const MeshInstance& r);

        void init() override;

        bool load(const GDevicePtr& _ptr) override;
        bool unload() override;

        void calculateAABB();

        const std::vector<MeshData>& getMeshData() const { return m_data; }

        size_t getVertexCount() const;
        size_t getIndicesCount() const;
        size_t getMaterialsCount() const;

    private:
        std::vector<MeshData> m_data;
        AABB bbox;
        
        bool isSkinnedMesh = false;
    };

    using MeshInstancePtr = std::shared_ptr<MeshInstance>;
    
    class StaticMeshRenderable
    {
    public:
        StaticMeshRenderable() = default;

        void setModelName(const std::string& mdlName) { m_modelName = mdlName; };
        void createOnGPU(const MeshInstancePtr& mshInst, const GDevicePtr& dev, AssetManagerPtr& mng);

        const VertexBufferPtr& getVertexBufferPtr() const { return m_vb; }
        const IndexBufferPtr& getIndexBufferPtr() const { return m_ib; }

        const GPUTexture2DPtr& getGPUAlbedoTexture() const { return m_albedoTex; }
        const GPUTexture2DPtr& getGPUNormalTexture() const { return m_normalTex; }

    private:

        GPUTexture2DPtr setupTexture(const GDevicePtr& dev, AssetManagerPtr& mng, const PathKey& key);

    private:
        VertexBufferPtr m_vb;
        IndexBufferPtr m_ib;

        GPUTexture2DPtr m_albedoTex;
        GPUTexture2DPtr m_normalTex;

        MeshInstancePtr m_meshPtr;
        std::string m_modelName;
    };

    using StaticMeshRenderablePtr = std::shared_ptr<StaticMeshRenderable>;
}
