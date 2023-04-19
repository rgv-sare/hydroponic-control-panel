#ifndef HCP_MESHBUILDER_HPP
#define HCP_MESHBUILDER_HPP

#include <stdint.h>
#include <stack>
#include <vector>
#include <glm/glm.hpp>

#include "GLInclude.hpp"

// Vertex Attribute Members Masks
#define HCPVF_ATTRB_USAGE_MASK      0xFF000000
#define HCPVF_ATTRB_TYPE_SIZE_MASK  0x00F00000
#define HCPVF_ATTRB_TYPE_MASK       0x00FF0000
#define HCPVF_ATTRB_SIZE_MASK       0x0000FF00
#define HCPVF_ATTRB_NORMALIZED_MASK 0x000000FF

// Vertex Attribute Usage Enum
#define HCPVF_ATTRB_USAGE_POS       0x00000000 // Position, enum value 0
#define HCPVF_ATTRB_USAGE_UV        0x01000000 // UV,       enum value 1
#define HCPVF_ATTRB_USAGE_COLOR     0x02000000 // Color,    enum value 2
#define HCPVF_ATTRB_USAGE_NORMAL    0x03000000 // Normal,   enum value 3
#define HCPVF_ATTRB_USAGE_TEXID     0x04000000 // Texid,    enum value 4
#define HCPVF_ATTRB_USAGE_OTHER     0x05000000 // Other,    enum value 5

// Vertex Attribute Type Enum
#define HCPVF_ATTRB_TYPE_FLOAT      0x00400000 // Float           , size 4, enum value 0
#define HCPVF_ATTRB_TYPE_INT        0x00410000 // Integer         , size 4, enum value 1
#define HCPVF_ATTRB_TYPE_UINT       0x00420000 // Unsigned Integer, size 4, enum value 2
#define HCPVF_ATTRB_TYPE_SHORT      0x00230000 // Short           , size 2, enum value 3
#define HCPVF_ATTRB_TYPE_USHORT     0x00240000 // Unsigned Short  , size 2, enum value 4
#define HCPVF_ATTRB_TYPE_BYTE       0x00150000 // Byte            , size 1, enum value 5
#define HCPVF_ATTRB_TYPE_UBYTE      0x00160000 // Unsigned Byte   , size 1, enum value 6
#define HCPVF_ATTRB_TYPE_DOUBLE     0x00870000 // Double          , size 8, enum value 7


// Vertex Attribute Size
#define HCPVF_ATTRB_SIZE(x) ((uint32_t) x) << 8

// Vertex Normalized
#define HCPVF_ATTRB_NORMALIZED_TRUE 1
#define HCPVF_ATTRB_NORMALIZED_FALSE 0

struct HCPVertexAttribute
{
    uint32_t data;

    inline uint32_t getUsage() const { return data & HCPVF_ATTRB_USAGE_MASK; }
    inline uint32_t getSize() const { return (data & HCPVF_ATTRB_SIZE_MASK) >> 8; }
    inline uint32_t getType() const { return data & HCPVF_ATTRB_TYPE_MASK; }
    inline uint32_t getTypeSize() const { return (data & HCPVF_ATTRB_TYPE_SIZE_MASK) >> 20; }

    inline uint32_t getAPIType() const
    {
        uint32_t type = getType();

        switch (type)
        {
        case HCPVF_ATTRB_TYPE_FLOAT:
            return GL_FLOAT;
        case HCPVF_ATTRB_TYPE_INT:
            return GL_INT;
        case HCPVF_ATTRB_TYPE_UINT:
            return GL_UNSIGNED_INT;
        case HCPVF_ATTRB_TYPE_SHORT:
            return GL_SHORT;
        case HCPVF_ATTRB_TYPE_USHORT:
            return GL_UNSIGNED_SHORT;
        case HCPVF_ATTRB_TYPE_BYTE:
            return GL_BYTE;
        case HCPVF_ATTRB_TYPE_UBYTE:
            return GL_UNSIGNED_BYTE;
        default:
            return GL_FLOAT;
        }
    }

    inline int numBytes() const
    {
        uint32_t size = getSize();
        uint32_t typeSize = getTypeSize();

        return size * typeSize;
    }
    
    inline bool isNormalized() const
    {
        return data & HCPVF_ATTRB_NORMALIZED_MASK;
    }
};

struct HCPVertexFormat
{
    int size = 0;
    HCPVertexAttribute attributes[32];

    void apply() const;
    void unapply() const;

    inline HCPVertexAttribute& operator[](int index)
    {
       return attributes[index]; 
    }

    inline int vertexNumBytes() const
    {
        int numBytes = 0;

        for(int i = 0; i < size; i++)
        {
            numBytes += attributes[i].numBytes();
        }

        return numBytes;
    }
};

class HCPMeshBuilder
{
public:
    HCPMeshBuilder(const HCPVertexFormat& vtxFmt);
    HCPMeshBuilder(const HCPMeshBuilder&) = delete;
    ~HCPMeshBuilder();

    float defaultNormal[3];
    float defaultUV[2];
    float defaultColor[4];

    void reset();
    void drawArrays(GLenum mode);
    void drawElements(GLenum mode);
    void drawArraysInstanced(GLenum mode, int instances);
    void drawElementsInstanced(GLenum mode, int instances);

    HCPMeshBuilder& position(float x, float y, float z);
    HCPMeshBuilder& normal(float x, float y, float z);
    HCPMeshBuilder& normalDefault();
    HCPMeshBuilder& uv(float u, float v);
    HCPMeshBuilder& uvDefault();
    HCPMeshBuilder& colorRGB(float r, float g, float b);
    HCPMeshBuilder& colorRGBA(float r, float g, float b, float a);
    HCPMeshBuilder& colorDefault();
    HCPMeshBuilder& texid(uint32_t texid);

    HCPMeshBuilder& vertex(void* notUsed, ...);
    HCPMeshBuilder& index(size_t numIndicies, ...);
    HCPMeshBuilder& indexv(size_t numIndicies, const uint32_t* indicies);

    const HCPVertexFormat& getVertexFormat() const;
    const uint8_t* getVertexBuffer(size_t* getNumBytes) const;
    const uint32_t* getIndexBuffer(size_t* getNumBytes) const;

    glm::mat4& pushMatrix();
    glm::mat4& popMatrix();
    glm::mat4& resetMatrixStack();
    glm::mat4& getModelView();
private:
    HCPVertexFormat m_vertexFormat;

    // Modelview variables
    std::stack<glm::mat4> m_modelViewStack;
    glm::mat3 m_modelView3x3;

    // OpenGL variables
    GLuint m_glVBO;
    GLuint m_glEBO;
    GLuint m_glVAO;
    size_t m_glVertexBufferSize;
    size_t m_glElementBufferSize;

    // Book Keeping
    bool m_isRenderable;
    size_t m_numVerticies;
    size_t m_numIndicies;

    // Buffers
    std::vector<uint8_t> m_vertexDataBuffer;
    std::vector<uint8_t> m_indexDataBuffer;

    void initForRendering();
    void pushVertexData(size_t size, const void* data);
    void pushIndexData(size_t size, const void* data);
    void get3x3ModelView();
};

#endif // HCP_MESHBUILDER_HPP