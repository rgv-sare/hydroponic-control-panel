#include "Images.hpp"

#include <Logger.hpp>

#include <stb_image.h>

static HCPLogger i_imgLogger("Image");

static HCPImagePtr i_nullImage = std::make_shared<HCPImage>();

HCPImage::~HCPImage()
{
    if(m_id != 0xFFFFFFFF)
        glDeleteTextures(1, &m_id);
}

GLuint HCPImage::getID()
{
    return m_id;
}

int HCPImage::getWidth()
{
    return m_width;
}

int HCPImage::getHeight()
{
    return m_height;
}

void HCPImage::bindTexture(int texUnit)
{
    if(m_id == 0xFFFFFFFF)
        return;

    glActiveTexture(GL_TEXTURE0 + texUnit);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

HCPImage::HCPImage() :
    m_width(0),
    m_height(0),
    m_id(0xFFFFFFFF)
{
}

HCPImagePtr hcpimg::loadImage(const char* path)
{
    stbi_set_flip_vertically_on_load(true);

    int width, height, channels;
    unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
    if (!data)
    {
        i_imgLogger.errorf("Failed to load image: %s", path);
        return i_nullImage;
    }

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (channels == 3)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }
    else if (channels == 4)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    else
    {
        i_imgLogger.errorf("Unsupported number of channels: %d", channels);
        stbi_image_free(data);
        return i_nullImage;
    }

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    HCPImagePtr img = std::make_shared<HCPImage>();
    img->m_width = width;
    img->m_height = height;
    img->m_id = id;

    i_imgLogger.infof("Loaded image: %s %dx%d", path, width, height);

    return img;
}

HCPImagePtr hcpimg::loadImageFromMemory(const char* data, int size)
{
    stbi_set_flip_vertically_on_load(true);

    int width, height, channels;
    unsigned char* imgData = stbi_load_from_memory((const stbi_uc*)data, size, &width, &height, &channels, 0);
    if (!imgData)
    {
        i_imgLogger.errorf("Failed to load image from memory");
        return i_nullImage;
    }

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (channels == 3)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imgData);
    }
    else if (channels == 4)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);
    }
    else
    {
        i_imgLogger.errorf("Unsupported number of channels: %d", channels);
        stbi_image_free(imgData);
        return i_nullImage;
    }

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(imgData);

    HCPImagePtr img = std::make_shared<HCPImage>();
    img->m_width = width;
    img->m_height = height;
    img->m_id = id;

    i_imgLogger.infof("Loaded image from memory: %dx%d", width, height);

    return img;
}