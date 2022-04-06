
# Volume rendering with Raycasting 

## Description

This is a class project for cs6635 at University of Utah. The project is intended to provide a guided tutorial on volume rendering with OpenGL using Raycasting algorithm. 

## Build

This project is not intended for you to build as I only upload the src code but many dependencies are missing, so you will not be able to directly build with src. Some of the dependencies: 
- IMGUI
- GLEW  
- GLFW 
- CyCodeBase (Cem Yuksel)

## Tutorial

#### What is DVR?

Direct volume rendering is a special type of computer graphics techniques that takes 1d scalar value (voxel) to produce images. The major difference of DVR comparing to traditional volume rendering is that the it takes scalar value rather than vertices in the context of Graphics Programming. In the end of this guide, you can produce cool images like this 

![Demo](./res/demo1.png)

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

We first need to obtain and understand the data for us to render. We are going to use raw format volume data. You can obtain data from this [website](https://klacansky.com/open-scivis-datasets/). The raw format data comes in different format in term byte layout, in our project, the format would be `uint8` always. Also make note the dimension of the data. For testing purpose, I am going to use the boston teapot since it is small. 

In order to render our data, we need to first send the volumetric data to our GPU. We can do this using a 3D Texture in OpenGL.

```
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
```
You can see this code in `GL_CreateTexture3D()` in MyUtils.h. 

=============================================================

#### Draw a cube

Refer back to the Raycasting algorithm and graphics pipeline, in order to utilize our GPU to do Raycasting. We need a *"canvas"* to run our Raycasting algorithm. This is where we need to render a cube first in OpenGL. By drawing a cube in the Vertex Shader and passing its texture coordinate, we can get the corresponding pixel texture coordinate. I contructed an cube in the positive world axis so I can easily save some effort in author our own texture coordiantes. See following simple vertex shader, you can get the cube vertices in main.cpp. 

```
#version 410 core

layout (location = 0) in vec3 aPos;
out vec3 TexCoord;
uniform mat4 MVP;

void main()
{
    TexCoord = vec3(aPos.xy ,-aPos.z);
    gl_Position = MVP * vec4(aPos, 1.0);
}
```

=============================================================

#### Ray Casting Algorithm

In this project, I am going to use Ray casting Algorithm. There are many techniques to do volume rendering. The major two approaches are Texture-based and Ray casting. The concept of texture-based is rather simple, where you can perceive the 3D volume as finite number of 2d images (texture). You can render those 2d slices with blending to get the volume. Since there are plenty resources online that implement this methods: [code project](https://www.codeproject.com/Articles/352270/Getting-Started-with-Volume-Rendering-using-OpenGL) and [nividia gpu gems](https://developer.nvidia.com/gpugems/gpugems/part-vi-beyond-triangles/chapter-39-volume-rendering-techniques), I am going to implement the later approach. 

Raycasting is a subset of Raytracing, in fact, it is much more easier to implement in term of the mathmatical details. Raycasting algorithm shoots rays into the scene (per pixel). Then we simply sample our data (3d voxel) along ray direction in some uniform interval. And we can do certain shading/lightting calculation and output a color. ![raycast](./res/raycast.png) *[EG2006](https://www.cg.informatik.uni-siegen.de/data/Tutorials/EG2006/RTVG04_GPU_Raycasting.pdf)*

It is much easier to understand it if we see the code, here is a psudocode. For details, refer to quad.fs.

```
color Raycast(in rayDir)
{
   dst = back color
   src = front color
   pos = point of intersection

   for(int i = 0; i < iter; i++)
   {
      //get volume data at current pos
      scalar = texture(volumeTex, pos).r;

      //get the color of the current sample
      src = getColor(scalar);

      //accumulate and blend with previous sample color
      dst = blend(src, dst);

      //advance ray
      pos += rayDir;

      //check if pos out of bound
      if(pos is out of box)
            break;
   }

   return dst;
}
```
For a given pixel, we need to start at where ray intersect our volume. Then we need to get the scalar data at that position of the volume. We use `src` and `dst` to keep track the current sample color and previous sampel color along the ray direction so that we can apply blending (compositing) operation on the colors. We keep doing this until `pos` is out of the range of the bounding box. Simple, right ? Now, let us put this into opengl code.

=============================================================

#### From theory to code 

The trickest part about this project is actually converting our above pseudocode into a real life application. First, we need to obtain the initial position and ray direction. Note that everything when we are doing in our ray traversal must be in the 3d texture space, which left-hand positive [0 - 1] coordiante system. This means that we must convert everything that is in the world space into the 3d texture space. 

Fortunately, our initial position `pos` will be the input `textureCoord` because rasterizer will automatically gives us the correct corresponding `textureCoord` of the cube at each pixel. Thus the only thing we need to convert is the camera(eye) position `cameraPos`. This convertion is relatively simple as the converting  OpenGL NDC (Normalized Device Coordinate) to 3d texture coordinate is simply negating the z-axis. And to convert from the world space to NDC, we just need to multiply the cameraPos by the inverse of the model matrix. You can find this in `mainloop()` in main.cpp line 141. 

```
tmp = model.GetInverse() * camera.pos;
cameraPosInSTR = cy::Vec3f(tmp.x, tmp.y, -tmp.z);
prog["cameraPosInSTR"] = cameraPosInSTR;
```

Another mystery we haven't solved is the `blend(a, b)` function. This function, as in it name, is to blend any given two color. The genral blend formula is `c = c1 * f1 + c2 * f2`. For exmaple, let` c1 = (1.0, 0.0, 0.0)`, `c2 = (0.0, 1.0, 0.0)`, `f1 = 0.5`, `f2 = 0.9`. If I want to blend a "over" b to get color c, `c = (1.0, 0.0, 0.0) * 0.5 + (0.0, 1.0, 0.0) * 0.9`. There are multiple ways to choose `f1` and `f2`, but the general formula stays the same. In our case, I used the [Nvidia's formula](https://developer.nvidia.com/gpugems/gpugems/part-vi-beyond-triangles/chapter-39-volume-rendering-techniques)  with a slight twist.

```
    c = c1 * c1.a + c2 * (1.0 - c1.a) // back-to-front
    or
    c = c2 * 1 + c1 * (1 - c2.a) * c1.a // front-to-back
    c.a = c2.a + (1 - c2.a) * c1.a 
```
In the fragment shader
```
    //back-to-front
    dst = src * src.a + (1.0 - src.a) * dst;
    //front to back src under dst
    dst.rgb += (1 - dst.a) * src.rgb * src.a;
    dst.a   += (1 - dst.a) * src.a;
```
=============================================================


Have fun! 
Instructor: Chris Johnson (University of Utah)
Author: Jerry Zhang

 
