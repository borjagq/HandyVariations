/**
 * @file bone.cpp
 * @brief Bone class implementation file.
 * @version 2.0.1 (2023-08-15)
 * @date 2023-07-21
 * @author Borja García Quiroga <garcaqub@tcd.ie>
 *
 *
 * Copyright (c) Borja García Quiroga, All Rights Reserved.
 *
 * The information and material provided below was developed as partial
 * requirements for the MSc in Computer Science at Trinity College Dublin,
 * Ireland.
 */

#include "bone.h"

#include <string>

#include "GL/glew.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace bgq_opengl {

    Bone::Bone() {
        
        this->bone_id = -1;
        this->offset = glm::mat4(1.0f);
        this->name = "";
        this->transforms = glm::mat4(1.0f);
        this->parent = nullptr;

    }
        
    Bone::Bone(const GLint bone_id, glm::mat4 offset, std::string name) {
        
        this->bone_id = bone_id;
        this->offset = offset;
        this->offset_inv = glm::inverse(offset);
        this->name = name;
        this->transforms = glm::mat4(1.0f);
        this->parent = nullptr;
        
    }

    GLint Bone::getID() {
        
        return this->bone_id;
        
    }

    std::string Bone::getName() {
        
        return this->name;
        
    }

    glm::mat4 Bone::getOffset() {
        
        return this->offset;
        
    }

    glm::mat4 Bone::getTransformMatrix() {
        
        return this->transforms;
        
    }

    void Bone::setParent(Bone* parent) {
        
        this->parent = parent;
        
    }

    void Bone::addChild(Bone *child) {
        
        this->children.push_back(child);
        
    }

    void Bone::resetTransforms() {
        
        this->transforms = glm::mat4(1.0f);
        
        // Reset the offset.
        //this->offset = this->og_offset;
        //this->offset_inv = glm::inverse(this->offset);
        
    }

    void Bone::rotate(float x, float y, float z, float angle) {

        // Get the angle as radians.
        float radians = glm::radians(angle);

        // Create a rotation matrix.
        glm::mat4 identity_matrix(1.0f);
        glm::mat4 rotation_matrix = glm::rotate(identity_matrix, radians, glm::vec3(x, y, z));
        rotation_matrix = this->offset_inv * rotation_matrix * this->offset;
        
        // Apply the rotations after moving it to the joint and back.
        this->transforms = this->transforms * rotation_matrix;
        
        // For each child, propagate this rotation.
        for (unsigned int i = 0; i < this->children.size(); i++) {
            
            this->children[i]->propagateRotate(rotation_matrix);
            
        }

    }

    void Bone::propagateRotate(glm::mat4 propagate) {
        
        // Apply the rotations after moving it to the joint and back.
        this->transforms = propagate * this->transforms;
        
        // For each child, propagate this rotation.
        for (unsigned int i = 0; i < this->children.size(); i++) {
            
            this->children[i]->propagateRotate(propagate);
            
        }
        
    }

}  // namespace bgq_opengl
