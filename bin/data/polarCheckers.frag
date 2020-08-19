#define PI 3.14159

vec2 fragCoord = gl_FragCoord.xy;

uniform float iTime;
uniform vec2 iResolution;

in vec2 texCoordVarying;

out vec4 outputColor;

float posSin(float x) {
    return sin(x) / 2. + .5;
}

float Circle(vec2 p, float r) {
    float d = length(p) - r;
    d = smoothstep(.001, .0, d);
    return d;
}

float Rect(vec2 p, vec2 s) {
    vec2 sH = s / 2.;
    float d = distance(p, vec2(
        clamp(p.x, -sH.x, sH.x),
        clamp(p.y, -sH.y, sH.y)
    ));
    d = smoothstep(.001, .0, d);
    return d;
}

mat2 Rot(float a) {
    float s = sin(a);
    float c = cos(a);
    return mat2(c, -s, s, c);
}

vec2 LogPolar(vec2 uv) {
	return vec2(log(length(uv)), atan(uv.y, uv.x));
}

void main() {
    //vec2 fragCoord = gl_FragCoord.xy;
    vec2 uv = (fragCoord.xy - iResolution * .5) / iResolution.y;
    // Flip Y coordinates, because we want zero to be in the bottom left corner of the screen
    uv.y *= -1.;
    float t = iTime;

    vec3 col = vec3(0);

	uv = fract(abs(uv));
	//uv.x += t;
	uv *= 6. / PI;
	uv = LogPolar(uv);
	uv.y *= .1 + posSin(2.);
	uv.y += t * .2;
	uv.x -= t * .4;

	//uv *= Rot(t * .1 + posSin(t * .4));
	//uv += t * .1;
	//uv *= 1. + 4. * posSin(t * .3);
	//uv = abs(uv);

    // float r = .2;
    // vec2 p = uv - vec2(sin(t * 2.) * r, cos(t) * r);
    // p *= rot(t);
    // float d = Circle(p, .1 + sin(t) * .2);
    // d = Rect(p, vec2(.1 + (.5 + .5 * sin(t)) * .2));

    float scale = 1. + posSin(t) * 4.;//3.;
    vec2 sUv = uv * 3.;
    vec2 gUv = fract(sUv) - .5;
    vec2 id = floor(sUv);
	gUv *= (1.1 + (.2 + sin(t) * .4)) - posSin(t + length(id));

    mat2 rot = Rot(PI * .25 * t * .2 + length(id) * length(id));
    vec2 p = vec2(gUv);// * rot;
    float d = Rect(p, vec2(.7));
    col += d * vec3(.9, .5, 0.);
	//col += -dot(id.x, id.y);
    // col.rg += gUv;

    gUv *= 7.;
    id = floor(gUv);
    //gUv = abs(gUv);
    gUv = fract(gUv) - .5;
    p = vec2(gUv);
    p *= Rot(PI * -.25 * t - length(id) * length(id));
    d = Rect(p, vec2(.7));
    col -= d;// * vec3(normalize(length(id)));
	//col = col * -1. + 1.;

    outputColor = vec4(col, 1.);
}
