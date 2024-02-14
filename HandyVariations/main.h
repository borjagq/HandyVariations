/**
 * @file main.h
 * @brief Main header file.
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

#ifndef BGQ_OPENGL_MAIN_H_
#define BGQ_OPENGL_MAIN_H_

#define WINDOW_NAME "HandyVariations"
#define NORM_SIZE 1.0
#define MAX_BONE_INFLUENCE 4
#define INTERFACE_WIDTH 450
#define INTERFACE_HEIGHT 630

#include <vector>
#include <string>
#include <ctime>
#include <fstream>
#include <random>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "classes/background/background.h"
#include "classes/camera/camera.h"
#include "classes/object_rigged/object_rigged.h"
#include "classes/shader/shader.h"
#include "classes/texture/texture.h"

/*
*****************************************
* CONFIGURE THE DATASET PARAMETERS HERE *
*****************************************
*/

int window_width = 224;
int window_height = 224;
bool store_dataset = true;
int num_of_joint_angles = 31000;
int num_of_arm_positions = 31000;
int num_of_arm_rotations = 31000;
int num_of_skin_tones = 31000;
int num_of_lighting = 31000;
int num_of_shininess = 31000;
int num_of_backgrounds = 15000;
int num_of_camera_params = 1;
int dataset_size = 100000;
bool process_running = false;
std::string dataset_path = "...";
std::string backgrounds_path = "...";

bgq_opengl::Camera *camera;             /// The camera.
bgq_opengl::Shader *shader;             /// The main program shader.
bgq_opengl::Shader *shaderPnt;          /// The shaders for the auxiliary control points.
bgq_opengl::Shader *shaderBck;          /// The shaders for the background.
bgq_opengl::Background *backbox;        /// The background.
GLFWwindow *window = 0;                 /// Window ID.
GLFWwindow *interface_window = 0;       /// Interface window ID.
int frame_count = 0;                    /// The frame count of the system.
std::random_device rd;                  /// Randomness device.
std::mt19937 gen(rd());                 /// Randomness generator.

bgq_opengl::ObjectRigged *dis_pnt;      /// The object used to display points.
bgq_opengl::ObjectRigged *hand;         /// The hand that will be used for generating the dataset.
std::vector<glm::vec3> keypoints;       /// The keypoints of the hand.

std::string dataset_id = "";            /// The slug that identifies the dataset.
std::ofstream annotations_file;         /// The file containing the final annotations.
std::ofstream k_matrices_file;          /// The file containing the k_matrices.

std::vector<glm::vec3> arm_positions;               /// The list containingthe different arm positions.
std::vector<glm::vec3> arm_rotations;               /// The list containing the different arm rotations.
std::vector<std::vector<glm::vec3> > joint_angles;  /// The list containing the different joint rotations.
std::vector<float> skin_tones;                      /// The list containing the skin tone multipliers.
std::vector<bgq_opengl::Light> light_variations;    /// The list containing the light parameters.
std::vector<float> shine_variations;                /// The list containing the shininess variations.
std::vector<int> background_indices;                /// The list containing the shininess variations.

/// Specifies the background color.
const glm::vec4 background(40 / 255.0, 40 / 255.0, 40 / 255.0, 1.0);

/// Maps each keypoint to its reference bone.
const std::vector<int> key_mapping = {1, 2, 3, 4, 4, 6, 7, 14, 14, 9, 15, 18, 18,
    11, 16, 19, 19, 13, 17, 20, 20};

/// Maps the keypoints to their reference vertex in the mesh.
std::vector<int> keypoint_bone_map = {36563, 30249, 53106, 790, 528, 28613, 20338,
    21906, 17825, 38509, 25734, 24593, 23377, 28657, 9382, 10482, 6617, 60805,
    15913, 16162, 12608};

/// Maps indices to bones.
const std::vector<std::string> name_joint_mapping = {"Bone037", "Bone038", "Bone039",
    "Bone040", "Bone043", "Bone044", "Bone045", "Bone048", "Bone049", "Bone050",
    "Bone053", "Bone054", "Bone055", "Bone058", "Bone059", "Bone060"};

/**
 * @brief Clean everything to end the program.
 *
 * Clean everything to end the program.
 */
void clean();

/**
 * @brief Calculate the keypoints.
 *
 * Calculate the appropriate keypoints.
 */
void calculateKeypoints();

/**
 * @brief Display the OpenGL elements.
 *
 * Display all the OpenGL elements in the scene.
 */
void displayElements();

/**
 * @brief Display the GUI.
 *
 * This function will display the GUI elements.
 */
void displayInterface();

/**
 * @brief Display a control point in the scene.
 *
 * Display a control point in the scene.
 *
 * @param ctrl_pnt The control point that will be displayed.
 * @param size The size that will be used for displaying the point.
 */
void displayControlPoint(const glm::vec3 ctrl_pnt, const float size);

/**
 * @brief Init the elements of the program
 *
 * Initialize the objects, elements and all.
 */
void initElements();

/**
 * @brief Init the environment.
 *
 * Initialize the GLFW interface.
 */
void initInterface();

/**
 * @brief Init the renderer window..
 *
 * Initialize the GLFW and OpenGL renderer.
 */
void initRendererWindow();

/**
 * @brief Init the variations that will be used in the dataset.
 *
 * Init the variations that will be used in the dataset.
 */
void initVariations();

/**
 * @brief Save the buffer to an image.
 *
 * Save the frame buffer to an image.
 *
 * @param filepath The name of the resulting image.
 * @param save_window The opengl window.
 */
void saveImage(char* filepath, GLFWwindow* save_window);

/**
 * @brief Store the data to the dataset folder.
 *
 * Store the image and keypoints to the dataset.
 */
void storeDataToDataset();

/**
 * @brief Update the scene.
 *
 * Update the scene.
 */
void updateScene();

/**
 * @brief Main function.
 * 
 * Main function.
 */
int main(int argc, char** argv);

#endif  //!BGQ_OPENGL_MAIN_H_
