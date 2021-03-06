in vec2 texCoord;

out vec4 fragmentColor;



uniform sampler2D positionFbo;
uniform sampler2D vertexPosTexture;
uniform sampler2D fboSource;

//uniform sampler2D uSource;
//uniform sampler2D uVertex;
//uniform float uCount;
//uniform float uBias;
//uniform vec2 uRes;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform float numSamples;

void main() {

    vec3 vert = texture(vertexPosTexture, texCoord).rgb;

    vert = vec3(viewMatrix * modelMatrix * vec4(vert, 1));

    float z = texture(positionFbo, vert.xy + 0.5).b;

    float uBias = 0.01;


    float o = 0.0;
    if ((vert.z - z) < -uBias) {
        o = 1.0;
    }

//    o = 1.0;


    float src = texture(fboSource, texCoord).r;


//    fragmentColor = vec4( o/numSamples , 0, 0, 1);

    o = ((numSamples - 1.0)/numSamples) * src + (1.0/numSamples) * o;//(1.0 - uFrac) * o + uFrac * src;
//    o = o/numSamples + src;

    fragmentColor = vec4( o , 0, 0, 1);


//    fragmentColor = vec4(vert.z*100, 0, 0, 1);
//    /   fragmentColor = vec4(z*100, 0, 0, 1);


//    fragmentColor = vec4(texture(vertexPosTexture, texCoord).rgb, 1);

//    fragmentColor = vec4( vec3(0.3, 0,0 ) ,1);

//    fragmentColor = vec4(vec3(texCoord,0), 1);

}
