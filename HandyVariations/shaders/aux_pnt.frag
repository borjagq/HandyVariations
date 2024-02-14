#version 330 core

in vec3 vertexPosition;         // Position from the VS.

out vec4 outColor; // Outputs color in RGBA.

void main() {

    // Final color.
    outColor = vec4(169.0 / 255.0, 3.0 / 255.0, 252.0 / 255.0, 1.0);
    
}
