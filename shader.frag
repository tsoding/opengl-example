#version 130

in vec2 texcoord;
uniform sampler2D tex;
uniform vec2 position;
uniform vec2 direction;
uniform vec2 resolution;
uniform float dt;

#define RADIUS 100.0f
#define TRAIL_COUNT 2
#define TRAIL_DIST RADIUS
#define TRAIL_RADIUS_DEC 20.0f

void main() {
    float background_brighness = 0.1f;
    // gl_FragColor = mix(texture(tex, texcoord), vec4(1.0f, 0.0f, 0.0f, 1.0f), 0.25f);

    gl_FragColor = vec4(background_brighness,
                        background_brighness,
                        background_brighness,
                        1.0f);

    vec2 pos = position;
    vec2 dir = direction;
    int n = int(floor(TRAIL_DIST / length(direction) / dt));

    for (int i = 0; i < TRAIL_COUNT; ++i) {
        float radius = RADIUS - TRAIL_RADIUS_DEC * i;

        vec2 d = gl_FragCoord.xy - pos;
        
        if (dot(d, d) < radius * radius) {
            gl_FragColor = vec4(0.5f, 1.0f, 0.5f, 1.0f);
            return;
        }

        for (float j = 0; j < n; ++j) {
            if ((pos.x - RADIUS) <= 0.0f || (pos.x + RADIUS) >= resolution.x) dir.x = -dir.x;
            if ((pos.y - RADIUS) <= 0.0f || (pos.y + RADIUS) >= resolution.y) dir.y = -dir.y;
            pos -= dir * dt;
        }
    }
}
