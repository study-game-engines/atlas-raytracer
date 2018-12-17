#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include "../System.h"
#include "Texture.h"

class Texture2D : public Texture {

public:
    /**
     * Constructs a Texture2D object.
     * @param dataFormat
     * @param width
     * @param height
     * @param internalFormat
     * @param wrapping
     * @param filtering
     * @param anisotropicFiltering
     * @param generateMipMaps
     */
    Texture2D(GLenum dataType, int32_t width, int32_t height, int32_t sizedFormat,
              int32_t wrapping, int32_t filtering, bool anisotropicFiltering, bool generateMipMaps);


    Texture2D(string filename, bool colorSpaceConversion = true);

    void Bind(uint32_t unit);

    void Unbind();

    /**
     * Resizes the texture
     * @param width The new width of the texture.
     * @param height The new height of the texture.
     * @warning This results in a loss of texture data.
     */
    void Resize(int32_t width, int32_t height);

    void SetData(vector<uint8_t>& data);

    vector<uint8_t> GetData();

    void SaveToPNG(string filename);

protected:
    void ReserveStorage(int32_t mipCount);

};

#endif