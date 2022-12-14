#pragma comment(lib, "winmm.lib")
#include "playground.h"

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cassert>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include "parse_stl.h"

#include <vector>
#include <memory>
#include <random>
#include<windows.h>

//include time
#include <time.h>

#include "gameLogic/GameObject.h"
#include "gameLogic/scene1/LightingDemoObj.h"
#include "gameLogic/scene2/RectangleObj.h"


std::vector< std::shared_ptr<GameObject> > gameObjects;
float applicationStartTimeStamp; //time stamp of application start
glm::vec3 startPos = glm::vec3(50, 20, 0);
glm::vec3 endPosScene1 = glm::vec3(-0.3, 6.7, -8.8);
int switchedScene = 0;

int main(void)
{
    //Initialize window
    bool windowInitialized = initializeWindow();
    if (!windowInitialized) return -1;

	
    
   
    
    //Initialize vertex buffer
   // bool vertexbufferInitialized = initializeVertexbuffer();
    //if (!vertexbufferInitialized) return -1;
    
	//textureTest();

    glEnable(GL_DEPTH_TEST);

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders("VertexShaderScene1.vertexshader", "FragmentShaderScene1.fragmentshader");
  
    PlaySound(TEXT("../music/smooth_sailing.wav"), NULL, SND_FILENAME | SND_ASYNC);
    Sleep(200);
    applicationStartTimeStamp = (float)glfwGetTime();
	std::shared_ptr<GameObject> lightingDemoObj = std::make_shared<LightingDemoObj>(programID, "../stlFiles/Utah_teapot.stl", width/height);
	gameObjects.push_back(lightingDemoObj);

	

    createVPTransformation();

    /*curr_x = 50;
    curr_y = 20;
    curr_z = 0;*/

   curr_x = startPos.x;
   curr_y = startPos.y;
   curr_z = startPos.z;

    //start animation loop until escape key is pressed
    do {

        updateAnimationLoop();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0 && curr_time < 35.0f);


    //Cleanup and close window
    cleanupVertexbuffer();
    glDeleteProgram(programID);
    closeWindow();

    return 0;
}

void parseStl(std::vector< glm::vec3 >& vertices,
    std::vector< glm::vec3 >& normals,
    std::string stl_file_name)
{
    stl::stl_data info = stl::parse_stl(stl_file_name);
    std::vector<stl::triangle> triangles = info.triangles;
    for (int i = 0; i < info.triangles.size(); i++) {
        stl::triangle t = info.triangles.at(i);
        glm::vec3 triangleNormal = glm::vec3(t.normal.x,
            t.normal.y,
            t.normal.z);
        //add vertex and normal for point 1:
        vertices.push_back(glm::vec3(t.v1.x, t.v1.y, t.v1.z));
        normals.push_back(triangleNormal);
        //add vertex and normal for point 2:
        vertices.push_back(glm::vec3(t.v2.x, t.v2.y, t.v2.z));
        normals.push_back(triangleNormal);
        //add vertex and normal for point 3:
        vertices.push_back(glm::vec3(t.v3.x, t.v3.y, t.v3.z));
        normals.push_back(triangleNormal);
    }
}




