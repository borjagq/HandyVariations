/**
 * @file mesh.h
 * @brief Mesh class header file.
 * @version 1.0.0 (2022-11-26)
 * @date 2022-11-26
 * @author Borja García Quiroga <garcaqub@tcd.ie>
 *
 *
 * Copyright (c) Borja García Quiroga, All Rights Reserved.
 *
 * The information and material provided below was developed as partial
 * requirements for the MSc in Computer Science at Trinity College Dublin,
 * Ireland.
 */

#ifndef BGQ_OPENGL_CLASSES_MESH_H_
#define BGQ_OPENGL_CLASSES_MESH_H_

#include <map>
#include <vector>

#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "classes/bone/bone.h"
#include "classes/camera/camera.h"
#include "classes/shader/shader.h"
#include "classes/texture/texture.h"
#include "classes/ebo/ebo.h"
#include "classes/vbo/vbo.h"
#include "classes/vao/vao.h"
#include "structs/vertex/vertex.h"
#include "structs/bounding_box/bounding_box.h"

namespace bgq_opengl {

	/**
	* @brief Implementation of a Mesh class.
	*
	* Implementation of a Mesh class that will allow us to handle the geometric part
	* of the objects in the VBOs.
	*
	* @author Borja García Quiroga <garcaqub@tcd.ie>
	*/
	class Mesh {

		public:
        
            /**
             * @brief Loads a given mesh into the object structure.
             *
             * Loads the passed mesh into the rigged object structure as a new mesh.
             *
             * @param scene The scene that holds the mesh.
             * @param mesh The mesh that will be loaded.
             */
            Mesh(const aiScene* scene, const aiMesh* mesh);
            
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
             * Gets the bounding box of the mesh.
             *
             * @returns The bounding box struct.
             */
            BoundingBox getBoundingBox() const;

			/**
			 * @brief Get the indices of the mesh.
			 *
			 * Get the indices of the mesh.
			 */
			std::vector<GLuint> getIndices();
			
			/**
			 * @brief Get the textures.
			 *
			 * Get the textures.
			 */
			std::vector<Texture> getTextures();
			
			/**
			 * @brief Get the VAO.
			 *
			 * Get the VAO.
			 */
			VAO getVAO();
			
			/**
			 * @brief Get the vertices of the mesh.
			 *
			 * Get the vertices of the mesh.
			 */
			std::vector<Vertex> getVertices();
        
            /**
             * @brief Get the object shininess.
             *
             * Get the object shininess.
             */
            float getShininess();
        
            /**
             * @brief Set the object shininess.
             *
             * Set the object shininess.
             */
            void setShininess(float shine);
        
            /**
             * @brief Get the current transform matrix.
             *
             * Get the current transform matrix.
             *
             * @returns The transform matrix.
             */
            glm::mat4 getTransformMat();
        
            /**
             * @brief Set the transform matrix.
             *
             * Set the transform matrix.
             *
             * @param transform The transform matrix.
             */
            void setTransformMat(glm::mat4 transform);
        
            /**
             * @brief Add a new texture to the mesh.
             *
             * Add a new texture that will be passed to the shader.
             *
             * @param image Image containing the texture.
             * @param name Texture type.
             */
            void addTexture(const char* image, const char* name);

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
			 * @brief Reset the transformations.
			 *
			 * Reset the transformations.
			 */
			void resetTransforms();

			/**
			 * @brief Add a rotation matrix to the model.
			 *
			 * Add a rotation matrix to the model.
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
			 * @brief Add a scaling matrix to the model.
			 *
			 * Add a scaling matrix to the model.
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
        
            /**
             * @brief Transform an Assimp matrix to glm.
             *
             * Transforms an assimp matrix to a glm matrix.
             *
             * @param og The original assimp matrix.
             *
             * @returns The equivalent glm mat4.
             */
            static glm::mat4 aiMatToGLM(const aiMatrix4x4& og);
        
            /**
             * @brief Loads the bone hierarchy.
             *
             * Loads the bone hierarchy into the map.
             *
             * @param node The node being navigated.
             * @param parent The parent node or a null.
             */
            void loadBoneHierarchy(aiNode* node, aiNode* parent);
        
            /**
             * @brief Updates the vertex information.
             *
             * Updates the vertex information to always keep the most weighted bones.
             *
             * @param vertex_id The index of the vertex in the list.
             * @param bone_id The id identifying the bone in the list.
             * @param weight The given weight to that bone.
             */
            void updateVertexBones(int vertex_id, int bone_id, float weight);

			std::vector<GLuint> indices;				/// Indices of the vertices.
			std::vector<Texture> textures;				/// Textures that will color this mesh.
			VAO vao;									/// VAO containing this object.
			std::vector<Vertex> vertices;				/// Mesh vertices.
			glm::mat4 transforms = glm::mat4(1.0f);		/// Tranform matrixes that will be passed to the shader.
            float shininess = 1.0;                      /// Shininess parameter.
            glm::mat4 global_trans = glm::mat4(1.0f);   /// The global tranform obtained from the model.
            int bone_count = 0;                         /// The current bone count.
            std::map<std::string, Bone> bone_mapping;   /// The mapping of names into bones.

	};

}  // namespace bgq_opengl

#endif  //! BGQ_OPENGL_CLASSES_MESH_H_
