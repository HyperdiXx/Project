#include "egraphics.h"

namespace EProject
{
    Color Color::red = Color(1.0f, 0.0f, 0.0f, 1.0f);
    Color Color::green = Color(0.0f, 1.0f, 0.0f, 1.0f);
    Color Color::blue = Color(0.0f, 0.0f, 1.0f, 1.0f);
    Color Color::white = Color(1.0f, 1.0f, 1.0f, 1.0f);

    float PrimitiveFactory::getVertexPrimitive(size_t index)
    {
        assert(index >= 0 && index <= 7);

        return vertexIndexDataQuadArray[index];
    }

    float PrimitiveFactory::getVertexUVPrimitive(size_t index)
    {
        assert(index >= 0 && index <= 7);

        return vertexIndexDataQuadArrayTex[index];
    }

    Render2D::Render2D(const GDevicePtr& _dev) :
        DeviceHolder(_dev)
    {

    }

    Render2D::Render2D(const GDevicePtr& _dev, const Camera2DPtr& camera) :
        DeviceHolder(_dev),
        m_cameraPtr(camera)
    {
        
    }

    void Render2D::createBaseShader()
    {
        m_triangle = m_device->createShaderProgram();

        auto shadersDir = PathHandler::getShadersDir();

        ShaderInput vsInputTri = {};

        vsInputTri.filePath = shadersDir / "triangle.hlsl";
        vsInputTri.entyPoint = "vs_main";
        vsInputTri.target = "vs_5_0";
        vsInputTri.type = ShaderType::Vertex;

        ShaderInput psInputTri = {};

        psInputTri.filePath = shadersDir / "triangle.hlsl";
        psInputTri.entyPoint = "ps_main";
        psInputTri.target = "ps_5_0";
        psInputTri.type = ShaderType::Pixel;

        m_triangle->compileFromFile(vsInputTri);
        m_triangle->compileFromFile(psInputTri);
        m_triangle->create();
    }

    void Render2D::init(std::shared_ptr<AssetManager>& mng)
    {
        m_mng = mng;

        createBaseShader();

        const auto texDir = PathHandler::getTexturesDir();
       
        PathKey grassKey(texDir / "grass.png");        
        Asset<Texture2D> grassTex = mng->getAsset<Texture2D>(grassKey.path);

        auto texGrass = m_device->createTexture2D();
        texGrass->setState(grassTex->getFormat(), grassTex->getSize(), 0, 1, grassTex->getData());

        m_posColorLayout = getLayoutSelector()->add("POS", LayoutType::Float, 3)
                                              ->add("COL", LayoutType::Float, 3)
                                              ->end();
        
        m_posTextureLayout = getLayoutSelector()->add("POS", LayoutType::Float, 3)
                                              ->add("UV",  LayoutType::Float, 2)
                                              ->end();


        auto vbLayoutPos = getLayoutSelector()->add("POS", LayoutType::Float, 3)
                                              ->end();

        /*float vertexDataArrayPos[] =
        {
             0.0f,  0.5f,  0.0f,
             0.5f, -0.5f,  0.0f,
            -0.5f, -0.5f,  0.0f
        };*/

        //float vertexDataArray[] =
        //{
        //       15.0f,  15.0f,  0.0f, 1.0f, 0.0f, 0.0f,  // point at top
        //       15.0f, -15.0f,  0.0f, 0.0f, 1.0f, 0.0f,  // point at bottom-right
        //      -15.0f, -15.0f,  0.0f, 0.0f, 0.0f, 1.0f,  // point at bottom-left

        //      -15.0f,  -15.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // point at bottom-left               
        //       15.0f,  15.0f,  0.0f, 1.0f, 0.0f, 0.0f,  // point at bottom-left               
        //      -15.0f,  15.0f,  0.0f, 0.0f, 1.0f, 0.0f   // point at bottom-left               
        //};

        std::vector<int> index_data = { 0, 1, 2, 2, 3, 0 };

        int sizeofPosColor = sizeof(VertexPosColor);
        m_vertexQuadBatch.reserve(batchCount);

        unsigned int* indexQuadBatch = new unsigned int[batchCount * 6];

        uint32_t offset = 0;
        for (uint32_t i = 0; i < batchCount * 6; i += 6)
        {
            indexQuadBatch[i] = offset + 0;
            indexQuadBatch[i + 1] = offset + 1;
            indexQuadBatch[i + 2] = offset + 2;

            indexQuadBatch[i + 3] = offset + 2;
            indexQuadBatch[i + 4] = offset + 3;
            indexQuadBatch[i + 5] = offset + 0;

            offset += 4;
        }

        m_vb = m_device->createVertexBuffer();
        //m_vb->setState(m_posColorLayout, batchCount * 4, m_vertexQuadBatch.data());
        
        m_vb->setState(m_posTextureLayout, batchCount * 4, m_vertexQuadBatch.data());

        m_ib = m_device->createIndexBuffer();
        m_ib->setState(static_cast<int>(batchCount * index_data.size()), indexQuadBatch);

        static const char* projectionMatrix = "projection";
        static const char* albedoTexture = "albedoTex";

        m_triangle->setValue(projectionMatrix, m_cameraPtr->getProj());
        m_triangle->setResource(albedoTexture, texGrass);
       
        isInited = true;
    }

