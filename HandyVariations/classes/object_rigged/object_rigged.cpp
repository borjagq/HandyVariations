/**
 * @file object.cpp
 * @brief Object class implementation file.
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

#include "object_rigged.h"

#include <cassert>
#include <iostream>
#include <string>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "classes/mesh/mesh.h"
#include "classes/bone/bone.h"
#include "structs/vertex/vertex.h"
#include "structs/bounding_box/bounding_box.h"

namespace bgq_opengl {

    ObjectRigged::ObjectRigged(const char *filename) {

        // Import the scene from the file.
        const aiScene* scene = aiImportFile(filename, aiProcess_Triangulate);

        // Check if the scene was not read correctly.
        if (!scene) {
            
            std::cerr << "Could not read mesh on file " << filename << std::endl;
            exit(1);
            
        }

        for (int i = 0; i < scene->mNumMeshes; i++) {
            
            // Get this mesh from assimp.
            const aiMesh* mesh = scene->mMeshes[i];
            const aiString name = mesh->mName;
            
            // Skip the meshes that are buggy.
            if (strcmp(filename, "sphere.obj") && strcmp(name.C_Str(), "WrapHand003"))
                continue;
            
            // Load this mesh into the system.
            this->meshes.push_back(Mesh(scene, mesh));
            
        }

        aiReleaseImport(scene);

	}

    std::map<std::string, Bone> ObjectRigged::getBoneMap() {
        
        // Init the map to be returned.
        std::map<std::string, Bone> ret_bones;
        
        // Iterate through the meshes getting their bones.
        for (unsigned int i = 0; i < this->meshes.size(); i++) {
            
            // Get the map of this mesh.
            std::map<std::string, Bone> mesh_map = this->meshes[i].getBoneMap();
            
            // Add those to the final map.
            ret_bones.insert(mesh_map.begin(), mesh_map.end());
            
        }
        
        return ret_bones;
        
    }

    std::vector<Bone> ObjectRigged::getBones() {
        
        // Init a resulting vector.
        std::vector<Bone> total_bones;
        
        // Iterate through the meshes getting their bones.
        for (unsigned int i = 0; i < this->meshes.size(); i++) {
            
            // Get the bones.
            std::vector<Bone> mesh_bones = this->meshes[i].getBones();
            
            // Append those to the resulting vector.
            total_bones.insert(total_bones.end(), mesh_bones.begin(), mesh_bones.end());
            
        }
        
        return total_bones;
        
    }

    BoundingBox ObjectRigged::getBoundingBox() {

        // Create the bb.
        BoundingBox global_bb = this->meshes[0].getBoundingBox();

        // Loop through the vertices and get tge min and max values.
        for (int i = 1; i < this->meshes.size(); i++) {

            // Get the current bb.
            BoundingBox bb = this->meshes[i].getBoundingBox();

            if (global_bb.min.x > bb.min.x)
                global_bb.min.x = bb.min.x;

            if (global_bb.min.y > bb.min.y)
                global_bb.min.y = bb.min.y;

            if (global_bb.min.z > bb.min.z)
                global_bb.min.z = bb.min.z;

            if (global_bb.max.x < bb.max.x)
                global_bb.max.x = bb.max.x;

            if (global_bb.max.y < bb.max.y)
                global_bb.max.y = bb.max.y;

            if (global_bb.max.z < bb.max.z)
                global_bb.max.z = bb.max.z;

        }

        return global_bb;

    }

    std::vector<Mesh> ObjectRigged::getMeshes() {
        
        return this->meshes;
        
    }

    void ObjectRigged::draw(Shader &shader, Camera &camera) {
        
        // Iterate through the different meshes and just propagate.
        for (int i = 0; i < this->meshes.size(); i++) {
            
            this->meshes[i].draw(shader, camera);
            
        }
        
    }

    void ObjectRigged::resetBones() {
        
        for (unsigned int i = 0; i < this->meshes.size(); i++) {
            
            this->meshes[i].resetBones();
            
        }
        
    }

    void ObjectRigged::resetTransforms() {
        
        for (unsigned int i = 0; i < this->meshes.size(); i++) {
            
            this->meshes[i].resetTransforms();
            
        }
        
    }

    void ObjectRigged::rotate(float x, float y, float z, float angle) {

        for (unsigned int i = 0; i < this->meshes.size(); i++) {
            
            this->meshes[i].rotate(x, y, z, angle);
            
        }
        
    }

    void ObjectRigged::rotateBone(std::string bone_name, float x, float y, float z, float angle) {
        
        for (unsigned int i = 0; i < this->meshes.size(); i++) {
            
            this->meshes[i].rotateBone(bone_name, x, y, z, angle);
            
        }
        
    }

    void ObjectRigged::scale(float x, float y, float z) {

        for (unsigned int i = 0; i < this->meshes.size(); i++) {
            
            this->meshes[i].scale(x, y, z);
            
        }
        
    }

    void ObjectRigged::translate(float x, float y, float z) {

        for (unsigned int i = 0; i < this->meshes.size(); i++) {
            
            this->meshes[i].translate(x, y, z);
            
        }
        
    }

}
