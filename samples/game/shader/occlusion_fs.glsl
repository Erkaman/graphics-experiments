in vec2 texCoord;

out vec4 fragmentColor;



uniform sampler2D positionFbo;
uniform sampler2D vertexPosTexture;

//uniform sampler2D uSource;
//uniform sampler2D uVertex;
//uniform float uCount;
//uniform float uBias;
//uniform vec2 uRes;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {

    vec3 vert = texture(vertexPosTexture, texCoord).rgb;

    vert = vec3(projectionMatrix * viewMatrix * modelMatrix * vec4(vert, 1));

    float z = texture(positionFbo, vert.xy + 0.5).b;

    float uBias = 0.1;


    float o = 0.0;
    if ((vert.z - z) < -uBias) {
        o = 1.0;
    }

//    o = 1.0;
    fragmentColor = vec4(o, 0, 0, 1);
//    fragmentColor = vec4(vert.z*100, 0, 0, 1);
//       fragmentColor = vec4(z*100, 0, 0, 1);


//    fragmentColor = vec4(texture(vertexPosTexture, texCoord).rgb, 1);

//    fragmentColor = vec4( vec3(0.3, 0,0 ) ,1);

//    fragmentColor = vec4(vec3(texCoord,0), 1);

}
