in vec2 texCoord;
uniform sampler2D font_tex;
uniform vec3 color;

out vec4 fragmentColor;

void main(void) {

    vec4 sample  = texture(font_tex, texCoord).rgba;


    fragmentColor  =vec4(sample.rgb * color , sample.a);
}
