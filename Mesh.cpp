#include "Mesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>

#include "Logger.hpp"

HCPLogger i_meshLogger("Mesh");

namespace fs = std::filesystem;



std::vector<HCPMeshPtr> hcpm::loadMeshes(const char* path)
{
    std::vector<HCPMeshPtr> meshes;
    fs::path directory = fs::path(path).parent_path();
    fs::path fileName = fs::path(path).stem();

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);

    if(!scene)
    {
        i_meshLogger.errorf("Failed to load meshes %s: %s", path, importer.GetErrorString());
        return meshes;
    }

    for(unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        HCPMeshPtr hcpmesh = std::make_shared<HCPMesh>();
        meshes.push_back(hcpmesh);
        const aiMesh* mesh = scene->mMeshes[i];

        if(mesh->mName.length) hcpmesh->m_name = mesh->mName.data;
        else hcpmesh->m_name = fileName.u8string();

        i_meshLogger.infof("Loading mesh %s", hcpmesh->m_name.c_str());

        hcpmesh->m_numVertices = mesh->mNumVertices;

        // Get Position Data
        for(unsigned int j = 0; j < mesh->mNumVertices; j++)
        {
            aiVector3D pos = mesh->mVertices[j];
            hcpmesh->m_positions.emplace_back(pos.x, pos.y, pos.z);
        }

        // Get Texture Coordinate Data
        if(mesh->mTextureCoords[0])
            for(unsigned int j = 0; j < mesh->mNumVertices; j++)
            {
                aiVector3D texCoord = mesh->mTextureCoords[0][j];
                hcpmesh->m_texCoords.emplace_back(texCoord.x, texCoord.y);
            }

        // Get Normal Data
        for(unsigned int j = 0; j < mesh->mNumVertices; j++)
        {
            aiVector3D normal = mesh->mNormals[j];
            hcpmesh->m_normals.emplace_back(normal.x, normal.y, normal.z);
        }

        // Get Color Data
        if(mesh->mColors[0])
            for(unsigned int j = 0; j < mesh->mNumVertices; j++)
            {
                aiColor4D color = mesh->mColors[0][j];
                hcpmesh->m_colors.emplace_back(color.r, color.g, color.b, color.a);
            }

        // Get Index Data
        hcpmesh->m_numIndices = mesh->mNumFaces * 3;
        for(unsigned int j = 0; j < mesh->mNumFaces; j++)
        {
            uint32_t* faceIndicies = mesh->mFaces[j].mIndices;
            hcpmesh->m_indices.emplace_back(faceIndicies[0]);
            hcpmesh->m_indices.emplace_back(faceIndicies[1]);
            hcpmesh->m_indices.emplace_back(faceIndicies[2]);
        }

        // Get Texture (if any)
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        if(material && aiGetMaterialTextureCount(material, aiTextureType_DIFFUSE))
        {
            aiString textureName;
            aiGetMaterialTexture(material, aiTextureType_DIFFUSE, 0, &textureName);
            std::string texturePath = (directory / fs::path(textureName.data)).u8string();

            i_meshLogger.submodule(hcpmesh->m_name.c_str()).infof("Loading texture %s", texturePath.c_str());
            hcpimg::loadImage(texturePath.c_str());
        }

        hcpmesh->m_positions.shrink_to_fit();
        hcpmesh->m_texCoords.shrink_to_fit();
        hcpmesh->m_normals.shrink_to_fit();
        hcpmesh->m_colors.shrink_to_fit();
        hcpmesh->m_indices.shrink_to_fit();
    }

    return meshes;
}

HCPMesh::HCPMesh() :
    m_numVertices(0),
    m_numIndices(0),
    m_isRenderable(false),
    m_advisedRenderable(false),
    m_glVAO(0),
    m_glVBO(0),
    m_glEBO(0),
    m_texture(hcpimg::nullImage())
{

}

HCPMesh::~HCPMesh()
{
    if(m_isRenderable)
    {
        glDeleteVertexArrays(1, &m_glVAO);
        glDeleteBuffers(1, &m_glVBO);
        glDeleteBuffers(1, &m_glEBO);
    }
}

const HCPImagePtr HCPMesh::getTexture() const
{
    return m_texture;
}

void HCPMesh::pushMeshArrays(HCPMeshBuilder& meshBuilder) const
{
    const HCPVertexFormat& vtxFmt = meshBuilder.getVertexFormat();
    
    if(m_positions.empty() || m_indices.empty()) return;

    for(size_t i = 0; i < m_numIndices; i++)
    {
        uint32_t index = m_indices[i];

        putVertex(meshBuilder, vtxFmt, index);
    }
}

