
# Volume rendering with Raycasting 

## Description

This is a class project for cs6635 at University of Utah. The project is intended to provide a guided tutorial on volume rendering with OpenGL using Raycasting algorithm. 

## Build

This project is not intended for you to build as I only upload the src code but many dependencies are missing, so you will not be able to directly build with src. Some of the dependencies: 
- IMGUI
- GLEW  
- GLFW 

## Tutorial

#### What is DVR?

Direct volume rendering is a special type of computer graphics techniques that takes 1d scalar value (voxel) to produce images. The major difference of DVR comparing to traditional volume rendering is that the it takes scalar value rather than vertices in the context of Graphics Programming. In the end of this guide, you can produce cool images like this 

![Demo](./res/demo1.png)

=============================================================

#### Ray Casting Algorithm

In this project, I am going to use Ray casting Algorithm. There are many techniques to do volume rendering. The major two approaches are Texture-based and Ray casting. The concept of texture-based is rather simple, where you can perceive the 3D volume as finite number of 2d images (texture). You can render those 2d slices with blending to get the volume. Since there are plenty resources online that implement this methods: [code project](https://www.codeproject.com/Articles/352270/Getting-Started-with-Volume-Rendering-using-OpenGL) and [nividia gpu gems](https://developer.nvidia.com/gpugems/gpugems/part-vi-beyond-triangles/chapter-39-volume-rendering-techniques), I am going to implement the later approach. 

Raycasting is a subset of Raytracing, in fact, it is much more easier to implement in term of the mathmatical details. Raycasting algorithm shoots rays into the scene and simply sample our data (3d voxel) along ray direction in some uniform interval. ![raycast](./res/raycast.png) *[EG2006](https://www.cg.informatik.uni-siegen.de/data/Tutorials/EG2006/RTVG04_GPU_Raycasting.pdf)*

=============================================================

#### What is OpenGL?

OpenGL is a graphics API that been provided by GPU manufactures (ex. Nvidia and Intel). It exposes application layer to the programmer. Due to the design of GPU, we can quickly utlize the rasterization pipeline to generate interactive renderings. OpenGL (or similar graphics API such as DirectX) is the backend of many application we have used in this course (ImageVis3d and Paraview). In this tutorial, I am not going to explain all the details with OpenGL but rather some of major components. If you want to learn more about OpenGL, you should refer to [learnopengl](https://learnopengl.com/).

=============================================================

#### Set up our project

OpenGL itself doesn't come with windows to display, so we need an library to display a window. I am going to use **GLFW**. You can use whatever of your choice. We also need a library wrangler that can help us find OpenGL function pointers. I am going to use **GLEW**. If you want a detailed guide, go to [learnopengl](https://learnopengl.com/). Here is my set up code 

``` 
        if (!glfwInit())
            std::cout << "glfw error" << std::endl;
        const char *glsl_version = "#version 330";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "volume rendering", NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            std::cout << "glfw error" << std::endl;
        }
        glfwMakeContextCurrent(window);
        if (glewInit() != GLEW_OK)
            std::cout << "glew error" << std::endl;
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetKeyCallback(window, key_callback);
        glfwSetCursorPosCallback(window, cursor_pos_callback);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
        glEnable(GL_DEPTH_TEST);
``` 
you can also see this in `init()` in main.cpp. 

=============================================================

#### Shader and Graphics pipeline

Our GPU is designed to run rasterization graphics pipeline efficient. This is how we achieve real time rendering in this project. OpenGL gives us partial control of this graphics pipeline through shaders. I am going to give a quick overview of graphics pipeline: 

Input Data > Vertex Processing > Rasterization > Pixel Processing 

As a programer, we can send data through our CPU to GPU. And OpenGL gives us **vertex shader** and **fragment shader** to access Vertex processing and Pixel processing stage.

=============================================================

#### Getting Volumetric Data

TODO

=============================================================

#### Draw a cube

TODO

=============================================================

#### Now Cast Rays

TODO

=============================================================


Have fun! 
Instructor: Chris Johnson (University of Utah)
Author: Jerry Zhang

 
