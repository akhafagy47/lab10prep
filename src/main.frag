#version 120 // glsl version 1.2 for OpenGL 2.1

uniform vec2 windowSize; // in pixels
uniform vec2 mousePos;   // in window coords (origin at bottom-left)
uniform float time;      // milliseconds since program start

const float CIRCLE_RADIUS = 10.0;
const float SCROLL_SPEED = 0.0001; // gradient periods per millisecond

void main(void) {
    // normalized [0, 1]^2 position of this fragment in the window:
    vec2 uv = gl_FragCoord.xy / windowSize;
    // animation: scroll the gradient horizontally. Ping-pong (triangle wave) the x coordinate so the
    // gradient mirrors instead of jumping, which keeps it seamless. At time 0, u == uv.x.
    float u = fract(uv.x * 0.5 + time * SCROLL_SPEED) * 2.0;
    u = 1.0 - abs(u - 1.0);
    // bilinear gradient: black (0,0), red (1,0), green (0,1), yellow (1,1).
    vec4 background = vec4(u, uv.y, 0.0, 1.0);
    // white circle around the mouse; use step + mix instead of an if to avoid warp divergence.
    float inCircle = step(distance(gl_FragCoord.xy, mousePos), CIRCLE_RADIUS);
    gl_FragColor = mix(background, vec4(1.0), inCircle);
}
