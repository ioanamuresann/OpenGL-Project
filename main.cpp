#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"
#include "Mesh.hpp"

#include <iostream>

//ceata
int fog_enable = 0;
GLint fog_location;
GLfloat fog_density = 0.02f;

// window
gps::Window myWindow;
int glWindowWidth = 1700;
int glWindowHeight = 950;

// for skybox
std::vector<const GLchar*> faces;
gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;
glm::mat4 birdMatrix;
glm::mat4 snowMatrix;
glm::mat4 carMatrix;
glm::mat4 dogMatrix;
glm::mat4 fallingMatrix;
glm::mat4 boatMatrix;
glm::mat4 lightRotation;
glm::mat4 auxMatrix;
glm::mat4 ghioceiMatrix;
glm::mat4 ghiocelMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;

// camera
gps::Camera myCamera(
    glm::vec3(2.0f, 5.0f, -15.0f),
    glm::vec3(-4.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));
GLfloat cameraSpeed = 0.1f;

GLboolean pressedKeys[1024];

// models
gps::Model3D myScene;
gps::Model3D bird;
gps::Model3D car;
gps::Model3D dog;
gps::Model3D snow1;
gps::Model3D snow2;
gps::Model3D snow3;
gps::Model3D snow4;
gps::Model3D snow5;
gps::Model3D snow6;
gps::Model3D snow7;
gps::Model3D snow8;
gps::Model3D falling1;
gps::Model3D falling2;
gps::Model3D falling3;
gps::Model3D falling4;
gps::Model3D falling5;
gps::Model3D falling6;
gps::Model3D falling7;
gps::Model3D boat;
gps::Model3D ghiocei;
gps::Model3D ghiocel;

// miscare caine
float miscareDS;
float miscareFS;

GLfloat angle;

//miscare pasare sus
GLfloat birdMovement = 0.0f;

//miscare masina
GLfloat carTranslation = 0.0f;

//miscare fulgi
GLfloat snowFalling = 0.0f;

//miscare ghiocel
GLfloat ghiocelFalling = 0.0f;
GLfloat ghiocelFalling2 = 0.0f;

// shaders
gps::Shader myBasicShader;
gps::Shader lightShader;
gps::Shader depthMapShader;

//for mouse movements
GLboolean mouseMoves = true;
GLfloat xposLast = glWindowWidth / 2.0f;
GLfloat yposLast = glWindowHeight / 2.0f;

//animatie camera
bool startAnimatieCamera = false;
float cameraAnimation;

//pentru a controla miscarea copacilor(simulare vant)
float wind2;
float wind1;
float wind3;
float wind4;
float wind5;

//lumina de tip spot
int spot_enable;
float lumina_spot1;
float lumina_spot2;
glm::vec3 directieLuminaSpot;
glm::vec3 pozitieLuminaSpot;


// lumina punctiforma
int luminaPunctiformaEnable = 0;
glm::vec3 luminaPunctiforma;
GLuint locatieLuminaPunctiforma;

//pentru umbre
const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;
GLuint shadowMapFBO;
GLuint depthMapTexture;
GLfloat unghiLumina;


//functie pentru animatie camera
void cameraAnimationFunction() {
    if (startAnimatieCamera) {
        if (cameraAnimation < 50) {
            myCamera.move(gps::MOVE_RIGHT, 0.05f);
            cameraAnimation += 1.0f;
        }
        else if (cameraAnimation < 100)
        {

            myCamera.move(gps::MOVE_FORWARD, 0.1f);
            cameraAnimation += 1.0f;
        }
        else if (cameraAnimation < 150)
        {

            myCamera.move(gps::MOVE_BACKWARD, 0.05f);
            cameraAnimation += 1.0f;
        }
        else if (cameraAnimation < 200)
        {

            myCamera.move(gps::MOVE_LEFT, 0.05f);
            cameraAnimation += 1.0f;
        }
        else {
            cameraAnimation = 0;
        }


        angle += 0.55f;
        // update model matrix for the scene
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for the scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }
}

GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            error = "STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            error = "STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
    myBasicShader.useShaderProgram();
    //actualizare matrice de proiectie
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);
    GLint projLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glViewport(0, 0, width, height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}
//pentru skybox
void initFaces()
{
    faces.push_back("skybox/posx.jpg");
    faces.push_back("skybox/negx.jpg");
    faces.push_back("skybox/posy.jpg");
    faces.push_back("skybox/negy.jpg");
    faces.push_back("skybox/posz.jpg");
    faces.push_back("skybox/negz.jpg");

}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (mouseMoves)
    {
        xposLast = xpos;
        yposLast = ypos;
        mouseMoves = false;
    }

    GLfloat offset_X = xpos - xposLast;
    GLfloat offset_Y = yposLast - ypos;

    xposLast = xpos;
    yposLast = ypos;

    myCamera.rotate(offset_X, offset_Y);
}

void processMovement() {

    // move camera forward
    if (pressedKeys[GLFW_KEY_W])
    {
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    // move camera backward
    if (pressedKeys[GLFW_KEY_S])
    {
        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    // move camera left
    if (pressedKeys[GLFW_KEY_A])
    {
        myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    // move camera right
    if (pressedKeys[GLFW_KEY_D])
    {
        myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    // rotate camera to the left
    if (pressedKeys[GLFW_KEY_Q])
    {
        angle -= 1.0f;
        // update model matrix for myScene
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for myScene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    // rotate camera to the right
    if (pressedKeys[GLFW_KEY_R])
    {
        angle += 1.0f;
        // update model matrix for myScene
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for myScene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }
   //pentru a misca lumina
    if (pressedKeys[GLFW_KEY_K]) {
        unghiLumina -= 0.8f;
        auxMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(unghiLumina), glm::vec3(0, 1, 0));
        myBasicShader.useShaderProgram();
        normalMatrix = glm::mat3(glm::inverseTranspose(view * auxMatrix));
    }
    //pentru a misca lumina
    if (pressedKeys[GLFW_KEY_L]) {
        unghiLumina += 0.8f;
        auxMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(unghiLumina), glm::vec3(0, 1, 0));
        myBasicShader.useShaderProgram();
        normalMatrix = glm::mat3(glm::inverseTranspose(view * auxMatrix));
    }
    
    // start preview
    if (pressedKeys[GLFW_KEY_1]) {
        startAnimatieCamera = true;
    }

    // stop preview
    if (pressedKeys[GLFW_KEY_2]) {
        startAnimatieCamera = false;
    }
    //wireframe
    if (pressedKeys[GLFW_KEY_7]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    //poligonal
    if (pressedKeys[GLFW_KEY_8]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }

    //solid mode
    if (pressedKeys[GLFW_KEY_9]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
 

    //start lumina de tip spot (far)
    if (pressedKeys[GLFW_KEY_F]) {
        myBasicShader.useShaderProgram();
        spot_enable = 1;
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "spot_enable"), spot_enable);
    }

    //stop lumina de tip spot (far)
    if (pressedKeys[GLFW_KEY_G]) {
        myBasicShader.useShaderProgram();
        spot_enable = 0;
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "spot_enable"), spot_enable);

    }
    //start lumina punctiforma
    if (pressedKeys[GLFW_KEY_H]) {
        myBasicShader.useShaderProgram();
        luminaPunctiformaEnable = 1;
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "luminaPunctiformaEnable"), luminaPunctiformaEnable);
    }

    //stop lumina punctiforma
    if (pressedKeys[GLFW_KEY_J]) {
        myBasicShader.useShaderProgram();
        luminaPunctiformaEnable = 0;
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "luminaPunctiformaEnable"), luminaPunctiformaEnable);
    }

    //porneste ceata
    if (pressedKeys[GLFW_KEY_V]) {
        myBasicShader.useShaderProgram();
        fog_enable = 1;
        fog_location = glGetUniformLocation(myBasicShader.shaderProgram, "fog_enable");
        glUniform1i(fog_location, fog_enable);

    }

    //opreste ceata
    if (pressedKeys[GLFW_KEY_B]) {
        myBasicShader.useShaderProgram();
        fog_enable = 0;
        fog_location = glGetUniformLocation(myBasicShader.shaderProgram, "fog_enable");
        glUniform1i(fog_location, fog_enable);

    }

    //miscare stanga caine
    if (pressedKeys[GLFW_KEY_LEFT]) {
        if (miscareDS < 100)
            miscareDS += 0.05;
    }

    //miscare dreapta caine
    if (pressedKeys[GLFW_KEY_RIGHT]) {
        if (miscareDS < 100)
            miscareDS -= 0.05;
    }

    //miscare fata caine
    if (pressedKeys[GLFW_KEY_UP]) {
        if (miscareFS < 100)
            miscareFS += 0.05;
    }

    //miscare spate caine
    if (pressedKeys[GLFW_KEY_DOWN]) {
        if (miscareFS < 100)
            miscareFS -= 0.05;
    }

    //cadere fulgi
    if (pressedKeys[GLFW_KEY_Y]) {
        if (snowFalling >= -6)
            snowFalling -= 0.04;
        else
            snowFalling = 0;
    }
    //cadere ghiocel
    if (pressedKeys[GLFW_KEY_U]) {
        if (ghiocelFalling <= 0.3)
            ghiocelFalling += 0.004;
        else if (ghiocelFalling2 >= -90.0)
            ghiocelFalling2 -= 0.7;
    }
    
}

