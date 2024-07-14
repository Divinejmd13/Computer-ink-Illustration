#include "utils.h"
#include "BSPtree.h"
#include "PlanarMap.h"
#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#define STB_IMAGE_IMPLEMENTATION 
#include <stb_image.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>


// Globals
int screen_width = 640, screen_height = 640;
GLint vModel_uniform, vView_uniform, vProjection_uniform;
glm::mat4 modelT, viewT, projectionT; // The model, view, and projection transformations
double oldX, oldY, currentX, currentY;
bool isDragging = false;

void setupModelTransformation(unsigned int &);
void setupViewTransformation(unsigned int &);
void setupOrthographicProjection(unsigned int &);
void setupProjectionTransformation(unsigned int &);
struct OBJModel {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> textureCoords;
};

OBJModel loadOBJ(const char* filename) {
    OBJModel objModel;

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("File not found\n");
        exit(EXIT_FAILURE);
    }

    while (true) {
        char head[128];
        int res = fscanf(file, "%s", head);
        if (res == EOF) {
            break; // EOF = End Of File. Quit the loop.
        }

        if (strcmp(head, "v") == 0) {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            objModel.vertices.push_back(vertex);
        } else if (strcmp(head, "vt") == 0) {
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            objModel.textureCoords.push_back(uv);
        } else if (strcmp(head, "f") == 0) {
            // Process face data if needed
        }
    }

    fclose(file);
    return objModel;
}

struct CubeModel {
    // Vertices
    std::vector<glm::vec3> vertices = {
        // Front face
        {-5.0, -5.0, 5.0},
        {5.0, -5.0, 5.0},
        {5.0, 5.0, 5.0},
        {-5.0, 5.0, 5.0},
         // Back face
        {-5.0, -5.0, -5.0},
        {5.0, -5.0, -5.0},
        {5.0, 5.0, -5.0},
        {-5.0, 5.0, -5.0},

        // Top face
        {-5.0, 5.0, 5.0},
        {5.0, 5.0, 5.0},
        {5.0, 5.0, -5.0},
        {-5.0, 5.0, -5.0},

        // Bottom face
        {-5.0, -5.0, 5.0},
        {5.0, -5.0, 5.0},
        {5.0, -5.0, -5.0},
        {-5.0, -5.0, -5.0},

        // Left face
        {-5.0, -5.0, -5.0},
        {-5.0, 5.0, -5.0},
        {-5.0, 5.0, 5.0},
        {-5.0, -5.0, 5.0},

        // Right face
        {5.0, -5.0, -5.0},
        {5.0, 5.0, -5.0},
        {5.0, 5.0, 5.0},
        {5.0, -5.0, 5.0}

       
    };

    std::vector<glm::vec2> textureCoords = {
        // Front face
        {0.0, 0.0},
        {1.0, 0.0},
        {1.0, 1.0},
        {0.0, 1.0},

        // Back face
        {0.0, 0.0},
        {1.0, 0.0},
        {1.0, 1.0},
        {0.0, 1.0},

        // Top face
        {0.0, 1.0},
        {1.0, 1.0},
        {1.0, 0.0},
        {0.0, 0.0},

        // Bottom face
        {0.0, 0.0},
        {1.0, 0.0},
        {1.0, 1.0},
        {0.0, 1.0},

        // Left face
        {0.0, 0.0},
        {1.0, 0.0},
        {1.0, 1.0},
        {0.0, 1.0},

        // Right face
        {0.0, 0.0},
        {1.0, 0.0},
        {1.0, 1.0},
        {0.0, 1.0}
    };
};


int main(int, char**) {
    // Setup window
    GLFWwindow* window = setupWindow(screen_width, screen_height);
    glfwSetWindowTitle(window, "CG_PROJECT");  

    ImGuiIO& io = ImGui::GetIO(); // Create IO object

    ImVec4 clearColor = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);

    unsigned int shaderProgram = createProgram("./shaders/vshader.vs", "./shaders/fshader.fs");
    glUseProgram(shaderProgram);

    unsigned int texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load and Generate the Texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load("tex.png", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
       
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
     unsigned int texture2;
    glGenTextures(1, &texture2);
    glActiveTexture(GL_TEXTURE1);  // Use a different texture unit
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width2, height2, nrChannels2;
    unsigned char* data2 = stbi_load("textures.png", &width2, &height2, &nrChannels2, 0);
    if (data2) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture2" << std::endl;
    }
    stbi_image_free(data2);
    unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

    setupModelTransformation(shaderProgram);
    setupOrthographicProjection(shaderProgram);
    setupProjectionTransformation(shaderProgram);

    CubeModel mySquareModel;
    PlanarMap planarMap;
    BSPtree cubeBSP(mySquareModel.vertices, mySquareModel.textureCoords);
