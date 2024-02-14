/**
 * @file bone.h
 * @brief Bone class header file.
 * @version 1.0.0 (2023-07-21)
 * @date 2023-08-21
 * @author Borja García Quiroga <garcaqub@tcd.ie>
 *
 *
 * Copyright (c) Borja García Quiroga, All Rights Reserved.
 *
 * The information and material provided below was developed as partial
 * requirements for the MSc in Computer Science at Trinity College Dublin,
 * Ireland.
 */

#ifndef BGQ_OPENGL_CLASS_BONE_H_
#define BGQ_OPENGL_CLASS_BONE_H_

#include <string>

#include "GL/glew.h"
#include "glm/glm.hpp"

namespace bgq_opengl {

	/**
	 * @brief A geometry bone.
	 * 
	 * This class represents a bone of a given mesh and all its attributes.
	 */
	class Bone {
        
        public:
        
            /**
             * @brief Creates a new bone instance.
             *
             * Creates a new bone class with the given id and offset.
             */
            Bone();
        
            /**
             * @brief Creates a new bone instance.
             *
             * Creates a new bone class with the given id and offset.
             *
             * @param bone_id The id of the bone.
             * @param offset The offset matrix of the bone.
             * @param name The name of the bone.
             */
            Bone(const GLint bone_id, glm::mat4 offset, std::string name);
        
            /**
             * @brief Gets the id.
             *
             * Gets the id of the bone.
             *
             * @returns The bone id.
             */
            GLint getID();
        
            /**
             * @brief Gets the name.
             *
             * Gets the name of the bone.
             *
             * @returns The bone name.
             */
            std::string getName();
        
            /**
             * @brief Gets the offset.
             *
             * Gets the offset matrix to tranform from bone to object.
             *
             * @returns The bone offset.
             */
            glm::mat4 getOffset();
        
            /**
             * @brief Gets the transformations.
             *
             * Gets the transformation matrix.
             *
             * @returns The bone transformation matrix.
             */
            glm::mat4 getTransformMatrix();
        
            /**
             * @brief Set the parent bone.
             *
             * Set the name of the parent bone.
             *
             * @param parent The new name of the parent bone.
             */
            void setParent(Bone* parent);
        
            /**
             * @brief Add a new child.
             *
             * Add the name of a new child to the children list.
             *
             * @param child The name of the new child.
             */
            void addChild(Bone* child);
        
            /**
             * @brief Reset the bone transforms.
             *
             * Reset the bone transforms.
             */
            void resetTransforms();
        
            /**
             * @brief Rotates the bone.
             *
             * Rotates the bone around the joint.
             *
             * @param x The x rotation.
             * @param y The y rotation.
             * @param z The z rotation.
             * @param angle The angle to rotate.
             */
            void rotate(float x, float y, float z, float angle);
        
        protected:
        
            /**
             * @brief Propagates parent rotation.
             *
             * Rotates the bone around the joint of parent bone.
             *
             * @param propagate The offset to be propagated.
             */
            void propagateRotate(glm::mat4 propagate);
        
        private:

            GLint bone_id;	                /// Bone id in the matrices.
            glm::mat4 offset;               /// Transforms vertices from model space to bone space.
            glm::mat4 offset_inv;           /// Inverse of the offset matrix.
            std::string name;               /// Name of the bone.
            glm::mat4 transforms;           /// Transforms vertices.
            Bone* parent;                   /// The name of the parent bone.
            std::vector<Bone*> children;    /// The names of the children nodes.

	};

} // namespace bgq_opengl

#endif //!BGQ_OPENGL_CLASS_BONE_H_
