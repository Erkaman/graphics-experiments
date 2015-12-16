layout (location = 0) in vec3 Position;
layout (location = 1) in float Type;
layout (location = 2) in float Size;
layout (location = 3) in vec4 Color;

out float type;
out float size;
out vec4 color;

void main(){
    gl_Position = vec4(Position, 1.0);

    type = Type;
    size = Size;
    color = Color;

}
