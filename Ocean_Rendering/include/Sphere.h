#pragma once

#define PI 3.14159265358979

#ifndef SPHERE_H
#define SPHERE_H

class Sphere
{
private:
    std::vector<float> sphere_vertices;
    //std::vector<float> sphere_texcoord;
    std::vector<float> sphere_colorcoords;
    std::vector<int> sphere_indices;
    GLuint VBO, VAO, EBO;
    float radius = 1.0f;
    int sectorCount = 36;
    int stackCount = 18;

public:

    ~Sphere()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
    Sphere(float r, int sectors, int stacks, glm::vec3 pos, float timeOfDay, float nightfactor)
    {
        radius = r;
        sectorCount = sectors;
        stackCount = stacks;


        /* GENERATE VERTEX ARRAY */
        float x, y, z, xy;                              // vertex position
        float lengthInv = 1.0f / radius;    // vertex normal
        //float s, t;                                     // vertex texCoord

        float sectorStep = (float)(2 * PI / sectorCount);
        float stackStep = (float)(PI / stackCount);
        float sectorAngle, stackAngle;

        for (int i = 0; i <= stackCount; ++i)
        {
            stackAngle = (float)(PI / 2 - i * stackStep);        // starting from pi/2 to -pi/2
            xy = 1.02f * radius * cosf(stackAngle);             // r * cos(u)
            z = radius * sinf(stackAngle);              // r * sin(u)

            // add (sectorCount+1) vertices per stack
            // the first and last vertices have same position and normal, but different tex coords
            for (int j = 0; j <= sectorCount; ++j)
            {
                sectorAngle = j * sectorStep;           // starting from 0 to 2pi

                // vertex position (x, y, z)
                x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)  
                y = xy * sinf(sectorAngle);				// r * cos(u) * sin(v)
                sphere_vertices.push_back(x + pos.x);
                sphere_vertices.push_back(z + pos.y);
                sphere_vertices.push_back(y + pos.z);

                //Our color will depend on the stack angle
                //glm::vec4 z = glm::vec4(38.f / 255.f, 76.f / 255.f, 142.f / 255.f, 1.0f); //Zenith                
                //glm::vec4 h = glm::vec4(163.f / 255.f, 196.f / 255.f, 232.f / 255.f, 1.0f); //Horizon  
                //glm::vec4 h = glm::vec4(197.f / 255.f, 153.f / 255.f, 124.f / 255.f, 1.0f); //Horizon at sunset/sunrise
                /*glm::vec4 z = glm::vec4(0, 0, 0, 1);
                glm::vec4 h = glm::vec4(0, 0, 0, 1);
                float p = stackAngle;

                float c1 = std::abs(std::sin(p)) / (std::abs(std::sin(p)) + std::abs(std::cos(p)));
                float c2 = std::abs(std::cos(p)) / (std::abs(std::sin(p)) + std::abs(std::cos(p)));

                glm::vec4 skyColor = c1 * z + c2 * h;*/


                //float timeOfDay = 0.5f;

                glm::vec4 noonColorZenith = glm::vec4(0.286, 0.75, 1.0, 1.0);
                glm::vec4 duskColorZenith = glm::vec4(0.286, 0.376, 0.5, 1.0);

                glm::vec4 noonColorHorizon = glm::vec4(0.643, 0.875, 1.0, 1.0);                
                glm::vec4 duskColorHorizon = glm::vec4(0.859, 0.627, 0.5, 1.0);

                //We can first interpolate to obtain the local zenith and horizon colors based on the time of day.
                glm::vec4 localZenith;                


                localZenith.x = glm::mix(0.286, 0.286, timeOfDay);
                localZenith.y = glm::mix(0.75, 0.376, timeOfDay);
                localZenith.z = glm::mix(1.0, 0.5, timeOfDay);

                glm::vec4 localHorizon;
                localHorizon.x = glm::mix(0.643, 0.859, timeOfDay);
                localHorizon.y = glm::mix(0.875, 0.627, timeOfDay);
                localHorizon.z = glm::mix(1.0, 0.5, timeOfDay);

                //Then we can use these with the stack angle. 
                glm::vec4 skyColorA = glm::vec4(1.0);

                float stackInfluence = 1.0 - stackAngle / (PI / 2);

                skyColorA.x = glm::mix(localZenith.x, localHorizon.x, std::pow(stackInfluence, 8));
                skyColorA.y = glm::mix(localZenith.y, localHorizon.y, std::pow(stackInfluence, 8));
                skyColorA.z = glm::mix(localZenith.z, localHorizon.z, std::pow(stackInfluence, 8));

                float sectorInfluence = std::abs(sectorAngle - PI) / PI; //1 means it is in the direction of the sun (range 0,1)

                sectorInfluence = std::max<float>(1 - timeOfDay, sectorInfluence);

                //Mixes the current isotropic sky color with the local zenith color
                glm::vec4 skyColor = glm::vec4(1.0);
                skyColor.x = nightfactor * glm::mix(localZenith.x, skyColorA.x, sectorInfluence);
                skyColor.y = nightfactor * glm::mix(localZenith.y, skyColorA.y, sectorInfluence);
                skyColor.z = nightfactor * glm::mix(localZenith.z, skyColorA.z, sectorInfluence);

                
                

                

                //float red = glm::mix(0.286, 0.643, std::pow(1.0 -  glm::abs(stackAngle) / (PI/2), 2));
                //float green = glm::mix(0.75, 0.875, std::pow(1.0 - glm::abs(stackAngle) / (PI/2), 2));                
                
                //float red = glm::mix(0.286, 0.859, std::pow(1.0 - glm::abs(stackAngle) / (PI / 2), 2));
                //float green = glm::mix(0.376, 0.627, std::pow(1.0 - glm::abs(stackAngle) / (PI/2), 2));           


                //0.286f, 0.376f, 0.5f
                //0.859f, 0.627f, 0.5f

                //glm::vec4 skyColor = glm::vec4(red, green, 0.5, 1.0);

                /*glm::vec4 skyColor = glm::vec4(0.286, 0.75, 1.0, 1.0); 

                if (stackAngle < 0) 
                {
                    skyColor = glm::vec4(0.643, 0.875, 1.0, 1.0);
                }*/


                //if the stackangle is less than the horizon value render horizon color only. 
                /*if (stackAngle < 0)
                    skyColor = h;*/

                sphere_vertices.push_back(skyColor.x);
                sphere_vertices.push_back(skyColor.y);
                sphere_vertices.push_back(skyColor.z);
                sphere_vertices.push_back(skyColor.w);

                //std::cout << "Sector angle " << i << ": " << sectorAngle << std::endl;

            }

            //std::cout << "Stack Angle: " << stackAngle << std::endl; [1.5, -1.5]

        }
        /* GENERATE VERTEX ARRAY */


