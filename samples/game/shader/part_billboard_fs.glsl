uniform sampler2D gColorMap;

in vec2 TexCoord;
out vec4 fragmentColor;

in vec4 Color;

uniform sampler2D depthTexture;

uniform float windowWidth;
uniform float windowHeight;

uniform float znear;
uniform float zfar;


float toLinearDepth(float depth) {

    return 2.0 * znear * zfar / (zfar + znear - (2.0 * depth - 1.0) * (zfar - znear));
}

void main()
{
    float depth = texture(depthTexture,  vec2(gl_FragCoord.x/windowWidth, gl_FragCoord.y/(windowHeight)) ).r;

    depth = toLinearDepth(depth);

    float ztest = 1.0 - clamp( (gl_FragCoord.z  / gl_FragCoord.w) -depth, 0, 1);

    fragmentColor =  Color * texture(gColorMap, TexCoord) * ztest;

//    fragmentColor =  vec4(vec3(toLinearDepth(depth) / 100 ),1);

}
