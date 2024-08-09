/*
* Kyle Dale
* CS330-Comp Graphic and Visualization
* Module 7 Project: 3D Scene
* Professor Enkema
* 6/14/ - 6/18/2023



SOME CODE AND COMMENTS WERE TAKEN FROM...
https://learnopengl.com/

IMAGE CITATIONS (TEXTURES NOT IN USE NOT CITED, IF PUT IN USE *MUST CITE!*)

* Eliason, K. (2017, May 23). Photo by Kenny Eliason on unsplash. Red bricks wall photo – Free Wall Image on Unsplash. https://unsplash.com/photos/XEsx2NVpqWY
* Wikimedia Foundation. (2023, May 25). Southern New Hampshire University. Wikipedia. https://en.wikipedia.org/wiki/Southern_New_Hampshire_University
* Lapteva, K. (2021, August 17). Photo by Kseniya Lapteva on unsplash. Dark Wood Textrue – Free Texture Image on Unsplash. https://unsplash.com/photos/JN3HCJvZEFA
* Torres, C. (2019, April 5). Photo by Carlos Torres on unsplash. White building photo – Free Grey Image on Unsplash. https://unsplash.com/photos/Zn5vUy9UfJM
* SeekPNG. (2023). Texture on objects - dice texture . SeekPNG.com. https://www.seekpng.com/ipng/u2e6a9w7u2a9w7e6_texture-on-objects-dice-texture-png/ 
* bronzememorials.net. (2023). Custom Bronze Plaque. 9/11 Memorial Plaque|9 11 Plaques|9-11 Bronze Plaque Near Me 2023. https://www.bronzememorials.net/9-11Memorials.php 
* Wilkinson, S. (2021, March 12). Photo by Susan Wilkinson on unsplash. White and brown long haired dog photo – Free Grey Image on Unsplash. https://unsplash.com/photos/KCoCB27h15Y 
* Kunchit0710. (2018, December 21). Premium photo: Wall patterned background blurred. Freepik. https://www.freepik.com/premium-photo/wall-patterned-background-blurred_3619691.htm#query=limestone%20pyramid%20texture&amp;position=6&amp;from_view=search&amp;track=ais 
* fREEPIK. (2016, November 5). Free photo: Texture of damaged clay. Freepik. https://www.freepik.com/free-photo/texture-damaged-clay_968623.htm#query=pottery%20texture&amp;position=0&amp;from_view=keyword&amp;track=ais 
* fREEPIK. (2017, January 30). Free photo: Stucco grined cracked background. Freepik. https://www.freepik.com/free-photo/stucco-grined-cracked-background_1038844.htm#query=pottery%20texture&amp;position=4&amp;from_view=keyword&amp;track=ais 
*/

// Include necessary libraries for function
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <vector>
#include <cmath>

using namespace std;

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif


// Function declaration for handling framebuffer size changes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Function declaration for processing user input
void processInput(GLFWwindow* window);

// Settings
// --------------------------------------------------------------------------------------------------------
// General Settings
// --------------------
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
bool useTexture = true;
int textureChoice;

// Timing Settings
// --------------------
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = static_cast<float>(glfwGetTime());;

// Camera Settings
// --------------------
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;
float cameraSpeed = static_cast<float>(2.5 * deltaTime);
//float cameraSpeed = 0.5f;
bool isPerspective = true; // Initially set the view mode to perspective

// light settings
// --------------------
struct Light {
    glm::vec3 position;
    glm::vec3 color;
    float intensity;
};

Light fillLight = { glm::vec3(0.0f, 6.0f, 12.0f), glm::vec3(1.0f, 1.0f, 0.0f), 0.3f };
Light keyLight = { glm::vec3(0.0f, 25.0f, -12.0f), glm::vec3(1.0f, 1.0f, 1.0f), 2.0f };

// Mat settings
// --------------------
struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

Material material;

// Vertex shader source
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec4 aPos;
    layout (location = 1) in vec4 aColor;
    layout (location = 2) in vec2 aTexCoord;
    layout (location = 3) in vec3 aNormal;

    out vec4 ourColor;
    out vec2 TexCoord;
    out vec3 Normal;
    out vec3 FragPosition;

    uniform mat4 transform;
    uniform mat4 projection;
    uniform mat4 view;
    uniform mat4 model;

    void main()
    {
        gl_Position = projection * view * model * transform * aPos;
        ourColor = aColor;
        TexCoord = vec2(1.0 - aTexCoord.x, aTexCoord.y);
        Normal = mat3(transpose(inverse(model))) * aNormal;
        FragPosition = vec3(model * aPos);
    }
)";

// Fragment shader source
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;

    in vec4 ourColor;
    in vec2 TexCoord;
    in vec3 Normal;
    in vec3 FragPosition;

    struct Light {
        vec3 position;
        vec3 color;
        float intensity;
    };
    
    struct Material {
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        float shininess;
    }; 
  
    uniform Material material;

    uniform bool useTexture;
    uniform sampler2D texture1;
    uniform sampler2D texture2;
    uniform sampler2D texture3;
    uniform sampler2D texture4;
    uniform sampler2D texture5;
    uniform sampler2D texture6;
    uniform sampler2D texture7;
    uniform sampler2D texture8;
    uniform sampler2D texture9;
    uniform sampler2D texture10;
    uniform sampler2D texture11;
    uniform sampler2D texture12;
    uniform int textureChoice;
    uniform int colorChoice;

    uniform Light keyLight;
    uniform Light fillLight;

    void main()
    {
        vec3 ambientLightColor = vec3(0.4, 0.4, 0.4) * material.ambient; // Ambient light color

        // Calculate the light direction and normalize the surface normal
        vec3 lightDirectionKey = normalize(keyLight.position - FragPosition);
        vec3 lightDirectionFill = normalize(fillLight.position - FragPosition);
        vec3 surfaceNormal = normalize(Normal);

        // Calculate the diffuse and specular components for the key light
        float diffuseFactorKey = max(dot(surfaceNormal, lightDirectionKey), 0.0);
        vec3 diffuseColorKey = diffuseFactorKey * keyLight.color * keyLight.intensity * material.diffuse;
        vec3 reflectDirectionKey = reflect(-lightDirectionKey, surfaceNormal);
        float specularFactorKey = pow(max(dot(reflectDirectionKey, -normalize(FragPosition)), 0.0), 16.0);
        vec3 specularColorKey = specularFactorKey * keyLight.color * keyLight.intensity * material.specular;

        // Calculate the diffuse and specular components for the fill light
        float diffuseFactorFill = max(dot(surfaceNormal, lightDirectionFill), 0.0);
        vec3 diffuseColorFill = diffuseFactorFill * fillLight.color * fillLight.intensity * material.diffuse;
        vec3 reflectDirectionFill = reflect(-lightDirectionFill, surfaceNormal);
        float specularFactorFill = pow(max(dot(reflectDirectionFill, -normalize(FragPosition)), 0.0), 16.0);
        vec3 specularColorFill = specularFactorFill * fillLight.color * fillLight.intensity * material.specular;
        
        // Calculate the final color by combining ambient, diffuse, and specular components
        vec3 finalColor = material.ambient * ambientLightColor +
                          material.diffuse * (diffuseColorKey + diffuseColorFill) +
                          material.specular * (specularColorKey + specularColorFill) +
                          material.shininess;

        if (useTexture == true) {
            if (textureChoice == 0) {
                finalColor = texture(texture1, TexCoord).rgb;
            } else if (textureChoice == 1) {
                finalColor = texture(texture2, TexCoord).rgb;
            } else if (textureChoice == 2) {
                finalColor = texture(texture3, TexCoord).rgb;
            } else if (textureChoice == 3) {
                finalColor = texture(texture4, TexCoord).rgb;
            } else if (textureChoice == 4) {
                finalColor = texture(texture5, TexCoord).rgb;
            } else if (textureChoice == 5) {
                finalColor = texture(texture6, TexCoord).rgb;
            } else if (textureChoice == 6) {
                finalColor = texture(texture7, TexCoord).rgb;
            } else if (textureChoice == 7) {
                finalColor = texture(texture8, TexCoord).rgb;
            } else if (textureChoice == 8) {
                finalColor = texture(texture9, TexCoord).rgb;
            } else if (textureChoice == 9) {
                finalColor = texture(texture10, TexCoord).rgb;
            } else if (textureChoice == 10) {
                finalColor = texture(texture11, TexCoord).rgb;
            } else if (textureChoice == 11) {
                finalColor = texture(texture12, TexCoord).rgb;
            } else {
                if (colorChoice == 0) {
                    finalColor = ourColor.rgb;
                } else if (colorChoice == 1) {
                    finalColor = vec3(1.0, 1.0, 1.0) * 0.4;
                } else if (colorChoice == 2) {
                    finalColor = vec3(1.0);
                } else if (colorChoice == 3) {
                    finalColor = vec3(1.0, 0.0, 0.0);
                } else if (colorChoice == 4) {
                    finalColor = vec3(0.0, 1.0, 0.0);
                } else if (colorChoice == 5) {
                    finalColor = vec3(0.0, 0.0, 1.0);
                } else if (colorChoice == 6) {
                    finalColor = vec3(0.0, 0.0, 0.0);
                } else {
                    finalColor = vec3(0.15);
                }
            }
        }

        // Apply the lighting contributions
        finalColor = ambientLightColor * finalColor + diffuseColorKey + diffuseColorFill + specularColorKey + specularColorFill;

        if (colorChoice == 1) {
            FragColor = vec4(finalColor, 0.5);  // Set alpha to 0.5 for colorChoice 1
        } else {
            FragColor = vec4(finalColor, 1.0);  // Set alpha to 1.0 for other color choices
        }
    }
)";



    // FUNCTIONS TO GENERATE THE POT
    // -------------------------------------------------------------------------------------------------------------
        // Generate pot body (sphere)
        // ------------------------------
        void generateSphereVertices(std::vector<float>& sphereVertices, std::vector<unsigned int>& sphereIndices, float scale, const glm::vec3& spherePosition)
        {
            const int numLatitudeLines = 16;
            const int numLongitudeLines = 16;

            for (int lat = 0; lat <= numLatitudeLines; ++lat) {
                float latAngle = (lat * 3.14159f) / numLatitudeLines;
                float y = scale * cos(latAngle);
                float radius = scale * sin(latAngle);

                for (int lon = 0; lon < numLongitudeLines; ++lon) {
                    float lonAngle = (lon * 2.0f * 3.14159f) / numLongitudeLines;
                    float x = radius * cos(lonAngle);
                    float z = radius * sin(lonAngle);

                    // Position
                    float posX = x + spherePosition.x;
                    float posY = y + spherePosition.y;
                    float posZ = z + spherePosition.z;
                    sphereVertices.push_back(posX);
                    sphereVertices.push_back(posY);
                    sphereVertices.push_back(posZ);

                    // Color
                    sphereVertices.push_back(1.0f); // Red
                    sphereVertices.push_back(1.0f); // Green
                    sphereVertices.push_back(1.0f); // Blue

                    // Texture coordinates
                    float u = static_cast<float>(lon) / numLongitudeLines;
                    float v = static_cast<float>(lat) / numLatitudeLines;
                    sphereVertices.push_back(u);
                    sphereVertices.push_back(v);

                    // Normals
                    sphereVertices.push_back(posX);
                    sphereVertices.push_back(posY);
                    sphereVertices.push_back(posZ);

                    // Indices
                    if (lat < numLatitudeLines && lon < numLongitudeLines) {
                        unsigned int current = lat * numLongitudeLines + lon;
                        unsigned int next = current + numLongitudeLines + 1;

                        sphereIndices.push_back(current);
                        sphereIndices.push_back(next);
                        sphereIndices.push_back(current + 1);

                        sphereIndices.push_back(current + 1);
                        sphereIndices.push_back(next);
                        sphereIndices.push_back(next + 1);
                    }
                }
            }
        }

        // Generate pot neck (cylinder)
        // ------------------------------
        void generateCylinderVertices(std::vector<float>& cylinderVertices, std::vector<unsigned int>& cylinderIndices, float radius, float height, int numSegments, const glm::vec3& cylinderPosition)
        {
            const float segmentAngle = (2.0f * 3.14159f) / numSegments;

            // Generate vertices for the sides of the cylinder
            for (int i = 0; i <= numSegments; ++i) {
                float angle = i * segmentAngle;
                float x = radius * cos(angle);
                float z = radius * sin(angle);

                // Bottom vertex
                float bottomX = x + cylinderPosition.x;
                float bottomY = cylinderPosition.y - (height / 2.0f);
                float bottomZ = z + cylinderPosition.z;
                cylinderVertices.push_back(bottomX);
                cylinderVertices.push_back(bottomY);
                cylinderVertices.push_back(bottomZ);
                cylinderVertices.push_back(1.0f); // Red
                cylinderVertices.push_back(1.0f); // Green
                cylinderVertices.push_back(1.0f); // Blue
                cylinderVertices.push_back(static_cast<float>(i) / numSegments);
                cylinderVertices.push_back(0.0f);
                cylinderVertices.push_back(x);
                cylinderVertices.push_back(0.0f);
                cylinderVertices.push_back(z);

                // Top vertex
                float topX = x + cylinderPosition.x;
                float topY = cylinderPosition.y + (height / 2.0f);
                float topZ = z + cylinderPosition.z;
                cylinderVertices.push_back(topX);
                cylinderVertices.push_back(topY);
                cylinderVertices.push_back(topZ);
                cylinderVertices.push_back(1.0f); // Red
                cylinderVertices.push_back(1.0f); // Green
                cylinderVertices.push_back(1.0f); // Blue
                cylinderVertices.push_back(static_cast<float>(i) / numSegments);
                cylinderVertices.push_back(1.0f);
                cylinderVertices.push_back(x);
                cylinderVertices.push_back(0.0f);
                cylinderVertices.push_back(z);
            }

            // Generate indices for the sides of the cylinder
            for (int i = 0; i < numSegments; ++i) {
                unsigned int currentBottom = i * 2;
                unsigned int currentTop = i * 2 + 1;
                unsigned int nextBottom = (i + 1) * 2;
                unsigned int nextTop = (i + 1) * 2 + 1;

                // Triangle 1
                cylinderIndices.push_back(currentBottom);
                cylinderIndices.push_back(currentTop);
                cylinderIndices.push_back(nextTop);

                // Triangle 2
                cylinderIndices.push_back(currentBottom);
                cylinderIndices.push_back(nextTop);
                cylinderIndices.push_back(nextBottom);
            }
        }

    // * END FUNCTIONS TO GENERATE THE POT *
    // -------------------------------------------------------------------------------------------------------------
    


    // FUNCTIONS TO GENERATE THE LLAMA
    // -------------------------------------------------------------------------------------------------------------
     // front left leg
    // -------------------
        void generateFrontLeftLeg(std::vector<float>& frontLeftLegVertices, std::vector<unsigned int>& frontLeftLegIndices, float fllRadius, float fllHeight, int fllnumSegments, const glm::vec3& fllLegPosition)
        {
            const float segmentAngle = (2.0f * 3.14159f) / fllnumSegments;

            // Generate vertices for the sides of the front left leg
            for (int i = 0; i <= fllnumSegments; ++i) {
                float angle = i * segmentAngle;
                float x = fllRadius * cos(angle);
                float z = fllRadius * sin(angle);

                // Bottom vertex
                float bottomX = x + fllLegPosition.x;
                float bottomY = fllLegPosition.y - (fllHeight / 2.0f);
                float bottomZ = z + fllLegPosition.z;
                frontLeftLegVertices.push_back(bottomX);
                frontLeftLegVertices.push_back(bottomY);
                frontLeftLegVertices.push_back(bottomZ);
                frontLeftLegVertices.push_back(1.0f); // Red
                frontLeftLegVertices.push_back(1.0f); // Green
                frontLeftLegVertices.push_back(1.0f); // Blue
                frontLeftLegVertices.push_back(static_cast<float>(i) / fllnumSegments);
                frontLeftLegVertices.push_back(0.0f);
                frontLeftLegVertices.push_back(x);
                frontLeftLegVertices.push_back(0.0f);
                frontLeftLegVertices.push_back(z);

                // Top vertex
                float topX = x + fllLegPosition.x;
                float topY = fllLegPosition.y + (fllHeight / 2.0f);
                float topZ = z + fllLegPosition.z;
                frontLeftLegVertices.push_back(topX);
                frontLeftLegVertices.push_back(topY);
                frontLeftLegVertices.push_back(topZ);
                frontLeftLegVertices.push_back(1.0f); // Red
                frontLeftLegVertices.push_back(1.0f); // Green
                frontLeftLegVertices.push_back(1.0f); // Blue
                frontLeftLegVertices.push_back(static_cast<float>(i) / fllnumSegments);
                frontLeftLegVertices.push_back(1.0f);
                frontLeftLegVertices.push_back(x);
                frontLeftLegVertices.push_back(0.0f);
                frontLeftLegVertices.push_back(z);
            }

            // Generate indices for the sides of the front left leg
            for (int i = 0; i < fllnumSegments; ++i) {
                unsigned int currentBottom = i * 2;
                unsigned int currentTop = i * 2 + 1;
                unsigned int nextBottom = (i + 1) * 2;
                unsigned int nextTop = (i + 1) * 2 + 1;

                // Triangle 1
                frontLeftLegIndices.push_back(currentBottom);
                frontLeftLegIndices.push_back(currentTop);
                frontLeftLegIndices.push_back(nextTop);

                // Triangle 2
                frontLeftLegIndices.push_back(currentBottom);
                frontLeftLegIndices.push_back(nextTop);
                frontLeftLegIndices.push_back(nextBottom);
            }
        }

        // front right leg
        // -------------------
        void generateFrontRightLeg(std::vector<float>& frontRightLegVertices, std::vector<unsigned int>& frontRightLegIndices, float frlRadius, float frlHeight, int frlnumSegments, const glm::vec3& frlLegPosition)
        {
            const float segmentAngle = (2.0f * 3.14159f) / frlnumSegments;

            // Generate vertices for the sides of the front right leg
            for (int i = 0; i <= frlnumSegments; ++i) {
                float angle = i * segmentAngle;
                float x = frlRadius * cos(angle);
                float z = frlRadius * sin(angle);

                // Bottom vertex
                float bottomX = x + frlLegPosition.x;
                float bottomY = frlLegPosition.y - (frlHeight / 2.0f);
                float bottomZ = z + frlLegPosition.z;
                frontRightLegVertices.push_back(bottomX);
                frontRightLegVertices.push_back(bottomY);
                frontRightLegVertices.push_back(bottomZ);
                frontRightLegVertices.push_back(1.0f); // Red
                frontRightLegVertices.push_back(1.0f); // Green
                frontRightLegVertices.push_back(1.0f); // Blue
                frontRightLegVertices.push_back(static_cast<float>(i) / frlnumSegments);
                frontRightLegVertices.push_back(0.0f);
                frontRightLegVertices.push_back(x);
                frontRightLegVertices.push_back(0.0f);
                frontRightLegVertices.push_back(z);

                // Top vertex
                float topX = x + frlLegPosition.x;
                float topY = frlLegPosition.y + (frlHeight / 2.0f);
                float topZ = z + frlLegPosition.z;
                frontRightLegVertices.push_back(topX);
                frontRightLegVertices.push_back(topY);
                frontRightLegVertices.push_back(topZ);
                frontRightLegVertices.push_back(1.0f); // Red
                frontRightLegVertices.push_back(1.0f); // Green
                frontRightLegVertices.push_back(1.0f); // Blue
                frontRightLegVertices.push_back(static_cast<float>(i) / frlnumSegments);
                frontRightLegVertices.push_back(1.0f);
                frontRightLegVertices.push_back(x);
                frontRightLegVertices.push_back(0.0f);
                frontRightLegVertices.push_back(z);
            }

            // Generate indices for the sides of the front right leg
            for (int i = 0; i < frlnumSegments; ++i) {
                unsigned int currentBottom = i * 2;
                unsigned int currentTop = i * 2 + 1;
                unsigned int nextBottom = (i + 1) * 2;
                unsigned int nextTop = (i + 1) * 2 + 1;

                // Triangle 1
                frontRightLegIndices.push_back(currentBottom);
                frontRightLegIndices.push_back(currentTop);
                frontRightLegIndices.push_back(nextTop);

                // Triangle 2
                frontRightLegIndices.push_back(currentBottom);
                frontRightLegIndices.push_back(nextTop);
                frontRightLegIndices.push_back(nextBottom);
            }
        }

        // back left leg
        // -------------------
        void generateBackLeftLeg(std::vector<float>& backLeftLegVertices, std::vector<unsigned int>& backLeftLegIndices, float bllRadius, float bllHeight, int bllnumSegments, const glm::vec3& bllLegPosition)
        {
            const float segmentAngle = (2.0f * 3.14159f) / bllnumSegments;

            // Generate vertices for the sides of the back left leg
            for (int i = 0; i <= bllnumSegments; ++i) {
                float angle = i * segmentAngle;
                float x = bllRadius * cos(angle);
                float z = bllRadius * sin(angle);

                // Bottom vertex
                float bottomX = x + bllLegPosition.x;
                float bottomY = bllLegPosition.y - (bllHeight / 2.0f);
                float bottomZ = z + bllLegPosition.z;
                backLeftLegVertices.push_back(bottomX);
                backLeftLegVertices.push_back(bottomY);
                backLeftLegVertices.push_back(bottomZ);
                backLeftLegVertices.push_back(1.0f); // Red
                backLeftLegVertices.push_back(1.0f); // Green
                backLeftLegVertices.push_back(1.0f); // Blue
                backLeftLegVertices.push_back(static_cast<float>(i) / bllnumSegments);
                backLeftLegVertices.push_back(0.0f);
                backLeftLegVertices.push_back(x);
                backLeftLegVertices.push_back(0.0f);
                backLeftLegVertices.push_back(z);

                // Top vertex
                float topX = x + bllLegPosition.x;
                float topY = bllLegPosition.y + (bllHeight / 2.0f);
                float topZ = z + bllLegPosition.z;
                backLeftLegVertices.push_back(topX);
                backLeftLegVertices.push_back(topY);
                backLeftLegVertices.push_back(topZ);
                backLeftLegVertices.push_back(1.0f); // Red
                backLeftLegVertices.push_back(1.0f); // Green
                backLeftLegVertices.push_back(1.0f); // Blue
                backLeftLegVertices.push_back(static_cast<float>(i) / bllnumSegments);
                backLeftLegVertices.push_back(1.0f);
                backLeftLegVertices.push_back(x);
                backLeftLegVertices.push_back(0.0f);
                backLeftLegVertices.push_back(z);
            }

            // Generate indices for the sides of the back left leg
            for (int i = 0; i < bllnumSegments; ++i) {
                unsigned int currentBottom = i * 2;
                unsigned int currentTop = i * 2 + 1;
                unsigned int nextBottom = (i + 1) * 2;
                unsigned int nextTop = (i + 1) * 2 + 1;

                // Triangle 1
                backLeftLegIndices.push_back(currentBottom);
                backLeftLegIndices.push_back(currentTop);
                backLeftLegIndices.push_back(nextTop);

                // Triangle 2
                backLeftLegIndices.push_back(currentBottom);
                backLeftLegIndices.push_back(nextTop);
                backLeftLegIndices.push_back(nextBottom);
            }
        }

        // back right leg
        // -------------------
        void generateBackRightLeg(std::vector<float>& backRightLegVertices, std::vector<unsigned int>& backRightLegIndices, float brlRadius, float brlHeight, int brlnumSegments, const glm::vec3& brlLegPosition)
        {
            const float segmentAngle = (2.0f * 3.14159f) / brlnumSegments;

            // Generate vertices for the sides of the back right leg
            for (int i = 0; i <= brlnumSegments; ++i) {
                float angle = i * segmentAngle;
                float x = brlRadius * cos(angle);
                float z = brlRadius * sin(angle);

                // Bottom vertex
                float bottomX = x + brlLegPosition.x;
                float bottomY = brlLegPosition.y - (brlHeight / 2.0f);
                float bottomZ = z + brlLegPosition.z;
                backRightLegVertices.push_back(bottomX);
                backRightLegVertices.push_back(bottomY);
                backRightLegVertices.push_back(bottomZ);
                backRightLegVertices.push_back(1.0f); // Red
                backRightLegVertices.push_back(1.0f); // Green
                backRightLegVertices.push_back(1.0f); // Blue
                backRightLegVertices.push_back(static_cast<float>(i) / brlnumSegments);
                backRightLegVertices.push_back(0.0f);
                backRightLegVertices.push_back(x);
                backRightLegVertices.push_back(0.0f);
                backRightLegVertices.push_back(z);

                // Top vertex
                float topX = x + brlLegPosition.x;
                float topY = brlLegPosition.y + (brlHeight / 2.0f);
                float topZ = z + brlLegPosition.z;
                backRightLegVertices.push_back(topX);
                backRightLegVertices.push_back(topY);
                backRightLegVertices.push_back(topZ);
                backRightLegVertices.push_back(1.0f); // Red
                backRightLegVertices.push_back(1.0f); // Green
                backRightLegVertices.push_back(1.0f); // Blue
                backRightLegVertices.push_back(static_cast<float>(i) / brlnumSegments);
                backRightLegVertices.push_back(1.0f);
                backRightLegVertices.push_back(x);
                backRightLegVertices.push_back(0.0f);
                backRightLegVertices.push_back(z);
            }

            // Generate indices for the sides of the back right leg
            for (int i = 0; i < brlnumSegments; ++i) {
                unsigned int currentBottom = i * 2;
                unsigned int currentTop = i * 2 + 1;
                unsigned int nextBottom = (i + 1) * 2;
                unsigned int nextTop = (i + 1) * 2 + 1;

                // Triangle 1
                backRightLegIndices.push_back(currentBottom);
                backRightLegIndices.push_back(currentTop);
                backRightLegIndices.push_back(nextTop);

                // Triangle 2
                backRightLegIndices.push_back(currentBottom);
                backRightLegIndices.push_back(nextTop);
                backRightLegIndices.push_back(nextBottom);
            }
        }

        // Neck
        // -------------------
        void generateNeck(std::vector<float>& neckVertices, std::vector<unsigned int>& neckIndices, float neckRadius, float neckHeight, int necknumSegments, const glm::vec3& neckPosition)
        {
            const float segmentAngle = (2.0f * 3.14159f) / necknumSegments;

            // Generate vertices for the sides of the neck
            for (int i = 0; i <= necknumSegments; ++i) {
                float angle = i * segmentAngle;
                float x = neckRadius * cos(angle);
                float z = neckRadius * sin(angle);

                // Bottom vertex
                float bottomX = x + neckPosition.x;
                float bottomY = neckPosition.y - (neckHeight / 2.0f);
                float bottomZ = z + neckPosition.z;
                neckVertices.push_back(bottomX);
                neckVertices.push_back(bottomY);
                neckVertices.push_back(bottomZ);
                neckVertices.push_back(1.0f); // Red
                neckVertices.push_back(1.0f); // Green
                neckVertices.push_back(1.0f); // Blue
                neckVertices.push_back(static_cast<float>(i) / necknumSegments);
                neckVertices.push_back(0.0f);
                neckVertices.push_back(x);
                neckVertices.push_back(0.0f);
                neckVertices.push_back(z);

                // Top vertex
                float topX = x + neckPosition.x;
                float topY = neckPosition.y + (neckHeight / 2.0f);
                float topZ = z + neckPosition.z;
                neckVertices.push_back(topX);
                neckVertices.push_back(topY);
                neckVertices.push_back(topZ);
                neckVertices.push_back(1.0f); // Red
                neckVertices.push_back(1.0f); // Green
                neckVertices.push_back(1.0f); // Blue
                neckVertices.push_back(static_cast<float>(i) / necknumSegments);
                neckVertices.push_back(1.0f);
                neckVertices.push_back(x);
                neckVertices.push_back(0.0f);
                neckVertices.push_back(z);
            }

            // Generate indices for the sides of the neck
            for (int i = 0; i < necknumSegments; ++i) {
                unsigned int currentBottom = i * 2;
                unsigned int currentTop = i * 2 + 1;
                unsigned int nextBottom = (i + 1) * 2;
                unsigned int nextTop = (i + 1) * 2 + 1;

                // Triangle 1
                neckIndices.push_back(currentBottom);
                neckIndices.push_back(currentTop);
                neckIndices.push_back(nextTop);

                // Triangle 2
                neckIndices.push_back(currentBottom);
                neckIndices.push_back(nextTop);
                neckIndices.push_back(nextBottom);
            }
        }

        // Head
        // -------------------
        void generateHeadVertices(std::vector<float>& headVertices, std::vector<unsigned int>& headIndices, float headScale, const glm::vec3& headPosition)
        {
            const int numLatitudeLines = 16;
            const int numLongitudeLines = 16;

            for (int lat = 0; lat <= numLatitudeLines; ++lat) {
                float latAngle = (lat * 3.14159f) / numLatitudeLines;
                float y = headScale * cos(latAngle);
                float radius = headScale * sin(latAngle);

                for (int lon = 0; lon < numLongitudeLines; ++lon) {
                    float lonAngle = (lon * 2.0f * 3.14159f) / numLongitudeLines;
                    float x = radius * cos(lonAngle);
                    float z = radius * sin(lonAngle);

                    // Position
                    float posX = x + headPosition.x;
                    float posY = y + headPosition.y;
                    float posZ = z + headPosition.z;
                    headVertices.push_back(posX);
                    headVertices.push_back(posY);
                    headVertices.push_back(posZ);

                    // Color
                    headVertices.push_back(1.0f); // Red
                    headVertices.push_back(1.0f); // Green
                    headVertices.push_back(1.0f); // Blue

                    // Texture coordinates
                    float u = static_cast<float>(lon) / numLongitudeLines;
                    float v = static_cast<float>(lat) / numLatitudeLines;
                    headVertices.push_back(u);
                    headVertices.push_back(v);

                    // Normals
                    headVertices.push_back(posX);
                    headVertices.push_back(posY);
                    headVertices.push_back(posZ);

                    // Indices
                    if (lat < numLatitudeLines && lon < numLongitudeLines) {
                        unsigned int current = lat * numLongitudeLines + lon;
                        unsigned int next = current + numLongitudeLines + 1;

                        headIndices.push_back(current);
                        headIndices.push_back(next);
                        headIndices.push_back(current + 1);

                        headIndices.push_back(current + 1);
                        headIndices.push_back(next);
                        headIndices.push_back(next + 1);
                    }
                }
            }
        }

    // * END FUNCTIONS TO GENERATE THE LLAMA *
    // -------------------------------------------------------------------------------------------------------------


