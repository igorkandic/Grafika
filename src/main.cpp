#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <learnopengl/shader.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <learnopengl/model.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadCubemap(vector<std::string> faces);
void renderQuad();

glm::vec3 cameraPos = glm::vec3(30.0f, 5.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(-1.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float lastX = 400, lastY = 300;
float yaw = -180.0f, pitch = 0.0f;
bool firstMouse = true;
float fov = 45.0f;
float playerSpeed = 2.5f;
int main(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "base project", NULL, NULL);
    if(window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Shader shader("shaders/vertex_shader.vs", "shaders/fragment_shader.fs");
    Shader lightSourceShader("shaders/lightSourceVertexShader.vs", "shaders/lightSourceFragmentShader.fs");
    Shader skyboxShader("shaders/cubemap_vertex.vs", "shaders/cubemap_fragment.fs");
    Shader instanceShader("shaders/instance_vertex.vs", "shaders/instance_fragment.fs");
    Shader hdrShader("shaders/hdr.vs", "shaders/hdr.fs");
    Shader shaderBlur("shaders/blur.vs", "shaders/blur.fs");



    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
            glm::vec3( -0.02f,  7.6f,  0.0f),
            glm::vec3( 2.3f, -3.3f, -4.0f),
            glm::vec3(0.0f,  2.0f, 0.0f),
            glm::vec3( 0.0f,  0.0f, -3.0f)
    };

    Model tardisObj = Model("resources/objects/War tardis/War tardis.obj");
    Model portalObj = Model("resources/objects/portal/portal.obj");
    Model amogusObj = Model("resources/objects/amogus/Among Us2.obj");
    Model cubeObj = Model("resources/objects/cube/cube.obj");
    Model planetObj = Model("resources/objects/planet/planet.obj");
    Model rockObj = Model("resources/objects/rock/rock.obj");
    Model insideObj = Model("resources/objects/inside/inside.obj");
    Model sunObj = Model("resources/objects/sun/sun.obj");
    Model valjakObj = Model("resources/objects/valjak/valjak.obj");



    std::vector<std::string> faces = {"resources/textures/skybox/right.png", "resources/textures/skybox/left.png",
                                              "resources/textures/skybox/top.png", "resources/textures/skybox/bottom.png",
                                              "resources/textures/skybox/front.png", "resources/textures/skybox/back.png"};
    unsigned int cubemapTexture = loadCubemap(faces);
    float skyboxVertices[] = {
            // positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };
    unsigned int amount = 10000;
    glm::mat4 *modelMatrices;
    modelMatrices = new glm::mat4[amount];
    srand(glfwGetTime()); // initialize random seed
    float radius = 75.0;
    float offset = 20.0f;
    for(unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 rockModel = glm::mat4(1.0f);
        // 1. translation: displace along circle with 'radius' in range [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // keep height of field smaller compared to width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        rockModel = glm::translate(rockModel, glm::vec3(x, y, z));

        // 2. scale: scale between 0.05 and 0.25f
        float scale = (rand() % 20) / 100.0f + 0.05;
        rockModel = glm::scale(rockModel, glm::vec3(scale));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = (rand() % 360);
        rockModel = glm::rotate(rockModel, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. now add to list of matrices
        modelMatrices[i] = rockModel;
    }

// set up floating point framebuffer to render scene to
    unsigned int hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    unsigned int colorBuffers[2];
    glGenTextures(2, colorBuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(
                GL_TEXTURE_2D, 0, GL_RGBA16F, 800, 600, 0, GL_RGBA, GL_FLOAT, NULL
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // attach texture to framebuffer
        glFramebufferTexture2D(
                GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0
        );
    }
    // create depth and stencil buffer (renderbuffer)
    unsigned int rboDepthStencil;
    glGenRenderbuffers(1, &rboDepthStencil);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencil);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
    // attach buffers
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthStencil);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    unsigned int pingpongFBO[2];
    unsigned int pingpongBuffer[2];
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongBuffer);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
        glTexImage2D(
                GL_TEXTURE_2D, 0, GL_RGBA16F, 800, 600, 0, GL_RGBA, GL_FLOAT, NULL
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(
                GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0
        );
    }

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

    for (unsigned int i = 0; i < rockObj.meshes.size(); i++)
    {
        unsigned int VAO = rockObj.meshes[i].VAO;
        glBindVertexArray(VAO);
        // set attribute pointers for matrix (4 times vec4)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }


    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);



    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    hdrShader.use();
    hdrShader.setInt("scene", 0);
    hdrShader.setInt("bloomBlur", 1);

    while(!glfwWindowShouldClose(window)){
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glm::vec3 sunPos = glm::vec3(250.f, 100.f, 0.f);
        glm::vec3 insidePos = glm::vec3(10000.f, 0.f, 0.f);
        shader.use();
        shader.setVec3("viewPos", cameraPos);
        shader.setFloat("material.shininess", 32.0f);
        shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        shader.setVec3("pointLights[0].position", pointLightPositions[0]);
        shader.setVec3("pointLights[0].ambient", 0.f, 0.f, 0.f);
        shader.setVec3("pointLights[0].diffuse", (float)(glm::cos(5 * glfwGetTime()) + 1.f) / 2.f * 10.f, (float)(glm::cos(5 *glfwGetTime()) + 1.f) / 2.f * 1.f, (float)(glm::cos(5 *glfwGetTime()) + 1.f) / 2.f * 1.f);
        shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("pointLights[0].constant", 1.0f);
        shader.setFloat("pointLights[0].linear", 0.7f);
        shader.setFloat("pointLights[0].quadratic", 1.8f);

        shader.setVec3("pointLights[1].position", sunPos);
        shader.setVec3("pointLights[1].ambient", 0.1f, 0.1f, 0.1f);
        shader.setVec3("pointLights[1].diffuse", 50.f, 40.f, 3.f);
        shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("pointLights[1].constant", 1.0f);
        shader.setFloat("pointLights[1].linear", 0.2f);
        shader.setFloat("pointLights[1].quadratic", 0.01f);

//        shader.setVec3("pointLights[2].position", insidePos + pointLightPositions[2]);
//        shader.setVec3("pointLights[2].ambient", 0.1f, 0.1f, 0.1f);
//        shader.setVec3("pointLights[2].diffuse", 1.f, 1.f, 9.f);
//        shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
//        shader.setFloat("pointLights[2].constant", 1.0f);
//        shader.setFloat("pointLights[2].linear", 0.09f);
//        shader.setFloat("pointLights[2].quadratic", 0.032f);

        instanceShader.use();
        instanceShader.setVec3("viewPos", cameraPos);
        instanceShader.setFloat("material.shininess", 32.0f);
        instanceShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        instanceShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        instanceShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        instanceShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        instanceShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        instanceShader.setVec3("pointLights[0].ambient", 0.0f, 0.0f, 0.f);
        instanceShader.setVec3("pointLights[0].diffuse", 20.f, 1.f, 1.f);
        instanceShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        instanceShader.setFloat("pointLights[0].constant", 1.0f);
        instanceShader.setFloat("pointLights[0].linear", 0.09f);
        instanceShader.setFloat("pointLights[0].quadratic", 0.032f);

        instanceShader.setVec3("pointLights[1].position", sunPos);
        instanceShader.setVec3("pointLights[1].ambient", 0.1f, 0.1f, 0.1f);
        instanceShader.setVec3("pointLights[1].diffuse", 50.f, 40.f, 3.f);
        instanceShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        instanceShader.setFloat("pointLights[1].constant", 1.0f);
        instanceShader.setFloat("pointLights[1].linear", 0.2f);
        instanceShader.setFloat("pointLights[1].quadratic", 0.01f);

        shader.use();
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, .1f, 100000.0f);
        shader.setMat4("projection", projection);
        glm::mat4 view;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        shader.setMat4("view", view);

        glm::mat4 insideModel = glm::mat4(1.f);
        insideModel = glm::translate(insideModel, insidePos);


        glm::mat4 amogusModel = glm::mat4(1.f);
        amogusModel = glm::translate(amogusModel, glm::vec3(25.f, 0.f, 0.f));
        amogusModel = glm::scale(amogusModel, glm::vec3(0.3f, .3f, .3f));
        amogusModel = glm::rotate(amogusModel, (float)glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));

        glm::mat4 destModel = glm::mat4(1.f);
        destModel = glm::translate(destModel, insidePos + glm::vec3(5.5, 1.5, -2.65));



        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
        glStencilOp(GL_INCR, GL_KEEP, GL_KEEP);
        glStencilMask(0xFF);
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);

        glm::vec3 tardisPos = glm::vec3(0.f, 17.f, 0.f);
        glm::mat4 portalModel = glm::mat4(1.0f);
        glm::vec3 srcPos = glm::vec3(1.75f, 3.f, 0.f) + tardisPos;
        portalModel = glm::translate(portalModel, srcPos);
        portalModel = glm::scale(portalModel, glm::vec3(2.f, 3.f, 2.8f));
        shader.setMat4("model", portalModel);
        portalObj.Draw(shader);

        glm::mat4 destView = view * portalModel * glm::rotate(glm::mat4(1.0), glm::radians(-90.0f), glm::vec3(0.0,1.0,0.0)) * glm::inverse(destModel);

        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDepthMask(GL_TRUE);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glStencilMask(0xFF);
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        glStencilOp(GL_DECR, GL_KEEP, GL_KEEP);
        glm::mat4 clippedProjection;
        clippedProjection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, glm::distance(cameraPos, srcPos), 100.0f);

