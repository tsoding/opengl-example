#version 130

in vec2 texcoord;
uniform sampler2D tex;
uniform vec2 position;
uniform vec2 direction;

#define RADIUS 100.0f
#define TRAIL_COUNT 5
#define TRAIL_DIST RADIUS
#define TRAIL_RADIUS_DEC 20.0f

void main() {
    float background_brighness = 0.1f;
    // gl_FragColor = mix(texture(tex, texcoord), vec4(1.0f, 0.0f, 0.0f, 1.0f), 0.25f);

    gl_FragColor = vec4(background_brighness,
                        background_brighness,
                        background_brighness,
                        1.0f);

    for (int i = 0; i < TRAIL_COUNT; ++i) {
        float radius = RADIUS - TRAIL_RADIUS_DEC * i;
        vec2 actual_position = position - normalize(direction) * (i * TRAIL_DIST);

        if (distance(gl_FragCoord.xy, actual_position) < radius) {
            gl_FragColor = vec4(0.5f, 1.0f, 0.5f, 1.0f);
            return;
        }
    }
}
