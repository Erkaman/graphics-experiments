// This vertex shader simply outputs the input coordinates to the rasterizer. It only uses 2D coordinates.
layout (location = 0)in vec2 positionIn;

out vec4 posPos;

uniform sampler2D tex;

vec4 FxaaVertexShader(
vec2 pos,                 // Both x and y range {-1.0 to 1.0 across screen}.
vec2 rcpFrame) {          // {1.0/frameWidth, 1.0/frameHeight}
/*--------------------------------------------------------------------------*/
    #define FXAA_SUBPIX_SHIFT (1.0/4.0)
/*--------------------------------------------------------------------------*/
    vec4 posPos;
    posPos.xy = (pos.xy * 0.5) + 0.5;
    posPos.zw = posPos.xy - (rcpFrame * (0.5 + FXAA_SUBPIX_SHIFT));
    return posPos; }



void main()
{

    gl_Position = vec4(positionIn, 0.0, 1.0);

    vec2 rcpFrame = 1.0 / textureSize(tex, 0);
    posPos = FxaaVertexShader(positionIn, rcpFrame);
}