//        glm::mat4 blueLightModel = glm::translate(glm::mat4(1.f), insidePos + pointLightPositions[2]);
////        blueLightModel = glm::scale(blueLightModel, glm::vec3(2.f, 0.2f, 2.f));
//        lightSourceShader.setMat4("model", blueLightModel);
//        lightSourceShader.setMat4("view", destView);
//        lightSourceShader.setMat4("projection", clippedProjection);
//        lightSourceShader.setVec3("brightness", glm::vec3(0.1f, 0.1f, 1.f));
//        valjakObj.Draw(lightSourceShader);
//        lightSourceShader.setMat4("projection", projection);
//        lightSourceShader.setMat4("view", view);
//TODO plavo svetlo u tardisu fixati da ne obasjava celu planetu

        shader.setMat4("projection", clippedProjection);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, tardisPos);
        model = glm::rotate(model, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
        shader.setMat4("model", model);
        shader.setMat4("view", destView);
//        tardisObj.Draw(shader);
        glDisable(GL_CULL_FACE);
//        glEnable(GL_BLEND);
        shader.setMat4("model", insideModel);
        insideObj.Draw(shader);
//        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
//        shader.setMat4("model", amogusModel);
//        amogusObj.Draw(shader);



        shader.setMat4("projection", projection);


        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);


        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);
        glDisable(GL_STENCIL_TEST);

        shader.setMat4("model", portalModel);
        shader.setMat4("view", view);
        portalObj.Draw(shader);


        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_BLEND);


        shader.setMat4("model", model);
        shader.setMat4("view", view);
        tardisObj.Draw(shader);
