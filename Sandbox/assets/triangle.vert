#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_texCoord;

layout(location = 0) out vec3 out_normal;
layout(location = 1) out vec3 out_texCoord;
layout(location = 2) out vec3 out_fragPosWorld;

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
    vec4 posWorld = u_constants.worldMat * vec4(in_position, 1.0);
    gl_Position = u_camera.projMat * u_camera.viewMat * posWorld;

    out_normal = in_normal;
    out_texCoord = in_texCoord;
    out_fragPosWorld = posWorld.xyz;
}
