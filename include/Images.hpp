#ifndef HCP_IMAGES_HPP
#define HCP_IMAGES_HPP

#include "GLInclude.hpp"

#include <memory>

class HCPImage
{
public:
    HCPImage();
    ~HCPImage();

    GLuint getID() const;
    int getWidth() const;
    int getHeight() const;

    void bindTexture(int texUnit = 0) const;
private:
    int m_width;
    int m_height;
    GLuint m_id;

    friend class hcpimg;
};

typedef std::shared_ptr<HCPImage> HCPImagePtr;

class hcpimg
{
public:
    static HCPImagePtr loadImage(const char* path);
    static HCPImagePtr loadImageFromMemory(const char* data, int size);
};

#endif // HCP_IMAGES_HPP