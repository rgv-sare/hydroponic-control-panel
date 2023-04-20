#include <UIRender.hpp>

#include <Shaders.hpp>
#include <MeshBuilder.hpp>
#include <Logger.hpp>

#include <glm/gtc/matrix_transform.hpp> 

#define getVec4Color(intcolor) { ((intcolor >> 16) & 0xFF) / 255.0f, ((intcolor >> 8) & 0xFF) / 255.0f, (intcolor & 0xFF) / 255.0f, ((intcolor >> 24) & 0xFF) / 255.0f }
#define vec4Color(vec4color) vec4color.r, vec4color.g, vec4color.b, vec4color.a

// Batch Rendering
static HCPMeshBuilder* i_batchMeshBuilder = nullptr;

// Window bookkeeping
static GLFWwindow* i_window = nullptr;
static HCPInputContext* i_inputContext = nullptr;
static int i_windowWidth = 0;
static int i_windowHeight = 0;
static float i_uiScale = 1.0f;

// UIRender Logging
static HCPLogger i_logger("UIRender");

// Font Rendering
static HCPFontRenderer i_fontRenderer;
static int i_fontAtlasTexUnit = 0;

static inline void i_orientGradientQuad(float* dst, HCPDirection dir, float left, float top, float right, float bottom);
static void i_resizeCallback(GLFWwindow* window, int width, int height);
static void i_init();

void hcpui::init(GLFWwindow* window)
{
    GLFWwindow* previousWindow = i_window;
    i_window = window;

    if(i_window)
    {
        glfwSetWindowSizeCallback(i_window, i_resizeCallback);
        glfwGetWindowSize(i_window, &i_windowWidth, &i_windowHeight);


        i_inputContext = HCPInputs::get(i_window);
        i_init();
    }
    else if(previousWindow)
    {
        glfwSetWindowSizeCallback(previousWindow, nullptr);
        i_windowWidth = i_windowHeight = 0;
    }
}

HCPInputContext* hcpui::getInputContext()
{
    return i_inputContext;
}

