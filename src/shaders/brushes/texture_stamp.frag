//
//  brush_shader.frag
//  engine
//
//  Created by Daniel Cho on 12/7/25.
//

#version 450

layout(push_constant) uniform BrushPC {
    vec2 pos;   // world space coords (x, y)
    vec2 size;  // brush size (width, height)
    vec4 color; // rgba
} pc;

layout(binding = 0) uniform sampler2D brushTex;

layout(location = 0) in vec4 inColor; // empty value
layout(location = 1) in vec2 inUV;

layout(location = 0) out vec4 outColor;

void main() {
    float mask = texture(brushTex, inUV).r; // uniform image format stored in r
    outColor = vec4(pc.color.rgb, pc.color.a * mask);
}
