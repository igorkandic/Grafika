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
unsigned int loadTexture(const char *path);
void drawObjectsWithoutPortals();

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 lightPosition = glm::vec3(0.0f, 3.0f, 0.0f);
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float lastX = 400, lastY = 300;
float yaw = -90.0f, pitch = 0.0f;
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


    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
            glm::vec3( 0.7f,  0.2f,  2.0f),
            glm::vec3( 2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f,  2.0f, -12.0f),
            glm::vec3( 0.0f,  0.0f, -3.0f)
    };
    Model tardisObj = Model("resources/objects/War tardis/War tardis.obj");
    Model portalObj = Model("resources/objects/portal/portal.obj");
    Model amogusObj = Model("resources/objects/amogus/Among Us2.obj");
    Model cubeObj = Model("resources/objects/cube/cube.obj");






    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    const char* vendor = (const char *)(glGetString(GL_VENDOR));
    if(strcmp(vendor, "NVIDIA") == 0)
    {
        std::cout << "F&*K Y(! NVIDIA" << std::endl;
        return -1;
    }
    while(!glfwWindowShouldClose(window)){
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//        glClearColor(.1f, .1f, .1f, 1.f);
        shader.use();
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, .1f, 100.0f);
        shader.setMat4("projection", projection);
        glm::mat4 view;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        shader.setMat4("view", view);

        glm::mat4 insideModel = glm::mat4(1.f);
        insideModel = glm::translate(insideModel, glm::vec3(25.f, 0.f, 0.f));
        insideModel = glm::scale(insideModel, glm::vec3(5.f, 5.f, 5.f));


        glm::mat4 amogusModel = glm::mat4(1.f);
        amogusModel = glm::translate(amogusModel, glm::vec3(25.f, 0.f, 0.f));
        amogusModel = glm::scale(amogusModel, glm::vec3(0.3f, .3f, .3f));
        amogusModel = glm::rotate(amogusModel, (float)glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));

        glm::mat4 destModel = glm::mat4(1.f);
        destModel = glm::translate(destModel, glm::vec3(22.0, 2.5, 0.0));


        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
        glStencilOp(GL_INCR, GL_KEEP, GL_KEEP);
        glStencilMask(0xFF);

        glm::mat4 portalModel = glm::mat4(1.0f);
        portalModel = glm::translate(portalModel, glm::vec3(2.f, 2.f, 0.f));
        portalModel = glm::scale(portalModel, glm::vec3(2.f, 2.f, 2.f));
        shader.setMat4("model", portalModel);
        portalObj.Draw(shader);

        glm::mat4 destView = view * portalModel * glm::rotate(glm::mat4(1.0), glm::radians(180.0f), glm::vec3(0.0,1.0,0.0)) * glm::inverse(destModel);

        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDepthMask(GL_TRUE);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glStencilMask(0xFF);
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        glStencilOp(GL_DECR, GL_KEEP, GL_KEEP);
        glm::mat4 clippedProjection;
        clippedProjection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, glm::distance(cameraPos, glm::vec3(2.f, 2.f, 0.f)), 100.0f);
        shader.setMat4("projection", clippedProjection);
        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("model", model);
        shader.setMat4("view", destView);
        tardisObj.Draw(shader);
        shader.setMat4("model", insideModel);
        cubeObj.Draw(shader);
        shader.setMat4("model", amogusModel);
        amogusObj.Draw(shader);
        shader.setMat4("projection", projection);

        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);


        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);
        glDisable(GL_STENCIL_TEST);

        shader.setMat4("model", portalModel);
        shader.setMat4("view", view);
        portalObj.Draw(shader);


        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        shader.setMat4("model", model);
        shader.setMat4("view", view);
        tardisObj.Draw(shader);
        shader.setMat4("model", insideModel);
        cubeObj.Draw(shader);
        shader.setMat4("model", amogusModel);
        amogusObj.Draw(shader);




        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
void setLights(Shader shader)
{

}
void drawObjectsWithoutPortals(Shader shader, glm::vec3 pointLightPositions){
//    glEnable(GL_DEPTH_TEST);
//    glClearColor(.1f, .1f, .1f, 1.f);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    shader.use();
//    // directional light
//    shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
//    shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
//    shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
//    shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
//    // point light 1
//    shader.setVec3("pointLights[0].position", pointLightPositions[0]);
//    shader.setVec3("pointLights[0].ambient", 0.1f, 0.05f, 0.05f);
//    shader.setVec3("pointLights[0].diffuse", 1.0f, 0.0f, 0.0f);
//    shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
//    shader.setFloat("pointLights[0].constant", 1.0f);
//    shader.setFloat("pointLights[0].linear", 0.09f);
//    shader.setFloat("pointLights[0].quadratic", 0.032f);
//    // point light 2
//    shader.setVec3("pointLights[1].position", pointLightPositions[1]);
//    shader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
//    shader.setVec3("pointLights[1].diffuse", 0.f, 1.0f, 0.f);
//    shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
//    shader.setFloat("pointLights[1].constant", 1.0f);
//    shader.setFloat("pointLights[1].linear", 0.09f);
//    shader.setFloat("pointLights[1].quadratic", 0.032f);
//    // point light 3
//    shader.setVec3("pointLights[2].position", pointLightPositions[2]);
//    shader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
//    shader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
//    shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
//    shader.setFloat("pointLights[2].constant", 1.0f);
//    shader.setFloat("pointLights[2].linear", 0.09f);
//    shader.setFloat("pointLights[2].quadratic", 0.032f);
//    // point light 4
//    shader.setVec3("pointLights[3].position", pointLightPositions[3]);
//    shader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
//    shader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
//    shader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
//    shader.setFloat("pointLights[3].constant", 1.0f);
//    shader.setFloat("pointLights[3].linear", 0.09f);
//    shader.setFloat("pointLights[3].quadratic", 0.032f);
//    // spotLight
//    shader.setVec3("spotLight.position", cameraPos);
//    shader.setVec3("spotLight.direction", cameraFront);
//    shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
//    shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
//    shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
//    shader.setFloat("spotLight.constant", 1.0f);
//    shader.setFloat("spotLight.linear", 0.09f);
//    shader.setFloat("spotLight.quadratic", 0.032f);
//    shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
//    shader.setFloat("spotLight.focus", 128);
//
//    glm::mat4 projection;
//    projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, .1f, 100.0f);
//    shader.setMat4("projection", projection);
//    glm::mat4 view;
//    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
//    shader.setMat4("view", view);
//    // world transformation
//    glm::mat4 model = glm::mat4(1.0f);
////        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 1.0f, 0.0f));
//    shader.setMat4("model", model);
//    ourModel.Draw(shader);
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
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}