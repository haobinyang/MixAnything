#ifndef STRUCTS_MESH_H
#define STRUCTS_MESH_H

#include <Magnum/GL/Texture.h>

using namespace Magnum;
using namespace Math::Literals;

struct QuadVertex {
  Vector2 position;
  Vector2 textureCoordinates;
};

#endif