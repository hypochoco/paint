//
//  brush_shader.frag
//  engine
//
//  Created by Daniel Cho on 12/7/25.
//

#version 450

//layout(binding = 0) uniform sampler2D brushTex;
//
//layout(location = 0) in vec2 outUV;
//
//layout(location = 0) out vec4 outColor;
//
//void main() {
//    vec4 c = texture(brushTex, outUV);
//    outColor = c;
//}

layout(binding = 0) uniform sampler2D brushTex;

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec2 inUV;

layout(location = 0) out vec4 outColor;

void main() {
    float mask = texture(brushTex, inUV).r;
    float a = inColor.a * mask;
    outColor = vec4(inColor.rgb * a, a);

    vec4 c = texture(brushTex, inUV);
    outColor = c;
}

