#version 330 core

in vec2 texCoords;

uniform sampler2D image;

out vec4 outColor;

void main() {
    
    outColor = texture(image, texCoords);
        
}
