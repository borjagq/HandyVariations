/**
 * @file camera.h
 * @brief Camera class header file.
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

#ifndef BGQ_OPENGL_CLASSES_CAMERA_H_
#define BGQ_OPENGL_CLASSES_CAMERA_H_

#include "glm/glm.hpp"

namespace bgq_opengl {

	/**
	 * @brief Implements a camera class.
	 * 
	 * Implements a the abstraction of a camera to handle the POV when rendering a scene
	 * 
	 * @author Borja García Quiroga <garcaqub@tcd.ie>
	 */
	class Camera {

		public:

			/**
			 * @brief Constructs a camera instance.
			 * 
			 * Constructs a camera instance and sets its initial values.
			 * 
			 * @param position The camera coordinates.
			 * @param direction Where the camera is looking.
			 * @param fov Camera field of view.
			 * @param near Minimum distance rendered.
			 * @param far maximum distance rendered.
			 * @param width The camera width.
			 * @param height The camera height.
			 */
			Camera(glm::vec3 position, glm::vec3 direction, float fov, float near, float far, int width, int height);

			/**
			 * @brief Get the camera matrix.
			 *
			 * Get the camera matrix.
			 */
			glm::mat4 getCameraMatrix();

			/**
			 * @brief Get the camera direction.
			 * 
			 * Get the camera direction.
			 */
			glm::vec3 getDirection();

			/**
			 * @brief Get the camera position.
			 *
			 * Get the camera position.
			 */
			glm::vec3 getPosition();

			/**
			 * @brief Get the projection matrix.
			 * 
			 * Get the projection matrix corresponding to this camera.
			 */
			glm::mat4 getProjection();

			/**
			 * @brief Get the camera up vector.
			 *
			 * Get the camera up vector.
			 */
			glm::vec3 getUp();

			/**
			 * @brief Get the view matrix.
			 * 
			 * Get the view matrix corresponding to this camera.
			 */
			glm::mat4 getView();
        
            /**
             * @brief Set the width of the camera.
             *
             * Set the width of the camera.
             */
            void setWidth(int new_width);

            /**
             * @brief Set the height of the camera.
             *
             * Set the height of the camera.
             */
            void setHeight(int new_height);
        
            /**
             * @brief Get the width of the camera.
             *
             * Get the width of the camera.
             */
            int getWidth();

            /**
             * @brief Get the height of the camera.
             *
             * Get the height of the camera.
             */
            int getHeight();
        
            /**
             * @brief Get the near bound of the camera.
             *
             * Get the near bound of the camera.
             *
             * @returns The near bound.
             */
            float getNear();
        
            /**
             * @brief Get the far bound of the camera.
             *
             * Get the far bound of the camera.
             *
             * @returns The far bound.
             */
            float getFar();

            /**
             * @brief Add a translation matrix to the camera.
             *
             * Add a translation matrix to the camera.
             *
             * @param x The x rotation.
             * @param y The y rotation.
             * @param z The z rotation.
             * @param angle The angle to rotate.
             */
            void rotate(float x, float y, float z, float angle);
        
            /**
             * @brief Add a translation matrix to the camera.
             *
             * Add a translation matrix to the camera.
             *
             * @param x The x rotation.
             * @param y The y rotation.
             * @param z The z rotation.
             * @param angle The angle to rotate.
             */
            void rotateInPlace(float x, float y, float z, float angle);

            /**
             * @brief Add a translation matrix to the camera.
             *
             * Add a translation matrix to the camera.
             *
             * @param x The x translation.
             * @param y The y translation.
             * @param z The z translation.
             */
            void translate(float x, float y, float z);

		private:

			float far;					/// Maximum clipping limit.
			float fov;					/// Field of view;
			float near;					/// Minimum clipping limit.
            glm::vec3 direction;        /// Vector indicating where the camera is looking.
			glm::vec3 position;			/// Coordinates of the camera.
			glm::vec3 up;				/// Vector that tells us the rotation of the camera.
			int window_height;			/// Height of the GLUT window.
			int window_width;			/// Width of the GLUT window.

	};

}  // namespace bgq_opengl

#endif  //!BGQ_OPENGL_CLASSES_CAMERA_H_
