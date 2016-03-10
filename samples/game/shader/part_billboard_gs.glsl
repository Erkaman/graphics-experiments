layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 4) out;

uniform mat4 gVP;
uniform vec3 gCameraPos;

/*
in float type[];
in float size[];
in vec4 color[];
*/

//in vec4 positionIn[];

out vec2 TexCoord;
//out vec4 Color;

void main()
{
//	float billboardSize = size[0];
    float billboardSize = 10.0;

    vec3 Pos = gl_in[0].gl_Position.xyz;
    vec3 toCamera = normalize(gCameraPos - Pos);
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(toCamera, up) * billboardSize;

    Pos -= right * 0.5;
    Pos.y -= billboardSize * 0.5;
    gl_Position = gVP * vec4(Pos, 1.0);
    TexCoord = vec2(0.0, 0.0);
    //  Color = color[0];
    EmitVertex();

    Pos.y += billboardSize;
    gl_Position = gVP * vec4(Pos, 1.0);
    TexCoord = vec2(0.0, 1.0);
    //   Color = color[0];
    EmitVertex();

    Pos.y -= billboardSize;
    Pos += right;
    gl_Position = gVP * vec4(Pos, 1.0);
    TexCoord = vec2(1.0, 0.0);
    // Color = color[0];
    EmitVertex();

    Pos.y +=billboardSize;
    gl_Position = gVP * vec4(Pos, 1.0);
    TexCoord = vec2(1.0, 1.0);
    //   Color = color[0];
    EmitVertex();

    EndPrimitive();


}
