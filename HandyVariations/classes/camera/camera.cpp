/**
 * @file camera.cpp
 * @brief Camera class implementation file.
 * @version 1.0.0 (2022-11-23)
 * @date 2022-11-23
 * @author Borja García Quiroga <garcaqub@tcd.ie>
 *
 *
 * Copyright (c) Borja García Quiroga, All Rights Reserved.
 *
 * The information and material provided below was developed as partial
 * requirements for the MSc in Computer Science at Trinity College Dublin,
 * Ireland.
 */

#include "camera.h"

#include <iostream>

#include "GL/glew.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

namespace bgq_opengl {

	Camera::Camera(glm::vec3 position, glm::vec3 direction, float fov, float near, float far, int width, int height) {

		// Store all the parameters.
		this->direction = direction;
		this->position = position;
		this->far = far;
		this->fov = fov;
		this->near = near;
		this->window_width = width;
		this->window_height = height;

		// Set defaults.
		this->up = glm::vec3(0.0f, 1.0f, 0.0f);
	
	}

	glm::mat4 Camera::getCameraMatrix() {

        return this->getProjection() * this->getView();

	}

	glm::vec3 Camera::getDirection() {

        return this->direction;

	}

	glm::vec3 Camera::getPosition() {

        return this->position;

	}

	glm::mat4 Camera::getProjection() {
        
        // Adds perspective to the scene.
        float ratio = (float)this->window_width / this->window_height;
        return glm::perspective(glm::radians(this->fov), ratio, this->near, this->far);

	}

	glm::vec3 Camera::getUp() {

		return this->up;

	}

	glm::mat4 Camera::getView() {
        
        // Get the position and direction.
        glm::vec3 position = this->getPosition();
        glm::vec3 direction = this->getDirection();
        glm::vec3 up = this->getUp();
        
        // Calculate the view matrix.
        return glm::lookAt(position, position + direction, up);

	}

    void Camera::setWidth(int new_width) {
        
        this->window_width = new_width;
        
    }

    void Camera::setHeight(int new_height) {
        
        this->window_height = new_height;
        
    }

    int Camera::getWidth() {
        
        return this->window_width;
        
    }

    int Camera::getHeight() {
        
        return this->window_height;
        
    }

    float Camera::getNear() {
        
        return this->near;
        
    }

    float Camera::getFar() {
        
        return this->far;
        
    }

    void Camera::rotate(float x, float y, float z, float angle) {
        
        // First aff all, we must transform the up and direction vectors to be
        // in world coordinates instead of relative to the camera.
        glm::mat4 trans_matrix = glm::translate(glm::mat4(1.0), this->position);
        
        // Apply this transformation to the position.
        this->direction = glm::vec3(trans_matrix * glm::vec4(this->direction, 1.0f));
        this->up = glm::vec3(trans_matrix * glm::vec4(this->up, 1.0f));
        
        // Now, we rotate all the components across the center of the scene.
        // Get the angle as radians.
        float radians = glm::radians(angle);

        // Create a rotation matrix.
        glm::mat4 identity_matrix(1.0f);
        glm::mat4 rotation_matrix = glm::rotate(identity_matrix, radians, glm::vec3(x, y, z));
        
        // Apply this transformation to the position.
        this->position = glm::vec3(rotation_matrix * glm::vec4(this->position, 1.0f));
        this->direction = glm::vec3(rotation_matrix * glm::vec4(this->direction, 1.0f));
        this->up = glm::vec3(rotation_matrix * glm::vec4(this->up, 1.0f));
        
        // Finally, put the vectors back relative to the camera.
        trans_matrix = glm::translate(glm::mat4(1.0), -this->position);
        
        // Apply this transformation to the position.
        this->direction = glm::vec3(trans_matrix * glm::vec4(this->direction, 1.0f));
        this->up = glm::vec3(trans_matrix * glm::vec4(this->up, 1.0f));

    }

    void Camera::rotateInPlace(float x, float y, float z, float angle) {

        // Get the angle as radians.
        float radians = glm::radians(angle);

        // Create a rotation matrix.
        glm::mat4 identity_matrix(1.0f);
        glm::mat4 rotation_matrix = glm::rotate(identity_matrix, radians, glm::vec3(x, y, z));
        
        // Apply this transformation to the position.
        this->direction = glm::vec3(rotation_matrix * glm::vec4(this->direction, 1.0f));
        this->up = glm::vec3(rotation_matrix * glm::vec4(this->up, 1.0f));
        
    }

    void Camera::translate(float x, float y, float z) {

        // Get the translation matrix.
        glm::mat4 trans_matrix = glm::translate(glm::mat4(1.0), glm::vec3(x, y, z));
        
        // Apply this transformation to the position.
        this->position = glm::vec3(trans_matrix * glm::vec4(this->position, 1.0f));
        
    }

}  // namespace bgq_opengl
