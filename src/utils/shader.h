#ifndef UTILS_SHADER_H
#define UTILS_SHADER_H

#include <Magnum/Trade/ImageData.h>
#include <Corrade/Containers/StringView.h>
#include <Magnum/GL/Version.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Texture.h>
#include <utility>

using namespace Magnum;

static std::pair<GL::Shader, GL::Shader> compileShaders(
  const Corrade::Containers::StringView& vertexShaderSource,
  const Corrade::Containers::StringView& fragmentShaderSource
) {
  MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL330);

  GL::Shader vert{GL::Version::GL330, GL::Shader::Type::Vertex};
  GL::Shader frag{GL::Version::GL330, GL::Shader::Type::Fragment};

  vert.addSource(vertexShaderSource);
  frag.addSource(fragmentShaderSource);

  CORRADE_INTERNAL_ASSERT_OUTPUT(vert.compile() && frag.compile());

  return std::make_pair(std::move(vert), std::move(frag));
}

static GL::Texture2D createTexture(const Containers::Optional<Trade::ImageData2D>& imageData) {
  GL::Texture2D texture;
  texture.setWrapping(GL::SamplerWrapping::ClampToEdge)
    .setMagnificationFilter(GL::SamplerFilter::Linear)
    .setMinificationFilter(GL::SamplerFilter::Linear)
    .setStorage(1, GL::textureFormat(imageData->format()), imageData->size())
    .setSubImage(0, {}, *imageData);
  return texture;
}

static GL::Texture2D createTexture(const Vector2i& size) {
  GL::Texture2D texture;
  texture.setWrapping(GL::SamplerWrapping::ClampToEdge)
    .setMagnificationFilter(GL::SamplerFilter::Linear)
    .setMinificationFilter(GL::SamplerFilter::Linear)
    .setStorage(1, GL::TextureFormat::RGBA8, size);
  return texture;
}

#endif