#version 450

const int NUM_TEXTURES = 16;

layout(binding = 2) uniform sampler2D textures[NUM_TEXTURES]; // NUM_TEXTURES

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in flat uint instanceIndex;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PushConstants {
    int firstMaterialIndex;
    int count;
} pc;

void main() {
    outColor = texture(textures[int(min(instanceIndex, pc.firstMaterialIndex + pc.count - 1))], fragTexCoord);
}
