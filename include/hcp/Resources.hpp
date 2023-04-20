#ifndef HCP_RESOURCES_HPP
#define HCP_RESOURCES_HPP

#include "Images.hpp"
#include "Mesh.hpp"

namespace hcpr
{
    constexpr const char* getAppVersion()
    {
        return "v0.0.1";
    }

    void addImage(HCPImagePtr image, const char* name);
    bool hasImage(const char* name);
    HCPImagePtr getImage(const char* name);
    void removeImage(const char* name);

    void addMesh(HCPMeshPtr mesh, const char* name);
    bool hasMesh(const char* name);
    HCPMeshPtr getMesh(const char* name);
    void removeMesh(const char* name);
} // namespace hcpr


#endif // HCP_RESOURCES_HPP