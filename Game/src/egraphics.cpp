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

    Canvas::Canvas(const GDevicePtr& _dev, const Camera2D& camera) : 
        DeviceHolder(_dev),
        m_cameraPtr(std::make_shared<Camera2D>(camera))
    {
        
    }

    void Canvas::init(AssetManager& mng)
    {
        auto shadersDir = PathHandler::getShadersDir();
        auto texDir = PathHandler::getTexturesDir();

        m_triangle = m_device->createShaderProgram();

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
        
        PathKey pKey(texDir / "tree.png");

        Asset<Texture2D> white_tex = mng.getAsset<Texture2D>(pKey.path);

        auto tex2dWhite = m_device->createTexture2D();
        tex2dWhite->setState(white_tex->getFormat(), white_tex->getSize(), 0, 1, white_tex->getData());


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

        m_triangle->setResource(albedoTexture, tex2dWhite);

        isInited = true;
    }

    void Canvas::drawQuad(const glm::vec2& _pos)
    {
        assert(isInited);

        glm::vec3 scale = { 5.0f, 5.0f, 1.0f };
        glm::vec3 pos = glm::vec3(_pos.xy(), 0.0f);
        
        static glm::mat4 matrix = glm::mat4(1.0f);
        glm::mat4 transform = glm::translate(matrix, pos) * glm::scale(matrix, scale);

        glm::vec4 qVert1 = transform * glm::vec4(PrimitiveFactory::getVertexPrimitive(0), PrimitiveFactory::getVertexPrimitive(1), 0.0f, 1.0f);
        glm::vec4 qVert2 = transform * glm::vec4(PrimitiveFactory::getVertexPrimitive(2), PrimitiveFactory::getVertexPrimitive(3), 0.0f, 1.0f);
        glm::vec4 qVert3 = transform * glm::vec4(PrimitiveFactory::getVertexPrimitive(4), PrimitiveFactory::getVertexPrimitive(5), 0.0f, 1.0f);
        glm::vec4 qVert4 = transform * glm::vec4(PrimitiveFactory::getVertexPrimitive(6), PrimitiveFactory::getVertexPrimitive(7), 0.0f, 1.0f);

        glm::vec2 uv1 = glm::vec2(PrimitiveFactory::getVertexUVPrimitive(0), PrimitiveFactory::getVertexUVPrimitive(1));
        glm::vec2 uv2 = glm::vec2(PrimitiveFactory::getVertexUVPrimitive(2), PrimitiveFactory::getVertexUVPrimitive(3));
        glm::vec2 uv3 = glm::vec2(PrimitiveFactory::getVertexUVPrimitive(4), PrimitiveFactory::getVertexUVPrimitive(5));
        glm::vec2 uv4 = glm::vec2(PrimitiveFactory::getVertexUVPrimitive(6), PrimitiveFactory::getVertexUVPrimitive(7));

        m_vertexQuadBatch.emplace_back(qVert1.xyz(), uv1);
        m_vertexQuadBatch.emplace_back(qVert2.xyz(), uv2);
        m_vertexQuadBatch.emplace_back(qVert3.xyz(), uv3);
        m_vertexQuadBatch.emplace_back(qVert4.xyz(), uv4);

        isDirty = true;
    }

    void Canvas::drawQuad(const glm::vec3& _pos, const glm::vec4& _color)
    {
        assert(isInited);

        glm::vec3 scale = { 5.0f, 5.0f, 1.0f };
        
        static glm::mat4 matrix = glm::mat4(1.0f);
        glm::mat4 transform = glm::translate(matrix, _pos) * glm::scale(matrix, scale);

        glm::vec4 qVert1 = transform * glm::vec4(PrimitiveFactory::getVertexPrimitive(0), PrimitiveFactory::getVertexPrimitive(1), 0.0f, 1.0f);
        glm::vec4 qVert2 = transform * glm::vec4(PrimitiveFactory::getVertexPrimitive(2), PrimitiveFactory::getVertexPrimitive(3), 0.0f, 1.0f);
        glm::vec4 qVert3 = transform * glm::vec4(PrimitiveFactory::getVertexPrimitive(4), PrimitiveFactory::getVertexPrimitive(5), 0.0f, 1.0f);
        glm::vec4 qVert4 = transform * glm::vec4(PrimitiveFactory::getVertexPrimitive(6), PrimitiveFactory::getVertexPrimitive(7), 0.0f, 1.0f);

        m_vertexQuadBatch.emplace_back(qVert1.xyz(), _color);
        m_vertexQuadBatch.emplace_back(qVert2.xyz(), _color);
        m_vertexQuadBatch.emplace_back(qVert3.xyz(), _color);
        m_vertexQuadBatch.emplace_back(qVert4.xyz(), _color);

        isDirty = true;
    }

    void Canvas::drawQuad(const glm::vec2& _pos, const glm::vec3& _color)
    {
        drawQuad(glm::vec3(_pos.xy(), 0.0f), glm::vec4(_color.xyz(), 1.0f));
    }

    void Canvas::draw()
    {
        if (m_vertexQuadBatch.empty())
        {
            return;
        }

        if (isDirty)
        {            
            m_vb->setSubData(0, static_cast<int>(m_vertexQuadBatch.size()), m_vertexQuadBatch.data());
            isDirty = false;
        }

        m_triangle->setInputBuffers(m_vb, m_ib, {}, 0);

        m_device->getStates()->setBlend(true, Blend::Src_Alpha, Blend::Inv_Src_Alpha);
        m_device->getStates()->setCull(CullMode::None);

        m_triangle->drawIndexed(PrimTopology::Triangle, 0, m_ib->getIndexCount());
    }





}