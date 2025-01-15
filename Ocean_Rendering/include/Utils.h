#pragma once

#ifndef UTILS_H
#define UTILS_H


const float numTextures = 12.f;
std::unordered_map<int, float> fetchTexCoords = { {1, 0.0}, {2, 1 / numTextures}, {3, 2 / numTextures} ,
    {4, 3 / numTextures}, {5, 4 / numTextures}, {6, 5 / numTextures}, {7, 6 / numTextures}, {8, 7 / numTextures},
    {9, 8 / numTextures}, {10, 9 / numTextures}, {11, 10 / numTextures}, {12, 11 / numTextures} };


std::vector<glm::vec2> fetchTextureCoordSet(int type) {
    glm::vec2 t1 = glm::vec2(fetchTexCoords.find(type)->second, 1.0f);
    glm::vec2 t2 = glm::vec2(t1.x + 1 / numTextures, 1.0f);
    glm::vec2 t3 = glm::vec2(t1.x, 0.0f);
    glm::vec2 t4 = glm::vec2(t1.x + 1 / numTextures, 0.0f);
    return { t1,t2,t3,t4 };
}

void bind7Buffer(unsigned int& VAO, unsigned int& VBO, std::vector<float>& vertices)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void bind5Buffer(unsigned int& VAO, unsigned int& VBO, std::vector<float>& vertices)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// Define a hash function for glm::vec2
struct Vec2Hash {
    std::size_t operator()(const glm::vec2& v) const {
        // Combine the hash values of the x and y components
        std::size_t h1 = std::hash<float>{}(v.x);
        std::size_t h2 = std::hash<float>{}(v.y);
        return h1 ^ (h2 << 1); // Combine hashes
    }
};

// Define an equality function for glm::vec2
struct Vec2Equal {
    bool operator()(const glm::vec2& v1, const glm::vec2& v2) const {
        return v1.x == v2.x && v1.y == v2.y;
    }
};

void setPixelVec3(float* data, int x, int y, const glm::vec3& color) //Deprecation warning
{
    const static int N = 512;
    const static int nrChannels = 4;
    if (x >= 0 && x < N && y >= 0 && y < N)
    {
        data[(N * (N - 1 - y) + x) * nrChannels] = color.x;
        data[(N * (N - 1 - y) + x) * nrChannels + 1] = color.y;
        data[(N * (N - 1 - y) + x) * nrChannels + 2] = color.z;
        data[(N * (N - 1 - y) + x) * nrChannels + 3] = 1.0;
    }
}

float readPixelValue(float* data, int x, int y, int channel) //R=0, G=1, B=2
{
    const static int N = 512; 
    const static int nrChannels = 4;
    if (x >= 0 && x < N && y >= 0 && y < N)
    {
        return data[(N * (N - 1 - y) + x) * nrChannels + channel];
    }
    else
    {
        return 0.0f;
    }
}

void setPixelI(unsigned char* data, int x, int y, const glm::vec3& color)
{
    const static int N = 512;
    const static int nrChannels = 4;
    if (x >= 0 && x < N && y >= 0 && y < N)
    {
        data[(N * (N - 1 - y) + x) * nrChannels] = static_cast<int>(color.x * 255);
        data[(N * (N - 1 - y) + x) * nrChannels + 1] = static_cast<int>(color.y * 255);
        data[(N * (N - 1 - y) + x) * nrChannels + 2] = static_cast<int>(color.z * 255);
    }
}
int readPixel(unsigned char* data, int x, int y)
{
    const static int N = 512;
    const static int nrChannels = 4;
    if (x >= 0 && x < N && y >= 0 && y < N)
    {
        return static_cast<int>(data[(N * (N - 1 - y) + x) * nrChannels]);
    }
    else
    {
        return -1;
    }
}

void fft(std::vector<std::complex<double>>& a, bool invert)
{
    int n = a.size();
    if (n == 1) return;

    std::vector<std::complex<double>> a0(n / 2), a1(n / 2);

    for (int i = 0; 2 * i < n; i++)
    {
        a0[i] = a[2 * i];
        a1[i] = a[2 * i + 1];
    }

    fft(a0, invert);
    fft(a1, invert);

    double ang = 2 * PI / n * (invert ? -1 : 1);
    std::complex<double> w(1), wn(std::cos(ang), std::sin(ang));

    for (int i = 0; 2 * i < n; i++)
    {
        a[i] = a0[i] + w * a1[i];
        a[i + n / 2] = a0[i] - w * a1[i];
        if (invert)
        {
            a[i] /= 2;
            a[i + n / 2] /= 2;
        }
        w *= wn;
    }
}


void buffer7(unsigned int& VAO, unsigned int& VBO, std::vector<float>& vertices)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void buffer6(unsigned int& VAO, unsigned int& VBO, std::vector<float>& vertices)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}



//TEMPS


#endif