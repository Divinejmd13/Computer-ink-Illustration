#version 330 core

uniform sampler2D textureSampler;
uniform int useOutline;

in vec2 TexCoord;
in vec3 FragColor;  // Assuming you have this in your vertex shader output

out vec4 FragColor;

void main() {
    if (useOutline == 0) {
        FragColor = texture(textureSampler, TexCoord);  // Use texture
    } else {
        // Render the outline here
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);  // Red outline for simplicity
    }
}
