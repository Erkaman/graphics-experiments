uniform sampler2D gColorMap;

in vec2 TexCoord;
out vec4 fragmentColor;

in vec4 Color;

uniform sampler2D depthTexture;

uniform float windowWidth;
uniform float windowHeight;


float toLinearDepth(float depth) {
    float near = 0.1;
    float far = 500.0;

    return 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
}

void main()
{
    float depth = texture(depthTexture,  vec2(gl_FragCoord.x/windowWidth, gl_FragCoord.y/(windowHeight)) ).r;

    depth = toLinearDepth(depth);

    float ztest = 1.0 - clamp( (gl_FragCoord.z  / gl_FragCoord.w) -depth, 0, 1);

    fragmentColor =  Color * texture(gColorMap, TexCoord) * ztest;

//    fragmentColor =  vec4(vec3(toLinearDepth(depth) / 100 ),1);

}
