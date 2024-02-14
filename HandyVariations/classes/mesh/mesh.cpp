/**
 * @file mesh.cpp
 * @brief Mesh class implementation file.
 * @version 3.1.0 (2023-07-22)
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

#include "mesh.h"

#include <vector>
#include <stdexcept>
#include <iostream>

#include "GL/glew.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "classes/bone/bone.h"
#include "classes/camera/camera.h"
#include "classes/ebo/ebo.h"
#include "classes/shader/shader.h"
#include "classes/texture/texture.h"
#include "classes/vao/vao.h"
#include "structs/vertex/vertex.h"
#include "structs/bounding_box/bounding_box.h"

namespace bgq_opengl {

	Mesh::Mesh(const aiScene* scene, const aiMesh* mesh) {
        
        // Store the global transform.
        this->global_trans = aiMatToGLM(scene->mRootNode->mTransformation);
        this->global_trans = glm::inverse(this->global_trans);
        
        // If has materials, pass them.
        aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
        aiColor3D color;
        mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        float shine = 0.0;
        mat->Get(AI_MATKEY_SHININESS, shine);
        
        // Iterate through the vertices in the mesh.
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            
            // Build an empty vertex.
            bgq_opengl::Vertex vertex{
                glm::vec3(0.0f, 0.0f, 0.0f),    // Position.
                glm::vec3(1.0f, 1.0f, 1.0f),    // Normal.
                glm::vec3(0.0f, 0.0f, 1.0f),    // Color.
                glm::vec2(0.0f, 0.0f),          // UV coords.
                glm::vec3(0.0f, 1.0f, 0.0f),    // Tangente.
                glm::vec3(1.0f, 0.0f, 0.0f)     // Bitangente.
            };
            
            // Init the values of the bones.
            for (unsigned int i = 0; i < MAX_BONE_INFLUEN; i++) {
                
                vertex.bone_ids[i] = -1;
                vertex.bone_weights[i] = 0.0;
                
            }
            
            // If it has a position, subtitute it in the vertex.
            if (mesh->HasPositions()) {
                
                const aiVector3D* vp = &(mesh->mVertices[i]);
                vertex.position = glm::vec3(vp->x, vp->y, vp->z);
                
            }
            
            vertex.color = glm::vec3(color.r, color.g, color.b);
            
            // If it has a normal, subtitute it in the vertex.
            if (mesh->HasNormals()) {
                
                const aiVector3D* vn = &(mesh->mNormals[i]);
                vertex.normal = glm::vec3(vn->x, vn->y, vn->z);
                
            }
            
            // If it has UV textures, subtitute it in the vertex.
            if (mesh->HasTextureCoords(0)) {
                
                const aiVector3D* vt = &(mesh->mTextureCoords[0][i]);
                vertex.uv = glm::vec2(-vt->y, vt->x);
                
            }
            
            // Check if it has tangents.
            if (mesh->HasTangentsAndBitangents()) {
                
                const aiVector3D* tangent = &(mesh->mTangents[i]);
                vertex.tangent = glm::vec3(tangent->x, tangent->y, tangent->z);
                
                const aiVector3D* bitangent = &(mesh->mBitangents[i]);
                vertex.bitangent = glm::vec3(bitangent->x, bitangent->y, bitangent->z);
                
            }
            
            // Add that to vertices.
            this->vertices.push_back(vertex);
            
        }

        // Iterate through the bones in this mesh.
        for (unsigned int i = 0 ; i < mesh->mNumBones; i++) {
            
            // Get the bone name.
            std::string bone_name = mesh->mBones[i]->mName.C_Str();
            
            // Check if the bone has already been added to the map or not.
            if (bone_mapping.find(bone_name) == this->bone_mapping.end()) {
                
                // If we're here, it means that it is not and we need to create it.
                Bone new_bone(this->bone_count, aiMatToGLM(mesh->mBones[i]->mOffsetMatrix), bone_name);
                
                // Now we store that new bone in the map.
                bone_mapping[bone_name] = new_bone;
                
                // Increment the value to be able to use it next iterations.
                bone_count++;
                
            }
            
            // Retrieve the bone id.
            int bone_id = bone_mapping[bone_name].getID();
    
            // Get the weights of the bone.
            const aiVertexWeight* weights = mesh->mBones[i]->mWeights;
            int num_weights = mesh->mBones[i]->mNumWeights;

            // Iterate through the weighted vertex.
            for (int j = 0; j < num_weights; j++) {
                
                // Retrieve the vertex information.
                int vertex_id = weights[j].mVertexId;
                float weight = weights[j].mWeight;
                
                // This scenario cannot happen.
                assert(vertex_id <= vertices.size());
                
                // Update the vertex bone data.
                updateVertexBones(vertex_id, bone_id, weight);
                                    
            }
            
        }
                
        // Load the bone hierarchy into the bones.
        loadBoneHierarchy(scene->mRootNode, nullptr);
        
        // Lets store all the indices or faces.
        for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
            
            this->indices.push_back(mesh->mFaces[j].mIndices[0]);
            this->indices.push_back(mesh->mFaces[j].mIndices[1]);
            this->indices.push_back(mesh->mFaces[j].mIndices[2]);
            
        }
        
		// Generate a VAO and bind it, generate a VBO for the vertices and a EBO for the indices.
		this->vao.bind();
		VBO vbo(this->vertices);
		EBO ebo(this->indices);

		// Links VBO attributes such as coordinates and colors to VAO.
		vao.link_attribute(vbo, 0, 3, GL_FLOAT, sizeof(bgq_opengl::Vertex), (void*) offsetof(Vertex, position));
		vao.link_attribute(vbo, 1, 3, GL_FLOAT, sizeof(bgq_opengl::Vertex), (void*) offsetof(Vertex, normal));
		vao.link_attribute(vbo, 2, 3, GL_FLOAT, sizeof(bgq_opengl::Vertex), (void*) offsetof(Vertex, color));
        vao.link_attribute(vbo, 3, 2, GL_FLOAT, sizeof(bgq_opengl::Vertex), (void*) offsetof(Vertex, uv));
        vao.link_attribute(vbo, 4, 3, GL_FLOAT, sizeof(bgq_opengl::Vertex), (void*) offsetof(Vertex, tangent));
        vao.link_attribute(vbo, 5, 3, GL_FLOAT, sizeof(bgq_opengl::Vertex), (void*) offsetof(Vertex, bitangent));
        vao.link_attribute(vbo, 6, MAX_BONE_INFLUEN, GL_INT, sizeof(bgq_opengl::Vertex), (void*) offsetof(Vertex, bone_ids));
        vao.link_attribute(vbo, 7, MAX_BONE_INFLUEN, GL_FLOAT, sizeof(bgq_opengl::Vertex), (void*) offsetof(Vertex, bone_weights));

		vao.unbind();
		vbo.unbind();
		ebo.unbind();
        
        // Load the textures.
        this->textures.push_back(Texture("hand_base_color.jpg", "baseColor", 1));
        this->textures.push_back(Texture("hand_normals.jpg", "normalMap", 2));
        this->textures.push_back(Texture("hand_specular.jpg", "specularMap", 3));

	}

    std::map<std::string, Bone> Mesh::getBoneMap() {
                
        return this->bone_mapping;
        
    }

    std::vector<Bone> Mesh::getBones() {
        
        // Init the vector.
        std::vector<Bone> list_bones(this->bone_mapping.size());
        
        // Iterate through the elements in the map.
        for (std::map<std::string, Bone>::iterator it = this->bone_mapping.begin();
             it != this->bone_mapping.end(); it++) {
            
            // Get the elements.
            std::string name = it->first;
            Bone bone = it->second;
            
            // Add them to the list.
            list_bones[bone.getID()] = bone;
            
        }
                
        return list_bones;
        
    }

	std::vector<GLuint> Mesh::getIndices() {

		return this->indices;

	}

	std::vector<Texture>  Mesh::getTextures() {

		return this->textures;

	}

	VAO Mesh::getVAO() {

		return this->vao;

	}

	std::vector<Vertex> Mesh::getVertices() {

		return this->vertices;

	}

    float Mesh::getShininess() {
        
        return this->shininess;
        
    }

    void Mesh::setShininess(float shine) {
        
        this->shininess = shine;
        
    }

    glm::mat4 Mesh::getTransformMat() {
        
        return this->transforms;
        
    }

    void Mesh::setTransformMat(glm::mat4 transform) {
        
        this->transforms = transform;
        
    }

    void Mesh::addTexture(const char* image, const char* name) {
        
        // Create the new texture from the parameters.
        Texture new_tex(image, name, (int) this->textures.size() + 1);
        
        // Add this texture to the texture vector.
        this->textures.push_back(new_tex);
        
    }

	void Mesh::draw(Shader &shader, Camera &camera) {

		// Activate the VAO and the shader to access the uniforms.
		shader.activate();
		vao.bind();
        
		for (size_t i = 0; i < textures.size(); i++) {

			std::string name = textures[i].getName();
			std::string id = "";
			
            textures[i].bind();
			shader.passTexture(textures[i]);

		}

		// Pass the camera to the shader.
		shader.passCamera(camera);
        
        // Pass the shininess to the shader.
        shader.passFloat("materialShininess", this->shininess);
        
        // Pass the bones to the shader.
        // Iterate through the elements in the map.
        for (std::map<std::string, Bone>::iterator it = this->bone_mapping.begin();
             it != this->bone_mapping.end(); it++) {
            
            // Pass this bone to the shader.
            Bone bone = it->second;
            shader.passBone("boneMatrices", bone);
            
        }

		// Get the model matrix and pass it.
		glm::mat4 model = this->transforms;
		glUniformMatrix4fv(glGetUniformLocation(shader.getProgramID(), "Model"), 1, GL_FALSE, glm::value_ptr(model));

		// Get the View matrix and compute the modelView;
		glm::mat4 view = camera.getView();
		glm::mat4 model_view = view * model;
		glUniformMatrix4fv(glGetUniformLocation(shader.getProgramID(), "modelView"), 1, GL_FALSE, glm::value_ptr(model_view));

		// Get the normal matrix and pass it.
		glm::mat4 normal_matrix = glm::transpose(glm::inverse(model_view));
		glUniformMatrix4fv(glGetUniformLocation(shader.getProgramID(), "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normal_matrix));

		// Draw the actual Mesh
		glDrawElements(GL_TRIANGLES, (GLsizei) this->indices.size(), GL_UNSIGNED_INT, 0);

	}

	BoundingBox Mesh::getBoundingBox() const {

		// Create the bb.
		BoundingBox bb;

		// Init the bounding box with the first vertex.
		bb.min = glm::vec3(this->vertices[0].position.x, this->vertices[0].position.y, this->vertices[0].position.z);
		bb.max = glm::vec3(this->vertices[0].position.x, this->vertices[0].position.y, this->vertices[0].position.z);

		// Loop through the vertices and get tge min and max values.
		for (int i = 1; i < this->vertices.size(); i++) {

			if (bb.min.x > this->vertices[i].position.x)
				bb.min.x = this->vertices[i].position.x;

			if (bb.min.y > this->vertices[i].position.y)
				bb.min.y = this->vertices[i].position.y;

			if (bb.min.z > this->vertices[i].position.z)
				bb.min.z = this->vertices[i].position.z;

			if (bb.max.x < this->vertices[i].position.x)
				bb.max.x = this->vertices[i].position.x;

			if (bb.max.y < this->vertices[i].position.y)
				bb.max.y = this->vertices[i].position.y;

			if (bb.max.z < this->vertices[i].position.z)
				bb.max.z = this->vertices[i].position.z;

		}

		return bb;

	}

    void Mesh::resetBones() {
        
        // Iterate through the elements in the map.
        for (std::map<std::string, Bone>::iterator it = this->bone_mapping.begin();
             it != this->bone_mapping.end(); it++) {
            
            // Reset its transforms.
            it->second.resetTransforms();
            
        }
        
    }

	void Mesh::resetTransforms() {

		this->transforms = glm::mat4(1.0f);

	}

	void Mesh::rotate(float x, float y, float z, float angle) {

		// Get the angle as radians.
		float radians = glm::radians(angle);

		// Create a rotation matrix.
		glm::mat4 identity_matrix(1.0f);
		glm::mat4 rotation_matrix = glm::rotate(identity_matrix, radians, glm::vec3(x, y, z));

		this->transforms = rotation_matrix * this->transforms;

	}

    void Mesh::rotateBone(std::string bone_name, float x, float y, float z, float angle) {
        
        this->bone_mapping[bone_name].rotate(x, y, z, angle);
        
    }

	void Mesh::scale(float x, float y, float z) {

		// Create the scale matrix.
		glm::mat4 identity_matrix(1.0f);
		glm::mat4 scale_matrix = glm::scale(identity_matrix, glm::vec3(x, y, z));

        this->transforms = scale_matrix * this->transforms;

	}

	void Mesh::translate(float x, float y, float z) {

		// Get the translation matrix.
		glm::mat4 trans_matrix = glm::translate(glm::mat4(1.0), glm::vec3(x, y, z));

		// Apply it to the transormations.
        this->transforms = trans_matrix * this->transforms;

	}

    glm::mat4 Mesh::aiMatToGLM(const aiMatrix4x4& og) {
        
        glm::mat4 res;
        
        res[0][0] = og.a1;
        res[1][0] = og.a2;
        res[2][0] = og.a3;
        res[3][0] = og.a4;
        
        res[0][1] = og.b1;
        res[1][1] = og.b2;
        res[2][1] = og.b3;
        res[3][1] = og.b4;
        
        res[0][2] = og.c1;
        res[1][2] = og.c2;
        res[2][2] = og.c3;
        res[3][2] = og.c4;
        
        res[0][3] = og.d1;
        res[1][3] = og.d2;
        res[2][3] = og.d3;
        res[3][3] = og.d4;
        
        return res;
        
    }

    void Mesh::loadBoneHierarchy(aiNode* node, aiNode* parent) {
        
        // Check if this node has a parent.
        if (parent != nullptr) {
            
            // Get this node's name and its parent name.
            std::string node_name(node->mName.data);
            std::string pare_name(parent->mName.data);
            
            // Check if both the node and the parent are bones.
            if (bone_mapping.find(node_name) != bone_mapping.end() &&
                bone_mapping.find(pare_name) != bone_mapping.end()) {
                
                // If they are, store the relationships.
                bone_mapping[node_name].setParent(&bone_mapping[pare_name]);
                bone_mapping[pare_name].addChild(&bone_mapping[node_name]);

            }
            
        }
        
        // Recursively do the same for each child.
        for (uint i = 0 ; i < node->mNumChildren ; i++) {
            
            loadBoneHierarchy(node->mChildren[i], node);

        }

    }

    void Mesh::updateVertexBones(int vertex_id, int bone_id, float weight) {
        
        int lowest_weight_id = 0;
        float lowest_weight = this->vertices[vertex_id].bone_weights[0];
        
        // Iterate through the existing bones of the vertex to get the lowest weight.
        for (int k = 0; k < MAX_BONE_INFLUEN; k++) {
            
            // Check if this bone has a lower weight than the minimum found.
            if (this->vertices[vertex_id].bone_weights[k] < lowest_weight) {
                
                // Then, this is the new lowest.
                lowest_weight_id = k;
                lowest_weight = this->vertices[vertex_id].bone_weights[k];
                
            }
            
        }
        
        // If the current minimum is smaller than this weight, replace it.
        if (lowest_weight < weight) {
            
            this->vertices[vertex_id].bone_ids[lowest_weight_id] = bone_id;
            this->vertices[vertex_id].bone_weights[lowest_weight_id] = weight;
            
        }
        
    }

}  // namespace bgq_opengl
