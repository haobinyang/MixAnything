uniform mat4 camera;
uniform mat4 translation;
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 textureCoordinates;

out vec2 interpolatedTextureCoordinates;

void main() {
    interpolatedTextureCoordinates = textureCoordinates;

    gl_Position = camera * translation * position;
}