    void Render2D::setTextureLayer(int index)
    {
        static const char* albedoTexture = "albedoTex";
        
        auto texDir = PathHandler::getTexturesDir();

        PathKey treeKey(texDir / "tree.png");
        Asset<Texture2D> treeTex = m_mng->getAsset<Texture2D>(treeKey.path);

        auto texTree = m_device->createTexture2D();
        texTree->setState(treeTex->getFormat(), treeTex->getSize(), 0, 1, treeTex->getData());

        m_triangle->setResource(albedoTexture, texTree);
    }

    void Render2D::markDirty()
    {
        isDirty = true;
    }

    void Render2D::drawQuad(const glm::vec2& _pos)
    {
        assert(isInited);

        static constexpr glm::mat4 matrix = glm::mat4(1.0f);

        glm::vec3 scale = { 5.0f, 5.0f, 1.0f };
        glm::vec3 pos = glm::vec3(_pos.xy(), 0.0f);
        
        glm::mat4 transform = glm::translate(matrix, pos) * glm::scale(matrix, scale);

        glm::vec4 qVert1 = transform * glm::vec4(PrimitiveFactory::getVertexPrimitive(0), PrimitiveFactory::getVertexPrimitive(1), 0.0f, 1.0f);
        glm::vec4 qVert2 = transform * glm::vec4(PrimitiveFactory::getVertexPrimitive(2), PrimitiveFactory::getVertexPrimitive(3), 0.0f, 1.0f);
        glm::vec4 qVert3 = transform * glm::vec4(PrimitiveFactory::getVertexPrimitive(4), PrimitiveFactory::getVertexPrimitive(5), 0.0f, 1.0f);
        glm::vec4 qVert4 = transform * glm::vec4(PrimitiveFactory::getVertexPrimitive(6), PrimitiveFactory::getVertexPrimitive(7), 0.0f, 1.0f);

        glm::vec2 qUv1 = glm::vec2(PrimitiveFactory::getVertexUVPrimitive(0), PrimitiveFactory::getVertexUVPrimitive(1));
        glm::vec2 qUv2 = glm::vec2(PrimitiveFactory::getVertexUVPrimitive(2), PrimitiveFactory::getVertexUVPrimitive(3));
        glm::vec2 qUv3 = glm::vec2(PrimitiveFactory::getVertexUVPrimitive(4), PrimitiveFactory::getVertexUVPrimitive(5));
        glm::vec2 qUv4 = glm::vec2(PrimitiveFactory::getVertexUVPrimitive(6), PrimitiveFactory::getVertexUVPrimitive(7));

        m_vertexQuadBatch.emplace_back(qVert1.xyz(), qUv1);
        m_vertexQuadBatch.emplace_back(qVert2.xyz(), qUv2);
        m_vertexQuadBatch.emplace_back(qVert3.xyz(), qUv3);
        m_vertexQuadBatch.emplace_back(qVert4.xyz(), qUv4);

        isDirty = true;
    }

