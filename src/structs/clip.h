#ifndef STRUCTS_H
#define STRUCTS_H

#include <Corrade/Containers/StringView.h>
#include <Magnum/GL/Texture.h>
#include "../enum/common.h"

using namespace Magnum;
using namespace Math::Literals;

struct MeshConfig {
  Int width;
  Int height;
  Int x;
  Int y;
  Float rotate;
};

struct TimelineConfig {
  Int start;
  Int end;
};

struct BaseConfig {
  MeshConfig meshConfig;
  TimelineConfig timelineConfig;
  Effect effect;
  Int zIndex;
};

struct ImageConfig: BaseConfig {
  Corrade::Containers::StringView url;

  ImageConfig(
    MeshConfig meshConfig,
    TimelineConfig timelineConfig,
    Effect effect,
    Int zIndex,
    Corrade::Containers::StringView url
  ): BaseConfig{meshConfig, timelineConfig, effect, zIndex}, url{url} {}
};

struct VideoConfig: BaseConfig {
  Corrade::Containers::StringView url;
};

#endif
