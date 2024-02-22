#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/ImageView.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Math/Matrix4.h>
#include <iostream>

#include "TexturedQuadShader.h"
#include "clips/image.h"
#include "clips/video.h"
#include "third-part/json.hpp"
#include "utils/mesh.h"
#include "utils/network.h"
#include "shaders/effects/none.h"
#include "shaders/effects/gray.h"
#include "enum/common.h"
#include "structs/clip.h"
#include "helpers/video.h"

extern "C" {
    #include <libavcodec/avcodec.h>
}

using namespace Magnum;
using json = nlohmann::json;

class MyApplication: public Platform::Application {
    public:
        explicit MyApplication(const Arguments& arguments);

    private:
        void drawEvent() override;

        GL::Mesh _mesh;
        TexturedQuadShader _shader;
        GL::Texture2D _texture;
};

MyApplication::MyApplication(const Arguments& arguments): Platform::Application{arguments, Configuration{}.setSize({800, 600})} {
    using namespace Math::Literals;
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

    ImageConfig imageConfig = {
        {256, 256, 100, 100, 0.0f},
        {0, 0},
        Effect::None,
        0,
        "http://cos.chuangyi.qq.com/53709c9fea8bf1f108bf1e55e6a0a778834a0be4"
    };
    ImageConfig imageConfig2 = {
        {256, 256, 400, 150, 0.0f},
        {0, 0},
        Effect::Gray,
        0,
        "http://cos.chuangyi.qq.com/59f9bb054889af664674b175663d478a24a13c4c"
    };

    ImageClip imageClip(imageConfig);
    ImageClip imageClip2(imageConfig2);

    VideoConfig videoConfig = {
        {320, 180, 250, 100, 0.0f},
        {0, 0},
        Effect::None,
        0,
        "https://wxsnsencsvp.wxs.qq.com/130/20210/snssvpdownload/SH/reserved/yO1laMeugEczXO7icpmLNMPFxXqqCGcxSDUmx1vUhkxcrDC9udVwb6w7SWT20WiaqAm4gCcFjXr1CbibiaUMYe2tlTmeEamhDoia0?dis_k=1fdf827957fd11cceed08e2b39796aed&dis_t=1707212832&idx=1&m=d2a12d42a32b457547ea1bf293cce8a3&sha256=42dff61eaea6ba5735ee6cd90f117519918cf968d0cecd724b2338f1ecc8cc21&token=6xykWLEnztKQv2XKD7VLib1SCnUVPrS1P323hiaqYXlb9ibbpykuKjlfO93urtHkTyZqDtItITu6aMiaCz28xRlc1w"
    };
    VideoClip videoClip(videoConfig);

    _mesh = createQuadMesh({256.f,256.f}, TexturedQuadShader::Position{}, TexturedQuadShader::TextureCoordinates{});
    auto image = loadImageFromUrl("http://cos.chuangyi.qq.com/53709c9fea8bf1f108bf1e55e6a0a778834a0be4");
    CORRADE_INTERNAL_ASSERT(image);
    _texture = createTexture(image);

    Matrix4 orthographicCamera = Matrix4::orthographicProjection({0.0f, 600.0f}, {800.0f, 0.0f}, 0.0f, 1.0f);
    Matrix4 translate = Matrix4::translation({10.0f,50.0f, 0.0f});
    auto effect = GrayEffect(_texture);

    _shader
        .setColor(0xffb2b2_rgbf)
        .setCamera(orthographicCamera)
        .setTranslation(translate)
        .bindTexture(effect.getTexture())
        .draw(_mesh);

    imageClip.render(0);
    imageClip2.render(0);
    videoClip.render(0);

    swapBuffers();
}

void MyApplication::drawEvent() {
    // GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

    // using namespace Math::Literals;

    // Matrix4 orthographicCamera = Matrix4::orthographicProjection({0.0f, 600.0f}, {800.0f, 0.0f}, 0.0f, 1.0f);
    // Matrix4 translate = Matrix4::translation({10.0f,50.0f, 0.0f});
    // auto effect = GrayEffect(_texture);

    // _shader
    //     .setColor(0xffb2b2_rgbf)
    //     .setCamera(orthographicCamera)
    //     .setTranslation(translate)
    //     .bindTexture(effect.getTexture())
    //     .draw(_mesh);

    // swapBuffers();
    // redraw();
}

MAGNUM_APPLICATION_MAIN(MyApplication)