void initOpenGLWindow() {
    myWindow.Create(1024, 768, "OpenGL Project Core");
}

void setWindowCallbacks() {
    glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
}

void initOpenGLState() {

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    //glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK); // cull back face
    glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



}

void initModels() {
    myScene.LoadModel("models/scena/final.obj");
    bird.LoadModel("models/scena/bird/birdh.obj");
    car.LoadModel("models/scena/audi/audi.obj");
    dog.LoadModel("models/scena/dog/dog.obj");
    snow1.LoadModel("models/scena/coroanaCopac/treeGC.obj");
    snow2.LoadModel("models/scena/coroanaCopac/treeGC.obj");
    snow3.LoadModel("models/scena/coroanaCopac/treeGC.obj");
    snow4.LoadModel("models/scena/coroanaCopac/treeGC.obj");
    snow5.LoadModel("models/scena/coroanaCopac/treeGC.obj");
    snow6.LoadModel("models/scena/coroanaCopac/treeGC.obj");
    snow7.LoadModel("models/scena/coroanaCopac/treeGC.obj");
    snow8.LoadModel("models/scena/coroanaCopac/treeGC.obj");
    falling1.LoadModel("models/scena/ninsoare/ninsoare.obj");
    falling2.LoadModel("models/scena/ninsoare/ninsoare.obj");
    falling3.LoadModel("models/scena/ninsoare/ninsoare.obj");
    falling4.LoadModel("models/scena/ninsoare/ninsoare.obj");
    falling5.LoadModel("models/scena/ninsoare/ninsoare.obj");
    falling6.LoadModel("models/scena/ninsoare/ninsoare.obj");
    falling7.LoadModel("models/scena/ninsoare/ninsoare.obj");
    boat.LoadModel("models/scena/boat/boat2.obj");
    ghiocel.LoadModel("models/scena/snowdrop/ghiocel.obj");
    ghiocei.LoadModel("models/scena/snowdrop/ghiocei.obj");
}

void initShaders() {
    myBasicShader.loadShader("shaders/basic.vert", "shaders/basic.frag");
    lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
    lightShader.useShaderProgram();
    depthMapShader.loadShader("shaders/depthMap.vert", "shaders/depthMap.frag");
    depthMapShader.useShaderProgram();
}