void hcpui::setupUIRendering()
{
    glm::mat4 projection = glm::ortho(0.0f, (float) i_windowWidth, (float) i_windowHeight, 0.0f, -1.0f, 1.0f);
    hcps::setProjectionMatrix(projection);
    hcps::setModelViewMatrix(i_batchMeshBuilder->getModelView());

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

void hcpui::genQuad(float left, float top, float right, float bottom, uint32_t color, int texID)
{
    glm::vec4 colorVec = getVec4Color(color);

    genQuad(left, top, right, bottom, colorVec, texID);
}

void hcpui::genGradientQuad(HCPDirection direction, float left, float top, float right, float bottom, uint32_t color1, uint32_t color2, int texID)
{
    glm::vec4 colorVec1 = getVec4Color(color1);
    glm::vec4 colorVec2 = getVec4Color(color2);

    genGradientQuad(direction, left, top, right, bottom, colorVec1, colorVec2, texID);
}

void hcpui::genVerticalLine(float x, float top, float bottom, uint32_t color, float width)
{
    glm::vec4 colorVec = getVec4Color(color);

    genVerticalLine(x, top, bottom, colorVec, width);
}

void hcpui::genHorizontalLine(float y, float left, float right, uint32_t color, float width)
{
    glm::vec4 colorVec = getVec4Color(color);

    genHorizontalLine(y, left, right, colorVec, width);
}

void hcpui::genString(HCPAlignment alignment, const char* str, float x, float y, float scale, uint32_t color)
{
    glm::vec4 colorVec = getVec4Color(color);

    genString(alignment, str, x, y, scale, colorVec);
}

void hcpui::genString(const char* str, float x, float y, float scale, uint32_t color)
{
    glm::vec4 colorVec = getVec4Color(color);

    genString(HCPAlignment::TOP_LEFT, str, x, y, scale, colorVec);
}

void hcpui::genQuad(float left, float top, float right, float bottom, const glm::vec4& color, int texID)
{
    i_batchMeshBuilder->index(6, 0, 1, 2, 0, 2, 3);

    i_batchMeshBuilder->vertex(NULL,  left, bottom, 0.0f, 0.0f, 0.0f, vec4Color(color), texID);
    i_batchMeshBuilder->vertex(NULL, right, bottom, 0.0f, 0.0f, 0.0f, vec4Color(color), texID);
    i_batchMeshBuilder->vertex(NULL, right,    top, 0.0f, 0.0f, 0.0f, vec4Color(color), texID);
    i_batchMeshBuilder->vertex(NULL,  left,    top, 0.0f, 0.0f, 0.0f, vec4Color(color), texID);
}

void hcpui::genGradientQuad(HCPDirection direction, float left, float top, float right, float bottom, const glm::vec4& color1, const glm::vec4& color2, int texID)
{
    float positions[8];
    i_orientGradientQuad(positions, direction, left, top, right, bottom);

    i_batchMeshBuilder->index(6, 0, 1, 2, 0, 2, 3);

    i_batchMeshBuilder->vertex(NULL, positions[0], positions[1], 0.0f, 0.0f, 0.0f, vec4Color(color2), texID);
    i_batchMeshBuilder->vertex(NULL, positions[2], positions[3], 0.0f, 0.0f, 0.0f, vec4Color(color2), texID);
    i_batchMeshBuilder->vertex(NULL, positions[4], positions[5], 0.0f, 0.0f, 0.0f, vec4Color(color1), texID);
    i_batchMeshBuilder->vertex(NULL, positions[6], positions[7], 0.0f, 0.0f, 0.0f, vec4Color(color1), texID);
}

void hcpui::genVerticalLine(float x, float top, float bottom, const glm::vec4& color, float width)
{
    i_batchMeshBuilder->index(6, 0, 1, 2, 0, 2, 3);

    i_batchMeshBuilder->vertex(NULL, x,         top,    0.0f, 0.0f, 0.0f, vec4Color(color), 0);
    i_batchMeshBuilder->vertex(NULL, x,         bottom, 0.0f, 0.0f, 0.0f, vec4Color(color), 0);
    i_batchMeshBuilder->vertex(NULL, x + width, bottom, 0.0f, 0.0f, 0.0f, vec4Color(color), 0);
    i_batchMeshBuilder->vertex(NULL, x + width, top,    0.0f, 0.0f, 0.0f, vec4Color(color), 0);
}

void hcpui::genHorizontalLine(float y, float left, float right, const glm::vec4& color, float width)
{
    i_batchMeshBuilder->index(6, 0, 1, 2, 0, 2, 3);

    i_batchMeshBuilder->vertex(NULL, left,  y,          0.0f, 0.0f, 0.0f, vec4Color(color), 0);
    i_batchMeshBuilder->vertex(NULL, right, y,          0.0f, 0.0f, 0.0f, vec4Color(color), 0);
    i_batchMeshBuilder->vertex(NULL, right, y - width,  0.0f, 0.0f, 0.0f, vec4Color(color), 0);
    i_batchMeshBuilder->vertex(NULL, left,  y - width,  0.0f, 0.0f, 0.0f, vec4Color(color), 0);
}

void hcpui::genString(HCPAlignment alignment, const char* str, float x, float y, float scale, const glm::vec4& color)
{
    i_fontRenderer.setAnchor(alignment);
    i_fontRenderer.setTextSize(scale);
    i_fontRenderer.genString(*i_batchMeshBuilder, str, x, y, color);
}

void hcpui::genString(const char* str, float x, float y, float scale, const glm::vec4& color)
{
    i_fontRenderer.setAnchor(HCPAlignment::TOP_LEFT);
    i_fontRenderer.setTextSize(scale);
    i_fontRenderer.genString(*i_batchMeshBuilder, str, x, y, color);
}

void hcpui::renderBatch()
{
    hcps::UI();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    i_fontRenderer.bindAtlas();

    i_batchMeshBuilder->drawElements(GL_TRIANGLES);
    i_batchMeshBuilder->reset();
}

float hcpui::getStringwidth(const char* str, float scale)
{
    i_fontRenderer.setTextSize(scale);
    return i_fontRenderer.getStringWidth(str);
}

int hcpui::getWindowWidth()
{
    return i_windowWidth;
}

int hcpui::getWindowHeight()
{
    return i_windowHeight;
}

float hcpui::getUIWidth()
{
    return i_windowWidth / i_uiScale;
}

float hcpui::getUIHeight()
{
    return i_windowHeight / i_uiScale;
}

float hcpui::getUIScale()
{
    return i_uiScale;
}

float hcpui::getUICursorX()
{
    return i_inputContext->cursorX() / i_uiScale;
}

float hcpui::getUICursorY()
{
    return i_inputContext->cursorY() / i_uiScale;
}

void hcpui::pushStack()
{
    i_batchMeshBuilder->pushMatrix();
}

void hcpui::popStack()
{
    i_batchMeshBuilder->popMatrix();
}

void hcpui::translate(float x, float y)
{
    glm::mat4& mat = i_batchMeshBuilder->getModelView();

    mat = glm::translate(mat, glm::vec3(x, y, 0.0f));
}

void hcpui::scale(float x, float y)
{
    glm::mat4& mat = i_batchMeshBuilder->getModelView();

    mat = glm::scale(mat, glm::vec3(x, y, 1.0f));
}

void hcpui::rotate(float angle)
{
    glm::mat4& mat = i_batchMeshBuilder->getModelView();

    mat = glm::rotate(mat, angle, glm::vec3(0.0f, 0.0f, 1.0f));
}

void hcpui::multiplyMatrix(const glm::mat4& mat)
{
    i_batchMeshBuilder->getModelView() *= mat;
}

void hcpui::setMatrix(const glm::mat4& mat)
{
    i_batchMeshBuilder->getModelView() = mat;
}

glm::mat4& hcpui::getModelViewMatrix()
{
    return i_batchMeshBuilder->getModelView();
}

static inline void i_orientGradientQuad(float* dst, HCPDirection dir, float left, float top, float right, float bottom)
{
    switch(dir)
    {
    case HCPDirection::LEFT:
        dst[0] = left;  dst[1] = top;
        dst[2] = left;  dst[3] = bottom;
        dst[4] = right; dst[5] = bottom;
        dst[6] = right; dst[7] = top;
        break;
    case HCPDirection::RIGHT:
        dst[0] = right; dst[1] = bottom;
        dst[2] = right; dst[3] = top;
        dst[4] = left;  dst[5] = top;
        dst[6] = left;  dst[7] = bottom;
        break;
    case HCPDirection::TOP:
        dst[0] = left;  dst[1] = bottom;
        dst[2] = right; dst[3] = bottom;
        dst[4] = right; dst[5] = top;
        dst[6] = left;  dst[7] = top;
        break;
    default:
        dst[0] = left;  dst[1] = bottom;
        dst[2] = right; dst[3] = bottom;
        dst[4] = right; dst[5] = top;
        dst[6] = left;  dst[7] = top;
    }
}

static void i_resizeCallback(GLFWwindow* window, int width, int height)
{
    i_windowWidth = width;
    i_windowHeight = height;
}

static void i_init()
{
    glfwMakeContextCurrent(i_window);

    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    if(i_batchMeshBuilder) return;

    HCPVertexFormat vtxFmt;
    vtxFmt.size = 4;

    vtxFmt[0].data = HCPVF_ATTRB_USAGE_POS
                   | HCPVF_ATTRB_TYPE_FLOAT
                   | HCPVF_ATTRB_SIZE(3)
                   | HCPVF_ATTRB_NORMALIZED_FALSE;
    
    vtxFmt[1].data = HCPVF_ATTRB_USAGE_UV
                   | HCPVF_ATTRB_TYPE_FLOAT
                   | HCPVF_ATTRB_SIZE(2)
                   | HCPVF_ATTRB_NORMALIZED_FALSE;

    vtxFmt[2].data = HCPVF_ATTRB_USAGE_COLOR
                     | HCPVF_ATTRB_TYPE_FLOAT
                     | HCPVF_ATTRB_SIZE(4)
                     | HCPVF_ATTRB_NORMALIZED_FALSE;

    vtxFmt[3].data = HCPVF_ATTRB_USAGE_TEXID
                   | HCPVF_ATTRB_TYPE_UINT
                   | HCPVF_ATTRB_SIZE(1)
                   | HCPVF_ATTRB_NORMALIZED_FALSE;

    i_batchMeshBuilder = new HCPMeshBuilder(vtxFmt);

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &i_fontAtlasTexUnit);
    i_fontRenderer.setAtlasTexUnit(i_fontAtlasTexUnit);

    i_logger.infof("Initialized UIRender");
}