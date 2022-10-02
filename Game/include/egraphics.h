#pragma once

#include "emath.h"
#include "egapi.h"
#include "eutils.h"

namespace EProject
{
    struct VertexPosColor
    {
        float p1, p2, p3;
        float c1, c2, c3;

        VertexPosColor() {};
        VertexPosColor(const glm::vec3& pos, const glm::vec3& color) :
            p1(pos.x), p2(pos.y), p3(pos.z),
            c1(color.x), c2(color.y), c3(color.z) {}
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

        static float getVertexPrimitive(size_t index);
    };


    class Canvas : public DeviceHolder
    {
    public:
        Canvas() = default;
        Canvas(const GDevicePtr& _dev, const Camera2D& camera);

        void init();

        void drawQuad(const glm::vec3& _pos, const glm::vec4& _color);
        void drawQuad(const glm::vec2& _pos, const glm::vec3& _color);
        
        void draw();

    private:

        static constexpr size_t batchCount = 1024;

        VertexBufferPtr m_vb;
        IndexBufferPtr m_ib;

        ShaderProgramPtr m_triangle;
        StructuredBufferPtr m_sb;
    
        const Layout* m_posColorLayout = nullptr;

        Camera2DPtr m_cameraPtr;

        std::vector<VertexPosColor> m_vertexQuadBatch = {};
        std::vector<int> m_indexBuffer = {};
    
        int m_numVert = 0;

        bool isInited = false;
        bool isDirty = false;
    };
}
