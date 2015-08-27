layout (location = 0) in vec3 Position;
layout (location = 1) in float Type;
layout (location = 2) in float Size;

out float type;
out float size;

void main(){
    gl_Position = vec4(Position, 1.0);

    type = Type;
    size = Size;
}
