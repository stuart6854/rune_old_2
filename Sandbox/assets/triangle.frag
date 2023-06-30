#version 450

//layout(location = 0) in vec3 in_color;

layout(location = 0) out vec4 out_fragColor;

const vec3 LightColor = vec3(1.0f, 1.0f, 1.0f);
const float AmbientStrength = 0.1f;

void main()
{
    vec3 ambient = AmbientStrength * LightColor;

    vec3 objectColor = vec3(1.0f, 1.0f, 1.0f);
    vec3 finalColor = ambient * objectColor;
    out_fragColor = vec4(finalColor, 1.0);
    //    out_fragColor = vec4(in_color, 1.0);
}
