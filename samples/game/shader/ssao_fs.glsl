in vec2 texCoord;

out vec4 fragmentColor;

uniform sampler2D colorTexture;


void main() {

    vec4 color = texture(colorTexture, texCoord);

    fragmentColor = vec4(color.xyz,1);
}
