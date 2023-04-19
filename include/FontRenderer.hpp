#ifndef HCPFONT_RENDERER_HPP
#define HCPFONT_RENDERER_HPP

#include "MeshBuilder.hpp"

#include <stdint.h>
#include <glm/glm.hpp>

// Anchor enums
enum HCPAlignment
{
    TOP_LEFT,
    TOP_CENTER,
    TOP_RIGHT,
    CENTER_LEFT,
    CENTER,
    CENTER_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_CENTER,
    BOTTOM_RIGHT
};

class HCPFontRenderer
{
public:
    struct Glyph
    {
        int x, y, width, height, xOffset, yOffset, xAdvance;
        mutable float uvLeft = 0.0f, uvTop = 0.0f, uvRight = 0.0f, uvBottom = 0.0f;
    };

    struct Font
    {
        const char* name;
        int fontHeight;
        int lineHeight;

        int leftPadding;
        int rightPadding;
        int topPadding;
        int bottomPadding;

        const Glyph* glyphs;
        const uint8_t* atlas;
        size_t atlasLen;
        int numGlyphs;
    };

    HCPFontRenderer();
    ~HCPFontRenderer();

    HCPFontRenderer(const HCPFontRenderer& copy) = delete;

    const char* getFontName() const;
    float getFontHeight() const;
    float getFontLineHeight() const;

    float getStringWidth(const char* text) const;
    float getStringWidth(const char* text, size_t strLen) const;
    float getStringHeight() const;

    void setTextSize(float size);
    float getTextSize() const;

    void setAnchor(HCPAlignment anchorEnum);
    HCPAlignment getAnchor() const;

    void genString(HCPMeshBuilder& meshBuilder, const char* str, size_t strLen, float x, float y, uint32_t color);
    void genString(HCPMeshBuilder& meshBuilder, const char* str, float x, float y, uint32_t color);
    void genString(HCPMeshBuilder& meshBuilder, const char* str, float x, float y, const glm::vec4& color);
    void genString(HCPMeshBuilder& meshBuilder, const char* str, size_t strLen, float x, float y, const glm::vec4& color);

    void bindAtlas();

    void setAtlasTexUnit(int texUnit);
    int getTexUnit() const;
private:
    bool m_isRenderable;
    GLuint m_glAtlasTex;
    float m_atlasWidth = 0;
    float m_atlasHeight = 0;

    Font m_font;

    float m_scale;
    float m_textSize;

    HCPAlignment m_anchor;
    int m_texUnit;

    int getUnicodeFromUTF8(const uint8_t* str, int* bytesRead) const;
    void genChar(HCPMeshBuilder& meshBuilder, int unicode, float x, float y, float italics, bool bold, glm::vec4& color);
    glm::vec2 anchor(const char* str, size_t strLen, float x, float y);
    void initToRender();
};

#endif // HCPFONT_RENDERER_HPP