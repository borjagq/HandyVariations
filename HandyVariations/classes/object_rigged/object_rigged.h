/**
 * @file object.h
 * @brief Object class header file.
 * @version 1.0.0 (2022-10-21)
 * @date 2022-10-21
 * @author Borja García Quiroga <garcaqub@tcd.ie>
 *
 *
 * Copyright (c) Borja García Quiroga, All Rights Reserved.
 *
 * The information and material provided below was developed as partial
 * requirements for the MSc in Computer Science at Trinity College Dublin,
 * Ireland.
 */

#ifndef BGQ_OPENGL_CLASSES_OBJECT_RIGGED_H_
#define BGQ_OPENGL_CLASSES_OBJECT_RIGGED_H_

#include <vector>
#include <string>

#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "classes/mesh/mesh.h"
#include "structs/bounding_box/bounding_box.h"

namespace bgq_opengl {

	class ObjectRigged {
	
		public:
			
			/**
			 * @brief Loads a model in the specified format.
			 *
			 * Loads in a model from a file.
			 * 
			 * @param filename The name of the model file.
			 */
            ObjectRigged(const char* filename);
        
            /**
             * @brief Get the bone mapping.
             *
             * Get the bone mapping.
             *
             * @returns The map of names to bones.
             */
            std::map<std::string, Bone> getBoneMap();
        
            /**
             * @brief Get the bone list.
             *
             * Get the bone list.
             *
             * @returns a vector containing bone instances.
             */
            std::vector<Bone> getBones();
        
            /**
             * @brief Gets the bounding box.
             *
             * Gets the bounding box of all the geometries.
             *
             * @returns The bounding box struct.
             */
            BoundingBox getBoundingBox();
        
            /**
             * @brief Gets the meshes in the object.
             *
             * Gets the meshes in the object.
             *
             * @returns The list of meshes.
             */
            std::vector<Mesh> getMeshes();
        
            /**
             * @brief Draws the Mesh.
             *
             * Displays the Mesh in OpenGL.
             *
             * @param shader The shader program that will be used to render the mesh.
             * @param camera The camera that will be used to render the mesh.
             */
            void draw(Shader &shader, Camera &camera);
        
            /**
             * @brief Resets the bone transformations.
             *
             * Resets the bone transformations.
             */
            void resetBones();
        
            /**
             * @brief Reset
             *
             * Reset the global matrix of the model.
             */
            void resetTransforms();

            /**
             * @brief Add a translation matrix to the model.
             *
             * Add a translation matrix to the model.
             *
             * @param x The x rotation.
             * @param y The y rotation.
             * @param z The z rotation.
             * @param angle The angle to rotate.
             */
            void rotate(float x, float y, float z, float angle);
        
            /**
             * @brief Rotates the specified bone.
             *
             * Rotates the specified bone.
             *
             * @param bone_name The name of the bone to be rotated.
             * @param x The x rotation.
             * @param y The y rotation.
             * @param z The z rotation.
             * @param angle The angle to rotate.
             */
            void rotateBone(std::string bone_name, float x, float y, float z, float angle);

            /**
             * @brief Add a translation matrix to the model.
             *
             * Add a translation matrix to the model.
             *
             * @param x The x scale.
             * @param y The y scale.
             * @param z The z scale.
             */
            void scale(float x, float y, float z);

            /**
             * @brief Add a translation matrix to the model.
             *
             * Add a translation matrix to the model.
             *
             * @param x The x translation.
             * @param y The y translation.
             * @param z The z translation.
             */
            void translate(float x, float y, float z);

		private:
        
            std::vector<Mesh> meshes;   /// The meshes making up this rigged object.

	};

}

#endif  //!BGQ_OPENGL_CLASSES_OBJECT_RIGGED_H_
