#include "MeshBuilder.hpp"

#include <stdarg.h>

void HCPVertexFormat::apply() const
{
    int stride = vertexNumBytes();
    size_t pointer = 0;

    for(int i = 0; i < size; i++)
    {
        HCPVertexAttribute attrib = attributes[i];
        uint32_t a_numBytes = attrib.numBytes();
        uint32_t a_apiType = attrib.getAPIType();
        uint32_t a_size = attrib.getSize();
        bool a_normalized = attrib.isNormalized();

        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, a_size, a_apiType, a_normalized, stride, (void*) pointer);
        pointer += a_numBytes;
    }
}

void HCPVertexFormat::unapply() const
{
    for(int i = 0; i < size; i++)
    {
        glDisableVertexAttribArray(i);
    }
}

HCPMeshBuilder::HCPMeshBuilder(const HCPVertexFormat& vtxFmt) :
    defaultNormal{0.0, 1.0f, 0.0f},
    defaultUV{0.0f, 0.0f},
    defaultColor{1.0f, 1.0f, 1.0f, 1.0f},
    m_vertexFormat(vtxFmt),
    m_isRenderable(false),
    m_numVerticies(0),
    m_numIndicies(0)
{
    resetMatrixStack();

    const size_t BUFFER_STARTING_SIZE = 4098;

    m_vertexDataBuffer.reserve(BUFFER_STARTING_SIZE);
    m_indexDataBuffer.reserve(BUFFER_STARTING_SIZE);
}

HCPMeshBuilder::~HCPMeshBuilder()
{
    if(m_isRenderable)
    {
        glDeleteVertexArrays(1, &m_glVAO);
        glDeleteBuffers(1, &m_glVBO);
        glDeleteBuffers(1, &m_glEBO);
    }
}

void HCPMeshBuilder::reset()
{
    m_vertexDataBuffer.clear();
    m_indexDataBuffer.clear();
    m_numVerticies = 0;
    m_numIndicies = 0;
}

void HCPMeshBuilder::drawArrays(GLenum mode)
{
    drawArraysInstanced(mode, 1);
}

void HCPMeshBuilder::drawElements(GLenum mode)
{
    drawElementsInstanced(mode, 1);
}

void HCPMeshBuilder::drawArraysInstanced(GLenum mode, int instances)
{
    if(!m_isRenderable) initForRendering();

    glBindBuffer(GL_ARRAY_BUFFER, m_glVBO);

    if(m_vertexDataBuffer.capacity() <= m_glVertexBufferSize)
    {
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertexDataBuffer.size(), m_vertexDataBuffer.data());
    }
    else
    {
        glBufferData(GL_ARRAY_BUFFER, m_vertexDataBuffer.capacity(), m_vertexDataBuffer.data(), GL_DYNAMIC_DRAW);
        m_glVertexBufferSize = m_vertexDataBuffer.capacity();
    }

    glBindVertexArray(m_glVAO);
    glDrawArraysInstanced(mode, 0, (GLsizei) m_numVerticies, instances);
    glBindVertexArray(0);
}

void HCPMeshBuilder::drawElementsInstanced(GLenum mode, int instances)
{
    if(!m_isRenderable) initForRendering();
    
    glBindBuffer(GL_ARRAY_BUFFER, m_glVBO);

    if(m_vertexDataBuffer.capacity() <= m_glVertexBufferSize)
    {
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertexDataBuffer.size(), m_vertexDataBuffer.data());
    }
    else
    {
        glBufferData(GL_ARRAY_BUFFER, m_vertexDataBuffer.capacity(), m_vertexDataBuffer.data(), GL_DYNAMIC_DRAW);
        m_glVertexBufferSize = m_vertexDataBuffer.capacity();
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glEBO);

    if(m_indexDataBuffer.capacity() <= m_glElementBufferSize)
    {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_indexDataBuffer.size(), m_indexDataBuffer.data());
    }
    else
    {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexDataBuffer.capacity(), m_indexDataBuffer.data(), GL_DYNAMIC_DRAW);
        m_glElementBufferSize = m_indexDataBuffer.capacity();
    }

    glBindVertexArray(m_glVAO);
    glDrawArraysInstanced(mode, 0, (GLsizei) m_numVerticies, instances);
    glBindVertexArray(0);
}

HCPMeshBuilder& HCPMeshBuilder::position(float x, float y, float z)
{
    glm::vec4 pos(x, y, z, 1.0f);
    pos = m_modelViewStack.top() * pos;

    pushVertexData(sizeof(glm::vec3), &pos);

    m_numVerticies++;

    return *this;
}

