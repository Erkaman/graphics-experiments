layout (location = 0) in vec3 Position;
layout (location = 1) in float Type;

out float type;

void main(){
    gl_Position = vec4(Position, 1.0);

    type = Type;
}
