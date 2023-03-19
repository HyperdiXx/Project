#pragma once

namespace EObject
{
    class EBaseObject
    {
        using ObjectHandle = size_t;
    public:
        EBaseObject() = default;
        virtual ~EBaseObject() = default;

        EBaseObject(const EBaseObject&) = default;
        EBaseObject& operator=(const EBaseObject&) = default;
        EBaseObject(EBaseObject&&) = default;
        EBaseObject& operator=(EBaseObject&&) = default;

        // UUID

        ObjectHandle mId;

    private:


    };
}
