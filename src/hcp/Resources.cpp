#include "hcp/Resources.hpp"

#include "Logger.hpp"

#include <fstream>
#include <memory>
#include <unordered_map>

static HCPLogger i_logger("Resources");

static std::unordered_map<std::string, HCPImagePtr> i_images;
static std::unordered_map<std::string, HCPMeshPtr> i_meshes;
static std::unordered_map<std::string, std::unique_ptr<uint8_t[]>> i_resources;

inline static bool i_doesFileExist(const char* path)
{
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

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

const uint8_t* hcpr::loadResource(const char* path, const char* name, size_t* size)
{
    if(!i_doesFileExist(path))
    {
        i_logger.errorf("File %s does not exist", path);

        return nullptr;
    }

    i_logger.infof("Loading resource %s", path);
    std::ifstream resFile(path, std::ios::ate | std::ios::binary);

    if(!resFile.is_open())
    {
        i_logger.errorf("Failed to open file %s", path);

        return nullptr;
    }

    size_t fileSize = resFile.tellg();
    if(size) *size = fileSize;

    std::unique_ptr<uint8_t[]> data(new uint8_t[sizeof(size_t) + fileSize + 1]);

    *reinterpret_cast<size_t*>(data.get()) = fileSize;
    resFile.seekg(0, std::ios::beg);
    resFile.read(reinterpret_cast<char*>(data.get() + sizeof(size_t)), fileSize);
    data[sizeof(size_t) + fileSize] = '\0';

    i_resources[name] = std::move(data);

    resFile.close();

    return i_resources[name].get() + sizeof(size_t);
}

void hcpr::unloadResource(const char* name)
{
    if(i_resources.find(name) != i_resources.end())
    {
        i_resources.erase(name);
    }
}

const uint8_t* hcpr::getResource(const char* name, size_t* size)
{
    auto i_resource = i_resources.find(name);
    if(i_resource != i_resources.end())
    {
        if(size) *size = *reinterpret_cast<size_t*>(i_resource->second.get());
        return i_resource->second.get() + sizeof(size_t);
    }

    return nullptr;
}