#ifndef HCP_MESH_HPP
#define HCP_MESH_HPP

#include "Images.hpp"
#include "MeshBuilder.hpp"

#include <memory>
#include <string>

class HCPMesh
{
public:
    HCPMesh(const HCPMesh&) = delete;

    HCPMesh();
    ~HCPMesh();

    const HCPImagePtr getTexture() const;

    void pushMeshArrays(HCPMeshBuilder& builder) const;
    void pushMeshElements(HCPMeshBuilder& builder) const;

    void makeRenderable(HCPVertexFormat vtxFmt);
    void render(int mode = GL_TRIANGLES) const;
    void renderInstanced(int mode, GLsizei count) const;

    const glm::vec3* getPositionData() const;
    const glm::vec2* getTexCoordData() const;
    const glm::vec3* getNormalData() const;
    const glm::vec4* getColorData() const;
    const uint32_t* getIndices() const;

    size_t numVertices() const;
    size_t numIndices() const;
    const char* getName() const;
private:
    friend class hcpm;

    std::string m_name;

    std::vector<glm::vec3> m_positions;
    std::vector<glm::vec2> m_texCoords;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec4> m_colors;
    std::vector<uint32_t> m_indices;
    size_t m_numVertices;
    size_t m_numIndices;

    bool m_isRenderable;
    mutable bool m_advisedRenderable;
    GLuint m_glVAO;
    GLuint m_glVBO;
    GLuint m_glEBO;
    
    HCPImagePtr m_texture;

    void putVertex(HCPMeshBuilder& builder, const HCPVertexFormat& vtxFmt, uint32_t index) const;
};

typedef std::shared_ptr<HCPMesh> HCPMeshPtr;

class hcpm
{
public:
    static std::vector<HCPMeshPtr> loadMeshes(const char* path);
};

#endif // HCP_MESH_HPP