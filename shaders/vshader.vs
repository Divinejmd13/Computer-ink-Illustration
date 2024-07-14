#version 330 core
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

out vec2 TexCoord;

uniform mat4 vModel;
uniform mat4 vView;
uniform mat4 vProjection;

void main() {
    gl_Position = vProjection * vView * vModel * vec4(inPosition, 1.0);
    TexCoord = inTexCoord;
}
