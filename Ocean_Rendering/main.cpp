#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cmath>
#include <unordered_map>
#include <functional>
#include <random>
#include <complex>
#include <fstream>
#include <sstream>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#define PI 3.14159265358979

#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Utils.h"
#include "Sphere.h"

#define SCR_WIDTH 1200
#define SCR_HEIGHT 900


const char* VS2 = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 aTexCoords;\n"
"out vec2 TexCoord;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n"
"   TexCoord = aTexCoords;\n"
"}\n\0";
const char* FS2 = "#version 330 core\n"
"out vec4 fragColor;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
"   vec4 texColor = texture(ourTexture, TexCoord);\n"
"   if(texColor.x < 0.0) {fragColor = vec4(0.0, 0.0, 1.0, 1.0);}\n"
"   else {fragColor = vec4(1.0,0.0,0.0,1.0);}\n"
"}\n\0";


//Important modificaiton: Changed vertex shader vec3 inputs to vec4 (due to shader storage std430 layout)
const char* VS = "#version 460 core\n"
"layout (location = 0) in vec4 aPos;\n"
"layout (location = 1) in vec4 aNormal;\n"
"out vec3 normal;\n"
"out vec3 fragPos;\n"
"uniform mat4 projection;\n"
"uniform mat4 view;\n"
"uniform vec2 offsets[4];\n"
"void main()\n"
"{\n"
"   vec2 offset = offsets[gl_InstanceID];\n"
"   vec4 pos = projection * view * vec4(aPos.x + offset.x, aPos.y, aPos.z + offset.y, 1.0);\n"
"   gl_Position = pos.xyzw;\n"
"   normal = aNormal.xyz;\n"
"   fragPos = vec3(aPos.x + offset.x, aPos.y, aPos.z + offset.y);\n"
"}\n\0";
//fragment shader code from Tessendorf's renderman shader (Tessendorf, 2004)
const char* FS = "#version 460 core\n"
"in vec3 normal;\n"
"in vec3 fragPos;\n"
"out vec4 fragColor;\n"
"uniform vec3 cameraPos;\n"
"void main()\n"
"{\n"
"   vec4 upwelling = vec4(0.0, 0.2, 0.3, 1.0);\n" //from 0, 0.2, 0.3, 1.0
"   vec4 sky = vec4(0.69, 0.84, 1.0, 1.0);\n" //from 0.69, 0.84, 1.0
"   vec4 air = vec4(0.1, 0.1, 0.1, 1.0); //from 0.1,0.1,0.1 \n"
"   float nSnell = 1.34;\n"
"   float kDiffuse = 0.01; //from 0.91 \n"
"   float reflectivity;\n"
"   vec3 nI = normalize(cameraPos - fragPos);\n"
"   vec3 nN = normalize(normal);\n"
"   float costhetai = abs(dot(nI, nN));\n"
"   float thetai = acos(costhetai);\n"
"   float sinthetat = sin(thetai)/nSnell;\n"
"   float thetat = asin(sinthetat);\n"
"   if (thetai == 0.0)\n"
"   {\n"
"      reflectivity = (nSnell - 1)/(nSnell + 1);\n"
"      reflectivity = reflectivity * reflectivity;\n"
"   }\n"
"   else\n"
"   {\n"
"      float fs = sin(thetat - thetai) / sin(thetat + thetai);\n"
"      float ts = tan(thetat - thetai) / tan(thetat + thetai);\n"
"      reflectivity = 0.5 * ( fs*fs + ts*ts );\n"
"   }\n"
"   vec3 dPE = fragPos - cameraPos;\n"
"   float dist = length(dPE) * kDiffuse;\n"
"   dist = exp(-dist);\n"
"   fragColor = dist * ( reflectivity * sky + (1 - reflectivity) * upwelling) + (1-dist)* air;\n"
"}\n\0";

const char* FSb = "#version 460 core\n"
"in vec3 normal;\n"
"in vec3 fragPos;\n"
"out vec4 fragColor;\n"
"uniform vec3 cameraPos;\n"
"void main() {\n"
"    vec3 lightDir = normalize(vec3(0.3, 1.0, 0.4));\n"
"    vec3 lightColor = vec3(1.0, 1.0, 1.0); // Light color\n"
"    vec3 deepWaterColor = vec3(0.0, 0.2, 0.3); // Deep water base color\n"

"    vec3 N = normalize(normal); // Normal at the fragment\n"
"    vec3 V = normalize(cameraPos - fragPos); // View direction\n"
"    vec3 L = normalize(lightDir); // Light direction\n"

