#ifndef EFFECT_GRAY_H
#define EFFECT_GRAY_H

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Range.h>
#include <Magnum/GL/Framebuffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/GL/Context.h>
#include <Corrade/Containers/StringView.h>
#include "../../utils/mesh.h"
#include "../../utils/shader.h"

using namespace Magnum;

class GrayEffect: public GL::AbstractShaderProgram {
  public:
    typedef GL::Attribute<0, Vector2> Position;
    typedef GL::Attribute<1, Vector2> TextureCoordinates;

    explicit GrayEffect(GL::Texture2D& texture) {
      Vector2 size(texture.imageSize(0));
      _inputTexture = &texture;
      _outputTexture = createTexture(texture.imageSize(0));
      _orthographicCamera = Matrix4::orthographicProjection({0.0f, size.y()}, {size.x(), 0.0f}, .0f, 1.0f);
      createShaders();
      _mesh = createQuadMesh(size, GrayEffect::Position{}, GrayEffect::TextureCoordinates{});
      renderToTexture();
    };

    GL::Texture2D& getTexture() {
      return _outputTexture;
    };

  private:
    enum: Int { TextureUnit = 0 };

    GL::Mesh _mesh;
    GL::Texture2D* _inputTexture;
    GL::Texture2D _outputTexture;
    Matrix4 _orthographicCamera;
    Int _textureUniform;

    std::string vertString = R"(
      uniform mat4 projectMatrix;
      layout(location = 0) in vec4 position;
      layout(location = 1) in vec2 textureCoordinates;
      out vec2 interpolatedTextureCoordinates;
      void main() {
        interpolatedTextureCoordinates = textureCoordinates;
        gl_Position = projectMatrix * position;
      }
    )";
    std::string fragString = R"(
      uniform sampler2D textureData;
      in vec2 interpolatedTextureCoordinates;
      out vec4 fragmentColor;
      void main() {
        vec4 color = texture(textureData, interpolatedTextureCoordinates);
        float gray = dot(color.rgb, vec3(0.299, 0.587, 0.114));
        fragmentColor = vec4(gray, gray, gray, color.a);
      }
    )";

    void renderToTexture() {
      GL::Framebuffer frameBuffer{{{}, _inputTexture->imageSize(0)}};
      frameBuffer.attachTexture(GL::Framebuffer::ColorAttachment{0}, _outputTexture, 0)
        .mapForDraw({{0, GL::Framebuffer::ColorAttachment{0}}})
        .bind();
      _inputTexture->bind(TextureUnit);
      setUniform(_textureUniform, TextureUnit);
      draw(_mesh);

      // 重新绑定默认FBO
      GL::defaultFramebuffer.bind();
    };

    void createShaders() {
      auto shaders = compileShaders(vertString, fragString);
      attachShaders({shaders.first, shaders.second});
      CORRADE_INTERNAL_ASSERT_OUTPUT(link());
      _textureUniform = uniformLocation("textureData");
      setUniform(uniformLocation("projectMatrix"), _orthographicCamera);
    }
};

#endif