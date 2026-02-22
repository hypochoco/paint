//
//  brush_shader.vert
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

layout(location = 0) in vec3 inPosition; // brush quad vertex position
layout(location = 1) in vec4 inColor;    // brush texture vertex color
layout(location = 2) in vec2 inTexCoord; // brush texture coordinates

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec2 outUV;

// note: brush quad -> canvas quad

void main() {
    gl_Position = vec4(inPosition.xy * pc.size + pc.pos, 0.0, 1.0);
    outColor = inColor;
    outUV = inTexCoord;
}