//Pentru a atasa o textura ca un buffer de adancime pentru obiectul framebuffer, trebuie mai intai sa cream textura
void initFBO() {
    //TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
    //generate FBO ID
    glGenFramebuffers(1, &shadowMapFBO);

    //create depth texture for FBO
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
    //odata ce textura de adancime este creata, ea trebuie atasata ca buffer de adancime pentru obiectul framebuffer:
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
    //Deoarece prima trecere a algoritmului de mapare a umbrelor nu necesita atasament de coloare sau sablon,dar un obiect framebuffer nu ar fi complet fara ele, putem sa atribuim in mod explicit nimic acestor puncte de atasament
    
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    //Odata ce un obiect framebuffer este complet, ar trebui sa il dezactivam pana cand suntem gata sa il folosim:
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

glm::mat4 computeLightSpaceTrMatrix() {
    //TODO - Return the light-space transformation matrix
    //actualizare matricea de view a camerei
    glm::mat4 lightView = glm::lookAt(glm::mat3(lightRotation) *lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //constante 
    const GLfloat near_plane = 0.9f, far_plane = 15.0f;
    //actualizare matricea de proiectie ortogonala
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    //urmeaza a fi trimisa spre shadererul pentru umbre
    return lightSpaceMatrix;
}
void initUniforms() {
    myBasicShader.useShaderProgram();

    // create model matrix for scene
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

    // get view matrix for current camera
    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
    // send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for scene
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

    // create projection matrix
    projection = glm::perspective(glm::radians(45.0f),(float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height, 0.1f, 20.0f);
    projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
    // send projection matrix to shader
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //set the light direction (direction towards the light)
    lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
    lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
    // send light dir to shader
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

    //set light color
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
    lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
    // send light color to shader
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    //lumina de tip spot
    lumina_spot1 = glm::cos(glm::radians(40.5f));
    directieLuminaSpot = glm::vec3(0, -1, 0);
    pozitieLuminaSpot = glm::vec3(-0.5f, 0.6f, 0.0f);
    lumina_spot2 = glm::cos(glm::radians(100.5f));

    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "lumina_spot1"), lumina_spot1);
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "lumina_spot2"), lumina_spot2);
    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "directieLuminaSpot"), 1, glm::value_ptr(directieLuminaSpot));
    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "pozitieLuminaSpot"), 1, glm::value_ptr(pozitieLuminaSpot));


    //lumina punctiforma
    luminaPunctiforma = glm::vec3(-9.5f, 3.5f, 2.0f);
    //locatia luminii
    locatieLuminaPunctiforma = glGetUniformLocation(myBasicShader.shaderProgram, "luminaPunctiforma");
    glUniform3fv(locatieLuminaPunctiforma, 1, glm::value_ptr(luminaPunctiforma));

    //ceata
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "fog_density"), fog_density);

}

