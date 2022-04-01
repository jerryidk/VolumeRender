#include "MyUtils.h"

class App
{

public:
    char *AppName;
    char *Path;
    int x,y,z;

    App(char *name, char* path, int x, int y, int z)
    {
        AppName = name;
        Path = path;
        this->x = x;
        this->y = y;
        this->z = z;
    }

    void run()
    {
        init();
        createShader();
        mainloop();
        clean();
    }

private:
    GLFWwindow *window;
    cy::GLSLProgram prog;

    void init()
    {
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

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        
        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        glEnable(GL_TEXTURE_3D);
        glEnable(GL_DEPTH_TEST);
    }

    void createShader()
    {
        prog.BuildFiles("./shaders/quad.vs",
                        "./shaders/quad.fs");
    }

    void mainloop()
    {
        float Cube[] = {
            0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f,

            1.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f,
            0.0f, 1.0f, -1.0f,
            1.0f, 1.0f, -1.0f};

        int Order[] = {
            // front
            0, 1, 2,
            1, 2, 3,
            // back
            4, 5, 6,
            4, 6, 7,
            // bottom
            0, 2, 5,
            2, 4, 5,
            // top
            1, 3, 6,
            3, 6, 7,
            // left
            0, 1, 5,
            1, 5, 6,
            // right
            2, 3, 4,
            3, 4, 7};

        float transferfunc[] = {
            1.0, 0.0, 0.0, 1.0, 
            0.0, 0.0, 1.0, 1.0, 
        };

        int vao = GL_DrawCube(sizeof(Cube), sizeof(Order), Cube, Order);
        // Assuming data size is a byte = sizeof(char)
        int texture = GL_CreateTexture3D(this->Path, this->x, this->y, this->z);

        cy::Matrix4f projection = projection.Perspective(radian(45.0f),
                                                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        cy::Matrix4f view, model, s, r, t;
        cy::Vec4f tmp;
        cy::Vec3f cameraPosInSTR, m;
        m = cy::Vec3f(-0.5f, -0.5f, 0.5f);
        t.SetTranslation(m);
        s.SetScale(2.0f, 2.0f, 2.0f);
        model = s * t;

        ImGuiIO &io = ImGui::GetIO();
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            view.SetView(camera.pos, camera.lookat, camera.up);
            prog.Bind();
            prog["volumeTex"] = 0;
            prog["isovalue"] = isovalue;
            prog["stepsize"] = stepsize;
            prog["background"] = cy::Vec3f(background[0],background[1],background[2]);
            prog["MVP"] = projection * view * model;
            // Just translate in opposite direction
            // In general, just multiply by M inverse and negate z component.
            tmp = model.GetInverse() * camera.pos;
            cameraPosInSTR = cy::Vec3f(tmp.x, tmp.y, -tmp.z);
            prog["cameraPosInSTR"] = cameraPosInSTR;
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_3D, texture);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);



            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            onImgui = io.WantCaptureMouse;
            ImGui::Begin("Demo window");
            ImGui::SliderFloat("Isovalue", &isovalue, 0.0f, 1.0f);
            ImGui::SliderFloat("stepsize", &stepsize, 0.0f, 300.0f);
            ImGui::ColorEdit4("Cube color", background);
            ImGui::End();
            // Render dear imgui into screen
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
           
        }
    }

    void clean()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

int main(int argc, char* argv[])
{
    if(argc != 5)
        return -1;
    App app("volume rendering", argv[1], atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
    app.run();
    return 0;
}
