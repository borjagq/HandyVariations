#version 330 core

layout (location = 0) in vec3 inVertex; // Vertex.

uniform mat4 Model;                     // Imports the model matrix.
uniform mat4 View;                      // Imports the View matrix.
uniform mat4 Projection;                // Imports the projection matrix.
uniform mat4 modelView;                 // Imports the modelView already multiplied.
uniform mat4 normalMatrix;              // Imports the normal matrix.
uniform vec3 cameraPosition;            // Position of the camera.

out vec3 vertexPosition;                // Passes the current vertex to the fragment shader.

void main() {
    
    // Assigns the direct passes.
    vertexPosition = vec3(modelView * vec4(inVertex, 1.0));
    
    // Sets the visualized position by applying the camera matrix.
    gl_Position = Projection * vec4(vertexPosition, 1.0);
    
}
