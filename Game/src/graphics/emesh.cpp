#include "graphics/emesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>     
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

namespace EProject
{
    static constexpr uint32_t meshLoadFlags =
        aiProcess_CalcTangentSpace |        // Create binormals/tangents just in case
        aiProcess_Triangulate |             // Make sure we're triangles
        aiProcess_SortByPType |             // Split meshes by primitive type
        aiProcess_GenNormals |              // Make sure we have legit normals
        aiProcess_GenUVCoords |             // Convert UVs if required 
        aiProcess_OptimizeMeshes |          // Batch draws where possible
        aiProcess_ValidateDataStructure |
        aiProcess_ConvertToLeftHanded | aiProcess_FixInfacingNormals;    // Validation

    const Layout* MeshVertex::getLayout()
    {
        return getLayoutSelector()
            ->add("pos", LayoutType::Float, 3)
            ->add("normal", LayoutType::Float, 3)
            ->add("tangent", LayoutType::Float, 3)
            ->add("bitangent", LayoutType::Float, 3)
            ->add("uv", LayoutType::Float, 2)
            ->end(sizeof(MeshVertex));
    }

    const Layout* SkinnedMeshVertex::getLayout()
    {
        return getLayoutSelector()
            ->add("pos", LayoutType::Float, 3)
            ->add("normal", LayoutType::Float, 3)
            ->add("tangent", LayoutType::Float, 3)
            ->add("bitangent", LayoutType::Float, 3)
            ->add("bone_idx", LayoutType::Float, 4)
            ->add("bone_weight", LayoutType::Float, 4)
            ->add("uv", LayoutType::Float, 2)
            ->end(sizeof(SkinnedMeshVertex));
    }

    size_t Mesh::getIndicesCount() const
    {
        return indices.size();
    }
    
    size_t Mesh::getMaterialsCount() const
    {
        return materials.size();
    }

    void Mesh::addIndex(int32_t index)
    {
        indices.push_back(index);
    }

    void Mesh::addMaterial(const Material& mat)
    {
        materials.push_back(mat);
    }

    void Mesh::setName(const std::string& mshName)
    {
        name = mshName;
    }
    
    const std::string& Mesh::getName() const
    {
        return name;
    }

    const AABB& Mesh::getAABB() const
    {
        return bbox;
    }

    size_t MeshData::getVertexCount() const
    {
        return vertices.size();
    }

    void MeshData::addVertex(const MeshVertex& mshVertex)
    {
        vertices.push_back(mshVertex);
    }
    
    const AABB& MeshData::calculateAABB()
    {
        for (const auto& vert : vertices)
        {
            bbox += vert.pos;
        }  

        return bbox;
    }

    const Material& MeshData::getMaterial() const
    {
        assert(materialId != -1);

        return materials[materialId];
    }

    MeshInstance::~MeshInstance()
    {

    }

    MeshInstance::MeshInstance(const std::filesystem::path& _path) : IAsset(_path)
    {

    }
    
    MeshInstance::MeshInstance(const MeshInstance& r)
    {

    }

    void MeshInstance::init()
    {
        calculateAABB();
    }

    bool MeshInstance::load(const GDevicePtr& _ptr)
    {
        Assimp::Importer importer;
        
        const aiScene* scene = importer.ReadFile(m_path.string(), meshLoadFlags);
    
        if (!scene)
        {
            throw std::runtime_error("MeshInstance: Load failed: " + m_path.u8string());
            return false;
        }

        uint32_t vertexCount = 0;
        uint32_t indexCount = 0;

        m_data.reserve(scene->mNumMeshes);

        for (uint32_t i = 0; i < scene->mNumMeshes; ++i) 
        {
            auto& mesh = scene->mMeshes[i];
            const auto& mat = scene->mMaterials[mesh->mMaterialIndex];

            MeshData meshData = {};

            meshData.startVertex = vertexCount;
            meshData.startIndex = indexCount;
            meshData.indexCount = mesh->mNumFaces * 3;
            meshData.materialId = mesh->mMaterialIndex;

            meshData.setName(mesh->mName.C_Str());

            vertexCount += mesh->mNumVertices;
            indexCount += meshData.indexCount;

            for (uint32_t v = 0; v < mesh->mNumVertices; v++)
            {
                MeshVertex mshVertex = {};

                mshVertex.pos = { mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z };
                mshVertex.normal = { mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z };
                
                if (mesh->HasTextureCoords(0))
                {
                    mshVertex.uv = { mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y };
                }

                if (mesh->HasTangentsAndBitangents())
                {
                    mshVertex.tangent = { mesh->mTangents[v].x, mesh->mTangents[v].y, mesh->mTangents[v].z };
                    mshVertex.bitangent = { mesh->mBitangents[v].x, mesh->mBitangents[v].y, mesh->mBitangents[v].z };
                }

                //vertex.color = (skinnedMesh->scene->mMeshes[m]->HasVertexColors(0)) ? glm::make_vec3(&skinnedMesh->scene->mMeshes[m]->mColors[0][v].r) : glm::vec3(1.0f);

                meshData.addVertex(mshVertex);                
            }   

            for (uint32_t ij = 0; ij < mesh->mNumFaces; ++ij)
            {
                aiFace face = mesh->mFaces[ij];
                for (uint32_t f = 0; f < face.mNumIndices; ++f)
                {
                    meshData.addIndex(face.mIndices[f]);
                }
            }

            Material mshMat = {};

            mat->Get(AI_MATKEY_COLOR_DIFFUSE, mshMat.albedo);
            mat->Get(AI_MATKEY_COLOR_EMISSIVE, mshMat.emission);
            //mat->Get(AI_MATKEY_TEXTURE, mshMat.roughness);
            
            aiString materialName;
            aiString textureName;

            auto result = mat->Get(AI_MATKEY_NAME, materialName);
            if (result != AI_SUCCESS) materialName = "";

            auto texCount = mat->GetTextureCount(aiTextureType_DIFFUSE);
            if (texCount > 0)
            {
                result = mat->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), textureName);
                mshMat.albedo_map = textureName.C_Str();
            }

