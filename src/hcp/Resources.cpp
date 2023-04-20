#include "hcp/Resources.hpp"

#include <unordered_map>

static std::unordered_map<std::string, HCPImagePtr> i_images;
static std::unordered_map<std::string, HCPMeshPtr> i_meshes;

void hcpr::addImage(HCPImagePtr image, const char* name)
{
    i_images[name] = image;
}

bool hcpr::hasImage(const char* name)
{
    return i_images.find(name) != i_images.end();
}

HCPImagePtr hcpr::getImage(const char* name)
{
    return i_images[name];
}

void hcpr::removeImage(const char* name)
{
    i_images.erase(name);
}

void hcpr::addMesh(HCPMeshPtr mesh, const char* name)
{
    i_meshes[name] = mesh;
}

bool hcpr::hasMesh(const char* name)
{
    return i_meshes.find(name) != i_meshes.end();
}

HCPMeshPtr hcpr::getMesh(const char* name)
{
    return i_meshes[name];
}

void hcpr::removeMesh(const char* name)
{
    i_meshes.erase(name);
}