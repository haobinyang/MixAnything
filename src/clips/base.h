#ifndef CLIP_BASE_H
#define CLIP_BASE_H

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Attribute.h>
#include <Magnum/Math/Matrix4.h>
#include "../structs/clip.h"
#include "../structs/mesh.h"
#include "../utils/mesh.h"
#include "../utils/shader.h"

using namespace Magnum;
using namespace Math::Literals;

class BaseClip: public GL::AbstractShaderProgram {
  public:
    typedef GL::Attribute<0, Vector2> Position;
    typedef GL::Attribute<1, Vector2> TextureCoordinates;

    explicit BaseClip(MeshConfig config): _config(config) {
      Debug{} << config.width << config.height;
      _quad = createQuadMesh({static_cast<Float>(config.width), static_cast<Float>(config.height)}, BaseClip::Position{}, BaseClip::TextureCoordinates{});
      _projection = Matrix4::orthographicProjection({0.0f, 600.0f}, {800.0f, 0.0f}, .0f, 1.0f);
      _translation = Matrix4::translation({static_cast<Float>(config.x), static_cast<Float>(config.y), 0.0f});
      // _rotation = Matrix4::rotationZ(static_cast<Float>(config.rotate));
      createShaders();
    };
    virtual ~BaseClip() {};
    virtual void render(UnsignedInt currentTime) {};
    virtual void renderToTexture(UnsignedInt currentTime, GL::Texture2D& texture) {};

  protected:
    enum: Int { TextureUnit = 0 };

    MeshConfig _config;
    GL::Mesh _quad;
    Matrix4 _projection;
    Matrix4 _translation;
    Matrix4 _rotation;
    Int _textureUniform;

    std::string vertString = R"(
      uniform mat4 projection;
      uniform mat4 translation;
      uniform mat4 rotation;
      layout(location = 0) in vec4 position;
      layout(location = 1) in vec2 textureCoordinates;
      out vec2 interpolatedTextureCoordinates;
      void main() {
        interpolatedTextureCoordinates = textureCoordinates;
        gl_Position = projection * rotation * translation * position;
      }
    )";
    std::string fragString = R"(
      uniform sampler2D textureData;
      in vec2 interpolatedTextureCoordinates;
      out vec4 fragmentColor;
      void main() {
        fragmentColor = texture(textureData, interpolatedTextureCoordinates);
      }
    )";

    void createShaders() {
      auto shaders = compileShaders(vertString, fragString);
      attachShaders({shaders.first, shaders.second});
      CORRADE_INTERNAL_ASSERT_OUTPUT(link());
      _textureUniform = uniformLocation("textureData");
      // setUniform(uniformLocation("textureData"), TextureUnit);
      setUniform(uniformLocation("projection"), _projection);
      setUniform(uniformLocation("translation"), _translation);
      setUniform(uniformLocation("rotation"), _rotation);
    }
};

#endif
