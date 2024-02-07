uniform vec3 color = vec3(1.0, 1.0, 1.0);
uniform sampler2D textureData;

in vec2 interpolatedTextureCoordinates;

out vec4 fragmentColor;

void main() {
    vec2 flippedTexCoords = vec2(interpolatedTextureCoordinates.x, 1.0 - interpolatedTextureCoordinates.y);
    fragmentColor.rgb = texture(textureData, flippedTexCoords).rgb;
    fragmentColor.a = 1.0;
}