//        glDisable(GL_CULL_FACE);
////        glEnable(GL_BLEND);
//        shader.setMat4("model", insideModel);
//        insideObj.Draw(shader);
////        glDisable(GL_BLEND);
//        glEnable(GL_CULL_FACE);
//        shader.setMat4("model", amogusModel);
//        amogusObj.Draw(shader);




        // draw planet
        shader.use();
        glm::mat4 planetModel = glm::mat4(1.0f);
        planetModel = glm::translate(planetModel, glm::vec3(0.0f, -3.0f, 0.0f));
        planetModel = glm::scale(planetModel, glm::vec3(6.0f, 6.0f, 6.0f));
        shader.setMat4("model", planetModel);
        planetObj.Draw(shader);
        glm::mat4 sunModel = glm::mat4(1.f);
        sunModel = glm::translate(sunModel, sunPos);
        sunModel = glm::scale(sunModel, glm::vec3(20.0f, 20.0f, 20.0f));
        shader.setMat4("model", sunModel);
        lightSourceShader.use();
        lightSourceShader.setMat4("projection", projection);
        lightSourceShader.setMat4("view", view);
        lightSourceShader.setMat4("model", sunModel);
        lightSourceShader.setVec3("brightness", glm::vec3(50.f, 40.f, 3.f));
        sunObj.Draw(lightSourceShader);

        glm::mat4 redLightModel = glm::translate(glm::mat4(1.f), tardisPos + pointLightPositions[0]);
        redLightModel = glm::scale(redLightModel, glm::vec3(0.2f, 0.2f, 0.2f));
        lightSourceShader.setMat4("model", redLightModel);
        lightSourceShader.setVec3("brightness", glm::vec3((float)(glm::cos(5 *glfwGetTime()) + 1.f) / 2.f * 10.f, (float)(glm::cos(5 *glfwGetTime()) + 1.f) / 2.f * 1.f, (float)(glm::cos(5 *glfwGetTime()) + 1.f) / 2.f * 1.f));
        valjakObj.Draw(lightSourceShader);

        // draw meteorites
        instanceShader.use();
        instanceShader.setMat4("projection", projection);
        instanceShader.setMat4("view", view);
        instanceShader.setInt("texture_diffuse1", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, rockObj.textures_loaded[0].id); // note: we also made the textures_loaded vector public (instead of private) from the model class.
        for (unsigned int i = 0; i < rockObj.meshes.size(); i++)
        {

            glBindVertexArray(rockObj.meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(rockObj.meshes[i].indices.size()), GL_UNSIGNED_INT, 0, amount);
            glBindVertexArray(0);
        }


        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        skyboxShader.setMat4("view", glm::rotate(glm::mat4(glm::mat3(view)),(float)glfwGetTime() * 0.01f, glm::vec3(1.0f, 1.0f , 0.0f)));
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        bool horizontal = true, first_iteration = true;
        int amount = 10;
        shaderBlur.use();
        for (unsigned int i = 0; i < amount; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            shaderBlur.setInt("horizontal", horizontal);
            glBindTexture(
                    GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongBuffer[!horizontal]
            );
            renderQuad();
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        hdrShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[!horizontal]);
        renderQuad();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
                // positions        // texture Coords
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
void processInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        playerSpeed = 10.0f;
        fov = 51.0f;
    }
    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
    {
        playerSpeed = 2.5f;
        fov = 45.0f;
    }
    float cameraSpeed = playerSpeed * deltaTime;
    glm::vec3 front = glm::vec3(cameraFront.x, 0.0f, cameraFront.z);
    front = glm::normalize(front);
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos += cameraSpeed * front;
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos -= cameraSpeed * front;
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPos -= glm::normalize(glm::cross(front, cameraUp)) * cameraSpeed;
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPos += glm::normalize(glm::cross(front, cameraUp)) * cameraSpeed;
    }
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        cameraPos += glm::vec3(0.0f, cameraSpeed, 0.0f);
    }
    if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        cameraPos -= glm::vec3(0.0f, cameraSpeed, 0.0f);
    }
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;
    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if(fov < 1.0f)
        fov = 1.0f;
    if(fov > 45.0f)
        fov = 45.0f;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