    void Render2D::drawQuad(const glm::vec3& _pos, const glm::vec4& _color)
    {
        assert(isInited);

        static constexpr glm::mat4 matrix = glm::mat4(1.0f);

        glm::vec3 scale = { 5.0f, 5.0f, 1.0f };
        
        glm::mat4 transform = glm::translate(matrix, _pos) * glm::scale(matrix, scale);

        glm::vec4 qVert1 = transform * glm::vec4(PrimitiveFactory::getVertexPrimitive(0), PrimitiveFactory::getVertexPrimitive(1), _pos.z, 1.0f);
        glm::vec4 qVert2 = transform * glm::vec4(PrimitiveFactory::getVertexPrimitive(2), PrimitiveFactory::getVertexPrimitive(3), _pos.z, 1.0f);
        glm::vec4 qVert3 = transform * glm::vec4(PrimitiveFactory::getVertexPrimitive(4), PrimitiveFactory::getVertexPrimitive(5), _pos.z, 1.0f);
        glm::vec4 qVert4 = transform * glm::vec4(PrimitiveFactory::getVertexPrimitive(6), PrimitiveFactory::getVertexPrimitive(7), _pos.z, 1.0f);

        m_vertexQuadBatch.emplace_back(qVert1.xyz(), _color);
        m_vertexQuadBatch.emplace_back(qVert2.xyz(), _color);
        m_vertexQuadBatch.emplace_back(qVert3.xyz(), _color);
        m_vertexQuadBatch.emplace_back(qVert4.xyz(), _color);

        isDirty = true;
    }

    void Render2D::drawQuad(const glm::vec2& _pos, const glm::vec3& _color)
    {
        drawQuad(glm::vec3(_pos.xy(), -1.0f), glm::vec4(_color.xyz(), 1.0f));
    }

    void Render2D::draw()
    {
        if (!shouldDraw())
        {
            return;
        }

        updateDrawingBatch();
        drawImpl();
    }

    bool Render2D::shouldDraw() const
    {
        if (m_vertexQuadBatch.empty())
        {
            return false;
        }

        return true;
    }

    void Render2D::updateDrawingBatch()
    {
        if (isDirty)
        {
            static const char* projectionMatrix = "projection";            
            m_triangle->setValue(projectionMatrix, m_cameraPtr->getProj());
            
            m_vb->setSubData(0, static_cast<int>(m_vertexQuadBatch.size()), m_vertexQuadBatch.data());
            isDirty = false;
        }
    }

    void Render2D::drawImpl()
    {
        m_triangle->setInputBuffers(m_vb, m_ib, {}, 0);

        m_device->getStates()->setBlend(true, Blend::Src_Alpha, Blend::Inv_Src_Alpha);
        //m_device->getStates()->setCull(CullMode::None);

        m_triangle->drawIndexed(PrimTopology::Triangle, 0, m_ib->getIndexCount());

        m_device->getStates()->setBlend(false, Blend::Src_Alpha, Blend::Inv_Src_Alpha);
    }

    Render3D::Render3D(const GDevicePtr& _dev) : DeviceHolder(_dev)
    {

    }

    Render3D::Render3D(const GDevicePtr& _dev, const Camera3DPtr& camera) : DeviceHolder(_dev), m_cam3DPtr(camera)
    {

    }

    void Render3D::init(std::shared_ptr<AssetManager>& mng)
    {
        m_mng = mng;

        createShaderSemantics();
        createPBRShader(); 
    }

    void Render3D::setGeometryPass(const DirectLightComponent& dirLight)
    {
        m_dirLight = dirLight;
    }

    void Render3D::drawMeshModel(const StaticMeshComponent& mshPtr, const TransformComponent& trs)
    {
        const auto& mdl = mshPtr.m_model;        
        
        glm::mat4 mdlMatrix = glm::mat4(1.0f);

        // RightHanded Matrix Order Mul. transpose... Keep in my mind VULKAN!!!
        mdlMatrix = glm::translate(mdlMatrix, trs.mPos) * glm::toMat4(trs.mRot) * glm::scale(mdlMatrix, trs.mScale);
        mdlMatrix = glm::transpose(mdlMatrix);

        const glm::mat4 inverseMdl = glm::inverse(mdlMatrix);

        m_pbr->setResource(m_shaderSemanticsc.at("samplerDefault"), cSampler_Linear);

        m_pbr->setResource(m_shaderSemanticsc.at("albedoTexture"), mdl->getAlbedoTexturePtr());
        m_pbr->setResource(m_shaderSemanticsc.at("normalTexture"), mdl->getNormalTexturePtr());
        m_pbr->setResource(m_shaderSemanticsc.at("metallRoghnessTexture"), mdl->getMetallRoughnessTexturePtr());

        m_pbr->setValue(m_shaderSemanticsc.at("modelMatrix"), mdlMatrix);
        m_pbr->setValue(m_shaderSemanticsc.at("invModelMatrix"), inverseMdl);

        m_pbr->setValue(m_shaderSemanticsc.at("cameraPos"), m_cam3DPtr->getPosition());
        m_pbr->setValue(m_shaderSemanticsc.at("viewProjectionMatrix"),  m_cam3DPtr->getViewProj());

        m_pbr->setValue(m_shaderSemanticsc.at("lightPositions"), m_dirLight.mPos);
        m_pbr->setValue(m_shaderSemanticsc.at("lightColours"), m_dirLight.mColor);

        m_pbr->setInputBuffers(mdl->getVertexBufferPtr(), mdl->getIndexBufferPtr(), {}, 0);

        m_pbr->drawIndexed(PrimTopology::Triangle, 0, mdl->getIndexBufferPtr()->getIndexCount());
    }