HCPMeshBuilder& HCPMeshBuilder::normal(float x, float y, float z)
{
    glm::vec3 normal(x, y, z);
    get3x3ModelView();

    normal = m_modelView3x3 * normal;

    pushVertexData(sizeof(glm::vec3), &normal);

    return *this;
}

HCPMeshBuilder& HCPMeshBuilder::normalDefault()
{
    return normal(defaultNormal[0], defaultNormal[1], defaultNormal[2]);
}

HCPMeshBuilder& HCPMeshBuilder::uv(float u, float v)
{
    glm::vec2 uv(u, v);

    pushVertexData(sizeof(glm::vec2), &uv);

    return *this;
}

HCPMeshBuilder& HCPMeshBuilder::uvDefault()
{
    return uv(defaultUV[0], defaultUV[1]);
}

HCPMeshBuilder& HCPMeshBuilder::colorRGB(float r, float g, float b)
{
    return colorRGBA(r, g, b, defaultColor[3]);
}

HCPMeshBuilder& HCPMeshBuilder::colorRGBA(float r, float g, float b, float a)
{
    glm::vec4 rgba(r, g, b, a);

    pushVertexData(sizeof(glm::vec4), &rgba);

    return *this;
}

HCPMeshBuilder& HCPMeshBuilder::colorDefault()
{
    return colorRGBA(defaultColor[0], defaultColor[1], defaultColor[2], defaultColor[3]);
}

HCPMeshBuilder& HCPMeshBuilder::texid(uint32_t texid)
{
    pushVertexData(sizeof(uint32_t), &texid);

    return *this;
}

HCPMeshBuilder& HCPMeshBuilder::vertex(void* notUsed, ...)
{
    const HCPVertexFormat& vtxFmt = getVertexFormat();
    uint8_t newVtxBuffer[1024];
    int newVtxBufferPos = 0;

    int numAttributes = vtxFmt.size;
    int vertexSize = vtxFmt.vertexNumBytes();

    va_list attribArgs;
    va_start(attribArgs, notUsed);

    float   valuef;
    double  valued;
    int32_t value4b;
    int16_t value2b;
    int8_t  value1b;
    float* posInVertex = NULL;
    int posAttribSize = 0;

    for(int i = 0; i < numAttributes; i++)
    {
        uint32_t attribType  = vtxFmt.attributes[i].getType();
        uint32_t attribSize  = vtxFmt.attributes[i].getSize();
        uint32_t attribUsage = vtxFmt.attributes[i].getUsage();

        if(attribUsage == HCPVF_ATTRB_USAGE_POS)
        {
            posInVertex = (float*) (newVtxBuffer + newVtxBufferPos);
            posAttribSize = attribSize;
        }

        for(uint32_t j = 0; j < attribSize; j++)
        {
            switch(attribType)
            {
            case HCPVF_ATTRB_TYPE_INT:
            case HCPVF_ATTRB_TYPE_UINT:
                value4b = va_arg(attribArgs, int32_t);
                memcpy(newVtxBuffer + newVtxBufferPos, &value4b, sizeof(int32_t));
                newVtxBufferPos += sizeof(int32_t);
                break;
            case HCPVF_ATTRB_TYPE_SHORT:
            case HCPVF_ATTRB_TYPE_USHORT:
                value2b = va_arg(attribArgs, int32_t);
                memcpy(newVtxBuffer + newVtxBufferPos, &value2b, sizeof(int16_t));
                newVtxBufferPos += sizeof(int16_t);
                break;
            case HCPVF_ATTRB_TYPE_BYTE:
            case HCPVF_ATTRB_TYPE_UBYTE:
                value1b = va_arg(attribArgs, int32_t);
                memcpy(newVtxBuffer + newVtxBufferPos, &value1b, sizeof(int8_t));
                newVtxBufferPos += sizeof(uint8_t);
                break;
            case HCPVF_ATTRB_TYPE_FLOAT:
                valuef = (float) va_arg(attribArgs, double);
                memcpy(newVtxBuffer + newVtxBufferPos, &valuef, sizeof(float));
                newVtxBufferPos += sizeof(float);
                break;
            case HCPVF_ATTRB_TYPE_DOUBLE:
                valued = va_arg(attribArgs, double);
                memcpy(newVtxBuffer + newVtxBufferPos, &valued, sizeof(double));
                newVtxBufferPos += sizeof(double);
                break;
            }
        }
    }

    va_end(attribArgs);

    assert(posInVertex != NULL);
    assert(posAttribSize == 3);

    glm::vec4 pos(0.0f, 0.0f, 0.0f, 1.0f);
    memcpy(&pos, posInVertex, sizeof(glm::vec3));
    pos = m_modelViewStack.top() * pos;
    memcpy(posInVertex, &pos, sizeof(glm::vec3));

    pushVertexData(vertexSize, newVtxBuffer);
    m_numVerticies++;

    return *this;
}

