#include "FontRenderer.hpp"

#include <cstring>
#include <glm/glm.hpp>
#include <stb_image.h>
#include <cstring>

// Font placed here
#include "Consolas_font.hpp"

#include "GLInclude.hpp"
#include "Logger.hpp"
#include "UIRender.hpp"

#define getVec4Color(intcolor)\
{ ((intcolor >> 16) & 0xFF) / 255.0f,\
  ((intcolor >>  8) & 0xFF) / 255.0f,\
  (intcolor & 0xFF) / 255.0f,\
  ((intcolor >> 24) & 0xFF) / 255.0f }
#define vec4Color(vec4color) vec4color[0], vec4color[1], vec4color[2], vec4color[3]

static const char* m_FORMATTING_KEYS = "0123456789abcdefklmnor";

static const glm::vec4 m_COLOR_CODES[] = 
{
    glm::vec4(0.00f, 0.00f, 0.00f, 1.00f),
    glm::vec4(0.00f, 0.00f, 0.67f, 1.00f),
    glm::vec4(0.00f, 0.67f, 0.00f, 1.00f),
    glm::vec4(0.00f, 0.67f, 0.67f, 1.00f),
    glm::vec4(0.67f, 0.00f, 0.00f, 1.00f),
    glm::vec4(0.67f, 0.00f, 0.67f, 1.00f),
    glm::vec4(1.00f, 0.67f, 0.00f, 1.00f),
    glm::vec4(0.67f, 0.67f, 0.67f, 1.00f),
    glm::vec4(0.33f, 0.33f, 0.33f, 1.00f),
    glm::vec4(0.33f, 0.33f, 1.00f, 1.00f),
    glm::vec4(0.33f, 1.00f, 0.33f, 1.00f),
    glm::vec4(0.33f, 1.00f, 1.00f, 1.00f),
    glm::vec4(1.00f, 0.33f, 0.33f, 1.00f),
    glm::vec4(1.00f, 0.33f, 1.00f, 1.00f),
    glm::vec4(1.00f, 1.00f, 0.33f, 1.00f),
    glm::vec4(1.00f, 1.00f, 1.00f, 1.00f)
};

static HCPLogger m_logger("Font Renderer");

HCPFontRenderer::HCPFontRenderer() :
    m_isRenderable(false),
    m_glAtlasTex(0),
    m_font(Consolas_Font()),
    m_scale(1.0f),
    m_textSize((float) m_font.fontHeight),
    m_anchor(HCPAlignment::TOP_LEFT),
    m_texUnit(0)
{
    setTextSize(16);
}

HCPFontRenderer::~HCPFontRenderer()
{
    if(m_isRenderable)
    {
        glDeleteTextures(1, &m_glAtlasTex);
    }
}

const char* HCPFontRenderer::getFontName() const
{
    return m_font.name;
}

float HCPFontRenderer::getFontHeight() const
{
    return (float) m_font.fontHeight;
}

float HCPFontRenderer::getFontLineHeight() const
{
    return (float) m_font.lineHeight;
}

float HCPFontRenderer::getStringWidth(const char* str) const
{
    return getStringWidth(str, strlen(str));
}

float HCPFontRenderer::getStringWidth(const char* str, size_t strLen) const
{
    int cursor = 0;

    for(int i = 0; i < strLen;)
    {
        int bytesRead = 0;
        int unicode = getUnicodeFromUTF8((const uint8_t*) &str[i], &bytesRead);
        
        if(unicode == 167 && i + bytesRead < strLen)
        {
            i++;
        }
        else
        {
            Glyph glyph = m_font.glyphs[unicode];

            cursor += glyph.xAdvance - m_font.leftPadding - m_font.rightPadding;
        }

        i += bytesRead;
    }

    return cursor * m_scale;
}

float HCPFontRenderer::getStringHeight() const
{
    return m_textSize;
}

void HCPFontRenderer::setTextSize(float size)
{
    size = glm::max(0.1f, size);
    m_textSize = size;
    m_scale = size / m_font.fontHeight;
}

float HCPFontRenderer::getTextSize() const
{
    return m_textSize;
}

void HCPFontRenderer::setAnchor(HCPAlignment anchorEnum)
{
    m_anchor = anchorEnum;
}

HCPAlignment HCPFontRenderer::getAnchor() const
{
    return m_anchor;
}

void HCPFontRenderer::genString(HCPMeshBuilder& meshBuilder, const char* str, size_t strLen, float x, float y, uint32_t color)
{
    genString(meshBuilder, str, strLen, x, y, getVec4Color(color));
}

