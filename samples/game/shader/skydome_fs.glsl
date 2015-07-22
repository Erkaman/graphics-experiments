out vec4 fragmentColor;

in vec3 texCoord;

uniform float delta;

// return 1 when x > 0, otherwise return 0.
float when_positive(float x) {
    return max(sign(x), 0);
}

vec3 make_clouds(vec3 tc) {
    float y = tc.y;


    // make sure y is not negative.
    y = y * when_positive(y);

    vec3 sky = mix(vec3(0.50f, 0.50f, 0.66f), vec3(0.0f, 0.15f, 0.66f), y);

    return sky;
}

void main(){
    fragmentColor = vec4(make_clouds(texCoord), 1);
}
