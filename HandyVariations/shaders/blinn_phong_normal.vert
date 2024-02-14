#version 330 core

layout (location = 0) in vec3 inVertex; // Vertex.
layout (location = 1) in vec3 inNormal; // Normal (not necessarily normalized).
layout (location = 2) in vec3 inColor;  // Color (not necessarily normalized).
layout (location = 3) in vec2 inUV;     // UV coordinates.
layout (location = 4) in vec3 inTang;   // The tangent vector.
layout (location = 5) in vec3 inBitang; // The bitangent vector.
layout (location = 6) in ivec4 inBoneId;// The bone IDs.
layout (location = 7) in vec4 inWeights;// The bone weights.

uniform mat4 Model;                     // Imports the model matrix.
uniform mat4 View;                      // Imports the View matrix.
uniform mat4 Projection;                // Imports the projection matrix.
uniform mat4 modelView;                 // Imports the modelView already multiplied.
uniform mat4 normalMatrix;              // Imports the normal matrix.
uniform vec3 cameraPosition;            // Position of the camera.

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 boneMatrices[MAX_BONES];

out vec3 vertexPosition;                // Passes the current vertex to the fragment shader.
out vec3 vertexNormal;                  // Passes the normal to the fragment shader.
out vec3 vertexColor;                   // Passes the color to the fragment shader.
out vec2 vertexUV;                      // Passes the UV coordinates to the fragment shader.
out vec3 vertexTangent;                 // The vertex tangent.
out vec3 vertexBitangent;               // The vertex bitangent.

void main() {
    
    // Init the value of the variables to 0 to add the right values to them.
    vec4 interpolPosition = vec4(0.0, 0.0, 0.0, 0.0);
    vec3 interpolNormal = vec3(0.0, 0.0, 0.0);
    vec3 interpolTangent = vec3(0.0, 0.0, 0.0);
    vec3 interpolBitangent = vec3(0.0, 0.0, 0.0);

    // Init the total sums of weights to normalise them at the end.
    float accumWeight = 0.0;
    
    // Iterate through the influencing bones.
    for(int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        
        // If this bone is -1, it is not initialised. Don't use it.
        if(inBoneId[i] <= -1)
            continue;
        
        // Check that this bone is in the usable range.
        if(inBoneId[i] >= MAX_BONES)
            continue;
        
        // Apply the bone transforms to obtain the component points.
        vec4 partialPosition = boneMatrices[inBoneId[i]] * vec4(inVertex, 1.0);
        vec3 partialNormal = mat3(boneMatrices[inBoneId[i]]) * inNormal;
        vec3 partialTangent = mat3(boneMatrices[inBoneId[i]]) * inTang;
        vec3 partialBitangent = mat3(boneMatrices[inBoneId[i]]) * inBitang;

        // Add a pondered version of this point.
        interpolPosition += partialPosition * inWeights[i];
        interpolNormal += partialNormal * inWeights[i];
        interpolTangent += partialTangent * inWeights[i];
        interpolBitangent += partialBitangent * inWeights[i];

        // Add this weight to the weight accumulator to normalise the
        // pondered intermediate point.
        accumWeight += inWeights[i];
        
    }
    
    // If the accum weight is 0, we understand that no bones have been
    // applied.
    if (accumWeight == 0.0) {
        
        // Set the original point as the final point.
        interpolPosition = vec4(vertexPosition, 1.0);
        interpolNormal = inNormal;
        interpolTangent = inTang;
        interpolBitangent = inBitang;

    } else {
        
        // Normalise the boneVertex;
        interpolPosition /= accumWeight;
        interpolNormal /= accumWeight;
        interpolTangent /= accumWeight;
        interpolBitangent /= accumWeight;

    }

    // Assigns the direct passes.
    vertexPosition = vec3(modelView * interpolPosition);
    vertexColor = inColor;
    vertexUV = mat2(0.0, -1.0, 1.0, 0.0) * inUV;
    vertexNormal = vec3(normalMatrix * vec4(interpolNormal, 0.0));
    vertexTangent = vec3(normalMatrix * vec4(interpolTangent, 0.0));
    vertexBitangent = vec3(normalMatrix * vec4(interpolBitangent, 0.0));
    
    // Sets the visualized position by applying the camera matrix.
    gl_Position = Projection * vec4(vertexPosition, 1.0);
    
}
