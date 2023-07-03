#version 450

layout(location = 0) in vec3 in_normal;
layout(location = 1) in vec2 in_texCoord;
layout(location = 2) in vec3 in_fragPosWorld;

layout(location = 0) out vec4 out_fragColor;

layout(set = 1, binding = 0) uniform sampler2D u_diffuse;

const vec3 LightColor = vec3(1.0f, 1.0f, 1.0f);
const vec3 LightPos = vec3(1.0f, 1.0f, -1.0f);
const float AmbientStrength = 0.1f;

void main()
{
    vec3 norm = normalize(in_normal);
    vec3 lightDir = normalize(LightPos - in_fragPosWorld);

    vec3 ambient = AmbientStrength * LightColor;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * LightColor;

    vec3 objectColor = texture(u_diffuse, in_texCoord).xyz;
    vec3 finalColor = (ambient + diffuse) * objectColor;
    out_fragColor = vec4(finalColor, 1.0);
    //    out_fragColor = vec4(in_color, 1.0);
}
