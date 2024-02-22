#ifndef CLIP_VIDEO_H
#define CLIP_VIDEO_H

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
#include "../helpers/video.h"
#include <chrono>

using namespace Magnum;
using namespace Math::Literals;

class VideoClip: public BaseClip {
  public:
    explicit VideoClip(const VideoConfig& config): BaseClip(config.meshConfig), _config(config) {
      _video = new Video(config.url);
      std::vector<uint8_t> buffer = _video->getVideoFrameAtTime(60.0);
      Double time = 0.0;
      Double timeStep = 1.0 / _video->FrameRate();
      Debug{} << timeStep;
      Double duration = _video->Duration();
      auto start = std::chrono::high_resolution_clock::now();
      while (time < duration) {
        time += timeStep;
        auto start = std::chrono::high_resolution_clock::now();
        _video->getVideoFrameAtTime(time);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;
        // std::cout << diff.count() << " seconds\n" << std::endl;
      }
      auto end = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> diff = end - start;
      std::cout << "Program executed in " << diff.count() << " seconds\n" << std::endl;
      _texture = createTexture(buffer, _video->Width(), _video->Height());
    }

    ~VideoClip() {
      delete _video;
    }

    void render(UnsignedInt currentTime) {
      _texture.bind(TextureUnit);
      draw(_quad);
    }

  private:
    Video* _video = nullptr;
    VideoConfig _config;
    GL::Texture2D _texture;
};

#endif