"    float fresnel = pow(1.0 - max(dot(N, V), 0.0), 3.0) * 0.9 + 0.1;\n"


"    vec3 R = reflect(-V, N);\n"
"    vec3 reflectionColor = vec3(0.286, 0.75, 1.0);\n"
"    float diffuse = max(dot(N, L), 0.0);\n"
"    vec3 diffuseColor = lightColor * diffuse;\n"
"    vec3 halfwayDir = normalize(L + V);\n"
"    float spec = pow(max(dot(N, halfwayDir), 0.0), 64.0);\n"
"    vec3 specularColor = lightColor * spec;\n"
"    vec3 refractionColor = deepWaterColor;\n"
"    vec3 finalColor = mix(refractionColor, reflectionColor, fresnel);\n"
"    finalColor += diffuseColor + specularColor;\n"
"    fragColor = vec4(pow(finalColor, vec3(1.0 / 2.2)), 1.0);\n"
"}\n\0";

const char* FSc = "#version 460 core\n"
"in vec3 normal;\n"
"in vec3 fragPos;\n"
"out vec4 fragColor;\n"
"uniform vec3 cameraPos;\n"
"void main() {\n"
"   vec3 ambient = vec3(0.1, 0.1, 0.1);\n"
"   vec3 lightDir = normalize(cameraPos - fragPos); //vec3(0.3, 1.0, 0.4)\n"
"   float diff = max(dot(normal, lightDir), 0.0);\n"
"   vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);\n"
"   vec3 result = (ambient + diffuse) * vec3(0.0, 0.5, 1.0);\n"
"   fragColor = vec4(result, 1.0);\n"
"}\n\0";

const char* FSd = "#version 460 core\n"
"in vec3 normal;\n"
"in vec3 fragPos;\n"
"out vec4 fragColor;\n"
"uniform vec3 cameraPos;\n"
"void main()\n"
"{\n"
"   vec3 upwelling = vec3(0.0, 0.2, 0.3);\n" 
"   vec3 sky = vec3(0.227, 0.4, 0.6);\n"
"   vec3 viewDir = normalize(cameraPos - fragPos);\n"
"   vec3 normal = normalize(normal);\n"
"   float reflectivity = pow(1.0 - dot(viewDir, normal), 5.0);\n"
"   //if(reflectivity > 1.0) reflectivity = 1.0;\n"
"   //ATMOSPHERE ENVIRONMENT MAPPING\n"
"   vec3 reflectDir = normalize(reflect(-viewDir, normal));\n"
"   float stackAngle = asin(reflectDir.y);\n\n"
"   vec3 localZenith = vec3(0.286, 0.75, 1.0);\n"
"   vec3 localHorizon = vec3(0.643, 0.875, 1.0);\n"
"   vec3 skyColorA = vec3(1.0);\n"
"   float stackInfluence = 1.0 - stackAngle / (3.14159265/2);\n"
"   skyColorA.x = mix(localZenith.x, localHorizon.x, pow(stackInfluence, 8.0));\n"
"   skyColorA.y = mix(localZenith.y, localHorizon.y, pow(stackInfluence, 8.0));\n"
"   skyColorA.z = mix(localZenith.z, localHorizon.z, pow(stackInfluence, 8.0));\n"
"   vec3 fresnel = reflectivity * sky;\n"
"   //DIFFUSE\n"
"   vec3 diffuse = upwelling * (1.0 - reflectivity);\n"
"   vec3 finalColor = diffuse + fresnel;\n"
"   fragColor = vec4(finalColor, 1.0);\n"
"}\n\0";



const char* cVS = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec4 aColor;\n"
"out vec4 color;\n"
"uniform mat4 projection;\n"
"uniform mat4 view;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * vec4(aPos, 1.0);\n"
"   color = aColor;\n"
"}\n\0";
const char* cFS = "#version 330 core\n"
"out vec4 fragColor;\n"
"in vec4 color;\n"
"void main()\n"
"{\n"
"   fragColor = color;\n"
"}\n\0";