    void Render3D::drawMeshModel(const SkinnedMeshComponent& mshPtr, const TransformComponent& trs)
    {
        const auto& mdl = mshPtr.m_model;

        glm::mat4 mdlMatrix = glm::mat4(1.0f);

        // RightHanded Matrix Order Mul. transpose... Keep in my mind VULKAN!!!
        mdlMatrix = glm::translate(mdlMatrix, trs.mPos) * glm::toMat4(trs.mRot) * glm::scale(mdlMatrix, trs.mScale);
        mdlMatrix = glm::transpose(mdlMatrix);

        const glm::mat4 inverseMdl = glm::inverse(mdlMatrix);

       /* m_pbr->setResource(m_shaderSemanticsc.at("samplerDefault"), cSampler_Linear);

        m_pbr->setResource(m_shaderSemanticsc.at("albedoTexture"), mdl->getGPUAlbedoTexture());
        m_pbr->setResource(m_shaderSemanticsc.at("normalTexture"), mdl->getGPUNormalTexture());

        m_pbr->setValue(m_shaderSemanticsc.at("modelMatrix"), mdlMatrix);
        m_pbr->setValue(m_shaderSemanticsc.at("invModelMatrix"), inverseMdl);

        m_pbr->setValue(m_shaderSemanticsc.at("cameraPos"), m_cam3DPtr->getPosition());
        m_pbr->setValue(m_shaderSemanticsc.at("viewProjectionMatrix"), m_cam3DPtr->getViewProj());

        m_pbr->setInputBuffers(mdl->getVertexBufferPtr(), mdl->getIndexBufferPtr(), {}, 0);*/

        //m_pbr->drawIndexed(PrimTopology::Triangle, 0, mdl->getIndexBufferPtr()->getIndexCount());
    }

    void Render3D::createShaderSemantics()
    {
        m_shaderSemanticsc["viewProjectionMatrix"] = "vp";
        m_shaderSemanticsc["modelMatrix"] = "model";
        m_shaderSemanticsc["invModelMatrix"] = "invModel";
        m_shaderSemanticsc["cameraPos"] = "camPos";

        m_shaderSemanticsc["albedoTexture"] = "albedoTex";
        m_shaderSemanticsc["normalTexture"] = "normalTex";
        m_shaderSemanticsc["metallRoghnessTexture"] = "metallRoghnessTex";
        m_shaderSemanticsc["samplerDefault"] = "samplerDefault";

        m_shaderSemanticsc["lightPositions"] = "lightPositions";
        m_shaderSemanticsc["lightColours"] = "lightColours";
    }

    void Render3D::createPBRShader()
    {
        m_pbr = m_device->createShaderProgram();

        auto shadersDir = PathHandler::getShadersDir();

        ShaderInput vsInputTri = {};

        vsInputTri.filePath = shadersDir / "pbr.hlsl";
        vsInputTri.entyPoint = "vs_main";
        vsInputTri.target = "vs_5_0";
        vsInputTri.type = ShaderType::Vertex;

        ShaderInput psInputTri = {};

        psInputTri.filePath = shadersDir / "pbr.hlsl";
        psInputTri.entyPoint = "ps_main";
        psInputTri.target = "ps_5_0";
        psInputTri.type = ShaderType::Pixel;

        m_pbr->compileFromFile(vsInputTri);
        m_pbr->compileFromFile(psInputTri);
        m_pbr->create();
    }

}