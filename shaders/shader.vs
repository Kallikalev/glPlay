#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 FragPos;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0)); // multiply vertex coords by model matrix to get world coordinates
    Normal = mat3(transpose(inverse(model))) * aNormal; // creatre the normal matrix via trickery. TODO: do this in the cpu and send it over, for efficiency
}