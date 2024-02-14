/**
 * @file background.h
 * @brief Background class header file.
 * @version 1.0.0 (2023-08-03)
 * @date 2023-08-03
 * @author Borja García Quiroga <garcaqub@tcd.ie>
 *
 *
 * Copyright (c) Borja García Quiroga, All Rights Reserved.
 *
 * The information and material provided below was developed as partial
 * requirements for the MSc in Computer Science at Trinity College Dublin,
 * Ireland.
 */

#ifndef BGQ_OPENGL_CLASSES_BACKGROUND_H_
#define BGQ_OPENGL_CLASSES_BACKGROUND_H_

#include <vector>

#include "GL/glew.h"
#include "glm/glm.hpp"

#include "classes/camera/camera.h"
#include "classes/shader/shader.h"
#include "classes/cubemap/cubemap.h"
#include "classes/ebo/ebo.h"
#include "classes/vbo/vbo.h"
#include "classes/vao/vao.h"
#include "structs/vertex/vertex.h"
#include "structs/bounding_box/bounding_box.h"

namespace bgq_opengl {

	/**
	* @brief Implementation of a Background class.
	*
	* Implementation of a Geometry class that will allow us to handle the geometric part
	* of the objects in the VBOs.
	*
	* @author Borja García Quiroga <garcaqub@tcd.ie>
	*/
	class Background {

		public:
			
			/**
			 * @brief Initializes the Background.
			 * 
			 * Initializes the geometry and stores it.
			 */
            Background();

			/**
			 * @brief Draws the Geometry.
			 *
			 * Displays the Geometry in OpenGL.
			 */
			void draw(Shader &shader, Camera &camera);

		private:

            unsigned int vao, vbo, ebo;             /// The IDs of the buffers of OpenGL.

	};

}  // namespace bgq_opengl

#endif  //! BGQ_OPENGL_CLASSES_BACKGROUND_H_
