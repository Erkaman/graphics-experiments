layout (location = 0)in vec2 positionIn;
layout (location = 1)in vec2 texCoordIn;
out vec2 texCoord;

void main(void) {
    // in position (x,0,1) the postprocessing frame buffer is drawn, so if we also drew
    // the text at (x,0,1), the framebuffer gets drawn over the text. So the text ends up being hidden.
    // so we move the text forward a little to solve this problem.
  gl_Position = vec4(positionIn.xy, -0.1, 1);
  texCoord = texCoordIn;
}
