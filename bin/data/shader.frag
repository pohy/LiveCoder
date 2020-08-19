//#define PI 3.14159

uniform float iTime;
uniform vec2 iResolution;
uniform float iMidi0;
uniform float iMidi1;

in vec2 texCoordVarying;

out vec4 outputColor;

float posSin(float x) {
    return sin(x) / 2. + .5;
}

float Rect(vec2 p, vec2 s) {
    vec2 sH = s * .5;
    float d = distance(p, vec2(clamp(p.x, -sH.x, sH.y), clamp(p.y, -sH.y, sH.y)));
    d = smoothstep(.001, .0, d);
    return d;
}

mat2 Rot(float a) {
    float s = sin(a);
    float c = cos(a);
    return mat2(c, -s, s, c);
}

void main() {
    vec2 fragCoord = gl_FragCoord.xy;
    vec2 uv = (fragCoord.xy - iResolution * .5) / iResolution.y;
    // Flip Y coordinates, because we want zero to be in the bottom left corner of the screen
    uv.y *= -1.;
    float t = iTime * iMidi0;

    vec3 col = vec3(0);

    // float r = .2;
    // vec2 p = uv - vec2(sin(t * 2.) * r, cos(t) * r);
    // float d = length(p) - (.1 + sin(t) * .2);
    // d = smoothstep(.01, .0, d);
    // col += d * vec3(.9, .2, 5.);
    float d = Rect(uv * Rot(t) * (.1 + (1. - iMidi1) * 2.), vec2(.25));
    col += d;

    outputColor = vec4(col, 1.);
}
