#version 120 // glsl version 1.2 for OpenGL 2.1

uniform sampler2D texture;
uniform vec2 textureSize; // in pixels

const int RADIUS = 2; // 5x5 box filter

void main() {
    vec2 uv = gl_TexCoord[0].xy;
    // size of one texel in uv coords:
    vec2 texel = 1.0 / textureSize;
    vec4 sum = vec4(0.0);
    for (int dy = -RADIUS; dy <= RADIUS; ++dy) {
        for (int dx = -RADIUS; dx <= RADIUS; ++dx) {
            // samples past the edge are clamped to the closest valid pixel by SFML.
            sum += texture2D(texture, uv + vec2(dx, dy) * texel);
        }
    }
    float width = float(2 * RADIUS + 1);
    gl_FragColor = sum / (width * width);
}
