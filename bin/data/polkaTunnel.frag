#define PI 3.14159

float getFreq(float freq) {
    int tx = int(freq / 22000. * 512.);
    return texelFetch(iChannel0, ivec2(tx, 0), 0).r;
}

float getBand(float freq, float width) {
    float avg = 0.;
    float it = width / freq;
    for (float i = 0.; i < it; i++) {
        avg += getFreq(freq * i);
    }
    return avg;
}

vec2 toLogPolar(vec2 uv) {
    return vec2(log(length(uv)), atan(uv.y, uv.x));
}

mat2 rot(float a) {
    float s = sin(a);
    float c = cos(a);
    return mat2(c, -s, s, c);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
    vec2 uv = (fragCoord - iResolution.xy * .5) / iResolution.y;
    vec2 mouse = iMouse.xy / iResolution.xy;
    float t = iTime * .1;
    vec3 col = vec3(0);
    
    float snare = getFreq(5000.);
    
    uv *= 6. + (sin(t) * 3.);
    uv = fract(abs(uv));
    uv = toLogPolar(uv);
    uv.x -= t + snare * .1;
    uv *= 6. / PI;
    uv = fract(uv) - .5;
    
    float r = .25 + getFreq(80.) * .2;
    float dist = length(uv);
    float delta = fwidth(dist);
    float c = smoothstep(r * 1.01, r - delta, dist);
    vec3 color = mix(vec3(.8, .8, .5), vec3(.2, .2, .8), snare);
    col += c;//vec3(.8 + (snare * .2), .8 - (snare * .2), .5 - (snare * .2));
    //col.rg = id;
    //col += getFreq(60.);
    //col.rg += uv;

    fragColor = vec4(col,1.0);
}
