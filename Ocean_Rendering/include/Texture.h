#pragma once

#ifndef TEXTURE_H
#define TEXTURE_H

class Texture 
{
public:

	unsigned int ID;
	unsigned char* data;

	Texture(const std::string& filePath) 
	{
		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_2D, ID);
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);		
		data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
		if (data)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);	
		else
			std::cout << "Failed to load texture" << std::endl;

		//Same texture parameters for every texture of this class. 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	Texture(int sizeX, int sizeY) //For use in framebuffers!
	{
		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_2D, ID);								
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sizeX, sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}




};

class Texturef {
public:
	unsigned int ID;
	float* data;

	Texturef(int sizeX, int sizeY) 
	{
		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_2D, ID);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sizeX, sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
};

class computeShaderInputTexture {

public:
	unsigned int ID;
	//unsigned char* data;
	float* data = new float[512 * 512 * 4];

	computeShaderInputTexture(GLuint unit, GLenum access, int sizeX, int sizeY) {
		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_2D, ID);
		//stbi_set_flip_vertically_on_load(true);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, sizeX, sizeY, 0, GL_RGBA, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindImageTexture(unit, ID, 0, GL_FALSE, 0, access, GL_RGBA32F);
	}

	//Under construction, do not use for now. 
	computeShaderInputTexture(const std::string& filePath, GLuint unit, GLenum access) {
		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_2D, ID);
		int width, height, nrChannels;
		//stbi_set_flip_vertically_on_load(true);

		
		//data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
		/*if (data)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, data);
		else
			std::cout << "Failed to load compute shader texture" << std::endl;*/

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindImageTexture(unit, ID, 0, GL_FALSE, 0, access, GL_RGBA32F);
	}

	
};


#endif // !TEXTURE_H
