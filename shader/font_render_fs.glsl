in vec2 texCoord;
uniform sampler2D tex;
uniform vec3 color;

out vec4 fragmentColor;

void main(void) {

    vec4 sample  = texture(tex, texCoord).rgba;

    fragmentColor  =vec4(1,0,0,1); //vec4(sample.rgb * color , sample.a);
}