HCPMeshBuilder& HCPMeshBuilder::index(size_t numIndicies, ...)
{
    uint32_t indicies[64];
    va_list indexArgs;
    va_start(indexArgs, numIndicies);

    numIndicies = 64 < numIndicies ? 64 : numIndicies;

    for(size_t i = 0; i < numIndicies; i++)
    {
        indicies[i] = va_arg(indexArgs, uint32_t) + (uint32_t) m_numVerticies;
    }

    va_end(indexArgs);

    pushIndexData(numIndicies * sizeof(uint32_t), indicies);
    m_numIndicies += numIndicies;

    return *this;
}

HCPMeshBuilder& HCPMeshBuilder::indexv(size_t numIndicies, const uint32_t* indicies)
{
    uint32_t bufferedIndicies[128];
    size_t bufferedIndiciesPos = 0;
    for(size_t i = 0; i < numIndicies; i++)
    {
        bufferedIndicies[bufferedIndiciesPos++] = indicies[i] + (uint32_t) m_numVerticies;

        if(128 <= bufferedIndiciesPos)
        {
            pushIndexData(bufferedIndiciesPos * sizeof(uint32_t), bufferedIndicies);
            bufferedIndiciesPos = 0;
        }
    }

    if(0 < bufferedIndiciesPos)
    {
        pushIndexData(bufferedIndiciesPos * sizeof(uint32_t), bufferedIndicies);
    }

    m_numIndicies += numIndicies;

    return *this;
}

const HCPVertexFormat& HCPMeshBuilder::getVertexFormat() const
{
    return m_vertexFormat;
}

const uint8_t* HCPMeshBuilder::getVertexBuffer(size_t* getNumBytes) const
{
    if(getNumBytes)
    {
        *getNumBytes = m_vertexDataBuffer.size();
    }

    return m_vertexDataBuffer.data();
}

const uint32_t* HCPMeshBuilder::getIndexBuffer(size_t* getNumBytes) const
{
    if(getNumBytes)
    {
        *getNumBytes = m_indexDataBuffer.size();
    }

    return (uint32_t*) m_indexDataBuffer.data();
}

glm::mat4& HCPMeshBuilder::pushMatrix()
{
    m_modelViewStack.push(m_modelViewStack.top());
    return m_modelViewStack.top();
}

glm::mat4& HCPMeshBuilder::popMatrix()
{
    if(m_modelViewStack.size() <= 1)
    {
        return m_modelViewStack.top();
    }

    m_modelViewStack.pop();
    return m_modelViewStack.top();
}

glm::mat4& HCPMeshBuilder::resetMatrixStack()
{
    while(!m_modelViewStack.empty())
    {
        m_modelViewStack.pop();
    }

    m_modelViewStack.emplace(1.0f);
    return m_modelViewStack.top();
}

glm::mat4& HCPMeshBuilder::getModelView()
{
    return m_modelViewStack.top();
}

void HCPMeshBuilder::initForRendering()
{
    glGenBuffers(1, &m_glVBO);
    glGenBuffers(1, &m_glEBO);
    glGenVertexArrays(1, &m_glVAO);

    m_glVertexBufferSize = m_vertexDataBuffer.capacity();
    m_glElementBufferSize = m_indexDataBuffer.capacity();

    glBindVertexArray(m_glVAO);
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_glVBO);
        glBufferData(GL_ARRAY_BUFFER, m_glVertexBufferSize, NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_glElementBufferSize, NULL, GL_DYNAMIC_DRAW);

        m_vertexFormat.apply();
    }
    glBindVertexArray(0);

    m_isRenderable = true;
}

void HCPMeshBuilder::pushVertexData(size_t size, const void* data)
{
    m_vertexDataBuffer.resize(m_vertexDataBuffer.size() + size);
    memcpy(m_vertexDataBuffer.data() + m_vertexDataBuffer.size() - size, data, size);
}

void HCPMeshBuilder::pushIndexData(size_t size, const void* data)
{
    m_indexDataBuffer.resize(m_indexDataBuffer.size() + size);
    memcpy(m_indexDataBuffer.data() + m_indexDataBuffer.size() - size, data, size);
}

void HCPMeshBuilder::get3x3ModelView()
{
    glm::mat4& m_4x4 = m_modelViewStack.top();
    glm::mat3& m_3x3 = m_modelView3x3;

    for(int r = 0; r < 3; r++)
    {
        for(int c = 0; c < 3; c++)
        {
            m_3x3[r][c] = m_4x4[r][c];
        }
    }
}