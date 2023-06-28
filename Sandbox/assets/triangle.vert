#version 450

layout(location = 0) out vec3 out_color;

layout(set = 0, binding = 0) uniform CameraData
{
    mat4 projMat;
    mat4 viewMat;
} u_camera;

layout(push_constant) uniform Constants
{
    mat4 worldMat;
} u_constants;

void main()
{
    vec3 positions[3] =
    {
    { 0.0, 0.5, 0.0 },
    { 0.5, -0.5, 0.0 },
    { -0.5, -0.5, 0.0 }
    };
    vec3 colors[3] =
    {
    { 1.0, 0.0, 0.0 },
    { 0.0, 1.0, 0.0 },
    { 0.0, 0.0, 1.0 }
    };

    out_color = colors[gl_VertexIndex];
    gl_Position = u_camera.projMat * u_camera.viewMat * u_constants.worldMat * vec4(positions[gl_VertexIndex], 1.0);
}
