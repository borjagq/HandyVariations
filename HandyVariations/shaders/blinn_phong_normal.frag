#version 330 core

in vec3 vertexPosition;         // Position from the VS.
in vec3 vertexNormal;           // Normal from the VS.
in vec3 vertexColor;            // Color from the VS.
in vec2 vertexUV;               // UV coordinates from the VS.
in vec3 vertexTangent;          // The vertex tangent.
in vec3 vertexBitangent;        // The vertex bitangent.

uniform vec4 lightColor;        // Light color.
uniform vec3 lightPos;          // Light position.
uniform vec3 cameraPosition;    // Position of the camera.
uniform float skinTone;         // The skin tone modifier.
uniform float lightPower;       // Light power.
uniform float shininess;        // Object shininess.

uniform sampler2D baseColor;    // The base color texture.
uniform sampler2D normalMap;    // The normal map.
uniform sampler2D specularMap;  // The specular map.

const float minAmbLight = 0.25; // The minimum amount of light that will be used.
const float screenGamma = 2.2;  // Gamma correction params.

out vec4 outColor; // Outputs color in RGBA.

void main() {
    
    // Get the normal ready to use.
    vec3 normal = normalize(vertexNormal);
    vec3 tangent = normalize(vertexTangent);
    vec3 bitangent = normalize(vertexBitangent);
    
    // Get the matrix to convert stuff to tangent space.
    mat3 toTangentSpace = mat3(tangent, bitangent, normal);
    
    // Get the normal from the normal map.
    vec4 mappedNormal = texture(normalMap, vertexUV);
    
    // Transform it.
    normal = normalize(toTangentSpace * mappedNormal.xyz);

    // Get the light direction.
    vec3 lightDir = lightPos - vertexPosition;

    // Get the distance from the light to this fragment.
    float dist = length(lightDir);
    dist = dist * dist;
    
    // Normalize the light direction as a vector.
    lightDir = normalize(lightDir);

    // Get the lambertian component as stated in the docs.
    float lambertian = max(dot(lightDir, normal), 0.0);

    // Get the base color from the texture.
    vec4 textureColor = texture(baseColor, vertexUV);
    
    // Edit the texture to modify the tone.
    textureColor.x = max(0.0, min(textureColor.x * skinTone, 1.0));
    textureColor.y = max(0.0, min(textureColor.y * skinTone, 1.0));
    textureColor.z = max(0.0, min(textureColor.z * skinTone, 1.0));

    // init the specular.
    float specular = texture(specularMap, vertexUV).x;

    if (lambertian > 0.0) {

        // Get the direction from the position to the camera as a vector.
        vec3 viewDir = normalize(-vertexPosition);

        // Blinn-phong calculations.
        vec3 halfAngle = normalize(lightDir + viewDir);
        float specAngle = max(dot(halfAngle, normal), 0.0);
        specular = pow(specAngle, shininess);
       
    }

    // Get the minimum color.
    vec3 ambientColor = vec3(textureColor) * minAmbLight;

    // Get the diffuse final color.
    vec3 diffuseColor = vec3(textureColor) * lambertian * vec3(lightColor) * lightPower / dist;
    
    // Get the specular final color.
    vec3 specularColor = vec3(textureColor) * specular * vec3(lightColor) * lightPower / dist;

    // Get the final color that would go in the fragment.
    vec3 fragmentColor = ambientColor + diffuseColor + specularColor;
    
    // Apply gamma correction.
    fragmentColor = pow(fragmentColor, vec3(1.0 / screenGamma));

    // Final color.
    outColor = vec4(fragmentColor, 1.0);
    
}