            texCount = mat->GetTextureCount(aiTextureType_NORMALS);
            if (texCount > 0)
            {
                result = mat->Get(AI_MATKEY_TEXTURE(aiTextureType_NORMALS, 0), textureName);
                mshMat.normal_map = textureName.C_Str();
            }

            texCount = mat->GetTextureCount(aiTextureType_EMISSIVE);
            if (texCount > 0)
            {
                result = mat->Get(AI_MATKEY_TEXTURE(aiTextureType_EMISSIVE, 0), textureName);
                mshMat.emission_map = textureName.C_Str();
            }

            texCount = mat->GetTextureCount(aiTextureType_LIGHTMAP);
            if (texCount > 0)
            {
                result = mat->Get(AI_MATKEY_TEXTURE(aiTextureType_LIGHTMAP, 0), textureName);
                mshMat.metallic_map = textureName.C_Str();
            }

            texCount = mat->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS);
            if (texCount > 0)
            {
                result = mat->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE_ROUGHNESS, 0), textureName);
                mshMat.roughness_map = textureName.C_Str();
            }

            meshData.addMaterial(mshMat);

            m_data.emplace_back(std::move(meshData));
        }
    
        return true;
    }

    bool MeshInstance::unload()
    {
        return true;
    }

    void MeshInstance::calculateAABB()
    {
        for (auto& md : m_data)
        {
            const auto& aabb = md.calculateAABB();
            bbox += aabb;
        }
    }

    size_t MeshInstance::getVertexCount() const
    {
        size_t count = 0;

        for (const auto& md : m_data)
        {
            count += md.getVertexCount();
        }

        return count;
    }

    size_t MeshInstance::getIndicesCount() const
    {
        size_t count = 0;

        for (const auto& md : m_data)
        {
            count += md.getIndicesCount();
        }

        return count;
    }
    
    size_t MeshInstance::getMaterialsCount() const
    {
        size_t count = 0;

        for (const auto& md : m_data)
        {
            count += md.getMaterialsCount();
        }

        return count;
    }

    GPUTexture2DPtr StaticMeshRenderable::setupTexture(const GDevicePtr& dev, AssetManagerPtr& mng, const PathKey& key)
    {
        Asset<Texture2D> meshTex = mng->getAsset<Texture2D>(key.path);

        auto mshTexRend = dev->createTexture2D();
        mshTexRend->setState(meshTex->getFormat(), meshTex->getSize(), 0, 1, meshTex->getData());
        return mshTexRend;
    }

    void StaticMeshRenderable::createOnGPU(const MeshInstancePtr& mshInst, const GDevicePtr& dev, AssetManagerPtr& mng)
    {
        assert(!m_modelName.empty());

        m_meshPtr = mshInst;

        // create mesh gpu textures

        const auto modelsDir = PathHandler::getModelsDir();
        const auto& mshData = m_meshPtr->getMeshData();
        
        for (const auto& data : mshData)
        {
            const auto& mat = data.getMaterial();
            m_albedoTex = setupTexture(dev, mng, PathKey(modelsDir / m_modelName / mat.albedo_map));
            m_normalTex = setupTexture(dev, mng, PathKey(modelsDir / m_modelName / mat.normal_map));
            m_metallRoghnessTex = setupTexture(dev, mng, PathKey(modelsDir / m_modelName / mat.roughness_map));
        }

        // create gpu buffers

        const auto* layout = MeshVertex::getLayout();

        m_vb = dev->createVertexBuffer();
        m_vb->setState(layout, static_cast<int>(m_meshPtr->getVertexCount()), nullptr);

        const auto& data = m_meshPtr->getMeshData();

        size_t startVertex = 0;
        size_t startIndex = 0;

        m_ib = dev->createIndexBuffer();
        m_ib->setState(static_cast<int>(m_meshPtr->getIndicesCount()), nullptr);

        for (const auto& msh : data)
        {
            const size_t numVertices = msh.getVertexCount();
            const size_t numIndices = msh.getIndicesCount();

            m_vb->setSubData(static_cast<int>(startVertex), static_cast<int>(numVertices), msh.getVertexData());
            m_ib->setSubData(static_cast<int>(startIndex), static_cast<int>(numIndices), msh.getIndexData());

            startVertex += numVertices;
            startIndex += numIndices;
        }
    }
}
