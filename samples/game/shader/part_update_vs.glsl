layout (location = 0) in float type;
layout (location = 1) in vec3 position;
layout (location = 2) in vec3 velocity;
layout (location = 3) in float age;
layout (location = 4) in float size;
layout (location = 5) in vec4 color;
layout (location = 6) in float lifetime;
layout (location = 7) in float startSize;
layout (location = 8) in float endSize;

out float type0;
out vec3 position0;
out vec3 velocity0;
out float age0;
out float size0;
out vec4 color0;
out float lifetime0;
out float startSize0;
out float endSize0;

void main()
{
    type0 = type;
    position0 = position;
    velocity0 = velocity;
    age0 = age;
    size0 = size;
    color0 = color;
    lifetime0 = lifetime;
    startSize0 = startSize;
    endSize0 = endSize;
}
