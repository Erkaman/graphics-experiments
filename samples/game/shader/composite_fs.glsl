in vec2 texCoord;


out vec4 fragmentColor;

uniform sampler2D particlesTexture;

void main() {

    fragmentColor = vec4( texture(particlesTexture, texCoord).xyz ,1);
}
