uniform float iTime;
uniform vec2 iResolution;

in vec2 texCoordVarying;

out vec4 outputColor;

float posSin(float x) {
    return sin(x) / 2. + .5;
}

void main() {
    vec2 fragCoord = gl_FragCoord.xy;
    vec2 uv = (fragCoord.xy - iResolution * .5) / iResolution.y;
    // Flip Y coordinates, because we want zero to be in the bottom left corner of the screen
    uv.y *= -1.;
    float t = iTime;

    vec3 col = vec3(0);

    float r = .2;
    vec2 p = uv - vec2(sin(t * 2.) * r, cos(t) * r);
    float d = length(p) - (.1 + sin(t) * .2);
    d = smoothstep(.01, .0, d);
    col += d * vec3(.9, .2, 5.);

    outputColor = vec4(col, 1.);
}