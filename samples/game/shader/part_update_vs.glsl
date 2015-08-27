layout (location = 0) in float type;
layout (location = 1) in vec3 position;
layout (location = 2) in vec3 velocity;
layout (location = 3) in float age;
layout (location = 4) in float size;

out float type0;
out vec3 position0;
out vec3 velocity0;
out float age0;
out float size0;

void main()
{
    type0 = type;
    position0 = position;
    velocity0 = velocity;
    age0 = age;
    size0 = size;

}