void HCPFontRenderer::genString(HCPMeshBuilder& meshBuilder, const char* text, float x, float y, uint32_t color)
{
    genString(meshBuilder, text, strlen(text), x, y, color);
}

void HCPFontRenderer::genString(HCPMeshBuilder& meshBuilder, const char* text, float x, float y, const glm::vec4& color)
{
    genString(meshBuilder, text, strlen(text), x, y, color);
}

void HCPFontRenderer::genString(HCPMeshBuilder& meshBuilder, const char* str, size_t strLen, float x, float y, const glm::vec4& color)
{
    glm::vec2 anch = anchor(str, strLen, x, y);
    x = anch.x;
    y = anch.y;

    float xCursor = x;

    bool bold = false;
    bool strikethrough = false;
    bool underline = false;
    bool italic = false;
    glm::vec4 charColor(1.0f);

    for(int i = 0; i < strLen;)
    {
        int bytesRead = 0;
        int unicode = getUnicodeFromUTF8((const uint8_t*) &str[i], &bytesRead);

        if(unicode == 167 && i + bytesRead < strLen)
        {
            char key[] = { (char) tolower((int) str[i + bytesRead]), (char) 0 };
            int formatKey = (int) (strstr(m_FORMATTING_KEYS, key) - m_FORMATTING_KEYS);

            if(formatKey < 16)
            {
                if(formatKey < 0) formatKey = 15;

                strikethrough = false;
                underline = false;
                italic = false;
                bold = false;
                charColor = m_COLOR_CODES[formatKey];
            }
            else
            {
                switch(formatKey)
                {
                case 17:
                    bold = true;
                    break;
                case 18:
                    strikethrough = true;
                    break;
                case 19:
                    underline = true;
                    break;
                case 20:
                    italic = true;
                    break;
                case 21:
                    bold = false;
                    strikethrough = false;
                    underline = false;
                    italic = false;
                    charColor = glm::vec4(1.0f);
                    break;
                }
            }

            i++;
        }
        else
        {
            Glyph glyph = m_font.glyphs[unicode];

            float advance = (glyph.xAdvance - m_font.leftPadding - m_font.rightPadding) * m_scale;
            float italics = italic ? m_textSize * 0.07f : 0;
            glm::vec4 mixedColor = color * charColor;

            genChar(meshBuilder, unicode, xCursor, y, italics, bold, mixedColor);
            if(strikethrough)
            {
                hcpui::genHorizontalLine(y + m_textSize / 2, xCursor, xCursor + advance, mixedColor, m_textSize * 0.09f);
            }
            if(underline)
            {
                hcpui::genHorizontalLine(y + m_textSize * 1.05f, xCursor, xCursor + advance, mixedColor, m_textSize * 0.09f);
            }

            xCursor += advance;
        }

        i += bytesRead;
    }
}

int HCPFontRenderer::charsToFit(const char* str, float width) const
{
    return charsToFit(str, strlen(str), width);
}

int HCPFontRenderer::charsToFit(const char* str, size_t strLen, float width) const
{
    int chars = 0;
    float xCursor = 0;

    for(int i = 0; i < strLen;)
    {
        int bytesRead = 0;
        int unicode = getUnicodeFromUTF8((const uint8_t*) &str[i], &bytesRead);

        if(unicode == 167 && i + bytesRead < strLen)
        {
            i++;
        }
        else
        {
            Glyph glyph = m_font.glyphs[unicode];

            float advance = (glyph.xAdvance - m_font.leftPadding - m_font.rightPadding) * m_scale;

            if(xCursor + advance > width)
            {
                return chars;
            }

            xCursor += advance;
            chars++;
        }

        i += bytesRead;
    }

    return chars;
}

void HCPFontRenderer::bindAtlas()
{
    if(!m_isRenderable)
    {
        initToRender();
    }

    glActiveTexture(GL_TEXTURE0 + m_texUnit - 1);
    glBindTexture(GL_TEXTURE_2D, m_glAtlasTex);
    glActiveTexture(GL_TEXTURE0);
}

void HCPFontRenderer::setAtlasTexUnit(int texUnit)
{
    m_texUnit = texUnit;
}

int HCPFontRenderer::getTexUnit() const
{
    return m_texUnit;
}