void renderObjects(gps::Shader shader, bool depthPass) {
    //activez shaderul potrivit
    shader.useShaderProgram();

    //trimit matricea de model catre locatia sa din shaderul primit ca si parametru
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    //trimit matricea normala catre locatia sa din shader doar daca respecta conditia de depthPass(ajuta in cazul umbrelor)
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    //desenez scena(terenul si toate obiectele statice aranjate in blender)
    myScene.Draw(shader);

    //desenez barca de pe lac,care se va misca 
    boatMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
    //folosesc variabila wind1 si wind5 pentru a simula efectul de plutire a barcii pe apa
    boatMatrix = glm::translate(boatMatrix, glm::vec3(5.7f, -0.2f + wind5, -6.6f + wind1));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(boatMatrix));
    boat.Draw(shader);

    //desenez a doua barca de pe lac,care se va misca 
    boatMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
    //folosesc variabila wind1 si wind5 pentru a simula efectul de plutire a barcii pe apa
    boatMatrix = glm::translate(boatMatrix, glm::vec3(6.7f + wind1, -0.21f + wind5, -7.6f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(boatMatrix));
    boat.Draw(shader);

    //desenez ghiocelul care creste

   ghiocelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
   ghiocelMatrix = glm::translate(ghiocelMatrix, glm::vec3(1.7f, -0.3f+ghiocelFalling , -6.6f));
   ghiocelMatrix = glm::rotate(ghiocelMatrix, glm::radians(ghiocelFalling2), glm::vec3(1, 0, 0));
   ghiocelMatrix = glm::scale(ghiocelMatrix, glm::vec3(0.6f, 0.6f, 0.6f));
   glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(ghiocelMatrix));
   ghiocel.Draw(shader);

   //desenez ghiocei
   ghioceiMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
   ghioceiMatrix = glm::translate(ghioceiMatrix, glm::vec3(1.8f, 0.0f , -6.4f));
   ghioceiMatrix = glm::scale(ghioceiMatrix, glm::vec3(0.6f, 0.6f, 0.6f));
   glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(ghioceiMatrix));
   ghiocei.Draw(shader);


    //desenez pasarea care se va roti deasupra scenei continuu
    birdMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
    //folosesc variabila birdMovement care creste constat cand se ruleaza aplicatia
    birdMatrix = glm::rotate(birdMatrix, glm::radians(birdMovement), glm::vec3(0, 1, 0));
    birdMatrix = glm::translate(birdMatrix, glm::vec3(2.0f, 5.0f, 0.0f));
    birdMatrix = glm::rotate(birdMatrix, glm::radians(-120.0f), glm::vec3(0, 1, 0));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(birdMatrix));
    bird.Draw(shader);

    //desenez masina care se va misca pe drum
    carMatrix = glm::mat4(1.0f);
    carMatrix = glm::rotate(carMatrix, glm::radians(-90.0f), glm::vec3(0, 1, 0));
    carMatrix = glm::scale(carMatrix, glm::vec3(0.24f, 0.3f, 0.3f));
    carMatrix = glm::translate(carMatrix, glm::vec3(8.1f, 0.5f, -30.0f));
    //folosesc variabila carTranslation pentru a simula faptul ca masina se misca in linie dreapta pe sosea
    carMatrix = glm::translate(carMatrix, glm::vec3(0.0f, 0.0f, carTranslation));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(carMatrix));
    car.Draw(shader);

    //desenez cainele
    dogMatrix = glm::mat4(1.0f);
    dogMatrix = glm::rotate(dogMatrix, glm::radians(angle), glm::vec3(0, 1, 0));
    dogMatrix = glm::rotate(dogMatrix, glm::radians(-10.0f), glm::vec3(0, 1, 0));
    //folosesc doua constante miscareDS(dreapta-stanga)pentru a misca cainele pe axa x si miscareFS(fata-spate) pentru a misca cainele pe axa z,la apasarea unor taste
    dogMatrix = glm::translate(dogMatrix, glm::vec3(0.4f + miscareDS, -0.05f, -6.0f + miscareFS));
    dogMatrix = glm::scale(dogMatrix, glm::vec3(0.7f, 0.7f, 0.7f));

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(dogMatrix));
    dog.Draw(shader);

    //desenez zapada pe copaci si folosesc wind pentru a simula vantul
    snowMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
    snowMatrix = glm::translate(snowMatrix, glm::vec3(-2.2f + wind1, -1.0f, -1.4f));
    snowMatrix = glm::scale(snowMatrix, glm::vec3(1.4));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(snowMatrix));
    snow2.Draw(shader);

    //desenez zapada pe copaci si folosesc wind pentru a simula vantul
    snowMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
    snowMatrix = glm::translate(snowMatrix, glm::vec3(7.0f, -1.0f, -13.0f + wind2));
    snowMatrix = glm::scale(snowMatrix, glm::vec3(2.0));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(snowMatrix));
    snow1.Draw(shader);

    //desenez zapada pe copaci si folosesc wind pentru a simula vantul
    snowMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
    snowMatrix = glm::translate(snowMatrix, glm::vec3(-3.5f + wind3, -0.3f, 1.0f));
    snowMatrix = glm::scale(snowMatrix, glm::vec3(0.7));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(snowMatrix));
    snow3.Draw(shader);

    //desenez zapada pe copaci si folosesc wind pentru a simula vantul
    snowMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
    snowMatrix = glm::translate(snowMatrix, glm::vec3(-2.3f + wind1, -0.0f, -8.0f));
    snowMatrix = glm::scale(snowMatrix, glm::vec3(1.1f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(snowMatrix));
    snow4.Draw(shader);

    //desenez zapada pe copaci si folosesc wind pentru a simula vantul
    snowMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
    snowMatrix = glm::translate(snowMatrix, glm::vec3(5.9f, -1.0f, 4.0f + wind2));
    snowMatrix = glm::scale(snowMatrix, glm::vec3(1.1f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(snowMatrix));
    snow5.Draw(shader);

    //desenez zapada pe copaci si folosesc wind pentru a simula vantul
    snowMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
    snowMatrix = glm::translate(snowMatrix, glm::vec3(2.9f, -1.0f, 0.8f + wind1));
    snowMatrix = glm::scale(snowMatrix, glm::vec3(1.1f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(snowMatrix));
    snow6.Draw(shader);

    //desenez zapada pe copaci si folosesc wind pentru a simula vantul
    snowMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
    snowMatrix = glm::translate(snowMatrix, glm::vec3(-9.0f, -1.0f, -6.1f + wind1));
    snowMatrix = glm::scale(snowMatrix, glm::vec3(1.8f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(snowMatrix));
    snow7.Draw(shader);

    //desenez zapada pe copaci si folosesc wind pentru a simula vantul
    snowMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
    snowMatrix = glm::translate(snowMatrix, glm::vec3(2.5f, -0.7f, -3.7f + wind3));
    snowMatrix = glm::scale(snowMatrix, glm::vec3(0.8f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(snowMatrix));
    snow8.Draw(shader);

    //desenez fulgii din copaci si folosesc variabila snowFalling pentru a simula efectul de ninsoare
    fallingMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
    fallingMatrix = glm::translate(fallingMatrix, glm::vec3(-1.8f + wind4, 0.9f, -1.5f + wind4));
    fallingMatrix = glm::scale(fallingMatrix, glm::vec3(0.3f));
    fallingMatrix = glm::translate(fallingMatrix, glm::vec3(0.0f, snowFalling, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(fallingMatrix));
    falling1.Draw(shader);

    //desenez fulgii din copaci si folosesc variabila snowFalling pentru a simula efectul de ninsoare
    fallingMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
    fallingMatrix = glm::translate(fallingMatrix, glm::vec3(-2.0f + wind4, 0.8f, -1.4f + wind4));
    fallingMatrix = glm::scale(fallingMatrix, glm::vec3(0.5f));
    fallingMatrix = glm::translate(fallingMatrix, glm::vec3(0.0f, snowFalling, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(fallingMatrix));
    falling2.Draw(shader);

    //desenez fulgii din copaci si folosesc variabila snowFalling pentru a simula efectul de ninsoare
    fallingMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
    fallingMatrix = glm::translate(fallingMatrix, glm::vec3(-2.1f, 1.0f + wind4, -1.6f + wind4));
    fallingMatrix = glm::scale(fallingMatrix, glm::vec3(0.4f));
    fallingMatrix = glm::translate(fallingMatrix, glm::vec3(0.0f, snowFalling, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(fallingMatrix));
    falling3.Draw(shader);

    //desenez fulgii din copaci si folosesc variabila snowFalling pentru a simula efectul de ninsoare
    fallingMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
    fallingMatrix = glm::translate(fallingMatrix, glm::vec3(-2.2f + wind4, 1.2f + wind4, -2.0f + wind4));
    fallingMatrix = glm::scale(fallingMatrix, glm::vec3(0.5f));
    fallingMatrix = glm::translate(fallingMatrix, glm::vec3(0.0f, snowFalling, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(fallingMatrix));
    falling4.Draw(shader);

    //desenez fulgii din copaci si folosesc variabila snowFalling pentru a simula efectul de ninsoare
    fallingMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
    fallingMatrix = glm::translate(fallingMatrix, glm::vec3(-2.1f, 1.1f, -1.6f + wind4));
    fallingMatrix = glm::scale(fallingMatrix, glm::vec3(0.4f));
    fallingMatrix = glm::translate(fallingMatrix, glm::vec3(0.0f, snowFalling, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(fallingMatrix));
    falling5.Draw(shader);

    //desenez fulgii din copaci si folosesc variabila snowFalling pentru a simula efectul de ninsoare
    fallingMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
    fallingMatrix = glm::translate(fallingMatrix, glm::vec3(-2.0f + wind4, 1.0f, -1.9f));
    fallingMatrix = glm::scale(fallingMatrix, glm::vec3(0.3f));
    fallingMatrix = glm::translate(fallingMatrix, glm::vec3(0.0f, snowFalling, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(fallingMatrix));
    falling6.Draw(shader);

    ///desenez fulgii din copaci si folosesc variabila snowFalling pentru a simula efectul de ninsoare
    fallingMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
    fallingMatrix = glm::translate(fallingMatrix, glm::vec3(-1.5f, 0.9f, -1.6f + wind4));
    fallingMatrix = glm::scale(fallingMatrix, glm::vec3(0.4f));
    fallingMatrix = glm::translate(fallingMatrix, glm::vec3(0.0f, snowFalling, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(fallingMatrix));
    falling7.Draw(shader);

 

}

void renderScene() {
  
    // pentru a face vant diferit copacilor
    wind1 = sin(glfwGetTime()) * 0.05f;
    wind2 = sin(glfwGetTime()) * 0.07f;
    wind3 = sin(glfwGetTime()) * 0.01f;
    wind4 = sin(glfwGetTime()) * 0.1f;
    wind5 = sin(glfwGetTime()) * 0.009f;

    //apelez functia de animatie a camerei
    cameraAnimationFunction();

    //Rasterizarea in harta de adancime
    depthMapShader.useShaderProgram();

    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"), 1,GL_FALSE,glm::value_ptr(computeLightSpaceTrMatrix()));
    //aplic width si height pentru umbre,pentru o rezolutie buna
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    //desenez scena folosind shaderul de umbre
    renderObjects(depthMapShader,true);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    myBasicShader.useShaderProgram();
    //preiau matricea de camera
    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    //actualizez matricea de rotatie
    lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(unghiLumina), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

    //bind the shadow map
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 3);
    glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"),1,GL_FALSE,glm::value_ptr(computeLightSpaceTrMatrix()));

    //desenez scena folosind shaderul principal
    renderObjects(myBasicShader, false);

    //activez shaderul de lumina
    lightShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    auxMatrix = lightRotation;
    auxMatrix = glm::translate(auxMatrix, 1.0f * lightDir);
    auxMatrix = glm::scale(auxMatrix, glm::vec3(0.1f, 0.1f, 0.1f));
    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(auxMatrix));

    //desenz skyboxul
    mySkyBox.Draw(skyboxShader, view, projection);


}
void initSkyBoxShader()
{
    mySkyBox.Load(faces);
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    skyboxShader.useShaderProgram();
    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
        glm::value_ptr(view));

    projection = glm::perspective(glm::radians(45.0f), (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height, 0.1f, 1000.0f);
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
        glm::value_ptr(projection));
}

void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char* argv[]) {

    try {
        initOpenGLWindow();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
    initModels();
    initFBO();
    initShaders();
    initUniforms();
    setWindowCallbacks();

    //pentru skybox
    initFaces();
    initSkyBoxShader();

    glCheckError();
    // application loop
    while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processMovement();
        renderScene();
        //pentru ca pasarea sa se roteasca in cerc,in mod continuu
        if (birdMovement < 360.0f) {
            birdMovement += 1.0f;
        }
        else {
            birdMovement = 0;
        }
        //pentru ca masina sa se miste continuu,tot incrementez valoarea
        if (carTranslation < 60.0f) {
            carTranslation += 0.3f;
        }
        else {
            carTranslation = 500;
        }

        glfwPollEvents();
        glfwSwapBuffers(myWindow.getWindow());

        glCheckError();
    }

    cleanup();

    return EXIT_SUCCESS;
}
