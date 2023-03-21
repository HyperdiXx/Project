#pragma once

#include "emath.h"
#include "egapi.h"
#include "eutils.h"

#include <world/ecomponents.h>

namespace EProject
{
    struct VertexPosColor
    {
        float p1, p2, p3;
        float c1, c2, c3;

        VertexPosColor() = default;
        VertexPosColor(const glm::vec3& pos, const glm::vec3& color) :
            p1(pos.x), p2(pos.y), p3(pos.z),
            c1(color.x), c2(color.y), c3(color.z) {}
    };

    struct VertexPosTex
    {
        float p1, p2, p3;
        float u, v;

        VertexPosTex() = default;
        VertexPosTex(const glm::vec3& pos, const glm::vec2& _uv) :
            p1(pos.x), p2(pos.y), p3(pos.z),
            u(_uv.x),  v(_uv.y) {}
    };

    struct Color : glm::vec4
    {
        Color() = default;
        Color(const glm::vec4& _col) : glm::vec4(_col) {};
        Color(float r, float g, float b, float a) : glm::vec4(r, g, b, a) {};
        
        static Color red;
        static Color green;
        static Color blue;
        static Color white;
    };

    struct PrimitiveFactory
    {
        static constexpr float vertexIndexDataQuadArray[8] =
        {
            -0.5f, -0.5f, // point at top-right
             0.5f, -0.5f, // point at bottom-right
             0.5f,  0.5f, // point at bottom-left          
            -0.5f,  0.5f  // point at top-left      
        };

        static constexpr float vertexIndexDataQuadArrayTex[8] =
        {
             0.0f,  1.0f, // point at top-right
             1.0f,  1.0f, // point at bottom-right
             1.0f,  0.0f, // point at bottom-left          
             0.0f,  0.0f  // point at top-left      
        };

        static float getVertexPrimitive(size_t index);
        static float getVertexUVPrimitive(size_t index);
    };

    // Implement light representation on render side
    class Light final
    {
    public:

    private:

    };

    class Render2D : public DeviceHolder
    {
    public:
        Render2D() = default;
        explicit Render2D(const GDevicePtr& _dev);
        explicit Render2D(const GDevicePtr& _dev, const Camera2DPtr& camera);

        void init(std::shared_ptr<AssetManager>&);

        void setTextureLayer(int index);
        void markDirty();

        void drawQuad(const glm::vec3& _pos, const glm::vec4& _color);
        void drawQuad(const glm::vec2& _pos, const glm::vec3& _color);
        void drawQuad(const glm::vec2& _pos);
        
        void draw();

    private:

        bool shouldDraw() const;
        void updateDrawingBatch();
        void drawImpl();
        
        void createBaseShader();
        void createShaderSemantics();

    private:

        static constexpr size_t batchCount = 1024;

        VertexBufferPtr m_vb;
        IndexBufferPtr m_ib;

        ShaderProgramPtr m_triangle;
        StructuredBufferPtr m_sb;
    
        const Layout* m_posColorLayout = nullptr;
        const Layout* m_posTextureLayout = nullptr;

        Camera2DPtr m_cameraPtr;
        std::shared_ptr<AssetManager> m_mng;

        //std::vector<VertexPosColor> m_vertexQuadBatch = {};
        std::vector<VertexPosTex> m_vertexQuadBatch = {};
        std::vector<int> m_indexBuffer = {};
    
        int m_numVert = 0;

        bool isInited = false;
        bool isDirty = false;
    };
   
    class Render3D : public DeviceHolder
    {
    public:
        Render3D() = default;
        explicit Render3D(const GDevicePtr& _dev);
        explicit Render3D(const GDevicePtr& _dev, const Camera3DPtr& camera);

        void init(std::shared_ptr<AssetManager>&);

        void setGeometryPass(const DirectLightComponent& dirLight);

        void drawMeshModel(const StaticMeshComponent& mshPtr, const TransformComponent& trs);
        void drawMeshModel(const SkinnedMeshComponent& mshPtr, const TransformComponent& trs);

    private:

        void createPBRShader();
        void createShaderSemantics();
        void drawMesh();

    private:

        ShaderProgramPtr m_pbr;
        StructuredBufferPtr m_sb;

        std::shared_ptr<AssetManager> m_mng;
        Camera3DPtr m_cam3DPtr;

        DirectLightComponent m_dirLight;

        std::unordered_map<const char*, const char*> m_shaderSemanticsc;
    };
}
