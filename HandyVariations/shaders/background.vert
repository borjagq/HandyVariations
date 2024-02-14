#version 330 core

layout (location = 0) in vec3 inVertex;    // Vertex.

uniform mat4 View;            // Imports the View matrix.
uniform mat4 Projection;    // Imports the projection matrix.

out vec2 texCoords;

void main() {
    
    // Apply the transformations so that it moves with the camera.
    vec4 newPosition = Projection * View * vec4(inVertex, 1.0f);
    
    // These coordinates are now homogeneous.
    // Transform to clipping space.
    texCoords = newPosition.xy / newPosition.w;
    
    // Transform them to pixel coordinates.
    texCoords.x = (texCoords.x / 1.6 + 1.0) / 2.0;
    texCoords.y = (texCoords.y + 1.0) / 2.0;
    
    // We have to make Z == W so that it's always in the back.
    gl_Position = vec4(newPosition.x, newPosition.y, newPosition.w, newPosition.w);

}
