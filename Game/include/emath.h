#pragma once

#include "glmh.h"
#include <limits>
#include <functional>
#include <vector>

static constexpr float cPI = 3.1415926535897932384626433832795f;
static constexpr float cPI2 = 6.283185307179586476925286766559f;

namespace EProject
{
    struct Plane;

    struct AABB
    {
        glm::vec3 m_min;
        glm::vec3 m_max;

        AABB()
        {
            fillEmpty();
        }

        inline glm::vec3 getPoint(int _indx) const
        {
            _indx %= 8;
            if (_indx < 0) _indx += 8;
            switch (_indx)
            {
                case 0: return { m_min.x, m_min.y, m_min.z };
                case 1: return { m_min.x, m_min.y, m_max.z };
                case 2: return { m_min.x, m_max.y, m_min.z };
                case 3: return { m_min.x, m_max.y, m_max.z };
                case 4: return { m_max.x, m_min.y, m_min.z };
                case 5: return { m_max.x, m_min.y, m_max.z };
                case 6: return { m_max.x, m_max.y, m_min.z };
                case 7: return { m_max.x, m_max.y, m_max.z };
                default: return { 0,0,0 };
            }
        }

        inline glm::vec3 getCenter() const
        {
            return (m_min + m_max) * 0.5f;
        }

        inline glm::vec3 getSize() const
        {
            return (m_max - m_min);
        }

        inline AABB& operator += (const glm::vec3& v)
        {
            m_min = glm::min(m_min, v);
            m_max = glm::max(m_max, v);
            return *this;
        }

        inline AABB& operator += (const AABB& b)
        {
            m_min = glm::min(m_min, b.m_min);
            m_max = glm::max(m_max, b.m_max);
            return *this;
        }

        inline AABB operator + (const AABB& b) const 
        {
            AABB res;
            res.m_min = glm::min(m_min, b.m_min);
            res.m_max = glm::max(m_max, b.m_max);
            return res;
        }

        inline AABB expand(float expansion) const
        {
            AABB res = *this;
            res.m_min -= glm::vec3(expansion, expansion, expansion);
            res.m_max += glm::vec3(expansion, expansion, expansion);
            return res;
        }

        inline AABB expand(glm::vec3 expansion) const 
        {
            AABB res = *this;
            res.m_min -= expansion;
            res.m_max += expansion;
            return res;
        }

        inline void fillEmpty()
        {
            m_min.x = std::numeric_limits<float>::max();
            m_min.y = std::numeric_limits<float>::max();
            m_min.z = std::numeric_limits<float>::max();
            m_max.x = std::numeric_limits<float>::lowest();
            m_max.y = std::numeric_limits<float>::lowest();
            m_max.z = std::numeric_limits<float>::lowest();
        }
    };
    
    inline AABB operator * (const glm::mat4& m, const AABB& b)
    {
        AABB res = {};
        for (int i = 0; i < 8; i++)
        {
            glm::vec4 tmp = (m * glm::vec4(b.getPoint(i), 1.0f));
            glm::vec3 tmp3 = tmp.xyz() / tmp.w;
            res += tmp3;
        }
        return res;
    }

    struct Plane
    {

    };



}
