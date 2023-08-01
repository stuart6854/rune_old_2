#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 outColor;

void main()
{
    const vec3 positions[3] = vec3[3](
        vec3(1, 1, 0),
        vec3(-1, 1, 0),
        vec3(0, -1, 0)
    );

    const vec3 colors[3] = vec3[3](
        vec3(1, 0, 0),
        vec3(0, 1, 0),
        vec3(0, 0, 1)
    );

//    outColor = colors[gl_VertexIndex];
//    gl_Position = vec4(positions[gl_VertexIndex], 1.0);

    outColor = inColor;
    gl_Position = vec4(inPosition, 1.0);
}