//==========================================================================================//
  // Load OBJ model
    const char* objFilename = "src/bunny.obj";
    OBJModel buddhaModel = loadOBJ(objFilename);
    float scaleFactor = 0.03;  // Adjust the scale factor as needed

// Apply scaling to the vertices
for (glm::vec3& vertex : buddhaModel.vertices) {
    vertex *= scaleFactor;
}
  BSPtree buddhaBSP(buddhaModel.vertices, buddhaModel.textureCoords);
//=========================================================================================//
   oldX = oldY = currentX = currentY = 0.0;
    int prevLeftButtonState = GLFW_RELEASE;
    while (!glfwWindowShouldClose(window)) {
       glfwPollEvents();
       glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        // Clear the color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set up the view matrix
        setupViewTransformation(shaderProgram);

        // Set up the projection matrix
        setupProjectionTransformation(shaderProgram);

        glBindTexture(GL_TEXTURE_2D, texture);
        
    // Render the BSP cube's boundaries in fill mode
  
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Switch to fill mode
    cubeBSP.render(modelT,texture);
    //   buddhaBSP.render(modelT, texture);
  glBindTexture(GL_TEXTURE_2D, texture2); 
   
    planarMap.addFace(mySquareModel.vertices);
    planarMap.generateOutlineStrokes(shaderProgram,texture2);

        // Swap the front and back buffers
        glfwSwapBuffers(window);
    }

    // Cleanup
    cleanup(window);

    return 0;
}

void setupModelTransformation(unsigned int& program) {
    // Modelling transformations (Model -> World coordinates)
    modelT = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0)); // Model coordinates are the world coordinates

    // Pass on the modelling matrix to the vertex shader
    glUseProgram(program);
    vModel_uniform = glGetUniformLocation(program, "vModel");
    if (vModel_uniform == -1) {
        fprintf(stderr, "Could not bind location: vModel\n");
        exit(0);
    }
    glUniformMatrix4fv(vModel_uniform, 1, GL_FALSE, glm::value_ptr(modelT));
}

void setupViewTransformation(unsigned int& program) {
   
    viewT = glm::lookAt(glm::vec3(0.0, 10, 40.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

    // Pass-on the viewing matrix to the vertex shader
    glUseProgram(program);
    vView_uniform = glGetUniformLocation(program, "vView");
    if (vView_uniform == -1) {
        fprintf(stderr, "Could not bind location: vView\n");
        exit(0);
    }
    glUniformMatrix4fv(vView_uniform, 1, GL_FALSE, glm::value_ptr(viewT));
}

void setupOrthographicProjection(unsigned int& program) {
    // Orthographic projection transformation
    projectionT = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 0.1f, 1000.0f);

    // Pass the orthographic projection matrix to the vertex shader
    glUseProgram(program);
    vProjection_uniform = glGetUniformLocation(program, "vProjection");
    if (vProjection_uniform == -1) {
        fprintf(stderr, "Could not bind location: vProjection\n");
        exit(0);
    }
    glUniformMatrix4fv(vProjection_uniform, 1, GL_FALSE, glm::value_ptr(projectionT));
}

void setupProjectionTransformation(unsigned int& program) {
    // Projection transformation
    projectionT = glm::perspective(45.0f, (GLfloat)screen_width / (GLfloat)screen_height, 0.1f, 1000.0f);

    // Pass on the projection matrix to the vertex shader
    glUseProgram(program);
    vProjection_uniform = glGetUniformLocation(program, "vProjection");
    if (vProjection_uniform == -1) {
        fprintf(stderr, "Could not bind location: vProjection\n");
        exit(0);
    }
    glUniformMatrix4fv(vProjection_uniform, 1, GL_FALSE, glm::value_ptr(projectionT));
}

