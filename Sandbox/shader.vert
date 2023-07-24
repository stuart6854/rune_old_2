#version 450

void main()
{
    const vec3[3] positions = vec3[](
        vec3( 1.0,  1.0, 0.0),
        vec3(-1.0,  1.0, 0.0),
        vec3( 0.0, -1.0, 0.0)
    );

    gl_Position = vec4(positions[gl_VertexIndex], 1.0);
}