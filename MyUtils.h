
#ifndef APP_H
#define APP_H

#define GLEW_STATIC
#define CY_GL_DONT_CHECK_CONTEXT
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_glfw.h"
#include "IMGUI/imgui_impl_opengl3.h"
#include "TF/trasnferfunc.h"

#include "cyCodeBase/cyTriMesh.h"
#include "cyCodeBase/cyMatrix.h"
#include "cyCodeBase/cyGL.h"

#include <iostream>
#include <fstream>

#include "camera.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos);
                                              
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

cy::Vec3f cameraPos(0.0f, 0.0f, 10.0f);
cy::Vec3f cameraTarget(0.0f, 0.0f, 0.0f);
cy::Vec3f cameraUp(0.0f, 1.0f, 0.0f);

Camera camera(cameraPos,cameraTarget,cameraUp);

double pre_xpos = 0;
double pre_ypos = 0;

cy::Vec3f lightPos(5.0f, 2.5f, 1.0f);
float lightyaw = 11.3f;
float lightpitch = 26.5f;

bool onImgui = false;
bool blendmode = false;
float isovalue = 0.46;
float stepsize = 100.0;
float background[3] = {0.0f, 0.0f, 0.0f};
float radian(float degree)
{
    return degree * M_PI / 180.0f;
}



/**
 * @brief
 *  Create a mesh with a vao and a vbo. This method is based on convention
 *  position is 3d, normal is 3d, and uv is 2d.
 *
 * @return mesh vao ID
 */
unsigned int GL_CreateVAO(float *vertices, size_t size, size_t stride, bool enable_nor, bool enable_uv)
{

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);

    if (enable_nor)
    {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(float)));
    }

    if (enable_uv)
    {
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void *)(6 * sizeof(float)));
    }

    return VAO;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    int esc_state = glfwGetKey(window, GLFW_KEY_ESCAPE);
    int space_state = glfwGetKey(window, GLFW_KEY_SPACE);
    int left_state = glfwGetKey(window, GLFW_KEY_LEFT);
    int right_state = glfwGetKey(window, GLFW_KEY_RIGHT);
    int one_state = glfwGetKey(window, GLFW_KEY_1);
    int two_state = glfwGetKey(window, GLFW_KEY_2);

    if (esc_state == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// glfw: mouse button
//---------------------------------------------------------------------------
void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos)
{

    int left_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    int right_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

    if (left_state == GLFW_PRESS && !onImgui)
    {
        if (xpos > pre_xpos)
            camera.RotateHorizontal(1.0f);
        if (xpos < pre_xpos)
            camera.RotateHorizontal(-1.0f);
        if (ypos > pre_ypos)
            camera.RotateVertical(1.0f);
        if (ypos < pre_ypos)
            camera.RotateVertical(-1.0f);
      
        pre_xpos = xpos;
        pre_ypos = ypos; 
    }

    if (right_state == GLFW_PRESS && !onImgui)
    {
        if (xpos > pre_xpos)
            camera.Zoom(3.0f);
        if (xpos < pre_xpos)
            camera.Zoom(-3.0f);
        pre_xpos = xpos;
        pre_ypos = ypos; 
    }
        
}

int GL_CreateTexture3D(char* filename, int x, int y, int z)
{
    char* data;

    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (file.is_open())
    {
        data = (char*) malloc(x * y * z);
        file.read(data, x * y * z);
        file.close();
    }
              
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_3D, texture);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, x, y, z, 0, GL_RED, GL_UNSIGNED_BYTE, data);

    free(data);

    return texture;
}

int GL_CreateTexture1D(char* data, int x)
{          
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_1D, texture);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8, x, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    return texture;
}

int GL_DrawCube( int size_vertices, int size_indices, float* vertices, int* indices)
{
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size_vertices, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_indices, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    return VAO;
}

#endif
