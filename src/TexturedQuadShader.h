#ifndef Magnum_Examples_TexturedQuad_TexturedQuadShader_h
#define Magnum_Examples_TexturedQuad_TexturedQuadShader_h

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix4.h>
#include <Corrade/Containers/Iterable.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/Containers/StringStl.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include "third-part/json.hpp"

using namespace Magnum;

class TexturedQuadShader: public GL::AbstractShaderProgram {
    public:
        typedef GL::Attribute<0, Vector2> Position;
        typedef GL::Attribute<1, Vector2> TextureCoordinates;

        explicit TexturedQuadShader() {
            MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL330);

            const Utility::Resource rs{"texturedquad-data"};

            GL::Shader vert{GL::Version::GL330, GL::Shader::Type::Vertex};
            GL::Shader frag{GL::Version::GL330, GL::Shader::Type::Fragment};

            vert.addSource(rs.getString("TexturedQuadShader.vert"));
            frag.addSource(rs.getString("TexturedQuadShader.frag"));

            CORRADE_INTERNAL_ASSERT_OUTPUT(vert.compile() && frag.compile());

            attachShaders({vert, frag});

            CORRADE_INTERNAL_ASSERT_OUTPUT(link());

            _colorUniform = uniformLocation("color");
            _cameraUniform = uniformLocation("camera");
            _translationUniform = uniformLocation("translation");
            _textureData = uniformLocation("textureData");
        };

        TexturedQuadShader& setColor(const Color3& color) {
            setUniform(_colorUniform, color);
            return *this;
        }

        TexturedQuadShader& bindTexture(GL::Texture2D& texture) {
            setUniform(_textureData, TextureUnit);
            texture.bind(TextureUnit);
            return *this;
        }

        TexturedQuadShader& setCamera(Matrix4& camera) {
            setUniform(_cameraUniform, camera);
            return *this;
        }

        TexturedQuadShader& setTranslation(Matrix4& translation) {
            setUniform(_translationUniform, translation);
            return *this;
        }

    private:
        enum: Int { TextureUnit = 0 };

        Int _colorUniform;
        Int _cameraUniform;
        Int _translationUniform;
        Int _textureData;
};

#endif
