//
//  brush_shader.vert
//  engine
//
//  Created by Daniel Cho on 12/7/25.
//

// todo: mess with these a little more to get a better understanding ...
// todo: alpha pre-multiplication not working ?

// todo:

//Since the texture is purely a shape mask regardless of its color, convert it to a single alpha value using luminance, then apply your brush color and pressure-driven opacity on top:
//Vertex shader — same as before, just pass color through:
//glsllayout(location = 0) out vec2 outUV;
//layout(location = 1) out vec4 outColor;
//
//void main() {
//    gl_Position = vec4(inPosition.xy * pc.size + pc.pos, 0.0, 1.0);
//    outUV = inTexCoord;
//    outColor = pc.color;
//}
//Fragment shader:
//glsllayout(binding = 0) uniform sampler2D brushTex;
//
//layout(location = 0) in vec2 outUV;
//layout(location = 1) in vec4 inColor;
//layout(location = 0) out vec4 outColor;
//
//void main() {
//    vec4 texSample = texture(brushTex, outUV);
//    
//    // convert texture to greyscale luminance — this is the brush shape mask
//    // regardless of what color the texture itself is
//    float mask = dot(texSample.rgb, vec3(0.2126, 0.7152, 0.0722));
//    
//    // also factor in the texture's own alpha if it has one (e.g. PNG with transparency)
//    mask *= texSample.a;
//    
//    // inColor.rgb = user selected brush color
//    // inColor.a  = opacity, e.g. driven by tablet pressure (0.0 - 1.0)
//    outColor = vec4(inColor.rgb, inColor.a * mask);
//}
//And update your push constant to make the pressure/opacity intent clear — though structurally it's the same vec4:
//glsllayout(push_constant) uniform BrushPC {
//    vec2 pos;
//    vec2 size;
//    vec4 color; // rgb = brush color, a = pressure/opacity
//} pc;
//On the CPU side you'd build the color as something like:
//cppglm::vec4 brushColor = glm::vec4(userColor.r, userColor.g, userColor.b, userOpacity * tabletPressure);
//The luminance weights (0.2126, 0.7152, 0.0722) are the standard ITU-R BT.709 coefficients — they account for how human vision perceives red, green, and blue brightness differently, so a yellow texture and a blue texture of the same "brightness" will produce the same mask strength. Using a plain average like 0.333 works but can feel slightly off perceptually.


// todo:

//Yes, absolutely. Converting to luminance per-fragment is wasteful since the texture content never changes — it's the same calculation repeated for every pixel of every brush stamp.
//The right place to do it is at upload time on the CPU, where you process the texture once and store it as a single-channel VK_FORMAT_R8_UNORM image. Then your shader just does:
//glslfloat mask = texture(brushTex, outUV).r;
//mask *= // (no need to factor in .a separately, you can bake that in too)
//If your source brush textures are RGBA PNGs, you'd preprocess on upload like:
//cppfor (int i = 0; i < width * height; i++) {
//    float r = src[i * 4 + 0] / 255.0f;
//    float g = src[i * 4 + 1] / 255.0f;
//    float b = src[i * 4 + 2] / 255.0f;
//    float a = src[i * 4 + 3] / 255.0f;
//    float luminance = 0.2126f * r + 0.7152f * g + 0.0722f * b;
//    dst[i] = (uint8_t)(luminance * a * 255.0f); // bake alpha in too
//}
//Then upload dst as VK_FORMAT_R8_UNORM. This gives you a compact single-channel mask texture, halves your texture bandwidth during rendering, and keeps the shader dead simple. It's the standard approach for brush mask textures in painting apps.

#version 450

layout(push_constant) uniform BrushPC {
    vec2 pos;  // world space coords (x, y)
    vec2 size; // brush size (width, height)
    vec4 color; // rgba
} pc;

layout(location = 0) in vec3 inPosition; // brush quad vertex position
layout(location = 1) in vec4 inColor;    // vertex color (not used here) ?
layout(location = 2) in vec2 inTexCoord; // texture coordinates

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec2 outUV;

void main() {
    // brush quad -> canvas quad
    outColor = inColor;
    gl_Position = vec4(inPosition.xy * pc.size + pc.pos, 0.0, 1.0);
    outUV = inTexCoord;
}
