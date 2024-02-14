/**
 * @file main.cpp
 * @brief Main file.
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

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define GLM_ENABLE_EXPERIMENTAL

#include "main.h"

#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>
#include <cmath>
#include <format>
#include <fstream>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui_stdlib.h"
#include "imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/common.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

#include "classes/background/background.h"
#include "classes/bone/bone.h"
#include "classes/camera/camera.h"
#include "classes/light/light.h"
#include "classes/object_rigged/object_rigged.h"
#include "classes/shader/shader.h"
#include "structs/bounding_box/bounding_box.h"

void clean() {

	// Delete all the shaders.
	shader->remove();
    
    // Terminate ImGUI.
    ImGui_ImplGlfw_Shutdown();
    
    // Close GL context and any other GLFW resources.
    glfwTerminate();
    
    // Check if we're actually producing the dataset.
    if (!store_dataset)
        return;
        
    // Go back one position to remove the last comma printed.
    long pos = annotations_file.tellp();
    annotations_file.seekp(pos - 2);
    annotations_file << "]\n"; // Write the start of the json.
    annotations_file.close();
    
    pos = k_matrices_file.tellp();
    k_matrices_file.seekp(pos - 2);
    k_matrices_file << "]\n"; // Write the start of the json.
    k_matrices_file.close();

}

void calculateKeypoints() {
    
    // Init the keypoints.
    keypoints = std::vector<glm::vec3>(21, glm::vec3(0.0f, 0.0f, 0.0f));
        
    // Get the bones.
    std::vector<bgq_opengl::Bone> bones = hand->getBones();
    
    // Get the vertices.
    std::vector<bgq_opengl::Vertex> vertices = hand->getMeshes()[0].getVertices();
    
    // For each bone
    for (unsigned int i = 0; i < key_mapping.size(); i++) {
        
        glm::vec4 aux_pnt = glm::vec4(keypoints[i], 1.0f);
        
        // Apply the inverse of the offset to this point to pass it from bone
        // to world point, as all bone centers are in the 0,0,0 relatively to
        // themselves (duh).
        aux_pnt = glm::inverse(bones[key_mapping[i]].getOffset()) * aux_pnt;
        
        // Get the initial value for each.
        // Check if it is one of the cases that needs further reconstruction.
        switch (i) {
                
            case 4:
                aux_pnt += glm::vec4(-2.712357f, 10.171295f, 18.986443f, 0.0f);
                break;

            case 8:
                aux_pnt += glm::vec4(-0.000015f, 8.476074f, 12.544624f, 0.0f);
                break;
                
            case 12:
                aux_pnt += glm::vec4(1.017120f, 10.171303f, 11.527489f, 0.0f);
                break;
                
            case 16:
                aux_pnt += glm::vec4(2.034241f, 12.544601f, 10.510353f, 0.0f);
                break;
                
            case 20:
                aux_pnt += glm::vec4(0.678085f, 10.171295f, 7.119926f, 0.0f);
                break;
                
            default:
                break;
                
        }

        // We now retrieve the closest vertex.
        bgq_opengl::Vertex closest = vertices[keypoint_bone_map[i]];
        
        // Now we apply to it the same logic as in the shader.
        
        // Init the value of the variables to 0 to add the right values to them.
        glm::vec4 interpol_pnt = glm::vec4(0.0, 0.0, 0.0, 0.0);

        // Init the total sums of weights to normalise them at the end.
        float accum_weight = 0.0f;
        
        // Iterate through the influencing bones.
        for (unsigned int j = 0; j < MAX_BONE_INFLUENCE; j++) {
            
            // If this bone is -1, it is not initialised. Don't use it.
            if(closest.bone_ids[j] <= -1)
                continue;
            
            // Check that this bone is in the usable range.
            if(closest.bone_ids[j] >= bones.size())
                continue;
            
            // Apply the bone transforms to obtain the component points.
            glm::vec4 partial_pnt = bones[closest.bone_ids[j]].getTransformMatrix() * aux_pnt;

            // Add a pondered version of this point.
            interpol_pnt += partial_pnt * closest.bone_weights[j];

            // Add this weight to the weight accumulator to normalise the
            // pondered intermediate point.
            accum_weight += closest.bone_weights[j];
            
        }
        
        // If the accum weight is 0, we understand that no bones have been
        // applied.
        if (accum_weight == 0.0) {
            
            // Set the original point as the final point.
            interpol_pnt = aux_pnt;

        } else {
            
            // Normalise the boneVertex;
            interpol_pnt /= accum_weight;

        }
        
        // Then, apply the very same transformations we applied to the hand.
        aux_pnt = hand->getMeshes()[0].getTransformMat() * interpol_pnt;
        
        // Put that back in the keypoint.
        keypoints[i] = glm::vec3(aux_pnt);
        
    }
            
}

void displayElements() {
    
    // Make the interface the current context.
    glfwMakeContextCurrent(window);
    
    // Specify the color of the background
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);

    // Clean the back buffer and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Load the textures.
    if (num_of_backgrounds > 1) {
        
        // Get the id for the background.
        std::uniform_int_distribution<int> var_shine(0, num_of_backgrounds - 1);
        int sel_var_back = var_shine(gen);
        int backgr_id = background_indices[sel_var_back];
        
        // Get the name with the right amount of zeros.
        std::string bg_filename = "000000000000" + std::to_string(backgr_id);
        bg_filename = bg_filename.substr(bg_filename.size() - 12);
        bg_filename = backgrounds_path + "rdm_bg_" + bg_filename + ".jpg";
                        
        // Create a new texture from this image.
        bgq_opengl::Texture back_text(bg_filename.c_str(), "image", 4);

        // Draw the background.
        shaderBck->activate();
        shaderBck->passTexture(back_text);
        backbox->draw(*shaderBck, *camera);
        
        // Empty texture
        back_text.remove();
                
    }
    
    // Pass the parameters to the shaders.
    shader->activate();
    
    // Selects the light to pass.
    std::uniform_int_distribution<int> var_lights(0, num_of_lighting - 1);
    int sel_var_light = var_lights(gen);
    shader->passLight(light_variations[sel_var_light]);
    
    // Select the skin_tone.
    std::uniform_int_distribution<int> var_skin_tone(0, num_of_skin_tones - 1);
    int sel_var_skin_tone = var_skin_tone(gen);
    shader->passFloat("skinTone", skin_tones[sel_var_skin_tone]);
    
    // Select the shininess.
    std::uniform_int_distribution<int> var_shine(0, num_of_shininess - 1);
    int sel_var_shine = var_shine(gen);
    shader->passFloat("shininess", shine_variations[sel_var_shine]);
    
    // Draw the hand.
    hand->draw(*shader, *camera);
    
    // Check if we're actually producing the dataset.
    if (!store_dataset) {
        
        // For each bone
        for (unsigned int i = 0; i < keypoints.size(); i++) {
            
            // Display the point.
            displayControlPoint(keypoints[i], 0.015f);
            
            continue;
            
        }
        
    }
    
    // Poll and handle events.
    glfwPollEvents();
    
    // Swap the buffers to display it.
    glfwSwapBuffers(window);
        
}

void displayInterface() {
    
    // Make the interface the current context.
    glfwMakeContextCurrent(interface_window);
    
    // Poll and handle events.
    glfwPollEvents();
    
    // Set the ImGUI frame.
    ImGuiIO& io = ImGui::GetIO();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    // Make it fill the entire space.
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(io.DisplaySize);
    
    // Begin the new widget.
    ImGui::Begin("Dataset parameters", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
    
    // Check if the process is running.
    // If it is, the form will be disabled.
    if (process_running) {
        ImGui::BeginDisabled();
    }
    
    // Get the size of the data.
    if (window_width < 1) window_width = 1;
    ImGui::InputInt("Image width", &window_width);
    if (window_height < 1) window_height = 1;
    ImGui::InputInt("Image height", &window_height);
    
    // Get the dataset size.
    if (dataset_size < 1) dataset_size = 1;
    ImGui::InputInt("Dataset size", &dataset_size);
    
    ImGui::Dummy(ImVec2(0.0f, 20.0f));
    
    // Get the number of variations.
    ImGui::SliderInt("Joint angles", &num_of_joint_angles, 1, dataset_size);
    ImGui::SliderInt("Arm positions", &num_of_arm_positions, 1, dataset_size);
    ImGui::SliderInt("Arm rotations", &num_of_arm_rotations, 1, dataset_size);
    ImGui::SliderInt("Random skin tones", &num_of_skin_tones, 1, dataset_size);
    ImGui::SliderInt("Lighting settings", &num_of_lighting, 1, dataset_size);
    ImGui::SliderInt("Shininess levels", &num_of_shininess, 1, dataset_size);
    ImGui::SliderInt("Backgrounds", &num_of_backgrounds, 1, dataset_size);
        
    ImGui::Dummy(ImVec2(0.0f, 20.0f));

    ImGui::Text("Enter the absolute paths");
    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    ImGui::InputText("Backgrounds dir", &backgrounds_path);
    ImGui::InputText("Output dir", &dataset_path);

    // Get whether to store the data or not.
    ImGui::Dummy(ImVec2(0.0f, 20.0f));
    ImGui::Checkbox("Store the resulting dataset", &store_dataset);
    
    // Set the button to start the process.
    ImGui::Dummy(ImVec2(0.0f, 20.0f));
    if (ImGui::Button("Start dataset generation")) {
        process_running = true;
    } else if (process_running) {
        ImGui::EndDisabled();
    }
    
    ImGui::Dummy(ImVec2(0.0f, 20.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 20.0f));

    // Display the progress bar.
    ImGui::ProgressBar((float) frame_count / dataset_size);

    // Finish the widget.
    ImGui::End();
    
    // Render the interface.
    ImGui::Render();
    
    // Specify the color of the background
    glClearColor(background.x, background.y, background.z, background.w);

    // Clean the back buffer and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Pass the data from ImGUI to OpenGL and swap the buffers.
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(interface_window);
    
}

void displayControlPoint(const glm::vec3 ctrl_pnt, const float size) {
    
    glDepthFunc(GL_ALWAYS);
    
    // Reset it.
    dis_pnt->resetTransforms();
    
    // Resize it to the desired size.
    dis_pnt->scale(size, size, size);
        
    // Center the object and get it in the right position.
    dis_pnt->translate(ctrl_pnt.x, ctrl_pnt.y, ctrl_pnt.z);
    
    dis_pnt->draw(*shaderPnt, *camera);
    
    glDepthFunc(GL_LESS);
    
}

void initElements() {
    
     // Get the elements that will be used to display control points.
     dis_pnt = new bgq_opengl::ObjectRigged("sphere.obj");
     
    // Init the shader.
    shader = new bgq_opengl::Shader("blinn_phong_normal.vert", "blinn_phong_normal.frag");
    shaderPnt = new bgq_opengl::Shader("aux_pnt.vert", "aux_pnt.frag");
    shaderBck = new bgq_opengl::Shader("background.vert", "background.frag");
    
    // Init the hand model.
    hand = new bgq_opengl::ObjectRigged("hand.fbx");
    
    // Creates the first camera object
    camera = new bgq_opengl::Camera(glm::vec3(0.0f, 0.3f, 1.5f), glm::vec3(0.0f, 0.0f, -1.0f), 45.0f, 0.1f, 100.0f, window_width, window_height);
    
    // Init the background that will hold the textures.
    backbox = new bgq_opengl::Background();
    
}

void initInterface() {
    
    // start GL context and O/S window using the GLFW helper library
    if (!glfwInit()) {
        std::cerr << "ERROR: could not start GLFW3" << std::endl;
        exit(1);
    }
    
    // MacOS initializations.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    // Load the icon image.
    GLFWimage icons[1];
    icons[0].pixels = stbi_load("icon.png", &icons[0].width, &icons[0].height, 0, 4);
    glfwSetWindowIcon(interface_window, 1, icons);
    stbi_image_free(icons[0].pixels);
        
    // Create the window.
    interface_window = glfwCreateWindow(INTERFACE_WIDTH, INTERFACE_HEIGHT, WINDOW_NAME, nullptr, nullptr);
    if (!interface_window) {
        std::cerr << "Error 121-1001 - Failed to create the window." << std::endl;
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(interface_window);
    glfwSwapInterval(1); // Enable vsync
    
    // Setup Dear ImGui context
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    
    // Setup ImGui binding
    ImGui_ImplGlfw_InitForOpenGL(interface_window, true);
    ImGui_ImplOpenGL3_Init("#version 150");
    
    // Load the font.
    std::string font = "/System/Library/Fonts/SFNSRounded.ttf";
    if (std::filesystem::exists(font)) {
        io.Fonts->AddFontFromFileTTF(font.c_str(), 17.0f);
    }
    
    // Set the font color.
    ImGuiStyle* style = &ImGui::GetStyle();
    style->Colors[ImGuiCol_Text] = ImVec4(223 / 255.0f, 223 / 255.0f, 223 / 255.0f, 1.00f);
    style->Colors[ImGuiCol_TextDisabled] = ImVec4(113 / 255.0f, 113 / 255.0f, 113 / 255.0f, 1.00f);
    style->Colors[ImGuiCol_WindowBg] = ImVec4(background.x, background.y, background.z, background.w);
    style->Colors[ImGuiCol_Border] = ImVec4(84 / 255.0f, 84 / 255.0f, 84 / 255.0f, 1.00f);
    style->Colors[ImGuiCol_FrameBg] = ImVec4(background.x, background.y, background.z, background.w);
    style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(background.x + 0.05, background.y + 0.05, background.z + 0.05, background.w);
    style->Colors[ImGuiCol_FrameBgActive] = ImVec4(background.x + 0.05, background.y + 0.05, background.z + 0.05, background.w);
    style->Colors[ImGuiCol_Button] = ImVec4(107 / 255.0f, 106 / 255.0f, 105 / 255.0f, 1.00f);
    style->Colors[ImGuiCol_ButtonHovered] = ImVec4((107 + 10) / 255.0f, (106 + 10) / 255.0f, (105 + 10) / 255.0f, 1.00f);
    style->Colors[ImGuiCol_ButtonActive] = ImVec4((107 + 40) / 255.0f, (106 + 40) / 255.0f, (105 + 40) / 255.0f, 1.00f);
    style->Colors[ImGuiCol_Separator] = ImVec4(65 / 255.0f, 65 / 255.0f, 65 / 255.0f, 1.00f);
    style->Colors[ImGuiCol_SliderGrab] = ImVec4(205 / 255.0f, 205 / 255.0f, 205 / 255.0f, 1.00f);

    // Set the spaces, borders and corners.
    style->WindowPadding = ImVec2(20.0f, 20.0f);
    style->FrameBorderSize = 1;
    style->SeparatorTextPadding = ImVec2(15.0f, 15.0f);;
    style->GrabRounding = 20;
    style->ChildRounding = 3;
    style->WindowRounding = 3;
    style->GrabMinSize = 20;
    style->FrameRounding = 3;
    
    // Initialize GLEW and OpenGL.
    GLenum res = glewInit();

    // Check for any errors.
    if (res != GLEW_OK) {
        std::cerr << "Error 121-1002 - GLEW could not be initialized:" << glewGetErrorString(res) << std::endl;
        exit(1);
    }
    
    // Get version info.
    std::cerr << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cerr << "OpenGL version supported " << glGetString(GL_VERSION) << std::endl;
    std::cerr << "GLSL version supported " << (char *) glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    
}

void initRendererWindow() {
    
    // start GL context and O/S window using the GLFW helper library
    if (!glfwInit()) {
        std::cerr << "ERROR: could not start GLFW3" << std::endl;
        exit(1);
    }
    
    // MacOS initializations.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    // Create the window.
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    float xscale, yscale;
    glfwGetMonitorContentScale(monitor, &xscale, &yscale);
    window = glfwCreateWindow(window_width / xscale, window_width / yscale, "Current image", NULL, NULL);
    
    if (!window) {
        std::cerr << "Error 121-1001 - Failed to create the window." << std::endl;
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);
    
    // Initialize GLEW and OpenGL.
    GLenum res = glewInit();

    // Check for any errors.
    if (res != GLEW_OK) {
        std::cerr << "Error 121-1002 - GLEW could not be initialized:" << glewGetErrorString(res) << std::endl;
        exit(1);
    }
    
    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    
}

void initVariations() {
    
    // Init the variations to just one value.
    arm_positions.push_back(glm::vec3(0.0f));
    arm_rotations.push_back(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
    joint_angles.push_back(std::vector<glm::vec3>(16, glm::vec3(1.0f)));
    skin_tones.push_back(1.0f);
    light_variations.push_back(bgq_opengl::Light(glm::vec3(5.0, 5.0, 5.0), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 20.0f));
    shine_variations.push_back(1.0f);
    
    // Create the buffer.
    char buffer[256];
    snprintf(buffer, 256, "%i_%i_%i_%i_%i_%i_%i_%i_%i", num_of_joint_angles, num_of_arm_positions, num_of_arm_rotations, num_of_skin_tones, num_of_lighting, num_of_shininess, num_of_backgrounds, num_of_camera_params, dataset_size);
    
    // Build the slug from the dataset parameters.
    dataset_id = std::string(buffer);
    
    // Print it to be aware of the destination.
    std::cout << "CURRENT DATASET: " << dataset_id << std::endl;
    
    // Init the variations themselves.
    if (num_of_joint_angles > 1) {
        
        // We will use a uniform distribution, as it makes sense for this kind of problem.
        std::uniform_real_distribution<float> finger_flexion(-5.0f, 90.0f);
        std::uniform_real_distribution<float> finger_abduction(-10.0f, 10.0f);
        std::uniform_real_distribution<float> thumb_flexion(-80.0f, 10.0f);
        std::uniform_real_distribution<float> thumb_abduction(-30.0f, 30.0f);
        std::uniform_real_distribution<float> wrist_flexion(-90.0f, 90.0f);
        std::uniform_real_distribution<float> wrist_abduction(-30.0f, 30.0f);
        std::uniform_real_distribution<float> wrist_pronation(-60.0f, 60.0f);
        
        // Do as many variations as specified.
        for (unsigned int i = 1; i < num_of_joint_angles; i++) {
            
            // Create the variations.
            std::vector<glm::vec3> new_joint_angle(16, glm::vec3(0.0f));
            
            // Generate the rotation for the appropriate angles.
            new_joint_angle[0] = glm::vec3(wrist_pronation(gen), wrist_flexion(gen), wrist_abduction(gen));
            new_joint_angle[1] = glm::vec3(0.0f, thumb_abduction(gen), thumb_flexion(gen) / 2.0f);
            new_joint_angle[2] = glm::vec3(0.0f, 0.0f, thumb_flexion(gen));
            new_joint_angle[3] = glm::vec3(0.0f, 0.0f, thumb_flexion(gen));
            new_joint_angle[4] = glm::vec3(0.0f, finger_flexion(gen), finger_abduction(gen));
            new_joint_angle[5] = glm::vec3(0.0f, finger_flexion(gen), 0.0f);
            new_joint_angle[6] = glm::vec3(0.0f, finger_flexion(gen), 0.0f);
            new_joint_angle[7] = glm::vec3(0.0f, finger_flexion(gen), finger_abduction(gen));
            new_joint_angle[8] = glm::vec3(0.0f, finger_flexion(gen), 0.0f);
            new_joint_angle[9] = glm::vec3(0.0f, finger_flexion(gen), 0.0f);
            new_joint_angle[10] = glm::vec3(0.0f, finger_flexion(gen), finger_abduction(gen));
            new_joint_angle[11] = glm::vec3(0.0f, finger_flexion(gen), 0.0f);
            new_joint_angle[12] = glm::vec3(0.0f, finger_flexion(gen), 0.0f);
            new_joint_angle[13] = glm::vec3(0.0f, finger_flexion(gen), finger_abduction(gen));
            new_joint_angle[14] = glm::vec3(0.0f, finger_flexion(gen), 0.0f);
            new_joint_angle[15] = glm::vec3(0.0f, finger_flexion(gen), 0.0f);
            
            // Push this new configuration into the list.
            joint_angles.push_back(new_joint_angle);
            
        }
        
    }
    
    // Init the variations themselves.
    if (num_of_arm_positions > 1) {
        
        // We will use a uniform distribution, as it makes sense for this kind of problem.
        std::uniform_real_distribution<float> dist_x(-0.3, 0.3);
        std::uniform_real_distribution<float> dist_y(-0.3, 0.5);
        std::uniform_real_distribution<float> dist_z(-0.4, 0.4);
        
        // Do as many variations as specified.
        for (unsigned int i = 1; i < num_of_arm_positions; i++) {
            
            arm_positions.push_back(glm::vec3(dist_x(gen), dist_y(gen), dist_z(gen)));
            
        }
        
    }
    
    // Init the variations themselves.
    if (num_of_arm_rotations > 1) {
        
        // We will use a uniform distribution, as it makes sense for this kind of problem.
        std::uniform_real_distribution<float> dist_angle(-90.0f, 90.0f);
        
        // Do as many variations as specified.
        for (unsigned int i = 1; i < num_of_arm_rotations; i++) {

            arm_rotations.push_back(glm::vec3(dist_angle(gen), dist_angle(gen), dist_angle(gen)));
            
        }
        
    }
    
    // Init the skin tones.
    if (num_of_skin_tones > 1) {
        
        // We will use a uniform distribution, as it makes sense for this kind of problem.
        std::uniform_real_distribution<float> dist_skin(0.05f, 2.0f);
        
        // Do as many variations as specified.
        for (unsigned int i = 1; i < num_of_skin_tones; i++) {

            skin_tones.push_back(dist_skin(gen));
            
        }
        
    }
    
    // Init the lighting.
    if (num_of_lighting > 1) {
        
        // We will use a uniform distribution, as it makes sense for this kind of problem.
        std::uniform_real_distribution<float> dist_light_pos(-5.0f, 5.0f);
        std::uniform_real_distribution<float> dist_light_intensity(5.0, 40.0f);

        // Do as many variations as specified.
        for (unsigned int i = 1; i < num_of_lighting; i++) {

            glm::vec3 light_pos(dist_light_pos(gen), dist_light_pos(gen), dist_light_pos(gen));
            light_variations.push_back(bgq_opengl::Light(light_pos, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
                                                         dist_light_intensity(gen)));
            
        }
        
    }
    
    // Init the skin tones.
    if (num_of_shininess > 1) {
        
        // We will use a uniform distribution, as it makes sense for this kind of problem.
        std::uniform_real_distribution<float> dist_shin(1.0f, 50.0f);
        
        // Do as many variations as specified.
        for (unsigned int i = 1; i < num_of_shininess; i++) {

            shine_variations.push_back(dist_shin(gen));
            
        }
        
    }
    
    // Init the skin tones.
    if (num_of_backgrounds > 1) {
        
        // We will use a uniform distribution, as it makes sense for this kind of problem.
        std::uniform_int_distribution<int> get_image(0, 14042);
        
        // Do as many variations as specified.
        for (unsigned int i = 0; i < num_of_backgrounds; i++) {

            background_indices.push_back(get_image(gen));
            
        }
        
    }
      
    // Check if we're actually producing the dataset.
    if (!store_dataset)
        return;
    
    // Create the folder in case it does not exist.
    snprintf(buffer, 256, "mkdir -p %s%s", dataset_path.c_str(), dataset_id.c_str());
    system(buffer);
    
    snprintf(buffer, 256, "mkdir -p %s%s/training", dataset_path.c_str(), dataset_id.c_str());
    system(buffer);
    
    snprintf(buffer, 256, "mkdir -p %s%s/training/rgb", dataset_path.c_str(), dataset_id.c_str());
    system(buffer);
    
    // Create the file for the annotations.
    snprintf(buffer, 256, "%s%s/training_xyz.json", dataset_path.c_str(), dataset_id.c_str());
    annotations_file.open(buffer, std::ofstream::out | std::ofstream::trunc);
    annotations_file << "["; // Write the start of the json.
    
    // Create the file for the k_matrices.
    snprintf(buffer, 256, "%s%s/training_K.json", dataset_path.c_str(), dataset_id.c_str());
    k_matrices_file.open(buffer, std::ofstream::out | std::ofstream::trunc);
    k_matrices_file << "["; // Write the start of the json.

}

void saveImage(char* filepath, GLFWwindow* save_window) {
    
    // Get the information from the window.
    int img_width, img_height;
    glfwGetFramebufferSize(save_window, &img_width, &img_height);
    GLsizei img_channels = 3;
    
    // Calculate the strides of the image.
    GLsizei stride = img_channels * img_width;
    stride += (stride % 4) ? (4 - stride % 4) : 0;
 
    // The buffer that will be used for the image.
    GLsizei buffer_size = stride * img_height;
    std::vector<char> img_buffer(buffer_size);
 
    // Read the pixels into the buffer.
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, img_width, img_height, GL_RGB, GL_UNSIGNED_BYTE, img_buffer.data());
 
    // Write the buffer into the file.
    stbi_flip_vertically_on_write(true);
    stbi_write_png(filepath, img_width, img_height, img_channels, img_buffer.data(), stride);

}

void storeDataToDataset() {
        
    // Store the image.
    char file_path[256];
    snprintf(file_path, 256, "%s%s/training/rgb/tmp_%08i.png", dataset_path.c_str(), dataset_id.c_str(), frame_count);
    saveImage(file_path, window);
    
    // Convert the image to jpg.
    char buffer[512];
    snprintf(buffer, 512, "sips -s format jpeg %s%s/training/rgb/tmp_%08i.png --out %s%s/training/rgb/%08i.jpg", dataset_path.c_str(), dataset_id.c_str(), frame_count, dataset_path.c_str(), dataset_id.c_str(), frame_count);
    system(buffer);
    
    // Delete the tmp png.
    snprintf(buffer, 512, "rm %s%s/training/rgb/tmp_%08i.png", dataset_path.c_str(), dataset_id.c_str(), frame_count);
    system(buffer);
    
    // Now we're gonna calculate, for each keypoints, their coordinates and matrixes.
    glm::mat4 mvp_matrix = camera->getProjection() * camera->getView();
    
    // For each keypoint, do that.
    annotations_file << "[";
    for (unsigned int i = 0; i < keypoints.size(); i++) {
        
        // Calculate its homogeneous coordinates.
        glm::vec4 homogeneous_keypoint = mvp_matrix * glm::vec4(keypoints[i], 1.0f);
        
        // Transform to clipping space.
        glm::vec3 clip_keypoint = glm::vec3(homogeneous_keypoint) / homogeneous_keypoint.w;
        
        // Transform them to pixel coordinates.
        glm::vec3 annotation;
        annotation.x = (clip_keypoint.x + 1.0f) / 2.0f * window_width;
        annotation.y = (-clip_keypoint.y + 1.0f) / 2.0f * window_height;
        annotation.z = 1.0f;
        
        // Put these contents into a buffer and print them.
        snprintf(buffer, 512, "[%f, %f, %f], ", annotation.x, annotation.y, annotation.z);
        annotations_file << std::string(buffer);
        
    }
    
    // Go back one position to remove the last comma printed.
    long pos = annotations_file.tellp();
    annotations_file.seekp(pos - 2);
    annotations_file << "], ";
    
    // Do the same for the k_matrices.
    k_matrices_file << "[[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]], ";

}

void updateScene() {
    
    glfwMakeContextCurrent(window);
        
    // Reset the transformations to not apply them on top.
    hand->resetTransforms();
    
    // Obtain the original bounding box of the hand.
    bgq_opengl::BoundingBox bb = hand->getBoundingBox();
    
    // Obtain the center and sizes of the bounding box.
    glm::vec3 center = (bb.min + bb.max) * 0.5f;
    glm::vec3 size = (bb.max - bb.min);
    float size_max = std::max(std::max(size.x, size.y), size.z);
    float scaling_factor = 1.0f / size_max;
    
    // Move the hand to center it.
    hand->translate(-center.x, -center.y, -center.z);
    
    // Scale it to normalise its displacements.
    hand->scale(scaling_factor, scaling_factor, scaling_factor);

    // Rotate it to obtain the right configuration.
    hand->rotate(1.0f, 0.0f, 0.0f, -90.0f);
    
    // Select and apply orientation.
    std::uniform_int_distribution<int> var_orientation(0, num_of_arm_rotations - 1);
    int sel_var_orientation = var_orientation(gen);
    
    // Rotate the hand.
    hand->rotate(1.0, 0.0, 0.0, arm_rotations[sel_var_orientation].x);
    hand->rotate(0.0, 1.0, 0.0, arm_rotations[sel_var_orientation].y);
    hand->rotate(0.0, 0.0, 1.0, arm_rotations[sel_var_orientation].z);

    // Select and apply position.
    std::uniform_int_distribution<int> var_position(0, num_of_arm_positions - 1);
    int sel_var_position = var_position(gen);
        
    // Move the hand to center it.
    hand->translate(arm_positions[sel_var_position].x, arm_positions[sel_var_position].y, arm_positions[sel_var_position].z);
    
    // Reset the joints.
    hand->resetBones();
    
    // Select and apply joint angles.
    std::uniform_int_distribution<int> var_joint_angles(0, num_of_joint_angles - 1);
    int sel_var_joint_angles = var_joint_angles(gen);
    
    // Iterate through the relevant bones mapped in the variable.
    for (int i = (int) name_joint_mapping.size() - 1; i >= 0; i--) {
        
        hand->rotateBone(name_joint_mapping[i], 1.0f, 0.0f, 0.0f, joint_angles[sel_var_joint_angles][i].x);
        hand->rotateBone(name_joint_mapping[i], 0.0f, 1.0f, 0.0f, joint_angles[sel_var_joint_angles][i].y);
        hand->rotateBone(name_joint_mapping[i], 0.0f, 0.0f, 1.0f, joint_angles[sel_var_joint_angles][i].z);
        
    }
    
}

int main(int argc, char** argv) {
    
    // Initialise the environment.
    initInterface();
    
    // Main loop.
    while(!glfwWindowShouldClose(interface_window)) {
        
        // Make the things to print everything.
        displayInterface();
        
        // Check if it has been marked to start the process.
        if (process_running)
            break;
        
    }
    
    // Init the variations.
    initVariations();
    
    // Init the renderer window.
    initRendererWindow();
    
    // Initialise the objects and elements.
    initElements();
    
	// Main loop.
    while(!glfwWindowShouldClose(window) && !glfwWindowShouldClose(interface_window)) {

        // Apply the alterations and update the scene.
        updateScene();
        
        // Obtain the corresponding keypoints.
        calculateKeypoints();
        
        // Display the scene.
        displayElements();
        
        // Check if we're actually producing the dataset.
        if (store_dataset) {
            // Store the generated data.
            storeDataToDataset();
        }
        
        // Make the things to print everything.
        displayInterface();
        
        // Increment the frame count.
        frame_count++;
        
        // If we've done enough frames, exit the loop.
        if (frame_count >= dataset_size)
            break;
        
    }
    
	// Clean everything and terminate.
	clean();

}