// main function
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Module 7 Project: 3D Scene - KD", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // Declares and defines a transformation matrix variable
    glm::mat4 transform = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

    // Shaders
    // ----------------------------------
    // Vertex shader
    // --------------------
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // Check for errors
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Vonfigures OpenGL state
    glEnable(GL_DEPTH_TEST);

    // Fragment shader
    // --------------------
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // Check for errors 
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Checks for link errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Get uniform locations
    unsigned int texture1Location = glGetUniformLocation(shaderProgram, "texture1");
    unsigned int texture2Location = glGetUniformLocation(shaderProgram, "texture2");
    unsigned int texture3Location = glGetUniformLocation(shaderProgram, "texture3");
    unsigned int texture4Location = glGetUniformLocation(shaderProgram, "texture4");
    unsigned int texture5Location = glGetUniformLocation(shaderProgram, "texture5");
    unsigned int texture6Location = glGetUniformLocation(shaderProgram, "texture6");
    unsigned int texture7Location = glGetUniformLocation(shaderProgram, "texture7");
    unsigned int texture8Location = glGetUniformLocation(shaderProgram, "texture8");
    unsigned int texture9Location = glGetUniformLocation(shaderProgram, "texture9");
    unsigned int texture10Location = glGetUniformLocation(shaderProgram, "texture10");
    unsigned int texture11Location = glGetUniformLocation(shaderProgram, "texture11");
    unsigned int texture12Location = glGetUniformLocation(shaderProgram, "texture12");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // -------------------------------------------------------




    // Vertice Data for Objects
    // -------------------------------------------------------------------------------------------------------------
    // 
    // Pyramid
    // ----------------------------------
    float pyramidVertices[] = {
        // Base
        // position            color           texture coordinates    normal
        -5.5f, -0.99f, 4.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,            0.0f, -1.0f, 0.0f,
        -4.5f, -0.99f, 4.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,            0.0f, -1.0f, 0.0f,
        -4.5f, -0.99f, 5.5f,   0.0f, 0.0f, 1.0f,  1.0f, 1.0f,            0.0f, -1.0f, 0.0f,

        -5.5f, -0.99f, 4.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,            0.0f, -1.0f, 0.0f,
        -4.5f, -0.99f, 5.5f,   0.0f, 0.0f, 1.0f,  1.0f, 1.0f,            0.0f, -1.0f, 0.0f,
        -5.5f, -0.99f, 5.5f,   0.0f, 1.0f, 0.0f,  0.0f, 1.0f,            0.0f, -1.0f, 0.0f,

        // Side 1
        -5.5f, -0.99f, 4.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,            -0.5f, 0.5f, 0.0f,
        -5.0f, 0.0f, 5.0f,    0.0f, 1.0f, 0.0f,  0.5f, 1.0f,            -0.5f, 0.5f, 0.0f,
        -4.5f, -0.99f, 4.5f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,            -0.5f, 0.5f, 0.0f,

        // Side 2
        -4.5f, -0.99f, 4.5f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,            0.5f, 0.5f, 0.0f,
        -5.0f, 0.0f, 5.0f,    0.0f, 1.0f, 0.0f,  0.5f, 0.0f,            0.5f, 0.5f, 0.0f,
        -4.5f, -0.99f, 5.5f,   0.0f, 0.0f, 1.0f,  0.0f, 1.0f,            0.5f, 0.5f, 0.0f,

        // Side 3
        -4.5f, -0.99f, 5.5f,   1.0f, 0.0f, 0.0f,  0.0f, 1.0f,            0.0f, 0.5f, 0.5f,
        -5.0f, 0.0f, 5.0f,    0.0f, 1.0f, 0.0f,  0.5f, 0.0f,            0.0f, 0.5f, 0.5f,
        -5.5f, -0.99f, 5.5f,   0.0f, 0.0f, 1.0f,  1.0f, 1.0f,            0.0f, 0.5f, 0.5f,

        // Side 4
        -5.5f, -0.99f, 5.5f,   1.0f, 0.0f, 0.0f,  0.0f, 1.0f,            -0.5f, 0.5f, 0.0f,
        -5.0f, 0.0f, 5.0f,    0.0f, 1.0f, 0.0f,  0.5f, 0.0f,            -0.5f, 0.5f, 0.0f,
        -5.5f, -0.99f, 4.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,            -0.5f, 0.5f, 0.0f,
    };


    // 9/11 Memorial Blocks
    // -------------------------------------------------------------------------------------------------------------
    // Block vertices (glass block)
    // ----------------------------------
    float cuboidBlockVertices[] = {
        // Positions           // Colors          // Texture coordinates    // Normals
        // Front face
        5.3f, -0.99f,  -6.3f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,            0.0f, 0.0f, -1.0f, // Bottom-left
        7.3f, -0.99f,  -6.3f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,            0.0f, 0.0f, -1.0f, // Bottom-right
        7.3f,  2.0f,   -6.3f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,            0.0f, 0.0f, -1.0f, // Top-right
        5.3f,  2.0f,   -6.3f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,            0.0f, 0.0f, -1.0f, // Top-left

        // Back face
        5.3f, -0.99f, -7.3f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,            0.0f, 0.0f, 1.0f, // Bottom-left
        7.3f, -0.99f, -7.3f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,            0.0f, 0.0f, 1.0f, // Bottom-right
        7.3f,  2.0f,  -7.3f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,            0.0f, 0.0f, 1.0f, // Top-right
        5.3f,  2.0f,  -7.3f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,            0.0f, 0.0f, 1.0f, // Top-left

        // Left face
        5.3f, -0.99f,  -6.3f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,            -1.0f, 0.0f, 0.0f, // Bottom-left
        5.3f, -0.99f, -7.3f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,            -1.0f, 0.0f, 0.0f, // Bottom-right
        5.3f,  2.0f,  -7.3f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,            -1.0f, 0.0f, 0.0f, // Top-right
        5.3f,  2.0f,   -6.3f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,            -1.0f, 0.0f, 0.0f, // Top-left

        // Right face
        7.3f, -0.99f,  -6.3f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,            1.0f, 0.0f, 0.0f, // Bottom-left
        7.3f, -0.99f, -7.3f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,            1.0f, 0.0f, 0.0f, // Bottom-right
        7.3f,  2.0f,  -7.3f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,            1.0f, 0.0f, 0.0f, // Top-right
        7.3f,  2.0f,   -6.3f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,            1.0f, 0.0f, 0.0f, // Top-left

        // Top face
        5.3f,  2.0f,   -6.3f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,            0.0f, -1.0f, 0.0f, // Bottom-left
        7.3f,  2.0f,   -6.3f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,            0.0f, -1.0f, 0.0f, // Bottom-right
        7.3f,  2.0f,  -7.3f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,            0.0f, -1.0f, 0.0f, // Top-right
        5.3f,  2.0f,  -7.3f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,            0.0f, -1.0f, 0.0f, // Top-left

        // Bottom face
        5.3f, -0.99f, 7.3f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,            0.0f, 1.0f, 0.0f, // Bottom-left
        7.3f, -0.99f, -7.3f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,            0.0f, 1.0f, 0.0f, // Bottom-right
        7.3f, -0.99f,  -6.3f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,            0.0f, 1.0f, 0.0f, // Top-right
        5.3f, -0.99f,  -6.3f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,            0.0f, 1.0f, 0.0f  // Top-left
    };


    unsigned int cuboidBlockIndices[] = {
        // Front face
        0, 1, 2,  // First triangle
        2, 3, 0,  // Second triangle

        // Back face
        4, 5, 6,  // First triangle
        6, 7, 4,  // Second triangle

        // Left face
        8, 9, 10, // First triangle
        10, 11, 8, // Second triangle

        // Right face
        12, 13, 14, // First triangle
        14, 15, 12, // Second triangle

        // Top face
        16, 17, 18, // First triangle
        18, 19, 16, // Second triangle

        // Bottom face
        20, 21, 22, // First triangle
        22, 23, 20  // Second triangle
    };


    // Left "tower" cuboid
    // ----------------------------------
    float leftBlockCuboidVertices[] = {
        // Positions           // Colors          // Texture coordinates  // Normals
        // Front face
        5.55f, 0.25f,  -6.55f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f,           0.0f, 0.0f, -1.0f, // Bottom-left
        6.05f, 0.25f,  -6.55f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f,           0.0f, 0.0f, -1.0f, // Bottom-right
        6.05f,  1.75f, -6.55f,    0.0f, 0.0f, 1.0f,   1.0f, 0.0f,           0.0f, 0.0f, -1.0f, // Top-right
        5.55f,  1.75f, -6.55f,    1.0f, 1.0f, 0.0f,   0.0f, 0.0f,         0.0f, 0.0f, -1.0f, // Top-left

        // Back face
        5.55f, 0.25f, -7.05f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f,           0.0f, 0.0f, 1.0f, // Bottom-left
        6.05f, 0.25f, -7.05f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f,           0.0f, 0.0f, 1.0f, // Bottom-right
        6.05f,  1.75f,  -7.05f,    0.0f, 0.0f, 1.0f,   1.0f, 0.0f,         0.0f, 0.0f, 1.0f, // Top-right
        5.55f,  1.75f,  -7.05f,    1.0f, 1.0f, 0.0f,   0.0f, 0.0f,         0.0f, 0.0f, 1.0f, // Top-left

        // Left face
        5.55f, 0.25f,  -6.55f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f,           -1.0f, 0.0f, 0.0f, // Bottom-left
        5.55f, 0.25f, -7.05f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f,           -1.0f, 0.0f, 0.0f, // Bottom-right
        5.55f,  1.75f,  -7.05f,    0.0f, 0.0f, 1.0f,   1.0f, 0.0f,         -1.0f, 0.0f, 0.0f, // Top-right
        5.55f,  1.75f,   -6.55f,    1.0f, 1.0f, 0.0f,   0.0f, 0.0f,         -1.0f, 0.0f, 0.0f, // Top-left

        // Right face
        6.05f, 0.25f,  -6.55f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f,           1.0f, 0.0f, 0.0f, // Bottom-left
        6.05f, 0.25f, -7.05f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f,           1.0f, 0.0f, 0.0f, // Bottom-right
        6.05f,  1.75f,  -7.05f,    0.0f, 0.0f, 1.0f,   1.0f, 0.0f,         1.0f, 0.0f, 0.0f, // Top-right
        6.05f,  1.75f,   -6.55f,    1.0f, 1.0f, 0.0f,   0.0f, 0.0f,         1.0f, 0.0f, 0.0f, // Top-left

        // Top face
        5.55f,  1.75f,   -6.55f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f,         0.0f, -1.0f, 0.0f, // Bottom-left
        6.05f,  1.75f,   -6.55f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f,         0.0f, -1.0f, 0.0f, // Bottom-right
        6.05f,  1.75f,  -7.05f,    0.0f, 0.0f, 1.0f,   1.0f, 0.0f,         0.0f, -1.0f, 0.0f, // Top-right
        5.55f,  1.75f,  -7.05f,    1.0f, 1.0f, 0.0f,   0.0f, 0.0f,         0.0f, -1.0f, 0.0f, // Top-left

        // Bottom face
        5.55f, 0.25f, -7.05f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f,           0.0f, 1.0f, 0.0f, // Bottom-left
        6.05f, 0.25f, -7.05f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f,           0.0f, 1.0f, 0.0f, // Bottom-right
        6.05f, 0.25f,  -6.55f,    0.0f, 0.0f, 1.0f,   1.0f, 0.0f,           0.0f, 1.0f, 0.0f, // Top-right
        5.55f, 0.25f,  -6.55f,    1.0f, 1.0f, 0.0f,   0.0f, 0.0f,           0.0f, 1.0f, 0.0f, // Top-left
    };

    unsigned int leftBlockCuboidIndices[] = {
        // Front face
        0, 1, 2,
        2, 3, 0,

        // Back face
        4, 5, 6,
        6, 7, 4,

        // Left face
        8, 9, 10,
        10, 11, 8,

        // Right face
        12, 13, 14,
        14, 15, 12,

        // Top face
        16, 17, 18,
        18, 19, 16,

        // Bottom face
        20, 21, 22,
        22, 23, 20
    };

    float rightBlockCuboidVertices[] = {
        // Positions           // Colors          // Texture coordinates   // Normals
        // Front face
        6.55f, 0.25f,  -6.55f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f,             0.0f, 0.0f, -1.0f, // Bottom-left
        7.05f, 0.25f,  -6.55f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f,             0.0f, 0.0f, -1.0f, // Bottom-right
        7.05f,  1.75f,   -6.55f,    0.0f, 0.0f, 1.0f,   1.0f, 0.0f,             0.0f, 0.0f, -1.0f, // Top-right
        6.55f,  1.75f,   -6.55f,    1.0f, 1.0f, 0.0f,   0.0f, 0.0f,             0.0f, 0.0f, -1.0f, // Top-left

        // Back face
        6.55f, 0.25f, -7.05f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f,             0.0f, 0.0f, 1.0f, // Bottom-left
        7.05f, 0.25f, -7.05f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f,             0.0f, 0.0f, 1.0f, // Bottom-right
        7.05f,  1.75f,  -7.05f,    0.0f, 0.0f, 1.0f,   1.0f, 0.0f,             0.0f, 0.0f, 1.0f, // Top-right
        6.55f,  1.75f,  -7.05f,    1.0f, 1.0f, 0.0f,   0.0f, 0.0f,             0.0f, 0.0f, 1.0f, // Top-left

        // Left face
        6.55f, 0.25f,  -6.55f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f,             -1.0f, 0.0f, 0.0f, // Bottom-left
        6.55f, 0.25f, -7.05f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f,             -1.0f, 0.0f, 0.0f, // Bottom-right
        6.55f,  1.75f,  -7.05f,    0.0f, 0.0f, 1.0f,   1.0f, 0.0f,             -1.0f, 0.0f, 0.0f, // Top-right
        6.55f,  1.75f,   -6.55f,    1.0f, 1.0f, 0.0f,   0.0f, 0.0f,             -1.0f, 0.0f, 0.0f, // Top-left

        // Right face
        7.05f, 0.25f,  -6.55f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f,             1.0f, 0.0f, 0.0f, // Bottom-left
        7.05f, 0.25f, -7.05f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f,             1.0f, 0.0f, 0.0f, // Bottom-right
        7.05f,  1.75f,  -7.05f,    0.0f, 0.0f, 1.0f,   1.0f, 0.0f,             1.0f, 0.0f, 0.0f, // Top-right
        7.05f,  1.75f,   -6.55f,    1.0f, 1.0f, 0.0f,   0.0f, 0.0f,             1.0f, 0.0f, 0.0f, // Top-left

        // Top face
        6.55f,  1.75f,   -6.55f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f,             0.0f, -1.0f, 0.0f, // Bottom-left
        7.05f,  1.75f,   -6.55f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f,             0.0f, -1.0f, 0.0f, // Bottom-right
        7.05f,  1.75f,  -7.05f,    0.0f, 0.0f, 1.0f,   1.0f, 0.0f,             0.0f, -1.0f, 0.0f, // Top-right
        6.55f,  1.75f,  -7.05f,     1.0f, 1.0f, 0.0f,   0.0f, 0.0f,             0.0f, -1.0f, 0.0f, // Top-left

        // Bottom face
        6.55f, 0.25f, -7.05f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f,             0.0f, 1.0f, 0.0f, // Bottom-left
        7.05f, 0.25f, -7.05f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f,             0.0f, 1.0f, 0.0f, // Bottom-right
        7.05f, 0.25f,  -6.55f,    0.0f, 0.0f, 1.0f,   1.0f, 0.0f,             0.0f, 1.0f, 0.0f, // Top-right
        6.55f, 0.25f,  -6.55f,    1.0f, 1.0f, 0.0f,   0.0f, 0.0f,             0.0f, 1.0f, 0.0f, // Top-left
    };

    unsigned int rightBlockCuboidIndices[] = {
        // Front face
        0, 1, 2,
        2, 3, 0,

        // Back face
        4, 5, 6,
        6, 7, 4,

        // Left face
        8, 9, 10,
        10, 11, 8,

        // Right face
        12, 13, 14,
        14, 15, 12,

        // Top face
        16, 17, 18,
        18, 19, 16,

        // Bottom face
        20, 21, 22,
        22, 23, 20
    };

    float textBlockVertices[] = {
        // Positions           // Colors          // Texture coordinates   // Normals
        // Front face
        5.8f, -0.75f,  -6.55f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,             0.0f, 0.0f, -1.0f, // Bottom-left
         6.8f, -0.75f,  -6.55f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,             0.0f, 0.0f, -1.0f, // Bottom-right
         6.8f,  0.0f,  -6.55f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,             0.0f, 0.0f, -1.0f, // Top-right
        5.8f,  0.0f,  -6.55f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,             0.0f, 0.0f, -1.0f, // Top-left

        // Back face
        5.8f, -0.75f, -7.05f,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f,             0.0f, 0.0f, 1.0f,  // Bottom-left
         6.8f, -0.75f, -7.05f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f,             0.0f, 0.0f, 1.0f,  // Bottom-right
         6.8f,  0.0f, -7.05f,    0.0f, 0.0f, 1.0f,   1.0f, 1.0f,             0.0f, 0.0f, 1.0f,  // Top-right
        5.8f,  0.0f, -7.05f,    1.0f, 1.0f, 0.0f,   0.0f, 1.0f,             0.0f, 0.0f, 1.0f,  // Top-left

        // Left face
        5.8f, -0.75f,  -6.55f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,             -1.0f, 0.0f, 0.0f, // Bottom-left
        5.8f, -0.75f, -7.05f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,             -1.0f, 0.0f, 0.0f, // Bottom-right
        5.8f,  0.0f, -7.05f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,             -1.0f, 0.0f, 0.0f, // Top-right
        5.8f,  0.0f,  -6.55f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,             -1.0f, 0.0f, 0.0f, // Top-left

        // Right face
         6.8f, -0.75f,  -6.55f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,             1.0f, 0.0f, 0.0f,  // Bottom-left
         6.8f, -0.75f, -7.05f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,             1.0f, 0.0f, 0.0f,  // Bottom-right
         6.8f,  0.0f, -7.05f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,             1.0f, 0.0f, 0.0f,  // Top-right
         6.8f,  0.0f,  -6.55f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,             1.0f, 0.0f, 0.0f,  // Top-left

         // Top face
         5.8f,  0.0f,  -6.55f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,             0.0f, -1.0f, 0.0f, // Bottom-left
          6.8f,  0.0f,  -6.55f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,             0.0f, -1.0f, 0.0f, // Bottom-right
          6.8f,  0.0f, -7.05f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,             0.0f, -1.0f, 0.0f, // Top-right
         5.8f,  0.0f, -7.05f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f,             0.0f, -1.0f, 0.0f, // Top-left

         // Bottom face
         5.8f, -0.75f, -7.05f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,             0.0f, 1.0f, 0.0f,  // Bottom-left
          6.8f, -0.75f, -7.05f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,             0.0f, 1.0f, 0.0f,  // Bottom-right
          6.8f, -0.75f,  -6.55f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,             0.0f, 1.0f, 0.0f,  // Top-right
         5.8f, -0.75f,  -6.55f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f,             0.0f, 1.0f, 0.0f,  // Top-left
    };

    unsigned int textBlockIndices[] = {
        // Front face
        0, 1, 2,
        2, 3, 0,

        // Back face
        4, 5, 6,
        6, 7, 4,

        // Left face
        8, 9, 10,
        10, 11, 8,

        // Right face
        12, 13, 14,
        14, 15, 12,

        // Top face
        16, 17, 18,
        18, 19, 16,

        // Bottom face
        20, 21, 22,
        22, 23, 20
    };
    // * END 9/11 MEMORIAL BLOCK VERTICES AND INDICES
    // -------------------------------------------------------------------------------------------------------------

    // Dice
    // -------------------------------------------------------------------------------------------------------------
    // Dice 1 vertices and indices
    // ----------------------------------
    float dice1Vertices[] = {
        // Positions           // Colors          // Texture coordinates   // Normals
        // Front face
        5.5f, -0.99f, 4.3f,   1.0f, 0.0f, 0.0f,   0.0f, 0.33f,             0.0f, 0.0f, -1.0f, // Bottom-left
        6.5f, -0.99f, 4.3f,    0.0f, 1.0f, 0.0f,   0.25f, 0.33f,             0.0f, 0.0f, -1.0f, // Bottom-right
        6.5f, 0.0f, 4.3f,     0.0f, 0.0f, 1.0f,   0.25f, 0.66f,             0.0f, 0.0f, -1.0f, // Top-right
        5.5f, 0.0f, 4.3f,    1.0f, 1.0f, 0.0f,   0.0f, 0.66f,             0.0f, 0.0f, -1.0f, // Top-left

        // Back face
        6.5f, -0.99f, 5.3f,    1.0f, 0.0f, 0.0f,   0.5f, 0.33f,             0.0f, 0.0f, 1.0f,  // Bottom-left
        5.5f, -0.99f, 5.3f,     0.0f, 1.0f, 0.0f,   0.75f, 0.33f,             0.0f, 0.0f, 1.0f,  // Bottom-right
        5.5f, 0.0f, 5.3f,      0.0f, 0.0f, 1.0f,   0.75f, 0.66f,             0.0f, 0.0f, 1.0f,  // Top-right
        6.5f, 0.0f, 5.3f,     1.0f, 1.0f, 0.0f,   0.5f, 0.66f,             0.0f, 0.0f, 1.0f,  // Top-left

        // Left face
        5.5f, 0.0f, 5.3f,     1.0f, 0.0f, 0.0f,   0.25f, 0.33f,             -1.0f, 0.0f, 0.0f, // Bottom-left
        5.5f, 0.0f, 4.3f,    0.0f, 1.0f, 0.0f,    0.5f, 0.33f,            -1.0f, 0.0f, 0.0f, // Bottom-right
        5.5f, -0.99f, 4.3f,   0.0f, 0.0f, 1.0f,    0.5f, 0.66f,            -1.0f, 0.0f, 0.0f, // Top-right
        5.5f, -0.99f, 5.3f,    1.0f, 1.0f, 0.0f,    0.25f, 0.66f,            -1.0f, 0.0f, 0.0f, // Top-left

        // Right face
        6.5f, 0.0f, 5.3f,      1.0f, 0.0f, 0.0f,    0.75f, 0.33f,            1.0f, 0.0f, 0.0f,  // Bottom-left
        6.5f, 0.0f, 4.3f,     0.0f, 1.0f, 0.0f,    1.0f, 0.33f,            1.0f, 0.0f, 0.0f,  // Bottom-right
        6.5f, -0.99f, 4.3f,    0.0f, 0.0f, 1.0f,    1.0f, 0.66f,            1.0f, 0.0f, 0.0f,  // Top-right
        6.5f, -0.99f, 5.3f,     1.0f, 1.0f, 0.0f,    0.75f, 0.66f,            1.0f, 0.0f, 0.0f,  // Top-left

        // Bottom face
        5.5f, -0.99f, 4.3f,   1.0f, 0.0f, 0.0f,    0.5f, 0.0f,            0.0f, -1.0f, 0.0f, // Bottom-left
        6.5f, -0.99f, 4.3f,    0.0f, 1.0f, 0.0f,    0.75f, 0.0f,            0.0f, -1.0f, 0.0f, // Bottom-right
        6.5f, -0.99f, 5.3f,     0.0f, 0.0f, 1.0f,    0.75f, 0.33f,            0.0f, -1.0f, 0.0f, // Top-right
        5.5f, -0.99f, 5.3f,    1.0f, 1.0f, 0.0f,    0.5f, 0.33f,             0.0f, -1.0f, 0.0f, // Top-left

        // Top face
        5.5f, 0.0f, 4.3f,    1.0f, 0.0f, 0.0f,    0.5f, 0.66f,             0.0f, 1.0f, 0.0f,  // Bottom-left
        6.5f, 0.0f, 4.3f,     0.0f, 1.0f, 0.0f,    0.75f, 0.66f,             0.0f, 1.0f, 0.0f,  // Bottom-right
        6.5f, 0.0f, 5.3f,      0.0f, 0.0f, 1.0f,    0.75f, 1.0f,             0.0f, 1.0f, 0.0f,  // Top-right
        5.5f, 0.0f, 5.3f,     1.0f, 1.0f, 0.0f,    0.5f, 1.0f,             0.0f, 1.0f, 0.0f   // Top-left
    };

    unsigned int dice1Indices[] = {
        // Front face
        0, 1, 2,
        2, 3, 0,

        // Back face
        4, 5, 6,
        6, 7, 4,

        // Left face
        8, 9, 10,
        10, 11, 8,

        // Right face
        12, 13, 14,
        14, 15, 12,

        // Top face
        16, 17, 18,
        18, 19, 16,

        // Bottom face
        20, 21, 22,
        22, 23, 20
    };

    // Dice 2 vertices and indices
    // ----------------------------------
    float dice2Vertices[] = {
        // Positions           // Colors          // Texture coordinates   // Normals
        // Front face
        4.0f, -0.99f, 5.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.33f,             0.0f, 0.0f, -1.0f, // Bottom-left
        5.0f, -0.99, 5.5f,    0.0f, 1.0f, 0.0f,   0.25f, 0.33f,             0.0f, 0.0f, -1.0f, // Bottom-right
        5.0f, 0.0f, 5.5f,     0.0f, 0.0f, 1.0f,   0.25f, 0.66f,             0.0f, 0.0f, -1.0f, // Top-right
        4.0f, 0.0f, 5.5f,    1.0f, 1.0f, 0.0f,   0.0f, 0.66f,             0.0f, 0.0f, -1.0f, // Top-left

        // Back face
        5.0f, -0.99f, 6.5f,    1.0f, 0.0f, 0.0f,   0.5f, 0.33f,             0.0f, 0.0f, 1.0f,  // Bottom-left
        4.0f, -0.99f, 6.5f,     0.0f, 1.0f, 0.0f,   0.75f, 0.33f,             0.0f, 0.0f, 1.0f,  // Bottom-right
        4.0f, 0.0f, 6.5f,      0.0f, 0.0f, 1.0f,   0.75f, 0.66f,             0.0f, 0.0f, 1.0f,  // Top-right
        5.0f, 0.0f, 6.5f,     1.0f, 1.0f, 0.0f,   0.5f, 0.66f,             0.0f, 0.0f, 1.0f,  // Top-left

        // Left face
        4.0f, 0.0f, 6.5f,     1.0f, 0.0f, 0.0f,   0.25f, 0.33f,             -1.0f, 0.0f, 0.0f, // Bottom-left
        4.0f, 0.0f, 5.5f,    0.0f, 1.0f, 0.0f,    0.5f, 0.33f,            -1.0f, 0.0f, 0.0f, // Bottom-right
        4.0f, -0.99f, 5.5f,   0.0f, 0.0f, 1.0f,    0.5f, 0.66f,            -1.0f, 0.0f, 0.0f, // Top-right
        4.0f, -0.99f, 6.5f,    1.0f, 1.0f, 0.0f,    0.25f, 0.66f,            -1.0f, 0.0f, 0.0f, // Top-left

        // Right face
        5.0f, 0.0f, 6.5f,      1.0f, 0.0f, 0.0f,    0.75f, 0.33f,            1.0f, 0.0f, 0.0f,  // Bottom-left
        5.0f, 0.0f, 5.5f,     0.0f, 1.0f, 0.0f,    1.0f, 0.33f,            1.0f, 0.0f, 0.0f,  // Bottom-right
        5.0f, -0.99f, 5.5f,    0.0f, 0.0f, 1.0f,    1.0f, 0.66f,            1.0f, 0.0f, 0.0f,  // Top-right
        5.0f, -0.99f, 6.5f,     1.0f, 1.0f, 0.0f,    0.75f, 0.66f,            1.0f, 0.0f, 0.0f,  // Top-left

        // Bottom face
        4.0f, -0.99f, 5.5f,   1.0f, 0.0f, 0.0f,    0.5f, 0.0f,            0.0f, -1.0f, 0.0f, // Bottom-left
        5.0f, -0.99f, 5.5f,    0.0f, 1.0f, 0.0f,    0.75f, 0.0f,            0.0f, -1.0f, 0.0f, // Bottom-right
        5.0f, -0.99f, 6.5f,     0.0f, 0.0f, 1.0f,    0.75f, 0.33f,            0.0f, -1.0f, 0.0f, // Top-right
        4.0f, -0.99f, 6.5f,    1.0f, 1.0f, 0.0f,    0.5f, 0.33f,             0.0f, -1.0f, 0.0f, // Top-left

        // Top face
        4.0f, 0.0f, 5.5f,    1.0f, 0.0f, 0.0f,    0.5f, 0.66f,             0.0f, 1.0f, 0.0f,  // Bottom-left
        5.0f, 0.0f, 5.5f,     0.0f, 1.0f, 0.0f,    0.75f, 0.66f,             0.0f, 1.0f, 0.0f,  // Bottom-right
        5.0f, 0.0f, 6.5f,      0.0f, 0.0f, 1.0f,    0.75f, 1.0f,             0.0f, 1.0f, 0.0f,  // Top-right
        4.0f, 0.0f, 6.5f,     1.0f, 1.0f, 0.0f,    0.5f, 1.0f,             0.0f, 1.0f, 0.0f   // Top-left
    };

    unsigned int dice2Indices[] = {
        // Front face
        0, 1, 2,
        2, 3, 0,

        // Back face
        4, 5, 6,
        6, 7, 4,

        // Left face
        8, 9, 10,
        10, 11, 8,

        // Right face
        12, 13, 14,
        14, 15, 12,

        // Top face
        16, 17, 18,
        18, 19, 16,

        // Bottom face
        20, 21, 22,
        22, 23, 20
    };

    // * END DICE VERTICES AND INDICES *
    // -------------------------------------------------------------------------------------------------------------



    // GENERATE CLAY POT VERTICES AND INDICES
    // -------------------------------------------------------------------------------------------------------------
    // Generate sphere vertices and indices
    // -------------------
    std::vector<float> sphereVertices;
    std::vector<unsigned int> sphereIndices;
    float sphereScale = 1.0f; // Default Scale value
    glm::vec3 spherePosition(-4.5f, -0.2f, -4.0f);
    generateSphereVertices(sphereVertices, sphereIndices, sphereScale, spherePosition);

    // Generate cylinder vertices and indices
    // -------------------
    std::vector<float> cylinderVertices;
    std::vector<unsigned int> cylinderIndices;
    float cylinderRadius = 0.4f; // Default radius value
    float cylinderHeight = 1.0f; // Default height
    glm::vec3 cylinderPosition(-4.5f, 0.6f, -4.0f); // Default position
    int numSegments = 16;
    generateCylinderVertices(cylinderVertices, cylinderIndices, cylinderRadius, cylinderHeight, numSegments, cylinderPosition);


    // * END GENERATION OF CLAY POT VERTICES AND INDICES *
    // -------------------------------------------------------------------------------------------------------------



    // GENERATE LLAMA VERTICES AND INDICES
    // -------------------------------------------------------------------------------------------------------------
    // FEET
    // -----------------------------------
    // front left foot
    // -------------------
    float frontLeftFootVertices[] = {
        // Positions           // Colors          // Texture coordinates   // Normals
        // Front face
        -0.5f, -0.99f,  0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,             0.0f, 0.0f, -1.0f, // Bottom-left
         0.0f, -0.99f,  0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,             0.0f, 0.0f, -1.0f, // Bottom-right
         0.0f,  -0.5f,  0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,             0.0f, 0.0f, -1.0f, // Top-right
        -0.5f,  -0.5f,  0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,             0.0f, 0.0f, -1.0f, // Top-left

        // Back face
        -0.5f, -0.99f, -0.5f,    1.0f, 0.0f, 0.0f,   1.0f, 0.0f,             0.0f, 0.0f, 1.0f,  // Bottom-left
         0.0f, -0.99f, -0.5f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f,             0.0f, 0.0f, 1.0f,  // Bottom-right
         0.0f,  -0.5f, -0.5f,    0.0f, 0.0f, 1.0f,   0.0f, 1.0f,             0.0f, 0.0f, 1.0f,  // Top-right
        -0.5f,  -0.5f, -0.5f,    1.0f, 1.0f, 0.0f,   1.0f, 1.0f,             0.0f, 0.0f, 1.0f,  // Top-left

        // Left face
        -0.5f, -0.99f,  0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,             -1.0f, 0.0f, 0.0f, // Bottom-left
        -0.5f, -0.99f, -0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,             -1.0f, 0.0f, 0.0f, // Bottom-right
        -0.5f,  -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,             -1.0f, 0.0f, 0.0f, // Top-right
        -0.5f,  -0.5f,  0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,             -1.0f, 0.0f, 0.0f, // Top-left

        // Right face
         0.0f, -0.99f,  0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,             1.0f, 0.0f, 0.0f,  // Bottom-left
         0.0f, -0.99f, -0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,             1.0f, 0.0f, 0.0f,  // Bottom-right
         0.0f,  -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,             1.0f, 0.0f, 0.0f,  // Top-right
         0.0f,  -0.5f,  0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,             1.0f, 0.0f, 0.0f,  // Top-left

         // Top face
         -0.5f,  -0.5f,  0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,             0.0f, -1.0f, 0.0f, // Bottom-left
          0.0f,  -0.5f,  0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,             0.0f, -1.0f, 0.0f, // Bottom-right
          0.0f,  -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,             0.0f, -1.0f, 0.0f, // Top-right
         -0.5f,  -0.5f, -0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,             0.0f, -1.0f, 0.0f, // Top-left

         // Bottom face
         -0.5f, -0.99f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,             0.0f, 1.0f, 0.0f,  // Bottom-left
          0.0f, -0.99f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,             0.0f, 1.0f, 0.0f,  // Bottom-right
          0.0f, -0.99f,  0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,             0.0f, 1.0f, 0.0f,  // Top-right
         -0.5f, -0.99f,  0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,             0.0f, 1.0f, 0.0f,  // Top-left
    };

    unsigned int frontLeftFootIndices[] = {
        // Front face
        0, 1, 2,
        2, 3, 0,

        // Back face
        4, 5, 6,
        6, 7, 4,

        // Left face
        8, 9, 10,
        10, 11, 8,

        // Right face
        12, 13, 14,
        14, 15, 12,

        // Top face
        16, 17, 18,
        18, 19, 16,

        // Bottom face
        20, 21, 22,
        22, 23, 20
    };

    // front right foot
    // -------------------
    float frontRightFootVertices[] = {
        // Positions           // Colors          // Texture coordinates   // Normals
        // Front face
        -1.5f, -0.99f,  0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,             0.0f, 0.0f, -1.0f, // Bottom-left
         -1.0f, -0.99f,  0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,             0.0f, 0.0f, -1.0f, // Bottom-right
         -1.0f,  -0.5f,  0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,             0.0f, 0.0f, -1.0f, // Top-right
        -1.5f,  -0.5f,  0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,             0.0f, 0.0f, -1.0f, // Top-left

        // Back face
        -1.5f, -0.99f, -0.5f,    1.0f, 0.0f, 0.0f,   1.0f, 0.0f,             0.0f, 0.0f, 1.0f,  // Bottom-left
         -1.0f, -0.99f, -0.5f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f,             0.0f, 0.0f, 1.0f,  // Bottom-right
         -1.0f,  -0.5f, -0.5f,    0.0f, 0.0f, 1.0f,   0.0f, 1.0f,             0.0f, 0.0f, 1.0f,  // Top-right
        -1.5f,  -0.5f, -0.5f,    1.0f, 1.0f, 0.0f,   1.0f, 1.0f,             0.0f, 0.0f, 1.0f,  // Top-left

        // Left face
        -1.5f, -0.99f,  0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,             -1.0f, 0.0f, 0.0f, // Bottom-left
        -1.5f, -0.99f, -0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,             -1.0f, 0.0f, 0.0f, // Bottom-right
        -1.5f,  -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,             -1.0f, 0.0f, 0.0f, // Top-right
        -1.5f,  -0.5f,  0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,             -1.0f, 0.0f, 0.0f, // Top-left

        // Right face
         -1.0f, -0.99f,  0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,             1.0f, 0.0f, 0.0f,  // Bottom-left
         -1.0f, -0.99f, -0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,             1.0f, 0.0f, 0.0f,  // Bottom-right
         -1.0f,  -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,             1.0f, 0.0f, 0.0f,  // Top-right
         -1.0f,  -0.5f,  0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,             1.0f, 0.0f, 0.0f,  // Top-left

         // Top face
         -1.5f,  -0.5f,  0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,             0.0f, -1.0f, 0.0f, // Bottom-left
          -1.0f,  -0.5f,  0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,             0.0f, -1.0f, 0.0f, // Bottom-right
          -1.0f,  -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,             0.0f, -1.0f, 0.0f, // Top-right
         -1.5f,  -0.5f, -0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,             0.0f, -1.0f, 0.0f, // Top-left

         // Bottom face
         -1.5f, -0.99f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,             0.0f, 1.0f, 0.0f,  // Bottom-left
          -1.0f, -0.99f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,             0.0f, 1.0f, 0.0f,  // Bottom-right
          -1.0f, -0.99f,  0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,             0.0f, 1.0f, 0.0f,  // Top-right
         -1.5f, -0.99f,  0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,             0.0f, 1.0f, 0.0f,  // Top-left
    };

    unsigned int frontRightFootIndices[] = {
        // Front face
        0, 1, 2,
        2, 3, 0,

        // Back face
        4, 5, 6,
        6, 7, 4,

        // Left face
        8, 9, 10,
        10, 11, 8,

        // Right face
        12, 13, 14,
        14, 15, 12,

        // Top face
        16, 17, 18,
        18, 19, 16,

        // Bottom face
        20, 21, 22,
        22, 23, 20
    };

    // back left foot
    // -------------------
    float backLeftFootVertices[] = {
        // Positions           // Colors          // Texture coordinates   // Normals
        // Front face
        -0.5f, -0.99f, -2.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,             0.0f, 0.0f, -1.0f, // Bottom-left
         0.0f, -0.99f, -2.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,             0.0f, 0.0f, -1.0f, // Bottom-right
         0.0f,  -0.5f, -2.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,             0.0f, 0.0f, -1.0f, // Top-right
        -0.5f,  -0.5f, -2.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,             0.0f, 0.0f, -1.0f, // Top-left

        // Back face
        -0.5f, -0.99f, -2.5f,    1.0f, 0.0f, 0.0f,   1.0f, 0.0f,             0.0f, 0.0f, 1.0f,  // Bottom-left
         0.0f, -0.99f, -2.5f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f,             0.0f, 0.0f, 1.0f,  // Bottom-right
         0.0f,  -0.5f, -2.5f,    0.0f, 0.0f, 1.0f,   0.0f, 1.0f,             0.0f, 0.0f, 1.0f,  // Top-right
        -0.5f,  -0.5f, -2.5f,    1.0f, 1.0f, 0.0f,   1.0f, 1.0f,             0.0f, 0.0f, 1.0f,  // Top-left

        // Left face
        -0.5f, -0.99f, -2.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,             -1.0f, 0.0f, 0.0f, // Bottom-left
        -0.5f, -0.99f, -2.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,             -1.0f, 0.0f, 0.0f, // Bottom-right
        -0.5f,  -0.5f, -2.5f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,             -1.0f, 0.0f, 0.0f, // Top-right
        -0.5f,  -0.5f, -2.0f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,             -1.0f, 0.0f, 0.0f, // Top-left

        // Right face
         0.0f, -0.99f, -2.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,             1.0f, 0.0f, 0.0f,  // Bottom-left
         0.0f, -0.99f, -2.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,             1.0f, 0.0f, 0.0f,  // Bottom-right
         0.0f,  -0.5f, -2.5f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,             1.0f, 0.0f, 0.0f,  // Top-right
         0.0f,  -0.5f, -2.0f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,             1.0f, 0.0f, 0.0f,  // Top-left

         // Top face
         -0.5f,  -0.5f, -2.0f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,             0.0f, -1.0f, 0.0f, // Bottom-left
          0.0f,  -0.5f, -2.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,             0.0f, -1.0f, 0.0f, // Bottom-right
          0.0f,  -0.5f, -2.5f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,             0.0f, -1.0f, 0.0f, // Top-right
         -0.5f,  -0.5f, -2.5f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,             0.0f, -1.0f, 0.0f, // Top-left

         // Bottom face
         -0.5f, -0.99f, -2.5f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,             0.0f, 1.0f, 0.0f,  // Bottom-left
          0.0f, -0.99f, -2.5f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,             0.0f, 1.0f, 0.0f,  // Bottom-right
          0.0f, -0.99f, -2.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,             0.0f, 1.0f, 0.0f,  // Top-right
         -0.5f, -0.99f, -2.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,             0.0f, 1.0f, 0.0f  // Top-left
    };

    unsigned int backLeftFootIndices[] = {
        // Front face
        0, 1, 2,
        2, 3, 0,

        // Back face
        4, 5, 6,
        6, 7, 4,

        // Left face
        8, 9, 10,
        10, 11, 8,

        // Right face
        12, 13, 14,
        14, 15, 12,

        // Top face
        16, 17, 18,
        18, 19, 16,

        // Bottom face
        20, 21, 22,
        22, 23, 20
    };

    // back right foot
    // -------------------
    float backRightFootVertices[] = {
        // Positions           // Colors          // Texture coordinates   // Normals
        // Front face
        -1.5f, -0.99f, -2.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,             0.0f, 0.0f, -1.0f, // Bottom-left
         -1.0f, -0.99f, -2.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,             0.0f, 0.0f, -1.0f, // Bottom-right
         -1.0f,  -0.5f, -2.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,             0.0f, 0.0f, -1.0f, // Top-right
        -1.5f,  -0.5f, -2.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,             0.0f, 0.0f, -1.0f, // Top-left

        // Back face
        -1.5f, -0.99f, -2.5f,    1.0f, 0.0f, 0.0f,   1.0f, 0.0f,             0.0f, 0.0f, 1.0f,  // Bottom-left
         -1.0f, -0.99f, -2.5f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f,             0.0f, 0.0f, 1.0f,  // Bottom-right
         -1.0f,  -0.5f, -2.5f,    0.0f, 0.0f, 1.0f,   0.0f, 1.0f,             0.0f, 0.0f, 1.0f,  // Top-right
        -1.5f,  -0.5f, -2.5f,    1.0f, 1.0f, 0.0f,   1.0f, 1.0f,             0.0f, 0.0f, 1.0f,  // Top-left

        // Left face
        -1.5f, -0.99f, -2.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,             -1.0f, 0.0f, 0.0f, // Bottom-left
        -1.5f, -0.99f, -2.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,             -1.0f, 0.0f, 0.0f, // Bottom-right
        -1.5f,  -0.5f, -2.5f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,             -1.0f, 0.0f, 0.0f, // Top-right
        -1.5f,  -0.5f, -2.0f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,             -1.0f, 0.0f, 0.0f, // Top-left

        // Right face
         -1.0f, -0.99f, -2.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,             1.0f, 0.0f, 0.0f,  // Bottom-left
         -1.0f, -0.99f, -2.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,             1.0f, 0.0f, 0.0f,  // Bottom-right
         -1.0f,  -0.5f, -2.5f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,             1.0f, 0.0f, 0.0f,  // Top-right
         -1.0f,  -0.5f, -2.0f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,             1.0f, 0.0f, 0.0f,  // Top-left

         // Top face
         -1.5f,  -0.5f, -2.0f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,             0.0f, -1.0f, 0.0f, // Bottom-left
          -1.0f,  -0.5f, -2.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,             0.0f, -1.0f, 0.0f, // Bottom-right
          -1.0f,  -0.5f, -2.5f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,             0.0f, -1.0f, 0.0f, // Top-right
         -1.5f,  -0.5f, -2.5f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,             0.0f, -1.0f, 0.0f, // Top-left

         // Bottom face
         -1.5f, -0.99f, -2.5f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,             0.0f, 1.0f, 0.0f,  // Bottom-left
          -1.0f, -0.99f, -2.5f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,             0.0f, 1.0f, 0.0f,  // Bottom-right
          -1.0f, -0.99f, -2.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,             0.0f, 1.0f, 0.0f,  // Top-right
         -1.5f, -0.99f, -2.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,             0.0f, 1.0f, 0.0f,  // Top-left
    };

    unsigned int backRightFootIndices[] = {
        // Front face
        0, 1, 2,
        2, 3, 0,

        // Back face
        4, 5, 6,
        6, 7, 4,

        // Left face
        8, 9, 10,
        10, 11, 8,

        // Right face
        12, 13, 14,
        14, 15, 12,

        // Top face
        16, 17, 18,
        18, 19, 16,

        // Bottom face
        20, 21, 22,
        22, 23, 20
    };
    // LEGS & NECK
    // -----------------------------------
    // Generate front left leg vertices and indices
    // -------------------
    std::vector<float> frontLeftLegVertices;
    std::vector<unsigned int> frontLeftLegIndices;
    float fllRadius = 0.22f; // Default radius value
    float fllHeight = 1.0f; // Default height
    glm::vec3 fllLegPosition(-0.25f, -0.2f, -0.25f); // Default position
    int fllnumSegments = 16;
    generateFrontLeftLeg(frontLeftLegVertices, frontLeftLegIndices, fllRadius, fllHeight, fllnumSegments, fllLegPosition);

    // Generate front right leg vertices and indices
    // -------------------
    std::vector<float> frontRightLegVertices;
    std::vector<unsigned int> frontRightLegIndices;
    float frlRadius = 0.22f; // Default radius value
    float frlHeight = 1.0f; // Default height
    glm::vec3 frlLegPosition(-1.25f, -0.2f, -0.25f); // Default position
    int frlnumSegments = 16;
    generateFrontRightLeg(frontRightLegVertices, frontRightLegIndices, frlRadius, frlHeight, frlnumSegments, frlLegPosition);

    // Generate back left leg vertices and indices
    // -------------------
    std::vector<float> backLeftLegVertices;
    std::vector<unsigned int> backLeftLegIndices;
    float bllRadius = 0.22f; // Default radius value
    float bllHeight = 1.0f; // Default height
    glm::vec3 bllLegPosition(-0.25f, -0.2f, -2.25f); // Default position
    int bllnumSegments = 16;
    generateBackLeftLeg(backLeftLegVertices, backLeftLegIndices, bllRadius, bllHeight, bllnumSegments, bllLegPosition);

    // Generate back right leg vertices and indices
    // -------------------
    std::vector<float> backRightLegVertices;
    std::vector<unsigned int> backRightLegIndices;
    float brlRadius = 0.22f; // Default radius value
    float brlHeight = 1.0f; // Default height
    glm::vec3 brlLegPosition(-1.25f, -0.2f, -2.25f); // Default position
    int brlnumSegments = 16;
    generateBackRightLeg(backRightLegVertices, backRightLegIndices, brlRadius, brlHeight, brlnumSegments, brlLegPosition);

    // Generate neck vertices and indices
    // -------------------
    std::vector<float> neckVertices;
    std::vector<unsigned int> neckIndices;
    float neckRadius = 0.25f; // Default radius value
    float neckHeight = 0.5f; // Default height
    glm::vec3 neckPosition(-0.75f, 1.45f, -0.25f); // Default position
    int necknumSegments = 16;
    generateNeck(neckVertices, neckIndices, neckRadius, neckHeight, necknumSegments, neckPosition);

    // BODY
    // -----------------------------------
    float bodyVertices[] = {
        // Positions           // Colors          // Texture coordinates   // Normals
        // Front face
        -1.5f, 0.2f,  0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,             0.0f, 0.0f, -1.0f, // Bottom-left
         0.0f, 0.2f,  0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,             0.0f, 0.0f, -1.0f, // Bottom-right
         0.0f,  1.2f,  0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,             0.0f, 0.0f, -1.0f, // Top-right
        -1.5f,  1.2f,  0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,             0.0f, 0.0f, -1.0f, // Top-left

        // Back face
        -1.5f, 0.2f, -2.5f,    1.0f, 0.0f, 0.0f,   1.0f, 0.0f,             0.0f, 0.0f, 1.0f,  // Bottom-left
         0.0f, 0.2f, -2.5f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f,             0.0f, 0.0f, 1.0f,  // Bottom-right
         0.0f,  1.2f, -2.5f,    0.0f, 0.0f, 1.0f,   0.0f, 1.0f,             0.0f, 0.0f, 1.0f,  // Top-right
        -1.5f,  1.2f, -2.5f,    1.0f, 1.0f, 0.0f,   1.0f, 1.0f,             0.0f, 0.0f, 1.0f,  // Top-left

        // Left face
        -1.5f, 0.2f,  0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,             -1.0f, 0.0f, 0.0f, // Bottom-left
        -1.5f, 0.2f, -2.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,             -1.0f, 0.0f, 0.0f, // Bottom-right
        -1.5f,  1.2f, -2.5f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,             -1.0f, 0.0f, 0.0f, // Top-right
        -1.5f,  1.2f,  0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,             -1.0f, 0.0f, 0.0f, // Top-left

        // Right face
         0.0f, 0.2f,  0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,             1.0f, 0.0f, 0.0f,  // Bottom-left
         0.0f, 0.2f, -2.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,             1.0f, 0.0f, 0.0f,  // Bottom-right
         0.0f,  1.2f, -2.5f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,             1.0f, 0.0f, 0.0f,  // Top-right
         0.0f,  1.2f,  0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,             1.0f, 0.0f, 0.0f,  // Top-left

         // Top face
         -1.5f,  1.2f,  0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,             0.0f, -1.0f, 0.0f, // Bottom-left
          0.0f,  1.2f,  0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,             0.0f, -1.0f, 0.0f, // Bottom-right
          0.0f,  1.2f, -2.5f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,             0.0f, -1.0f, 0.0f, // Top-right
         -1.5f,  1.2f, -2.5f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,             0.0f, -1.0f, 0.0f, // Top-left

         // Bottom face
         -1.5f, 0.2f, -2.5f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,             0.0f, 1.0f, 0.0f,  // Bottom-left
          0.0f, 0.2f, -2.5f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,             0.0f, 1.0f, 0.0f,  // Bottom-right
          0.0f, 0.2f,  0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,             0.0f, 1.0f, 0.0f,  // Top-right
         -1.5f, 0.2f,  0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,             0.0f, 1.0f, 0.0f,  // Top-left
    };

    unsigned int bodyIndices[] = {
        // Front face
        0, 1, 2,
        2, 3, 0,

        // Back face
        4, 5, 6,
        6, 7, 4,

        // Left face
        8, 9, 10,
        10, 11, 8,

        // Right face
        12, 13, 14,
        14, 15, 12,

        // Top face
        16, 17, 18,
        18, 19, 16,

        // Bottom face
        20, 21, 22,
        22, 23, 20
    };

    // Head & Eyes
    // -------------------
    // Generate sphere vertices and indices
    std::vector<float> headVertices;
    std::vector<unsigned int> headIndices;
    float headScale = 0.65f; // Default Scale value
    glm::vec3 headPosition(-0.75f, 2.0f, -0.2f);
    generateHeadVertices(headVertices, headIndices, headScale, headPosition);

    float leftEyeVertices[] = {
        // Positions           // Colors          // Texture coordinates   // Normals
        // Front face
        -0.6f, 2.0f,  0.4f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,             0.0f, 0.0f, -1.0f, // Bottom-left
         -0.4f, 2.0f,  0.4f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,             0.0f, 0.0f, -1.0f, // Bottom-right
         -0.4f,  2.2f,  0.4f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,             0.0f, 0.0f, -1.0f, // Top-right
        -0.6f,  2.2f,  0.4f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,             0.0f, 0.0f, -1.0f, // Top-left

        // Back face
        -0.6f, 2.0f, 0.15f,    1.0f, 0.0f, 0.0f,   1.0f, 0.0f,             0.0f, 0.0f, 1.0f,  // Bottom-left
         -0.4f, 2.0f, 0.15f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f,             0.0f, 0.0f, 1.0f,  // Bottom-right
         -0.4f,  2.2f, 0.15f,    0.0f, 0.0f, 1.0f,   0.0f, 1.0f,             0.0f, 0.0f, 1.0f,  // Top-right
        -0.6f,  2.2f, 0.15f,    1.0f, 1.0f, 0.0f,   1.0f, 1.0f,             0.0f, 0.0f, 1.0f,  // Top-left

        // Left face
        -0.6f, 2.0f,  0.4f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,             -1.0f, 0.0f, 0.0f, // Bottom-left
        -0.6f, 2.0f, 0.15f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,             -1.0f, 0.0f, 0.0f, // Bottom-right
        -0.6f, 2.2f, 0.15f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,             -1.0f, 0.0f, 0.0f, // Top-right
        -0.6f, 2.2f,  0.4f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,             -1.0f, 0.0f, 0.0f, // Top-left

        // Right face
         -0.4f, 2.0f,  0.4f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,             1.0f, 0.0f, 0.0f,  // Bottom-left
         -0.4f, 2.0f, 0.15f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,             1.0f, 0.0f, 0.0f,  // Bottom-right
         -0.4f,  2.2f, 0.15f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,             1.0f, 0.0f, 0.0f,  // Top-right
         -0.4f,  2.2f,  0.4f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,             1.0f, 0.0f, 0.0f,  // Top-left

         // Top face
         -0.6f,  2.2f,  0.4f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,             0.0f, -1.0f, 0.0f, // Bottom-left
          -0.4f,  2.2f,  0.4f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,             0.0f, -1.0f, 0.0f, // Bottom-right
          -0.4f,  2.2f, 0.15f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,             0.0f, -1.0f, 0.0f, // Top-right
         -0.6f,  2.2f, 0.15f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,             0.0f, -1.0f, 0.0f, // Top-left

         // Bottom face
         -0.6f, 2.0f, 0.15f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,             0.0f, 1.0f, 0.0f,  // Bottom-left
          -0.4f, 2.0f, 0.15f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,             0.0f, 1.0f, 0.0f,  // Bottom-right
          -0.4f, 2.0f,  0.4f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,             0.0f, 1.0f, 0.0f,  // Top-right
         -0.6f, 2.0f,  0.4f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,             0.0f, 1.0f, 0.0f,  // Top-left
    };

    unsigned int leftEyeIndices[] = {
        // Front face
        0, 1, 2,
        2, 3, 0,

        // Back face
        4, 5, 6,
        6, 7, 4,

        // Left face
        8, 9, 10,
        10, 11, 8,

        // Right face
        12, 13, 14,
        14, 15, 12,

        // Top face
        16, 17, 18,
        18, 19, 16,

        // Bottom face
        20, 21, 22,
        22, 23, 20
    };

    float rightEyeVertices[] = {
        // Positions           // Colors          // Texture coordinates   // Normals
        // Front face
        -1.1f, 2.0f,  0.4f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,             0.0f, 0.0f, -1.0f, // Bottom-left
         -0.9f, 2.0f,  0.4f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,             0.0f, 0.0f, -1.0f, // Bottom-right
         -0.9f,  2.2f,  0.4f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,             0.0f, 0.0f, -1.0f, // Top-right
        -1.1f,  2.2f,  0.4f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,             0.0f, 0.0f, -1.0f, // Top-left

        // Back face
        -1.1f, 2.0f, 0.15f,    1.0f, 0.0f, 0.0f,   1.0f, 0.0f,             0.0f, 0.0f, 1.0f,  // Bottom-left
         -0.9f, 2.0f, 0.15f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f,             0.0f, 0.0f, 1.0f,  // Bottom-right
         -0.9f,  2.2f, 0.15f,    0.0f, 0.0f, 1.0f,   0.0f, 1.0f,             0.0f, 0.0f, 1.0f,  // Top-right
        -1.1f,  2.2f, 0.15f,    1.0f, 1.0f, 0.0f,   1.0f, 1.0f,             0.0f, 0.0f, 1.0f,  // Top-left

        // Left face
        -1.1f, 2.0f,  0.4f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,             -1.0f, 0.0f, 0.0f, // Bottom-left
        -1.1f, 2.0f, 0.15f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,             -1.0f, 0.0f, 0.0f, // Bottom-right
        -1.1f,  2.2f, 0.15f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,             -1.0f, 0.0f, 0.0f, // Top-right
        -1.1f,  2.2f,  0.4f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,             -1.0f, 0.0f, 0.0f, // Top-left

        // Right face
         -0.9f, 2.0f,  0.4f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,             1.0f, 0.0f, 0.0f,  // Bottom-left
         -0.9f, 2.0f, 0.15f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,             1.0f, 0.0f, 0.0f,  // Bottom-right
         -0.9f,  2.2f, 0.15f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,             1.0f, 0.0f, 0.0f,  // Top-right
         -0.9f,  2.2f,  0.4f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,             1.0f, 0.0f, 0.0f,  // Top-left

         // Top face
         -1.1f,  2.2f,  0.4f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,             0.0f, -1.0f, 0.0f, // Bottom-left
          -0.9f,  2.2f,  0.4f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,             0.0f, -1.0f, 0.0f, // Bottom-right
          -0.9f,  2.2f, 0.15f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,             0.0f, -1.0f, 0.0f, // Top-right
         -1.1f,  2.2f, 0.15f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,             0.0f, -1.0f, 0.0f, // Top-left

         // Bottom face
         -1.1f, 2.0f, 0.15f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,             0.0f, 1.0f, 0.0f,  // Bottom-left
          -0.9f, 2.0f, 0.15f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,             0.0f, 1.0f, 0.0f,  // Bottom-right
          -0.9f, 2.0f,  0.4f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,             0.0f, 1.0f, 0.0f,  // Top-right
         -1.1f, 2.0f,  0.4f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,             0.0f, 1.0f, 0.0f,  // Top-left
    };

    unsigned int rightEyeIndices[] = {
        // Front face
        0, 1, 2,
        2, 3, 0,

        // Back face
        4, 5, 6,
        6, 7, 4,

        // Left face
        8, 9, 10,
        10, 11, 8,

        // Right face
        12, 13, 14,
        14, 15, 12,

        // Top face
        16, 17, 18,
        18, 19, 16,

        // Bottom face
        20, 21, 22,
        22, 23, 20
    };

    // * END GENERATION OF LLAMA VERTICES AND INDICES *
    // -------------------------------------------------------------------------------------------------------------



    // Plane
    // ----------------------------------
    // Plane vertices, normals, and colors
    float planeVertices[] = {
        // Positions          // Colors             // Texture coordinates  // Normals
        7.5f, -1.0f,  7.5f,   1.0f, 0.0f, 0.0f,    1.0f, 0.0f,            0.0f, 1.0f, 0.0f,  // Top right
        7.5f, -1.0f, -7.5f,   1.0f, 0.5f, 0.0f,    1.0f, 1.0f,            0.0f, 1.0f, 0.0f,  // Bottom right
        -7.5f, -1.0f, -7.5f,  1.0f, 1.0f, 0.0f,    0.0f, 1.0f,            0.0f, 1.0f, 0.0f,  // Bottom left
        -7.5f, -1.0f,  7.5f,  0.0f, 1.0f, 0.0f,    0.0f, 0.0f,            0.0f, 1.0f, 0.0f   // Top left
    };

    // Indices for plane rendering
    unsigned int planeIndices[] = {
        0, 1, 3,   // First triangle
        1, 2, 3    // Second triangle
    };

    // Calculate the normals for the plane
    glm::vec3 planeNormal = glm::vec3(0.0f, 1.0f, 0.0f);

    // Assign the same normal to all vertices of the plane
    for (int i = 0; i < 4; i++) {
        planeVertices[i * 11 + 8] = planeNormal.x;
        planeVertices[i * 11 + 9] = planeNormal.y;
        planeVertices[i * 11 + 10] = planeNormal.z;
    }
    //  * END VERTEX AND INDICE INFORMATION *
    // -------------------------------------------------------------------------------------------------------------



    // VBO & VAO Creation
    // -------------------------------------------------------------------------------------------------------------

    //Pyramid
    // -------------------
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // Binds the Vertex Array Object first, then binds and sets vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
    // Set the position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Set the color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Set the texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // Set the normals attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // Unbind VAO
    glBindVertexArray(0);
    // aActivates Shader
    glUseProgram(shaderProgram);
    // Retrieves the location of the transform uniform variable in shader program
    unsigned int pyramidTransformLoc = glGetUniformLocation(shaderProgram, "transform");


    // Plane
    // -------------------
    unsigned int VBOPlane, VAOPlane, IBOPlane;
    glGenVertexArrays(1, &VAOPlane);
    glGenBuffers(1, &VBOPlane);
    glGenBuffers(1, &IBOPlane);
    // Binds the Vertex Array Object first, then binds and sets vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAOPlane);
    // Bind and set vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBOPlane);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    // Bind and set index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOPlane);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeIndices), planeIndices, GL_STATIC_DRAW);
    // Set the position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Set the color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Set the texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // Set the normals attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // Unbind VAO
    glBindVertexArray(0);
    // aActivates Shader
    glUseProgram(shaderProgram);
    // Retrieves the location of the transform uniform variable in shader program
    unsigned int planeTransformLoc = glGetUniformLocation(shaderProgram, "transform");


    // Lights
    // -------------------
    unsigned int keyLightVAO, keyLightVBO, fillLightVAO, fillLightVBO;
    // Generate and bind key light VAO and VBO
    glGenVertexArrays(1, &keyLightVAO);
    glGenBuffers(1, &keyLightVBO);
    glBindVertexArray(keyLightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, keyLightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
    // Set the position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Unbind VAO
    glBindVertexArray(0);
    // aActivates Shader
    glUseProgram(shaderProgram);
    // -------------------
    // Generate and bind fill light VAO and VBO
    glGenVertexArrays(1, &fillLightVAO);
    glGenBuffers(1, &fillLightVBO);
    glBindVertexArray(fillLightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, fillLightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
    // Set the position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Unbind VAO
    glBindVertexArray(0);
    // aActivates Shader
    glUseProgram(shaderProgram);


    // 9/11 Memorial VAO, VBO, and EBO
    // -------------------------------------------------------------------------------------------------------------
    // Cuboid Block (glass block)
    // ----------------------------------
    unsigned int VBOcuboidBlock, VAOCuboidBlock, EBOCuboidBlock;
    glGenVertexArrays(1, &VAOCuboidBlock);
    glGenBuffers(1, &VBOcuboidBlock);
    glGenBuffers(1, &EBOCuboidBlock);
    // Bind the Vertex Array Object first, then bind and set the vertex buffer(s), and configure the vertex attribute(s).
    glBindVertexArray(VAOCuboidBlock);
    // Bind and set up the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBOcuboidBlock);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cuboidBlockVertices), cuboidBlockVertices, GL_STATIC_DRAW);
    // Bind and set up the element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOCuboidBlock);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cuboidBlockIndices), cuboidBlockIndices, GL_STATIC_DRAW);
    // Set the position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Set the color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Set the texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // Set the normals attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // Unbind VAO
    glBindVertexArray(0);
    // Activate shader program
    glUseProgram(shaderProgram);

    // Tower Blocks
    // -------------------
    unsigned int towerVAO, towerVBO, towerEBO;
    glGenVertexArrays(1, &towerVAO);
    glBindVertexArray(towerVAO);
    glGenBuffers(1, &towerVBO);
    glGenBuffers(1, &towerEBO);
    // Bind the VBO and EBO
    glBindBuffer(GL_ARRAY_BUFFER, towerVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, towerEBO);
    // Set the vertex data for the tower blocks
    glBufferData(GL_ARRAY_BUFFER, sizeof(leftBlockCuboidVertices), leftBlockCuboidVertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(leftBlockCuboidIndices), leftBlockCuboidIndices, GL_STATIC_DRAW);
    // Set the position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Set the color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Set the texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // Set the normals attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // Unbind the VAO, VBO, and EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // Activate shader program
    glUseProgram(shaderProgram);

    unsigned int towerVAO1, towerVBO1, towerEBO1;
    glGenVertexArrays(1, &towerVAO1);
    glBindVertexArray(towerVAO1);
    glGenBuffers(1, &towerVBO1);
    glGenBuffers(1, &towerEBO1);
    // Bind the VBO and EBO
    glBindBuffer(GL_ARRAY_BUFFER, towerVBO1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, towerEBO1);
    // Set the vertex data for the tower blocks
    glBufferData(GL_ARRAY_BUFFER, sizeof(rightBlockCuboidVertices), rightBlockCuboidVertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rightBlockCuboidIndices), rightBlockCuboidIndices, GL_STATIC_DRAW);
    // Set the position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Set the color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Set the texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // Set the normals attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // Unbind the VAO, VBO, and EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // Activate shader program
    glUseProgram(shaderProgram);

    // Text Block
    // -------------------
    unsigned int textBlockVAO, textBlockVBO, textBlockEBO;
    glGenVertexArrays(1, &textBlockVAO);
    glBindVertexArray(textBlockVAO);
    glGenBuffers(1, &textBlockVBO);
    glGenBuffers(1, &textBlockEBO);
    // Bind the VBO and EBO
    glBindBuffer(GL_ARRAY_BUFFER, textBlockVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, textBlockEBO);
    // Set the vertex data for the text blocks
    glBufferData(GL_ARRAY_BUFFER, sizeof(textBlockVertices), textBlockVertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(textBlockIndices), textBlockIndices, GL_STATIC_DRAW);
    // Set the position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Set the color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Set the texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // Set the normals attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // Unbind the VAO, VBO, and EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // Activate shader program
    glUseProgram(shaderProgram);
    // * END 9/11 MEMORIAL BLOCK VAO, VBO, AND EBO CREATION *
    // -------------------------------------------------------------------------------------------------------------

    // DICE VAO, VBO, AND EBO'S CREATION
    // -------------------------------------------------------------------------------------------------------------
    // Dice 1 
    // ----------------------------------
    // Dice 1
    // -------------------
    unsigned int dice1VAO, dice1VBO, dice1EBO; // Add dice1EBO variable
    glGenVertexArrays(1, &dice1VAO);
    glBindVertexArray(dice1VAO);
    glGenBuffers(1, &dice1VBO);
    glGenBuffers(1, &dice1EBO); // Generate EBO buffer
    // Bind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, dice1VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(dice1Vertices), dice1Vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dice1EBO); // Bind the EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(dice1Indices), dice1Indices, GL_STATIC_DRAW); // Provide the index data
    // Unbind the VAO, VBO, EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(shaderProgram);

    // Dice 2
    // -------------------
    unsigned int dice2VAO, dice2VBO, dice2EBO; // Add dice2EBO variable
    glGenVertexArrays(1, &dice2VAO);
    glBindVertexArray(dice2VAO);
    glGenBuffers(1, &dice2VBO);
    glGenBuffers(1, &dice2EBO); // Generate EBO buffer
    // Bind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, dice2VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(dice2Vertices), dice2Vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dice2EBO); // Bind the EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(dice2Indices), dice2Indices, GL_STATIC_DRAW); // Provide the index data
    // Unbind the VAO, VBO, EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(shaderProgram);

    // * END DICE VAO, VBO, AND EBO'S CREATION *
    // -------------------------------------------------------------------------------------------------------------



    // CERAMIC POT VAO, VBO, and EBO
    // -------------------------------------------------------------------------------------------------------------
    // Sphere VAO, VBO, EBO
    // -------------------
    unsigned int sphereVAO, sphereVBO, sphereEBO;
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);
    // Bind the sphere VAO
    glBindVertexArray(sphereVAO);
    // Bind and fill the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), sphereVertices.data(), GL_STATIC_DRAW);
    // Bind and fill the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), sphereIndices.data(), GL_STATIC_DRAW);
    // Set the vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // Unbind VAO, VBO, and EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // Activate shader and get uniform location
    glUseProgram(shaderProgram);
    unsigned int sphereTransformLoc = glGetUniformLocation(shaderProgram, "transform");

    // Cylinder VAO, VBO, EBO
    // -------------------
    unsigned int cylinderVAO, cylinderVBO, cylinderEBO;
    glGenVertexArrays(1, &cylinderVAO);
    glGenBuffers(1, &cylinderVBO);
    glGenBuffers(1, &cylinderEBO);
    // Bind the cylinder VAO
    glBindVertexArray(cylinderVAO);
    // Bind and fill the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, cylinderVBO);
    glBufferData(GL_ARRAY_BUFFER, cylinderVertices.size() * sizeof(float), cylinderVertices.data(), GL_STATIC_DRAW);
    // Bind and fill the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinderEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cylinderIndices.size() * sizeof(unsigned int), cylinderIndices.data(), GL_STATIC_DRAW);
    // Set the vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // Unbind VAO, VBO, and EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // Activate shader and get uniform location
    glUseProgram(shaderProgram);

    // * END CERAMIC POT VAO, VBO, and EBO *
    // -------------------------------------------------------------------------------------------------------------



    // LLAMA VAO, VBO, and EBO CREATION
    // -------------------------------------------------------------------------------------------------------------
     // FEET
    // -----------------------------------
    // front left foot
    // -------------------
    unsigned int frontLeftFootVBO, frontLeftFootVAO, frontLeftFootEBO;
    glGenVertexArrays(1, &frontLeftFootVAO);
    glGenBuffers(1, &frontLeftFootVBO);
    glGenBuffers(1, &frontLeftFootEBO);
    // Bind the Vertex Array Object first, then bind and set the vertex buffer(s), and configure the vertex attribute(s).
    glBindVertexArray(frontLeftFootVAO);
    // Bind and set up the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, frontLeftFootVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(frontLeftFootVertices), frontLeftFootVertices, GL_STATIC_DRAW);
    // Bind and set up the element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frontLeftFootEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(frontLeftFootIndices), frontLeftFootIndices, GL_STATIC_DRAW);
    // Set the position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Set the color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Set the texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // Set the normals attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // Unbind VAO
    glBindVertexArray(0);
    // Activate shader program
    glUseProgram(shaderProgram);

    // front right foot
    // -------------------
    unsigned int frontRightFootVBO, frontRightFootVAO, frontRightFootEBO;
    glGenVertexArrays(1, &frontRightFootVAO);
    glGenBuffers(1, &frontRightFootVBO);
    glGenBuffers(1, &frontRightFootEBO);
    // Bind the Vertex Array Object first, then bind and set the vertex buffer(s), and configure the vertex attribute(s).
    glBindVertexArray(frontRightFootVAO);
    // Bind and set up the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, frontRightFootVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(frontRightFootVertices), frontRightFootVertices, GL_STATIC_DRAW);
    // Bind and set up the element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frontRightFootEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(frontRightFootIndices), frontRightFootIndices, GL_STATIC_DRAW);
    // Set the position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Set the color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Set the texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // Set the normals attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // Unbind VAO
    glBindVertexArray(0);
    // Activate shader program
    glUseProgram(shaderProgram);

    // back left foot
    // -------------------
    unsigned int backLeftFootVBO, backLeftFootVAO, backLeftFootEBO;
    glGenVertexArrays(1, &backLeftFootVAO);
    glGenBuffers(1, &backLeftFootVBO);
    glGenBuffers(1, &backLeftFootEBO);
    // Bind the Vertex Array Object first, then bind and set the vertex buffer(s), and configure the vertex attribute(s).
    glBindVertexArray(backLeftFootVAO);
    // Bind and set up the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, backLeftFootVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(backLeftFootVertices), backLeftFootVertices, GL_STATIC_DRAW);
    // Bind and set up the element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, backLeftFootEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(backLeftFootIndices), backLeftFootIndices, GL_STATIC_DRAW);
    // Set the position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Set the color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Set the texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // Set the normals attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // Unbind VAO
    glBindVertexArray(0);
    // Activate shader program
    glUseProgram(shaderProgram);

    // back right foot
    // -------------------
    unsigned int backRightFootVBO, backRightFootVAO, backRightFootEBO;
    glGenVertexArrays(1, &backRightFootVAO);
    glGenBuffers(1, &backRightFootVBO);
    glGenBuffers(1, &backRightFootEBO);
    // Bind the Vertex Array Object first, then bind and set the vertex buffer(s), and configure the vertex attribute(s).
    glBindVertexArray(backRightFootVAO);
    // Bind and set up the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, backRightFootVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(backRightFootVertices), backRightFootVertices, GL_STATIC_DRAW);
    // Bind and set up the element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, backRightFootEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(backRightFootIndices), backRightFootIndices, GL_STATIC_DRAW);
    // Set the position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Set the color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Set the texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // Set the normals attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // Unbind VAO
    glBindVertexArray(0);
    // Activate shader program
    glUseProgram(shaderProgram);

    // LEGS
    // -----------------------------------
    // Front Left Leg VAO, VBO, EBO
    // -------------------------
    unsigned int frontLeftLegVAO, frontLeftLegVBO, frontLeftLegEBO;
    glGenVertexArrays(1, &frontLeftLegVAO);
    glGenBuffers(1, &frontLeftLegVBO);
    glGenBuffers(1, &frontLeftLegEBO);
    // Bind the front left leg VAO
    glBindVertexArray(frontLeftLegVAO);
    // Bind and fill the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, frontLeftLegVBO);
    glBufferData(GL_ARRAY_BUFFER, frontLeftLegVertices.size() * sizeof(float), frontLeftLegVertices.data(), GL_STATIC_DRAW);
    // Bind and fill the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frontLeftLegEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, frontLeftLegIndices.size() * sizeof(unsigned int), frontLeftLegIndices.data(), GL_STATIC_DRAW);
    // Set the vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // Unbind VAO, VBO, and EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // Activate shader and get uniform location
    glUseProgram(shaderProgram);

    // Front Right Leg VAO, VBO, EBO
    // -------------------------
    unsigned int frontRightLegVAO, frontRightLegVBO, frontRightLegEBO;
    glGenVertexArrays(1, &frontRightLegVAO);
    glGenBuffers(1, &frontRightLegVBO);
    glGenBuffers(1, &frontRightLegEBO);
    // Bind the front left leg VAO
    glBindVertexArray(frontRightLegVAO);
    // Bind and fill the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, frontRightLegVBO);
    glBufferData(GL_ARRAY_BUFFER, frontRightLegVertices.size() * sizeof(float), frontRightLegVertices.data(), GL_STATIC_DRAW);
    // Bind and fill the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frontRightLegEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, frontRightLegIndices.size() * sizeof(unsigned int), frontRightLegIndices.data(), GL_STATIC_DRAW);
    // Set the vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // Unbind VAO, VBO, and EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // Activate shader and get uniform location
    glUseProgram(shaderProgram);

    // Back Left Leg VAO, VBO, EBO
    // -------------------------
    unsigned int backLeftLegVAO, backLeftLegVBO, backLeftLegEBO;
    glGenVertexArrays(1, &backLeftLegVAO);
    glGenBuffers(1, &backLeftLegVBO);
    glGenBuffers(1, &backLeftLegEBO);
    // Bind the front left leg VAO
    glBindVertexArray(backLeftLegVAO);
    // Bind and fill the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, backLeftLegVBO);
    glBufferData(GL_ARRAY_BUFFER, backLeftLegVertices.size() * sizeof(float), backLeftLegVertices.data(), GL_STATIC_DRAW);
    // Bind and fill the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, backLeftLegEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, backLeftLegIndices.size() * sizeof(unsigned int), backLeftLegIndices.data(), GL_STATIC_DRAW);
    // Set the vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // Unbind VAO, VBO, and EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // Activate shader and get uniform location
    glUseProgram(shaderProgram);

    // Back Right Leg VAO, VBO, EBO
    // -------------------------
    unsigned int backRightLegVAO, backRightLegVBO, backRightLegEBO;
    glGenVertexArrays(1, &backRightLegVAO);
    glGenBuffers(1, &backRightLegVBO);
    glGenBuffers(1, &backRightLegEBO);
    // Bind the front left leg VAO
    glBindVertexArray(backRightLegVAO);
    // Bind and fill the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, backRightLegVBO);
    glBufferData(GL_ARRAY_BUFFER, backRightLegVertices.size() * sizeof(float), backRightLegVertices.data(), GL_STATIC_DRAW);
    // Bind and fill the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, backRightLegEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, backRightLegIndices.size() * sizeof(unsigned int), backRightLegIndices.data(), GL_STATIC_DRAW);
    // Set the vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // Unbind VAO, VBO, and EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // Activate shader and get uniform location
    glUseProgram(shaderProgram);

    // Neck VAO, VBO, EBO
    // -------------------------
    unsigned int neckVAO, neckVBO, neckEBO;
    glGenVertexArrays(1, &neckVAO);
    glGenBuffers(1, &neckVBO);
    glGenBuffers(1, &neckEBO);
    // Bind the front left leg VAO
    glBindVertexArray(neckVAO);
    // Bind and fill the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, neckVBO);
    glBufferData(GL_ARRAY_BUFFER, neckVertices.size() * sizeof(float), neckVertices.data(), GL_STATIC_DRAW);
    // Bind and fill the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, neckEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, neckIndices.size() * sizeof(unsigned int), neckIndices.data(), GL_STATIC_DRAW);
    // Set the vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // Unbind VAO, VBO, and EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // Activate shader and get uniform location
    glUseProgram(shaderProgram);


    // Body VAO, VBO, EBO
    // -------------------------
    unsigned int bodyVBO, bodyVAO, bodyEBO;
    glGenVertexArrays(1, &bodyVAO);
    glGenBuffers(1, &bodyVBO);
    glGenBuffers(1, &bodyEBO);
    // Bind the Vertex Array Object first, then bind and set the vertex buffer(s), and configure the vertex attribute(s).
    glBindVertexArray(bodyVAO);
    // Bind and set up the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, bodyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bodyVertices), bodyVertices, GL_STATIC_DRAW);
    // Bind and set up the element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bodyEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bodyIndices), bodyIndices, GL_STATIC_DRAW);
    // Set the position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Set the color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Set the texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // Set the normals attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // Unbind VAO
    glBindVertexArray(0);
    // Activate shader program
    glUseProgram(shaderProgram);

    // Head VAO, VBO, EBO
    // -------------------------
    unsigned int headVAO, headVBO, headEBO;
    glGenVertexArrays(1, &headVAO);
    glGenBuffers(1, &headVBO);
    glGenBuffers(1, &headEBO);
    // Bind the sphere VAO
    glBindVertexArray(headVAO);
    // Bind and fill the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, headVBO);
    glBufferData(GL_ARRAY_BUFFER, headVertices.size() * sizeof(float), headVertices.data(), GL_STATIC_DRAW);
    // Bind and fill the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, headEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, headIndices.size() * sizeof(unsigned int), headIndices.data(), GL_STATIC_DRAW);
    // Set the vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // Unbind VAO, VBO, and EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // Activate shader and get uniform location
    glUseProgram(shaderProgram);

    // Eyes VAO, VBO, EBO
    // -------------------------
    // Left eye
    // --------
    unsigned int leftEyeVBO, leftEyeVAO, leftEyeEBO;
    glGenVertexArrays(1, &leftEyeVAO);
    glGenBuffers(1, &leftEyeVBO);
    glGenBuffers(1, &leftEyeEBO);
    // Bind the Vertex Array Object first, then bind and set the vertex buffer(s), and configure the vertex attribute(s).
    glBindVertexArray(leftEyeVAO);
    // Bind and set up the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, leftEyeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(leftEyeVertices), leftEyeVertices, GL_STATIC_DRAW);
    // Bind and set up the element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, leftEyeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(leftEyeIndices), leftEyeIndices, GL_STATIC_DRAW);
    // Set the position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Set the color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Set the texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // Set the normals attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // Unbind VAO
    glBindVertexArray(0);
    // Activate shader program
    glUseProgram(shaderProgram);

    // Right eye
    // --------
    unsigned int rightEyeVBO, rightEyeVAO, rightEyeEBO;
    glGenVertexArrays(1, &rightEyeVAO);
    glGenBuffers(1, &rightEyeVBO);
    glGenBuffers(1, &rightEyeEBO);
    // Bind the Vertex Array Object first, then bind and set the vertex buffer(s), and configure the vertex attribute(s).
    glBindVertexArray(rightEyeVAO);
    // Bind and set up the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, rightEyeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rightEyeVertices), rightEyeVertices, GL_STATIC_DRAW);
    // Bind and set up the element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rightEyeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rightEyeIndices), rightEyeIndices, GL_STATIC_DRAW);
    // Set the position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Set the color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Set the texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // Set the normals attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // Unbind VAO
    glBindVertexArray(0);
    // Activate shader program
    glUseProgram(shaderProgram);

    // * END LLAMA VAO, VBO, and EBO CREATION *
    // -------------------------------------------------------------------------------------------------------------
    

    // * END VAO, VBO, AND EBO CREATION *
    // -------------------------------------------------------------------------------------------------------------



    // Declares roationAxis
    glm::vec3 rotationAxis(0.0f, 1.0f, 0.0f);




    // * TEXTURE CREATION *
    // -------------------------------------------------------------------------------------------------------------
    unsigned int texture1, texture2, texture3, texture4, texture5, texture6, texture7, texture8, texture9, texture10, texture11, texture12;
    // Texture 1
    // ----------------------------------------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    //stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char* data = stbi_load("brick.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture 1: brick.jpg" << std::endl;
    }
    stbi_image_free(data);

    // Texture 2
    // ----------------------------------------
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    //stbi_set_flip_vertically_on_load(true);
    data = stbi_load("dicetexture.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture 2: dicetexture.png" << std::endl;
    }
    stbi_image_free(data);

    // Texture 3
    // ----------------------------------------
    glGenTextures(1, &texture3);
    glBindTexture(GL_TEXTURE_2D, texture3); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    // set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    //stbi_set_flip_vertically_on_load(true);
    data = stbi_load("wood.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture 3: wood.jpg" << std::endl;
    }
    stbi_image_free(data);

    // Texture 4
    // ----------------------------------------
    glGenTextures(1, &texture4);
    glBindTexture(GL_TEXTURE_2D, texture4); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    // set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    //stbi_set_flip_vertically_on_load(true);
    data = stbi_load("building.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture 4: building.jpg" << std::endl;
    }
    stbi_image_free(data);

    // Texture 5
    // ----------------------------------------
    glGenTextures(1, &texture5);
    glBindTexture(GL_TEXTURE_2D, texture5); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("plaque.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture 5: plaque.jpg" << std::endl;
    }
    stbi_image_free(data);

    // Texture 6
    // ----------------------------------------
    glGenTextures(1, &texture6);
    glBindTexture(GL_TEXTURE_2D, texture6); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    //stbi_set_flip_vertically_on_load(true);
    data = stbi_load("blackMarble.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture 6: blackMarble.jpg" << std::endl;
    }
    stbi_image_free(data);

    // Texture 7
    // ----------------------------------------
    glGenTextures(1, &texture7);
    glBindTexture(GL_TEXTURE_2D, texture7); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    //stbi_set_flip_vertically_on_load(true);
    data = stbi_load("fur.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture 7: fur.jpg" << std::endl;
    }
    stbi_image_free(data);

    // Texture 8
    // ----------------------------------------
    glGenTextures(1, &texture8);
    glBindTexture(GL_TEXTURE_2D, texture8); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    //stbi_set_flip_vertically_on_load(true);
    data = stbi_load("clayTextureBody.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture 8: clayTextureBody.jpg" << std::endl;
    }
    stbi_image_free(data);

    // Texture 9
    // ----------------------------------------
    glGenTextures(1, &texture9);
    glBindTexture(GL_TEXTURE_2D, texture9); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    //stbi_set_flip_vertically_on_load(true);
    data = stbi_load("neckClayTexture.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture 9: neckClayTexture.jpg" << std::endl;
    }
    stbi_image_free(data);

    // Texture 10
    // ----------------------------------------
    glGenTextures(1, &texture10);
    glBindTexture(GL_TEXTURE_2D, texture10); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    //stbi_set_flip_vertically_on_load(true);
    data = stbi_load("pyramidTexture.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture 10: pyramidTexture.jpg" << std::endl;
    }
    stbi_image_free(data);

    // Texture 11
    // ----------------------------------------
    glGenTextures(1, &texture11);
    glBindTexture(GL_TEXTURE_2D, texture11); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    //stbi_set_flip_vertically_on_load(true);
    data = stbi_load("seal.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture 11: stoneblocks.jpg" << std::endl;
    }
    stbi_image_free(data);

    // Texture 12
    // ----------------------------------------
    glGenTextures(1, &texture12);
    glBindTexture(GL_TEXTURE_2D, texture12); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    //stbi_set_flip_vertically_on_load(true);
    data = stbi_load("dicetexture1.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture 12: dicetexture1.jpg" << std::endl;
    }
    stbi_image_free(data);

    glUseProgram(shaderProgram);


    // Set texture units
    glUniform1i(texture1Location, 0);
    glUniform1i(texture2Location, 1);
    glUniform1i(texture3Location, 2);
    glUniform1i(texture4Location, 3);
    glUniform1i(texture5Location, 4);
    glUniform1i(texture6Location, 5);
    glUniform1i(texture7Location, 6);
    glUniform1i(texture8Location, 7);
    glUniform1i(texture9Location, 8);
    glUniform1i(texture10Location, 9);
    glUniform1i(texture11Location, 10);
    glUniform1i(texture12Location, 11);

    // * END TEXTURE CREATION *
    // -------------------------------------------------------------------------------------------------------------


    Material material;
    // Set the material uniforms in the shader program
    glUseProgram(shaderProgram);
    GLint materialAmbientLoc = glGetUniformLocation(shaderProgram, "material.ambient");
    GLint materialDiffuseLoc = glGetUniformLocation(shaderProgram, "material.diffuse");
    GLint materialSpecularLoc = glGetUniformLocation(shaderProgram, "material.specular");
    GLint materialShininessLoc = glGetUniformLocation(shaderProgram, "material.shininess");

    // RENDER LOOP
    // --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        float cameraSpeed = 2.5f * deltaTime;

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // create transformations
        glm::mat4 transform = glm::mat4(1.0f);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        float angle = glfwGetTime() * glm::radians(45.0f);

        // Camera
        // ------------------------------------------------------------------------

        glm::vec3 direction;

        // Declarations
        const float radius = 10.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;
        direction.x = cos(glm::radians(yaw)); // Note that we convert the angle to radians first
        direction.z = sin(glm::radians(yaw));
        direction.y = sin(glm::radians(pitch));
        // Camera Direction (Z)
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
        // Right Axis (X)
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
        // Up Axis (Y)
        glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
        ///* P is for perspective change
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        {
            // Toggle between orthographic and perspective views
            if (isPerspective)
            {
                // Switch to orthographic view
                isPerspective = false;
                projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f);
            }
            else
            {
                // Switch to perspective view
                isPerspective = true;
                projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);
            }

            // Update the projection matrix in the shader
            unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        }
        //*/

        // View Matrix
        // ----------------------------------------
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // Sets Transformation Matrices for Rendering the pyramid
        transform = glm::translate(transform, glm::vec3(0.0f, -0.5f, 0.0f)); // Move the pyramid to the origin
        transform = glm::rotate(transform, angle, glm::vec3(0.0f, 1.0f, 0.0f)); // Apply the rotation
        transform = glm::translate(transform, glm::vec3(0.0f, 0.5f, 0.0f));
        unsigned int pyramidTransformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(pyramidTransformLoc, 1, GL_FALSE, glm::value_ptr(transform));

        // Sets Transformation Matrices for Rendering the plane
        glm::mat4 planeTransform = glm::mat4(1.0f);
        planeTransform = glm::translate(planeTransform, glm::vec3(0.0f, 0.0f, 0.0f)); // Move the plane slightly below the pyramid
        unsigned int planeTransformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(planeTransformLoc, 1, GL_FALSE, glm::value_ptr(planeTransform));
        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Transformation matrix for the cuboid
        // --------------------
        //glm::mat4 cuboidTransform = glm::mat4(1.0f);
        //cuboidTransform = glm::translate(cuboidTransform, glm::vec3(0.0f, -0.5f, 0.0f)); // Move the cuboid to the desired position
        //cuboidTransform = glm::rotate(cuboidTransform, angle, glm::vec3(0.0f, 1.0f, 0.0f)); // Apply any necessary rotations or transformations
        // Retrieve the location of the transform uniform for the cuboid in the shader program
        //unsigned int cuboidTransformLoc = glGetUniformLocation(shaderProgram, "transform");
        // Set the value of the transform uniform for the cuboid
       // glUniformMatrix4fv(cuboidTransformLoc, 1, GL_FALSE, glm::value_ptr(cuboidTransform));


        // OBJECT RENDERING
        // ----------------------------------------

        // PYRAMID RENDER
        // --------------------
        // Bind textures
        // --------------------
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, texture4);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, texture5);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, texture6);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, texture7);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, texture8);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, texture9);
        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, texture10);
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, texture11);
        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_2D, texture12);

        // Set Mat properties
        material.ambient = glm::vec3(0.4, 0.4, 0.4);
        material.diffuse = glm::vec3(0.1, 0.07, 0.04);
        material.specular = glm::vec3(0.1, 0.1, 0.1);
        material.shininess = 0.05;

        // Update the material uniforms
        glUniform3fv(materialAmbientLoc, 1, glm::value_ptr(material.ambient));
        glUniform3fv(materialDiffuseLoc, 1, glm::value_ptr(material.diffuse));
        glUniform3fv(materialSpecularLoc, 1, glm::value_ptr(material.specular));
        glUniform1f(materialShininessLoc, material.shininess);

        // Render the pyramid
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), true);
        glUniform1i(glGetUniformLocation(shaderProgram, "textureChoice"), 9);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 24);
        glBindVertexArray(0);

        // PLANE RENDER
        // --------------------
        // Bind textures
        // --------------------
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, texture4);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, texture5);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, texture6);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, texture7);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, texture8);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, texture9);
        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, texture10);
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, texture11);
        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_2D, texture12);

        // Set Mat properties
        material.ambient = glm::vec3(0.4, 0.4, 0.4);
        material.diffuse = glm::vec3(0.05, 0.05, 0.05);
        material.specular = glm::vec3(0.5, 0.5, 0.5);
        material.shininess = 0.5;

        // Update the material uniforms
        glUniform3fv(materialAmbientLoc, 1, glm::value_ptr(material.ambient));
        glUniform3fv(materialDiffuseLoc, 1, glm::value_ptr(material.diffuse));
        glUniform3fv(materialSpecularLoc, 1, glm::value_ptr(material.specular));
        glUniform1f(materialShininessLoc, material.shininess);

        // Render the plane
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), true);
        glUniform1i(glGetUniformLocation(shaderProgram, "textureChoice"), 2);
        glBindVertexArray(VAOPlane);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);



        // DICE RENDERING
        // -----------------------------------------------------------------------------------------------------------
        // Bind textures
        // --------------------
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, texture4);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, texture5);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, texture6);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, texture7);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, texture8);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, texture9);
        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, texture10);
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, texture11);
        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_2D, texture12);

        // Set Mat properties
        material.ambient = glm::vec3(0.6, 0.6, 0.6);
        material.diffuse = glm::vec3(0.1, 0.1, 0.1);
        material.specular = glm::vec3(0.3, 0.3, 0.3);
        material.shininess = 0.3;

        // Update the material uniforms
        glUniform3fv(materialAmbientLoc, 1, glm::value_ptr(material.ambient));
        glUniform3fv(materialDiffuseLoc, 1, glm::value_ptr(material.diffuse));
        glUniform3fv(materialSpecularLoc, 1, glm::value_ptr(material.specular));
        glUniform1f(materialShininessLoc, material.shininess);

        // Render the Dice
        // Die 1
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), true);
        glUniform1i(glGetUniformLocation(shaderProgram, "textureChoice"), 1);
        glBindVertexArray(dice1VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Die 2
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), true);
        glUniform1i(glGetUniformLocation(shaderProgram, "textureChoice"), 1);
        glBindVertexArray(dice2VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // * END DICE RENDERING *
        // -----------------------------------------------------------------------------------------------------------

       

        // CERAMIC POT RENDERING
        // -----------------------------------------------------------------------------------------------------------
        // BODY (sphere)
        // --------------------
        // bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, texture4);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, texture5);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, texture6);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, texture7);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, texture8);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, texture9);
        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, texture10);
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, texture11);
        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_2D, texture12);

        // Set Mat properties
        material.ambient = glm::vec3(0.6, 0.6, 0.6);
        material.diffuse = glm::vec3(0.2, 0.1, 0.0);
        material.specular = glm::vec3(0.3, 0.3, 0.3);
        material.shininess = 0.3;

        // Update the material uniforms
        glUniform3fv(materialAmbientLoc, 1, glm::value_ptr(material.ambient));
        glUniform3fv(materialDiffuseLoc, 1, glm::value_ptr(material.diffuse));
        glUniform3fv(materialSpecularLoc, 1, glm::value_ptr(material.specular));
        glUniform1f(materialShininessLoc, material.shininess);

       // Render the Sphere
       // Set any necessary uniform variables
        glUseProgram(shaderProgram);
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), true);
        glUniform1i(glGetUniformLocation(shaderProgram, "textureChoice"), 7);
        glBindVertexArray(sphereVAO);
        // Draw the sphere using indices
        glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
        // Unbind the spheres VAO
        glBindVertexArray(0);

        // POT NECK (CYLINDER)
        // --------------------
        // bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, texture4);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, texture5);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, texture6);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, texture7);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, texture8);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, texture9);
        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, texture10);
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, texture11);
        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_2D, texture12);

        // Set Mat properties
        material.ambient = glm::vec3(0.4, 0.4, 0.4);
        material.diffuse = glm::vec3(0.2, 0.12, 0.04);
        material.specular = glm::vec3(0.3, 0.3, 0.3);
        material.shininess = 0.3;

        // Update the material uniforms
        glUniform3fv(materialAmbientLoc, 1, glm::value_ptr(material.ambient));
        glUniform3fv(materialDiffuseLoc, 1, glm::value_ptr(material.diffuse));
        glUniform3fv(materialSpecularLoc, 1, glm::value_ptr(material.specular));
        glUniform1f(materialShininessLoc, material.shininess);

        // Render the Cylinder
        // Set any necessary uniform variables
        glUseProgram(shaderProgram);
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), true);
        glUniform1i(glGetUniformLocation(shaderProgram, "textureChoice"), 8);
        glBindVertexArray(cylinderVAO);
        // Draw the cylinder using indices
        glDrawElements(GL_TRIANGLES, cylinderIndices.size(), GL_UNSIGNED_INT, 0);
        // Unbind the cylinders VAO
        glBindVertexArray(0);


        // * END CERAMIC POT RENDERING *
        // -----------------------------------------------------------------------------------------------------------



        // LLAMA RENDERING 
        // -----------------------------------------------------------------------------------------------------------
        // FEET
        // -----------------------------------
        // bind Texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, texture4);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, texture5);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, texture6);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, texture7);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, texture8);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, texture9);
        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, texture10);
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, texture11);
        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_2D, texture12);

        // Set Mat properties
        material.ambient = glm::vec3(0.4, 0.4, 0.4);
        material.diffuse = glm::vec3(0.05, 0.05, 0.05);
        material.specular = glm::vec3(0.1, 0.1, 0.1);
        material.shininess = 0.1;

        // Update the material uniforms
        glUniform3fv(materialAmbientLoc, 1, glm::value_ptr(material.ambient));
        glUniform3fv(materialDiffuseLoc, 1, glm::value_ptr(material.diffuse));
        glUniform3fv(materialSpecularLoc, 1, glm::value_ptr(material.specular));
        glUniform1f(materialShininessLoc, material.shininess);

        // Render front left foot block
        // --------------------
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), false);
        glUniform1i(glGetUniformLocation(shaderProgram, "colorChoice"), 6);
        glBindVertexArray(frontLeftFootVAO);
        glDrawElements(GL_TRIANGLES, sizeof(frontLeftFootVertices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Render front right foot block
        // --------------------
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), false);
        glUniform1i(glGetUniformLocation(shaderProgram, "colorChoice"), 6);
        glBindVertexArray(frontRightFootVAO);
        glDrawElements(GL_TRIANGLES, sizeof(frontRightFootVertices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Render back left foot block
        // --------------------
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), false);
        glUniform1i(glGetUniformLocation(shaderProgram, "colorChoice"), 6);
        glBindVertexArray(backLeftFootVAO);
        glDrawElements(GL_TRIANGLES, sizeof(backLeftFootVertices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Render back right foot block
        // --------------------
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), false);
        glUniform1i(glGetUniformLocation(shaderProgram, "colorChoice"), 6);
        glBindVertexArray(backRightFootVAO);
        glDrawElements(GL_TRIANGLES, sizeof(backRightFootVertices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // LEGS & NECK
        // -----------------------------------
        // bind Texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, texture4);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, texture5);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, texture6);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, texture7);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, texture8);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, texture9);
        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, texture10);
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, texture11);
        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_2D, texture12);

        // Set Mat properties
        material.ambient = glm::vec3(0.6, 0.6, 0.6);
        material.diffuse = glm::vec3(0.4, 0.4, 0.32);
        material.specular = glm::vec3(0.05, 0.05, 0.05);
        material.shininess = 0.05;

        // Update the material uniforms
        glUniform3fv(materialAmbientLoc, 1, glm::value_ptr(material.ambient));
        glUniform3fv(materialDiffuseLoc, 1, glm::value_ptr(material.diffuse));
        glUniform3fv(materialSpecularLoc, 1, glm::value_ptr(material.specular));
        glUniform1f(materialShininessLoc, material.shininess);

        // Render the Front Left Leg
        // Set any necessary uniform variables
        glUseProgram(shaderProgram);
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), true);
        glUniform1i(glGetUniformLocation(shaderProgram, "textureChoice"), 6);
        glBindVertexArray(frontLeftLegVAO);
        // Draw the front left leg using indices
        glDrawElements(GL_TRIANGLES, frontLeftLegIndices.size(), GL_UNSIGNED_INT, 0);
        // Unbind the front left leg's VAO
        glBindVertexArray(0);

        // Render the Front Right Leg
        glBindVertexArray(frontRightLegVAO);
        // Draw the front left leg using indices
        glDrawElements(GL_TRIANGLES, frontRightLegIndices.size(), GL_UNSIGNED_INT, 0);
        // Unbind the front left leg's VAO
        glBindVertexArray(0);

        // Render the Back Left Leg
        glBindVertexArray(backLeftLegVAO);
        // Draw the front left leg using indices
        glDrawElements(GL_TRIANGLES, backLeftLegIndices.size(), GL_UNSIGNED_INT, 0);
        // Unbind the front left leg's VAO
        glBindVertexArray(0);

        // Render the Back Right Leg
        glBindVertexArray(backRightLegVAO);
        // Draw the front left leg using indices
        glDrawElements(GL_TRIANGLES, backRightLegIndices.size(), GL_UNSIGNED_INT, 0);
        // Unbind the front left leg's VAO
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, texture4);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, texture5);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, texture6);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, texture7);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, texture8);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, texture9);
        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, texture10);
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, texture11);
        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_2D, texture12);

        // Set Mat properties
        material.ambient = glm::vec3(0.6, 0.6, 0.6);
        material.diffuse = glm::vec3(0.4, 0.4, 0.32);
        material.specular = glm::vec3(0.05, 0.05, 0.05);
        material.shininess = 0.05;

        // Update the material uniforms
        glUniform3fv(materialAmbientLoc, 1, glm::value_ptr(material.ambient));
        glUniform3fv(materialDiffuseLoc, 1, glm::value_ptr(material.diffuse));
        glUniform3fv(materialSpecularLoc, 1, glm::value_ptr(material.specular));
        glUniform1f(materialShininessLoc, material.shininess);

        // Render the Neck
        glBindVertexArray(neckVAO);
        // Draw the front left leg using indices
        glDrawElements(GL_TRIANGLES, neckIndices.size(), GL_UNSIGNED_INT, 0);
        // Unbind the front left leg's VAO
        glBindVertexArray(0);

        // BODY & HEAD
        // -----------------------------------
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, texture4);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, texture5);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, texture6);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, texture7);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, texture8);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, texture9);
        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, texture10);
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, texture11);
        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_2D, texture12);

        // Set Mat properties
        material.ambient = glm::vec3(0.6, 0.6, 0.6);
        material.diffuse = glm::vec3(0.4, 0.4, 0.32);
        material.specular = glm::vec3(0.05, 0.05, 0.05);
        material.shininess = 0.05;

        // Update the material uniforms
        glUniform3fv(materialAmbientLoc, 1, glm::value_ptr(material.ambient));
        glUniform3fv(materialDiffuseLoc, 1, glm::value_ptr(material.diffuse));
        glUniform3fv(materialSpecularLoc, 1, glm::value_ptr(material.specular));
        glUniform1f(materialShininessLoc, material.shininess);

        // Render the body
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), true);
        glUniform1i(glGetUniformLocation(shaderProgram, "textureChoice"), 6);
        glBindVertexArray(bodyVAO);
        glDrawElements(GL_TRIANGLES, sizeof(bodyVertices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Render the head
        // Set any necessary uniform variables
        glUseProgram(shaderProgram);
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), true);
        glUniform1i(glGetUniformLocation(shaderProgram, "textureChoice"), 6);
        glBindVertexArray(headVAO);
        // Draw the sphere using indices
        glDrawElements(GL_TRIANGLES, headIndices.size(), GL_UNSIGNED_INT, 0);
        // Unbind the sphere's VAO
        glBindVertexArray(0);

        // EYES
        // -----------------------------------
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, texture4);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, texture5);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, texture6);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, texture7);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, texture8);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, texture9);
        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, texture10);
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, texture11);
        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_2D, texture12);

        // Set Mat properties
        material.ambient = glm::vec3(0.3, 0.3, 0.3);
        material.diffuse = glm::vec3(0.0, 0.0, 0.0);
        material.specular = glm::vec3(0.05, 0.05, 0.05);
        material.shininess = 0.1;

        // Update the material uniforms
        glUniform3fv(materialAmbientLoc, 1, glm::value_ptr(material.ambient));
        glUniform3fv(materialDiffuseLoc, 1, glm::value_ptr(material.diffuse));
        glUniform3fv(materialSpecularLoc, 1, glm::value_ptr(material.specular));
        glUniform1f(materialShininessLoc, material.shininess);

        // Left eye
        // -----------
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), false);
        glUniform1i(glGetUniformLocation(shaderProgram, "colorChoice"), 6);
        glBindVertexArray(leftEyeVAO);
        glDrawElements(GL_TRIANGLES, sizeof(leftEyeVertices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Right eye
        // -----------
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), false);
        glUniform1i(glGetUniformLocation(shaderProgram, "colorChoice"), 6);
        glBindVertexArray(rightEyeVAO);
        glDrawElements(GL_TRIANGLES, sizeof(rightEyeVertices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // * END LLAMA RENDERING *
        // -----------------------------------------------------------------------------------------------------------



        // 9/11 MEMORIAL BLOCKS RENDERING
        // -----------------------------------------------------------------------------------------------------------
        // Bind textures
        // --------------------
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, texture4);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, texture5);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, texture6);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, texture7);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, texture8);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, texture9);
        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, texture10);
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, texture11);
        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_2D, texture12);

        // Set Mat properties
        material.ambient = glm::vec3(0.4, 0.4, 0.4);
        material.diffuse = glm::vec3(0.05, 0.05, 0.05);
        material.specular = glm::vec3(0.05, 0.05, 0.05);
        material.shininess = 0.05;

        // Update the material uniforms
        glUniform3fv(materialAmbientLoc, 1, glm::value_ptr(material.ambient));
        glUniform3fv(materialDiffuseLoc, 1, glm::value_ptr(material.diffuse));
        glUniform3fv(materialSpecularLoc, 1, glm::value_ptr(material.specular));
        glUniform1f(materialShininessLoc, material.shininess);
        // Render left "tower" block
        // --------------------
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), true);
        glUniform1i(glGetUniformLocation(shaderProgram, "textureChoice"), 3);
        glBindVertexArray(towerVAO);
        glDrawElements(GL_TRIANGLES, sizeof(leftBlockCuboidVertices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Render right "tower" block
        // --------------------
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), true);
        glUniform1i(glGetUniformLocation(shaderProgram, "textureChoice"), 3);
        glBindVertexArray(towerVAO1);
        glDrawElements(GL_TRIANGLES, sizeof(rightBlockCuboidVertices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Set Mat properties
        material.ambient = glm::vec3(0.4, 0.4, 0.4);
        material.diffuse = glm::vec3(0.35, 0.30, 0.17);
        material.specular = glm::vec3(0.3, 0.3, 0.3);
        material.shininess = 0.3;

        // Update the material uniforms
        glUniform3fv(materialAmbientLoc, 1, glm::value_ptr(material.ambient));
        glUniform3fv(materialDiffuseLoc, 1, glm::value_ptr(material.diffuse));
        glUniform3fv(materialSpecularLoc, 1, glm::value_ptr(material.specular));
        glUniform1f(materialShininessLoc, material.shininess);
        // Render the text block
        // --------------------
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), true);
        glUniform1i(glGetUniformLocation(shaderProgram, "textureChoice"), 4);
        glBindVertexArray(textBlockVAO);
        glDrawElements(GL_TRIANGLES, sizeof(textBlockVertices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Enable blending
        glEnable(GL_BLEND);

        // Set the blending function
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Set Mat properties
        material.ambient = glm::vec3(0.9, 0.9, 0.9);
        material.diffuse = glm::vec3(0.6, 0.6, 0.6);
        material.specular = glm::vec3(0.7, 0.7, 0.7);
        material.shininess = 0.7;

        // Update the material uniforms
        glUniform3fv(materialAmbientLoc, 1, glm::value_ptr(material.ambient));
        glUniform3fv(materialDiffuseLoc, 1, glm::value_ptr(material.diffuse));
        glUniform3fv(materialSpecularLoc, 1, glm::value_ptr(material.specular));
        glUniform1f(materialShininessLoc, material.shininess);

        // Render the Cuboid Block
        // --------------------
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), false);
        glUniform1i(glGetUniformLocation(shaderProgram, "colorChoice"), 1); // Set colorChoice to 1 for white color
        glBindVertexArray(VAOCuboidBlock);
        glDrawElements(GL_TRIANGLES, sizeof(cuboidBlockIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glDisable(GL_BLEND);

        // * END 9/11 MEMORIAL BLOCKS RENDERING *
        // -----------------------------------------------------------------------------------------------------------



        // LIGHT RENDERING
        // -----------------------------------------------------------------------------------------------------------
        // Render the key light
        // --------------------
        glm::mat4 keyLightModelMatrix = glm::mat4(1.0f);
        keyLightModelMatrix = glm::translate(keyLightModelMatrix, keyLight.position);
        keyLightModelMatrix = glm::scale(keyLightModelMatrix, glm::vec3(0.1f)); // Adjust the scale as needed
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(keyLightModelMatrix));
        glUniform3fv(glGetUniformLocation(shaderProgram, "keyLight.position"), 1, glm::value_ptr(keyLight.position));
        glUniform3fv(glGetUniformLocation(shaderProgram, "keyLight.color"), 1, glm::value_ptr(keyLight.color));
        glUniform1f(glGetUniformLocation(shaderProgram, "keyLight.intensity"), keyLight.intensity);

        // Bind and render the key light VAO
        // --------------------
        glBindVertexArray(keyLightVAO);
        int numKeyLightVertices = sizeof(sphereVertices) / sizeof(sphereVertices[0]);
        glDrawArrays(GL_TRIANGLES, 0, numKeyLightVertices);
        glBindVertexArray(0);

        // Render the fill light
        // --------------------
        glm::mat4 fillLightModelMatrix = glm::mat4(1.0f);
        fillLightModelMatrix = glm::translate(fillLightModelMatrix, fillLight.position);
        fillLightModelMatrix = glm::scale(fillLightModelMatrix, glm::vec3(0.1f)); // Adjust the scale as needed
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(fillLightModelMatrix));
        glUniform3fv(glGetUniformLocation(shaderProgram, "fillLight.position"), 1, glm::value_ptr(fillLight.position));
        glUniform3fv(glGetUniformLocation(shaderProgram, "fillLight.color"), 1, glm::value_ptr(fillLight.color));
        glUniform1f(glGetUniformLocation(shaderProgram, "fillLight.intensity"), fillLight.intensity);

        // Bind and render the fill light VAO
        // --------------------
        glBindVertexArray(fillLightVAO);
        int numFillLightVertices = sizeof(sphereVertices) / sizeof(sphereVertices[0]);
        glDrawArrays(GL_TRIANGLES, 0, numFillLightVertices);
        glBindVertexArray(0);

        // * END LIGHT RENDERING *
        // -----------------------------------------------------------------------------------------------------------


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAOPlane);
    glDeleteBuffers(1, &VBOPlane);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // W is 'forward'
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    // S is 'backward'
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    // A is 'left'
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    // D is 'right'
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    // Q is 'up'
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cameraPos += cameraSpeed * glm::vec3(0.0f, 1.0f, 0.0f);
    // E is 'down'
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cameraPos -= cameraSpeed * glm::vec3(0.0f, 1.0f, 0.0f);
    // + is 'speed up'
    if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
        cameraSpeed += 0.1f;
    // - is 'slow down'
    if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
        cameraSpeed -= 0.1f;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
// Scroll mouse wheel to change camera speed
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

    cameraSpeed += (float)yoffset * 0.1f;  // Adjust the camera speed based on yoffset

    // Add any additional logic or constraints if necessary

    // Example constraint: Ensure cameraSpeed doesn't go below a minimum value
    if (cameraSpeed < 0.1f)
        cameraSpeed = 0.1f;

    // Example constraint: Ensure cameraSpeed doesn't exceed a maximum value
    if (cameraSpeed > 5.0f)
        cameraSpeed = 5.0f;
}
