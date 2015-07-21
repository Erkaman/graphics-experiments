in vec2 texCoord;
uniform sampler2D font_tex;
uniform vec3 color;

out vec4 fragmentColor;

void main(void) {

    vec4 sample  = texture(font_tex, texCoord).rgba;

    float a = sample.a;

    sample = sample + vec4(1,1,1,0);

    sample.a = a;

    fragmentColor  =vec4(sample.rgb , sample.a);
}
