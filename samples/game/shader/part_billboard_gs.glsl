layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 4) out;

uniform mat4 gVP;
uniform vec3 gCameraPos;


in float size[];

in Vertex
{
    float size;
    vec4 color;

} vo[];


//in vec4 positionIn[];

out vec2 TexCoord;
out vec4 Color;

void main()
{

    float billboardSize = vo[0].size;

    vec3 Pos = gl_in[0].gl_Position.xyz;
    vec3 toCamera = normalize(gCameraPos - Pos);
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(toCamera, up) * billboardSize;

    vec4 v = (gVP * vec4(Pos,1.0));

    vec2 texCoord = vec2(v.x/v.w, v.y / v.w);
    // texCoord = texCoord * 0.5 + 0.5;

    /*
    if( (v.z / v.w) <texture(depthTexture,  texCoord ).r ) {
	return;
    }
    */


    vec4 c = vo[0].color;

//    c = vec4(texCoord, 0, 1);

    Pos -= right * 0.5;
    Pos.y -= billboardSize * 0.5;
    gl_Position = gVP * vec4(Pos, 1.0);
    TexCoord = vec2(0.0, 0.0);
    Color = c;
    EmitVertex();

    Pos.y += billboardSize;
    gl_Position = gVP * vec4(Pos, 1.0);
    TexCoord = vec2(0.0, 1.0);
    Color = c;
    EmitVertex();

    Pos.y -= billboardSize;
    Pos += right;
    gl_Position = gVP * vec4(Pos, 1.0);
    TexCoord = vec2(1.0, 0.0);
    Color = c;
    EmitVertex();

    Pos.y +=billboardSize;
    gl_Position = gVP * vec4(Pos, 1.0);
    TexCoord = vec2(1.0, 1.0);
    Color = c;
    EmitVertex();

    EndPrimitive();
}
