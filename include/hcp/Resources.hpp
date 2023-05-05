#ifndef HCP_RESOURCES_HPP
#define HCP_RESOURCES_HPP

#include "Images.hpp"
#include "Mesh.hpp"

#define HCP_RESOURCE_PATH(m, x) "res/" #m "/" #x
#define HCP_RESOURCE(m, x) #m ":" #x

namespace hcpr
{
    constexpr const char* getAppVersion()
    {
        return "v0.0.4";
    }

    // Images resources

    void addImage(HCPImagePtr image, const char* name);
    bool hasImage(const char* name);
    HCPImagePtr getImage(const char* name);
    void removeImage(const char* name);

    // Meshes resources

    void addMesh(HCPMeshPtr mesh, const char* name);
    bool hasMesh(const char* name);
    HCPMeshPtr getMesh(const char* name);
    void removeMesh(const char* name);

    // Misc resources

    const uint8_t* loadResource(const char* path, const char* name, size_t* size = nullptr);
    void unloadResource(const char* name);
    const uint8_t* getResource(const char* name, size_t* size = nullptr);
} // namespace hcpr


#endif // HCP_RESOURCES_HPP