void updateAnimationLoop()
{
    // set variable time to current time in miliseconds
	curr_time = (float)glfwGetTime() - applicationStartTimeStamp;

    switchCamera(curr_time);
    switchScenes(curr_time);
    
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use our shader
    glUseProgram(programID);

    if (glfwGetKey(window, GLFW_KEY_W)) curr_y += 0.011;
    else if (glfwGetKey(window, GLFW_KEY_S)) curr_y -= 0.011;
    else if (glfwGetKey(window, GLFW_KEY_A)) curr_x -= 0.011;
    else if (glfwGetKey(window, GLFW_KEY_D)) curr_x += 0.011;

    else if (glfwGetKey(window, GLFW_KEY_Z)) curr_z -= 0.11;
    else if (glfwGetKey(window, GLFW_KEY_T)) curr_z += 0.11;
    else if (glfwGetKey(window, GLFW_KEY_R)) curr_angle += 0.01;

	
    createVPTransformation();

    // Send our transformation to the currently bound shader, 
    // in the "MVP" uniform
    glUniformMatrix4fv(viewID, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projectionID, 1, GL_FALSE, &projection[0][0]);
    glUniform3f(cameraPosID, cameraPos.x, cameraPos.y, cameraPos.z);

    for (int i = 0; i < gameObjects.size(); i++)
    {
        gameObjects.at(i)->Update(curr_time);
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void switchCamera(float time) {
    if (time > 11.4 && time < 12.4) {
        float movementFinished = time - 11.4;;
        curr_x = (1 - movementFinished) * startPos.x + movementFinished * endPosScene1.x;
        curr_y = (1 - movementFinished) * startPos.y + movementFinished * endPosScene1.y;
        curr_z = (1 - movementFinished) * startPos.z + movementFinished * endPosScene1.z;
    }
}

void switchScenes(float time) {
    if (time > 19.4 && switchedScene < 2) {

        switchedScene = 2;
        programID = LoadShaders("VertexShaderScene2.vertexshader", "FragmentShaderScene2.fragmentshader");
        
        // create Rectangle Obj
        std::shared_ptr<GameObject> rectangleObj = std::make_shared<RectangleObj>(programID, (width / height), time);
		// clear all game objects
		gameObjects.clear();
		gameObjects.push_back(rectangleObj);
        
    }
	else if (time > 12.4 && switchedScene < 1) {
        switchedScene = 1;
        curr_x = endPosScene1.x;
        curr_y = endPosScene1.y;
        curr_z = endPosScene1.z;
        
        programID = LoadShaders("VertexShaderScene1.vertexshader", "FragmentShaderScene1Ending.fragmentshader");
		gameObjects[0]->setShaderProgramID(programID);

    }
}

bool initializeWindow()
{
    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return false;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    width = 1920;
	height = 1080;
    window = glfwCreateWindow(width, height, "Demo: Cube", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        getchar();
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return false;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.1f, 0.0f);
    return true;
}

void textureTest() {
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    vertexbuffer_size = 6;

    glm::vec2 triangleVertice1 = glm::vec2(0.0f, 0.0f);
    glm::vec2 triangleVertice2 = glm::vec2(0.0f, 1.0f);
    glm::vec2 triangleVertice3 = glm::vec2(1.0f, 1.0f);

    glm::vec2 secTriangleVertice1 = glm::vec2(0.0f, 0.0f);
    glm::vec2 secTriangleVertice2 = glm::vec2(1.0f, 1.0f);
    glm::vec2 secTriangleVertice3 = glm::vec2(1.0f, 0.0f);

    static GLfloat g_vertex_buffer_data[18];
    g_vertex_buffer_data[0] = triangleVertice1[0];
    g_vertex_buffer_data[1] = triangleVertice1[1];
    g_vertex_buffer_data[2] = 0.0f;
    g_vertex_buffer_data[3] = triangleVertice2[0];
    g_vertex_buffer_data[4] = triangleVertice2[1];
    g_vertex_buffer_data[5] = 0.0f;
    g_vertex_buffer_data[6] = triangleVertice3[0];
    g_vertex_buffer_data[7] = triangleVertice3[1];
    g_vertex_buffer_data[8] = 0.0f;
    g_vertex_buffer_data[9] = secTriangleVertice1[0];
    g_vertex_buffer_data[10] = secTriangleVertice1[1];
    g_vertex_buffer_data[11] = 0.0f;
    g_vertex_buffer_data[12] = secTriangleVertice2[0];
    g_vertex_buffer_data[13] = secTriangleVertice2[1];
    g_vertex_buffer_data[14] = 0.0f;
    g_vertex_buffer_data[15] = secTriangleVertice3[0];
    g_vertex_buffer_data[16] = secTriangleVertice3[1];
    g_vertex_buffer_data[17] = 0.0f;

    glGenBuffers(1, &vertexBuffer1D);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer1D);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);


    textureSampler2D = glGetUniformLocation(programID, "myTextureSampler");

    static const GLfloat g_uv_buffer_date[] = {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
    };

    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_date), g_uv_buffer_date, GL_STATIC_DRAW);

    static const GLubyte checkerboard_tex[] = {
       0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
       0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
       0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
       0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
       0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
       0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
       0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
       0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF
    };

    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTextureStorage2D(texID, 4, GL_R8, 8, 8);
    glTextureSubImage2D(texID,
        0,
        0, 0,
        8, 8,
        GL_RED,
        GL_UNSIGNED_BYTE,
        checkerboard_tex
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

}

