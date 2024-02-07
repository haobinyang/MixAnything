#ifndef UTILS_MESH_H
#define UTILS_MESH_H

#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Attribute.h>
#include "../structs/mesh.h"

using namespace Magnum;
using namespace Math::Literals;

static GL::Mesh createQuadMesh(
  const Vector2& size,
  GL::Attribute<0, Vector2> position,
  GL::Attribute<1, Vector2> textureCoordinates
) {
  const QuadVertex vertices[]{
    {{size.x(), size.y()}, {.0f, .0f}}, /* Bottom right */
    {{size.x(), 0.0f}, {.0f, 1.0f}}, /* Top right */
    {{0.0f, size.y()}, {1.0f, 0.0f}}, /* Bottom left */
    {{0.0f, 0.0f}, {1.0f, 1.0f}}  /* Top left */
  };
  const UnsignedInt indices[]{      /* 3--1 1 */
    0, 1, 2,                        /* | / /| */
    2, 1, 3                         /* |/ / | */
  };                                /* 2 2--0 */
  GL::Mesh mesh;
  mesh.setCount(Containers::arraySize(indices))
    .addVertexBuffer(GL::Buffer{vertices}, 0, position, textureCoordinates)
    .setIndexBuffer(GL::Buffer{indices}, 0, GL::MeshIndexType::UnsignedInt);
  return mesh;
};

#endif