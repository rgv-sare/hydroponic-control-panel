#include <Shaders.hpp>

#include <GLInclude.hpp>
#include <gl3_shaders.hpp>

#include <Logger.hpp>

static HCPLogger shaderLogger("Shaders");

static bool i_hasInit = false;
static int i_maxTextureUnits;
static int i_texutreUnits[32];
static GLuint i_currentShader = 0;

static glm::mat4 i_projectionMatrix = glm::mat4(1.0f);
static glm::mat4 i_modelViewMatrix = glm::mat4(1.0f);
static glm::vec4 i_color = glm::vec4(1.0f);

static void i_initShaders();

class BasicShader
{
public:
    BasicShader(const char* vertexShaderSource, const char* fragmentShaderSource)
    {
        this->vertexShaderSource = vertexShaderSource;
        this->fragmentShaderSource = fragmentShaderSource;
        hasInit = false;
    }

    GLuint programID;
    GLuint u_projectionMatrix;
    GLuint u_modelViewMatrix;
    GLuint u_color;
    GLuint u_textures;
    GLuint u_maxTextures;
    bool hasInit;

    void init()
    {   
        if (hasInit) return;
        if (!i_hasInit) i_initShaders();

        GLint success;

        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            GLchar infoLog[512];
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            shaderLogger.errorf("Vertex Shader Compilation Failed:\n%s", infoLog);
        }

        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            GLchar infoLog[512];
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            shaderLogger.errorf("Fragment Shader Compilation Failed:\n %s", infoLog);
        }

        programID = glCreateProgram();
        glAttachShader(programID, vertexShader);
        glAttachShader(programID, fragmentShader);
        glLinkProgram(programID);
        glGetProgramiv(programID, GL_LINK_STATUS, &success);
        if (!success)
        {
            GLchar infoLog[512];
            glGetProgramInfoLog(programID, 512, NULL, infoLog);
            shaderLogger.errorf("Shader Program Linking Failed:\n %s", infoLog);
        }

        u_projectionMatrix = glGetUniformLocation(programID, "u_projectionMatrix");
        u_modelViewMatrix = glGetUniformLocation(programID, "u_modelViewMatrix");
        u_color = glGetUniformLocation(programID, "u_color");
        u_textures = glGetUniformLocation(programID, "u_textures");
        u_maxTextures = glGetUniformLocation(programID, "u_maxTextures");

        hasInit = true;
    }

    void use()
    {
        if(!hasInit) init();

        if(i_currentShader != programID)
        {
            glUseProgram(programID);
            i_currentShader = programID;
        }
        
        glUniformMatrix4fv(u_projectionMatrix, 1, GL_FALSE, &i_projectionMatrix[0][0]);
        glUniformMatrix4fv(u_modelViewMatrix, 1, GL_FALSE, &i_modelViewMatrix[0][0]);
        glUniform4fv(u_color, 1, &i_color[0]);
        glUniform1iv(u_textures, i_maxTextureUnits, i_texutreUnits);
        glUniform1i(u_maxTextures, i_maxTextureUnits);
    }
private:
    const char* vertexShaderSource;
    const char* fragmentShaderSource;
};

static BasicShader i_POS_SHADER(POS_SHADER_vcode, POS_SHADER_fcode);
static BasicShader i_POS_UV_SHADER(POS_UV_SHADER_vcode, POS_UV_SHADER_fcode);
static BasicShader i_POS_COLOR_SHADER(POS_COLOR_SHADER_vcode, POS_COLOR_SHADER_fcode);
static BasicShader i_POS_UV_COLOR_TEXID_SHADER(POS_UV_COLOR_TEXID_SHADER_vcode, POS_UV_COLOR_TEXID_SHADER_fcode);
static BasicShader i_UI_SHADER(UI_SHADER_vcode, UI_SHADER_fcode);

void HCPShaders::setProjectionMatrix(const glm::mat4& proj)
{
    i_projectionMatrix = proj;
}

void HCPShaders::setModelViewMatrix(const glm::mat4& modelView)
{
    i_modelViewMatrix = modelView;
}

void HCPShaders::setColor(const glm::vec4& color)
{
    i_color = color;
}

void HCPShaders::setColor(float r, float g, float b, float a)
{
    i_color = glm::vec4(r, g, b, a);
}

glm::mat4 HCPShaders::getProjectionMatrix()
{
    return i_projectionMatrix;
}

glm::mat4 HCPShaders::getModelViewMatrix()
{
    return i_modelViewMatrix;
}

glm::vec4 HCPShaders::getColor()
{
    return i_color;
}

void HCPShaders::POS()
{
    i_POS_SHADER.use();
}

void HCPShaders::POS_UV()
{
    i_POS_UV_SHADER.use();
}

void HCPShaders::POS_COLOR()
{
    i_POS_COLOR_SHADER.use();
}

void HCPShaders::POS_UV_COLOR_TEXID()
{
    i_POS_UV_COLOR_TEXID_SHADER.use();
}

void HCPShaders::UI()
{
    i_UI_SHADER.use();
}

static void i_initShaders()
{
    if (i_hasInit) return;

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &i_maxTextureUnits);
    shaderLogger.infof("Max Texture Units: %d", i_maxTextureUnits);

    for (int i = 0; i < i_maxTextureUnits; i++)
    {
        i_texutreUnits[i] = i;
    }

    i_hasInit = true;
}