/*bool initializeMVPTransformation()
{
    // Get a handle for our "MVP" uniform
    GLuint MatrixIDnew = glGetUniformLocation(programID, "MVP");
    //MatrixID = MatrixIDnew;

  //  MatrixIDM = glGetUniformLocation(programID, "M");

	timeID = glGetUniformLocation(programID, "time");


    // Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 500.0f);
    //glm::mat4 Projection = glm::frustum(-2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f);
    // Camera matrix
    glm::mat4 View = glm::lookAt(
        glm::vec3(50, 50, 0), // Camera is at (4,3,-3), in World Space
        glm::vec3(0, 0, 0), // and looks at the origin
        glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
        
    );
    // Model matrix : an identity matrix (model will be at the origin)
    glm::mat4 Model = glm::mat4(1.0f);

    Model = glm::rotate(Model, curr_angle, glm::vec3(0.0f, 1.0f, 1.0f));

    float ratio = width / height;

    glm::mat4 transformation;//additional transformation for the model
    transformation[0][0] = ratio * 1; transformation[1][0] = 0.0; transformation[2][0] = 0.0; transformation[3][0] = curr_x;
    transformation[0][1] = 0.0; transformation[1][1] = 1.0; transformation[2][1] = 0.0; transformation[3][1] = curr_y;
    transformation[0][2] = 0.0; transformation[1][2] = 0.0; transformation[2][2] = 1.0; transformation[3][2] = curr_z;
    transformation[0][3] = 0.0; transformation[1][3] = 0.0; transformation[2][3] = 0.0; transformation[3][3] = 1.0;

    // Our ModelViewProjection : multiplication of our 3 matrices
   // MVP = Projection * View * transformation * Model; // Remember, matrix multiplication is the other way around
	//M =  View * transformation * Model;

    return true;

}*/

bool createVPTransformation() {
    
    GLuint viewIDNew = glGetUniformLocation(programID, "view");
    viewID = viewIDNew;

    GLuint projectionIDNew = glGetUniformLocation(programID, "projection");
    projectionID = projectionIDNew;

	GLuint cameraPosIDNew = glGetUniformLocation(cameraPosID, "cameraPos");
	cameraPosID = cameraPosIDNew;
    
    // Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 500.0f);
    //glm::mat4 Projection = glm::frustum(-2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f);
    // Camera matrix

	cameraPos = glm::vec3(curr_x, curr_y, curr_z);
    
    glm::mat4 View = glm::lookAt(
       cameraPos, // Camera is at (4,3,-3), in World Space
        glm::vec3(0, 0, 0), // and looks at the origin
        glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)

    );

    
    view = View;

	projection = Projection;

    return true;
}

bool initializeVertexbuffer()
{
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    //create vertex and normal data
    std::vector< glm::vec3 > vertices = std::vector< glm::vec3 >();
    std::vector< glm::vec3 > normals = std::vector< glm::vec3 >();
    parseStl(vertices, normals, "../stlFiles/Utah_teapot.stl");
    vertexbuffer_size = vertices.size() * sizeof(glm::vec3);

    // print normals to console

    glGenBuffers(2, vertexbuffer); //generate two buffers, one for the vertices, one for the normals

    //fill first buffer (vertices)
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    //fill second buffer (normals)
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    return true;

}




bool cleanupVertexbuffer()
{
    // Cleanup VBO
    glDeleteVertexArrays(1, &VertexArrayID);
    return true;
}

bool cleanupColorbuffer()
{
    // Cleanup VBO
    glDeleteBuffers(1, &colorbuffer);
    return true;
}

bool closeWindow()
{
    glfwTerminate();
    return true;
}