int main()
{

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GRAPHICS 08052024", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
        
    computeShaderInputTexture tempImage(1, GL_READ_WRITE, 16, 16);
    computeShaderInputTexture tempImage2(2, GL_READ_WRITE, 16, 16);

   
    computeShader FwaveheightShader("./shaders/Fwaveheight.txt");
    computeShader Fwavedxdy("./shaders/Fwavedxdy.txt");
    computeShader FFTrow("./shaders/FFTrownew.txt");
    computeShader FFTcol("./shaders/FFTcolnew.txt");
    computeShader meshingShader("./shaders/meshingShader.txt");
    computeShader quadrantSwapShader("./shaders/quadrantSwap.txt");
    computeShader numericalCorrectionShader("./shaders/numericalCorrection.txt");


    //Let's generate the oceanographic spectrum 
    computeShaderInputTexture phillipsSpectrum(0, GL_READ_WRITE, 512, 512);
    computeShaderInputTexture Fwaveheight(1, GL_READ_WRITE, 512, 512);
    computeShaderInputTexture Fwavedx(2, GL_READ_WRITE, 512, 512);
    computeShaderInputTexture Fwavedy(3, GL_READ_WRITE, 512, 512);
    computeShaderInputTexture waveheight(4, GL_READ_WRITE, 512, 512);
    computeShaderInputTexture wavedx(5, GL_READ_WRITE, 512, 512);
    computeShaderInputTexture wavedy(6, GL_READ_WRITE, 512, 512);



    //Populate the texture with gaussian noise in the red and green channels.     
    std::random_device d{};
    std::mt19937 gen{ d() };
    int N = 512;
    //Generating a Gaussian noise image
    std::normal_distribution<float> dist{ 0, 1 };
    //complex gaussian numbers
    for (int y = 0; y < N; y++)
    {
        for (int x = 0; x < N; x++)
        {
            float xi_r = dist(gen);
            float xi_i = dist(gen);
            setPixelVec3(phillipsSpectrum.data, x, y, glm::vec3(xi_r, xi_i, 0.0));
        }
    }


    //Phillips spectrum to filter gaussian pairs
    float L_x = 1000;
    float V = 31;
    float l = 0.01;
    float A = 0.0081;
    float L = V * V / 9.81;
    glm::vec2 windDir = glm::vec2(0.707, 0.707);
    for (int y = 0; y < N; y++)
    {
        for (int x = 0; x < N; x++)
        {
            float n = x - 256; float m = y - 256;
            glm::vec2 k = glm::vec2(2 * PI * n / L_x, 2 * PI * m / L_x);
            float wavenum = glm::length(k);
            glm::vec2 k_n = (wavenum == 0) ? glm::vec2(0.0, 0.0) : glm::normalize(k);

            float Phillips;
            if (wavenum == 0) Phillips = 0;
            else
            {
                Phillips = A * std::exp(-1 / (wavenum * wavenum * L * L + 1e-8)) * std::abs(glm::dot(k_n, windDir))
                    * std::abs(glm::dot(k_n, windDir)) * std::exp(-wavenum * wavenum * l * l) / (std::pow(wavenum + 1e-8, 4));
            }

            float xi_r = readPixelValue(phillipsSpectrum.data, x, y, 0);
            float xi_i = readPixelValue(phillipsSpectrum.data, x, y, 1);

            float h0bar_r = xi_r * std::sqrt(std::max(Phillips, 0.0f)) / std::sqrt(2.f);
            float h0bar_i = xi_i * std::sqrt(std::max(Phillips, 0.0f)) / std::sqrt(2.f);

            setPixelVec3(phillipsSpectrum.data, x, y, glm::vec3(h0bar_r, h0bar_i, 0.0f));

        }
    }

    //Important step we cannot miss: reuploading the unsigned char* data to the gpu. 
    glBindTexture(GL_TEXTURE_2D, phillipsSpectrum.ID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 512, 512, 0, GL_RGBA, GL_FLOAT, phillipsSpectrum.data);



    //Quad Vertices
#define V1 -1.0f, 1.0f, 0.0f
#define V2 1.0f, 1.0f, 0.0f
#define V3 -1.0f, -1.0f, 0.0f
#define V4 1.0f, -1.0f, 0.0f
#define T1 0.0f, 1.0f
#define T2 1.0f, 1.0f
#define T3 0.0f, 0.0f
#define T4 1.0f, 0.0f
    float tVertices[] = { V1, T1, V2, T2, V3, T3, V2, T2, V3, T3, V4, T4 };
    unsigned int VAO2, VBO2;
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);
    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tVertices), tVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
 

    Shader mShader(VS, FS);    
    Shader quadShader(VS2, FS2);
    Camera mainCamera(window);
    glEnable(GL_DEPTH_TEST);
    float lastFrame = 0.0f;
    float deltaTime = 0.0f;

    float testTime = 6.28f;


    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 50135232, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, VBO);


    float frameTime = 0.0f;

    Shader colorShader(cVS, cFS);

    glm::vec2 translations[4] = { glm::vec2(0.0f, 0.0f), glm::vec2(51.1f, 0.0f), glm::vec2(0.0f, 51.1f), glm::vec2(51.1f, 51.1f)};
    glUseProgram(mShader.ID);
    for (int i = 0; i < 4; i++) 
    {
        std::string name = "offsets[" + std::to_string(i) + "]";
        glUniform2f(glGetUniformLocation(mShader.ID, name.c_str()), translations[i].x, translations[i].y);
    }


    mainCamera.cameraSpeed = 20.f;
    mainCamera.cameraPos = glm::vec3(0.0, 25.0, 0.0);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glClearColor(0.47f, 0.59f, 0.62f, 1.0f); //0.69, 0.89, 1.0
        glClearColor(0.227f, 0.4f, 0.6f, 1.0f); //0.69, 0.89, 1.0

        //Sleep(1000);

        float t1 = glfwGetTime();

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        mainCamera.update(deltaTime);

        //NEW SHADER START
        glBindImageTexture(0, phillipsSpectrum.ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        glBindImageTexture(1, Fwaveheight.ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        glUseProgram(FwaveheightShader.program);
        glUniform1f(glGetUniformLocation(FwaveheightShader.program, "time"), currentFrame);
        glDispatchCompute(512, 512, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


        glBindImageTexture(2, Fwavedx.ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        glBindImageTexture(3, Fwavedy.ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        glUseProgram(Fwavedxdy.program);
        glUniform1f(glGetUniformLocation(Fwavedxdy.program, "time"), currentFrame);
        glDispatchCompute(512, 512, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);        

        

        //Quadrant swapping:        
        glBindImageTexture(1, Fwaveheight.ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        glUseProgram(quadrantSwapShader.program);
        glDispatchCompute(512, 256, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);        

        glBindImageTexture(1, Fwavedx.ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        glUseProgram(quadrantSwapShader.program);
        glDispatchCompute(512, 256, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        glBindImageTexture(1, Fwavedy.ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        glUseProgram(quadrantSwapShader.program);
        glDispatchCompute(512, 256, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        glBindImageTexture(1, Fwaveheight.ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        glBindImageTexture(2, Fwavedx.ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        glBindImageTexture(3, Fwavedy.ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);        
                
        

        glUseProgram(FFTrow.program);
        glDispatchCompute(1, 512, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        
        glUseProgram(FFTcol.program);
        glDispatchCompute(512, 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);           
        

        //Now, before meshing, rebind the fwaveheight, fwavedx and fwavedy so that they can be used in the meshingshader. 
        glBindImageTexture(4, Fwaveheight.ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        glBindImageTexture(5, Fwavedx.ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        glBindImageTexture(6, Fwavedy.ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        //NEW NORMALIZATION END.

        //Numerical correction
        glUseProgram(numericalCorrectionShader.program);
        glDispatchCompute(512, 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        //Meshing
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, VBO);

        glUseProgram(meshingShader.program);
        glDispatchCompute(511, 511, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        //Compute shaders end        

        //Render        
        glm::mat4 view = glm::lookAt(mainCamera.cameraPos, mainCamera.cameraPos + mainCamera.cameraFront, mainCamera.cameraUp);
        glm::mat4 projection = glm::perspective(45.0f, 1200.0f / 900.0f, 0.1f, 2000.0f);

        //3d mode
        glUseProgram(mShader.ID);
        glUniformMatrix4fv(glGetUniformLocation(mShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(mShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniform3fv(glGetUniformLocation(mShader.ID, "cameraPos"), 1, glm::value_ptr(mainCamera.cameraPos));
        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 1566726);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 1566726, 4);

        ////Quad mode
        //glUseProgram(quadShader.ID);
        //glBindTexture(GL_TEXTURE_2D, Fwaveheight.ID);
        //glBindVertexArray(VAO2);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        

        //Atmsophere render
        glUseProgram(colorShader.ID);
        glUniformMatrix4fv(glGetUniformLocation(colorShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(colorShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        Sphere atmosphere(512, 32, 32, glm::vec3(mainCamera.cameraPos), 0.0, 1.0);
        atmosphere.Draw();

              

        /*textR.renderText("FPS: " + std::to_string(1.f / deltaTime), 20.f, 570.f, 0.2f, glm::vec3(0.0f, 0.0f, 0.0f));
        textR.renderText("Position: " + std::to_string(mainCamera.cameraPos.x) + ", " + std::to_string(mainCamera.cameraPos.y) + ", "
            + std::to_string(mainCamera.cameraPos.z), 20.f, 550.f, 0.2f, glm::vec3(0.0f, 0.0f, 0.0f));*/


        glfwSwapBuffers(window);
        glfwPollEvents();


    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();

    return 0;
}