void HCPMesh::pushMeshElements(HCPMeshBuilder& meshBuilder) const
{
    const HCPVertexFormat& vtxFmt = meshBuilder.getVertexFormat();

    if(m_positions.empty() || m_indices.empty()) return;

    meshBuilder.indexv(m_numIndices, m_indices.data());

    for(size_t i = 0; i < m_numVertices; i++)
    {
        putVertex(meshBuilder, vtxFmt, (uint32_t) i);
    }
}

void HCPMesh::makeRenderable(HCPVertexFormat vtxFmt)
{
    if(m_isRenderable) return;

    HCPMeshBuilder meshBuilder(vtxFmt);

    pushMeshElements(meshBuilder);

    size_t vertexBufferSize, indexBufferSize;

    const uint8_t* vertexData = meshBuilder.getVertexBuffer(&vertexBufferSize);
    const uint32_t* indexData = meshBuilder.getIndexBuffer(&indexBufferSize);

    glGenVertexArrays(1, &m_glVAO);
    glGenBuffers(1, &m_glVBO);
    glGenBuffers(1, &m_glEBO);

    glBindVertexArray(m_glVAO);
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_glVBO);
        glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, vertexData, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, indexData, GL_STATIC_DRAW);

        vtxFmt.apply();
    }
    glBindVertexArray(0);

    m_isRenderable = true;
}

void HCPMesh::render(int mode) const
{
    if(!m_isRenderable && !m_advisedRenderable)
    {
        m_advisedRenderable = true;
        i_meshLogger.submodule(getName()).warnf("Mesh has not yet been made renderable. Call makeRenderable first.");
    }

    m_texture->bindTexture();

    glBindVertexArray(m_glVAO);
    {
        glDrawElements(mode, m_numIndices, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}

void HCPMesh::renderInstanced(int mode, GLsizei instances) const
{
    if(!m_isRenderable && !m_advisedRenderable)
    {
        m_advisedRenderable = true;
        i_meshLogger.submodule(getName()).warnf("Mesh has not yet been made renderable. Call makeRenderable first.");
    }

    m_texture->bindTexture();

    glBindVertexArray(m_glVAO);
    {
        glDrawElementsInstanced(mode, m_numIndices, GL_UNSIGNED_INT, 0, instances);
    }
    glBindVertexArray(0);
}

const glm::vec3* HCPMesh::getPositionData() const
{
    if(m_positions.empty()) return NULL;
    return m_positions.data();
}

const glm::vec2* HCPMesh::getTexCoordData() const
{
    if(m_texCoords.empty()) return NULL;
    return m_texCoords.data();
}

const glm::vec3* HCPMesh::getNormalData() const
{
    if(m_normals.empty()) return NULL;
    return m_normals.data();
}

const glm::vec4* HCPMesh::getColorData() const
{
    if(m_colors.empty()) return NULL;
    return m_colors.data();
}

const uint32_t* HCPMesh::getIndices() const
{
    if(m_indices.empty()) return NULL;
    return m_indices.data();
}

size_t HCPMesh::numVertices() const
{
    return m_numVertices;
}

size_t HCPMesh::numIndices() const
{
    return m_numIndices;
}

const char* HCPMesh::getName() const
{
    return m_name.c_str();
}

void HCPMesh::putVertex(HCPMeshBuilder& meshBuilder, const HCPVertexFormat& vtxFmt, uint32_t vertexID) const
{
    bool hasUVs = !m_texCoords.empty();
    bool hasNormals = !m_normals.empty();
    bool hasColors = !m_colors.empty();

    for(int attrb = 0; attrb < vtxFmt.size; attrb++)
    {
        uint32_t attrbUsage = vtxFmt.attributes[attrb].getUsage();

        glm::vec2 vec2;
        glm::vec3 vec3;
        glm::vec4 vec4;

        switch (attrbUsage)
        {
        case HCPVF_ATTRB_USAGE_POS:
            vec3 = m_positions[vertexID];
            meshBuilder.position(vec3.x, vec3.y, vec3.z);
            break;
        case HCPVF_ATTRB_USAGE_UV:
            if(hasUVs)
            {
                vec2 = m_texCoords[vertexID];
                meshBuilder.uv(vec2.s, vec2.t);
            }
            else meshBuilder.uvDefault();
            break;
        case HCPVF_ATTRB_USAGE_NORMAL:
            if(hasNormals)
            {
                vec3 = m_normals[vertexID];
                meshBuilder.normal(vec3.x, vec3.y, vec3.z);
            }
            else meshBuilder.normalDefault();
            break;
        case HCPVF_ATTRB_USAGE_COLOR:
            if(hasColors)
            {
                vec4 = m_colors[vertexID];
                meshBuilder.colorRGBA(vec4.r, vec4.g, vec4.b, vec4.a);
            }
            else meshBuilder.colorDefault();
            break;
        case HCPVF_ATTRB_USAGE_TEXID:
            meshBuilder.texid(0);
            break;
        }
    }
}