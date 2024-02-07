#ifndef CLIP_IMAGE_H
#define CLIP_IMAGE_H

#include <Corrade/Containers/StringView.h>
#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/Trade/ImageData.h>
#include <Corrade/Containers/Optional.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include "base.h"
#include "../structs/clip.h"
#include "../utils/network.h"
#include "../utils/shader.h"
#include "../utils/mesh.h"
#include "../enum/common.h"
#include "../shaders/effects/gray.h"

using namespace Magnum;
using namespace Math::Literals;

class ImageClip: public BaseClip {
  public:
    explicit ImageClip(const ImageConfig& config): BaseClip(config.meshConfig), _config(config) {
      _texture = createTexture(loadImageFromUrl(_config.url));
    }

    ~ImageClip() {
    }

    void render(UnsignedInt currentTime) {
      if (_config.effect == Effect::Gray) {
        auto effect = GrayEffect(_texture);
        Debug{} << effect.getTexture().imageSize(0);
        // GL::defaultFramebuffer.bind();
        // setUniform(uniformLocation("textureData"), TextureUnit);
        setUniform(_textureUniform, TextureUnit);
        effect.getTexture().bind(TextureUnit);
        // setUniform(_textureUniform, TextureUnit);
        // _texture.bind(TextureUnit);
      } else {
        _texture.bind(TextureUnit);
      }
      draw(_quad);
    }

  private:
    ImageConfig _config;
    GL::Texture2D _texture;
};

#endif