int HCPFontRenderer::getUnicodeFromUTF8(const uint8_t* str, int* bytesRead) const
{
    int unicode = 0;
    *bytesRead = 1;

    if(*str < 128) return *str;
    else if((*str & 224) == 192) * bytesRead = 2;
    else if((*str & 240) == 224) * bytesRead = 3;
    else if((*str & 248) == 240) * bytesRead = 4;
    else return *str;

    switch(*bytesRead)
    {
        case 2:
            unicode |= ((int) str[0] & 31) << 6;
            unicode |= str[1] & 63;
            break;
        case 3:
            unicode |= ((int) str[0] & 15) << 12;
            unicode |= ((int) str[1] & 63) << 6;
            unicode |= str[2] & 63;
            break;
        case 4:
            unicode |= ((int) str[0] & 8) << 18;
            unicode |= ((int) str[1] & 63) << 12;
            unicode |= ((int) str[2] & 63) << 6;
            unicode |= str[3] & 63;
    }

    return unicode;
}

void HCPFontRenderer::genChar(HCPMeshBuilder& meshBuilder, int unicode, float x, float y, float italics, bool bold, glm::vec4& color)
{
    if(256 <= unicode) // Cannot handle unicodes more than 256 at the moment
    {
        return;
    }

    int texUnit = m_texUnit + bold;
    Glyph glyph = m_font.glyphs[unicode];

    float left = (float) glyph.xOffset;
    float right = (float) glyph.xOffset + glyph.width;
    float top = (float) glyph.yOffset;
    float bottom = (float) glyph.yOffset + glyph.height;
    left *= m_scale; right *= m_scale;
    top *= m_scale; bottom *= m_scale;
    left += x; right += x;
    top += y; bottom += y;

    meshBuilder.index(6, 0, 1, 2, 0, 2, 3);
    meshBuilder.vertex(NULL, left  - italics, bottom, 0.0f, glyph.uvLeft , glyph.uvBottom, vec4Color(color), texUnit);
    meshBuilder.vertex(NULL, right - italics, bottom, 0.0f, glyph.uvRight, glyph.uvBottom, vec4Color(color), texUnit);
    meshBuilder.vertex(NULL, right + italics, top   , 0.0f, glyph.uvRight, glyph.uvTop   , vec4Color(color), texUnit);
    meshBuilder.vertex(NULL, left  + italics, top   , 0.0f, glyph.uvLeft,  glyph.uvTop   , vec4Color(color), texUnit);
}

glm::vec2 HCPFontRenderer::anchor(const char* str, size_t strLen, float x, float y)
{
    glm::vec2 newPos;
    float stringWidth = getStringWidth(str, strLen);
    float stringHeight = m_textSize;

    switch(m_anchor)
    {
    case HCPAlignment::CENTER_LEFT:
    case HCPAlignment::CENTER:
    case HCPAlignment::CENTER_RIGHT:
        newPos.y = y - stringHeight / 2.0f;
        break;
    case HCPAlignment::BOTTOM_LEFT:
    case HCPAlignment::BOTTOM_CENTER:
    case HCPAlignment::BOTTOM_RIGHT:
        newPos.y = y - stringHeight;
        break;
    default:
        newPos.y = y;
    }

    switch(m_anchor)
    {
    case HCPAlignment::TOP_CENTER:
    case HCPAlignment::CENTER:
    case HCPAlignment::BOTTOM_CENTER:
        newPos.x = x - stringWidth / 2.0f;
        break;
    case HCPAlignment::TOP_RIGHT:
    case HCPAlignment::CENTER_RIGHT:
    case HCPAlignment::BOTTOM_RIGHT:
        newPos.x = x - stringWidth;
        break;
    default:
        newPos.x = x;
    }

    return newPos;
}

void HCPFontRenderer::initToRender()
{
    int x, y, channels;
    stbi_set_flip_vertically_on_load(true);

    stbi_uc* image = stbi_load_from_memory(m_font.atlas, (int) m_font.atlasLen, &x, &y, &channels, STBI_grey_alpha);
    m_atlasWidth = (float) x;
    m_atlasHeight = (float) y;

    if(!image)
    {
        m_logger.errorf("Failed to load texture of font \"%s\".", m_font.name);
        m_isRenderable = true;
    }

    glGenTextures(1, &m_glAtlasTex);

    glBindTexture(GL_TEXTURE_2D, m_glAtlasTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, x, y, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(image);
    m_logger.infof("Loaded font texture of font \"%s\".", m_font.name);
    m_isRenderable = true;

    // Precalculate all uv mappings for each glyph
    for(size_t i = 0; i < m_font.numGlyphs; i++)
    {
        const Glyph& glyph = m_font.glyphs[i];

        glyph.uvLeft = glyph.x / m_atlasWidth;
        glyph.uvRight = (glyph.x + glyph.width) / m_atlasWidth;
        glyph.uvTop = 1.0f - (glyph.y / m_atlasHeight);
        glyph.uvBottom = 1.0f - ((glyph.y + glyph.height) / m_atlasHeight);
    }
}