        /* GENERATE INDEX ARRAY */
        int k1, k2;
        for (int i = 0; i < stackCount; ++i)
        {
            k1 = i * (sectorCount + 1);     // beginning of current stack
            k2 = k1 + sectorCount + 1;      // beginning of next stack

            for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
            {
                // 2 triangles per sector excluding first and last stacks
                // k1 => k2 => k1+1
                if (i != 0)
                {
                    sphere_indices.push_back(k1);
                    sphere_indices.push_back(k2);
                    sphere_indices.push_back(k1 + 1);
                }

                // k1+1 => k2 => k2+1
                if (i != (stackCount - 1))
                {
                    sphere_indices.push_back(k1 + 1);
                    sphere_indices.push_back(k2);
                    sphere_indices.push_back(k2 + 1);
                }
            }
        }
        /* GENERATE INDEX ARRAY */


        /* GENERATE VAO-EBO */
        //GLuint VBO, VAO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, (unsigned int)sphere_vertices.size() * sizeof(float), sphere_vertices.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (unsigned int)sphere_indices.size() * sizeof(unsigned int), sphere_indices.data(), GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        /* GENERATE VAO-EBO */


    }
    void Draw()
    {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES,
            (unsigned int)sphere_indices.size(),
            GL_UNSIGNED_INT,
            (void*)0);
        glBindVertexArray(0);
    }
};

#endif