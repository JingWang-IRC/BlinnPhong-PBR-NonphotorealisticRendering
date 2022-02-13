#define GLEW_STATIC
#include <iostream>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <math.h>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

#include "Shader.h"
#include "Camera.h"
#include "Material.h"
#include "LightDirectional.h"
//#include "LightPoint.h"
//#include "LightSpot.h"
#include "Mesh.h"
#include "Model.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#pragma region Model Data

// the vertices of the ground
float vertices_ground[] = {
	// positions          // normals           // texture coords
	-0.5f, -0.0f, -0.5f,   0.0f, 1.0f, 0.0f,   100.0f, 100.0f,   // top right
	 0.5f, -0.0f, -0.5f,   0.0f, 1.0f, 0.0f,   100.0f, 0.0f,   // bottom right
	 0.5f, -0.0f,  0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,   // bottom left
	-0.5f, -0.0f,  0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 100.0f    // top left 
};

// the indices of the vertices of the ground, passed to the EBO
unsigned int indices_ground[] = {
	0, 1, 2,
	2, 3, 0
};
#pragma endregion

#pragma region Light Declaration
// declare a light object
LightDirectional lightD(glm::vec3(10.0f, 30.0f, 20.0f), glm::vec3(glm::radians(110.0f), glm::radians(30.0f), 0), glm::vec3(5.0f, 5.0f, 5.0f));
#pragma endregion

#pragma region Camera Declaration
// Initialize camera class
Camera camera(glm::vec3(0, 3.0f, 20.0f), glm::radians(0.0f), glm::radians(-90.0f), glm::vec3(0, 1.0f, 0));
#pragma endregion

#pragma region Texture Loading Function
// here you should pass RGB when loading .jpg, while pass RGBA when loading .png
unsigned int LoadTextureImageToGPU(const char* filename, GLint internalFormat, GLenum format, int textureSlot)
{
	unsigned int TexBuffer;
	glGenTextures(1, &TexBuffer);

	glActiveTexture(GL_TEXTURE0 + textureSlot);
	glBindTexture(GL_TEXTURE_2D, TexBuffer);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannel;
	unsigned char *data = stbi_load(filename, &width, &height, &nrChannel, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		cout << "Texture image " << filename << " load failed." << endl;
	}
	stbi_image_free(data);

	//cout << filename << "Slot:  " << TexBuffer << endl;

	return TexBuffer;
}
#pragma endregion

#pragma region Camera Operation Funtions
// moving mouse and press some keys will change the position and the forward direction of the camera
double previous_xPos, previous_yPos;
bool first_initialise_mouse = true;

// press keys as input
void processInput_camera(GLFWwindow* window)
{

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.SpeedZ = 1.0;
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.SpeedZ = -1.0;
	}
	else
	{
		camera.SpeedZ = 0;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.SpeedX = 1.0;
	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.SpeedX = -1.0;
	}
	else
	{
		camera.SpeedX = 0;
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		camera.SpeedY = 1.0;
	}
	else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		camera.SpeedY = -1.0;
	}
	else
	{
		camera.SpeedY = 0;
	}
}

// move mouse to change the orientation of the camera
void mouse_callback_camera(GLFWwindow* window, double xPos, double yPos)
{
	if (first_initialise_mouse == true)
	{
		previous_xPos = xPos;
		previous_yPos = yPos;
		first_initialise_mouse = false;
	}

	double Delta_x = xPos - previous_xPos;
	double Delta_y = yPos - previous_yPos;

	previous_xPos = xPos;
	previous_yPos = yPos;

	//cout << Delta_x << "   " << Delta_y << endl;  // this line is for debug
	camera.ProcessMouseMovement(Delta_x, Delta_y);
}
#pragma endregion

int main(int argc, char* argv[])
{
	std::string exePath = argv[0];

#pragma region Open a Window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1400, 1000, "My openGL game", NULL, NULL);

	if (window == NULL)
	{
		cout << "Open window failed." << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		cout << "Init GlEW failed." << endl;
		glfwTerminate();
		return -1;
	}

	glfwSetCursorPosCallback(window, mouse_callback_camera);
	//glfwSetCursorPosCallback(window, mouse_callback_camera_ThirdPersonView);
	//glfwSetScrollCallback(window, scroll_callback_camera_ThirdPersonView);

	//glViewport(0, 0, 800, 600);
	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#pragma endregion

#pragma region Init Shader Program
	Shader* shader_ground = new Shader("VertexShader_ground.vert", "FragmentShader_ground.frag");
	Shader* shader_blinnPhong = new Shader("VertexShader_blinnPhong.vert", "FragmentShader_blinnPhong.frag");
	Shader* shader_cookTorrance = new Shader("VertexShader_cookTorrance.vert", "FragmentShader_cookTorrance.frag");
	Shader* shader_nonPhysical = new Shader("VertexShader_nonPhysical.vert", "FragmentShader_nonPhysical.frag");
	Shader* shader_silhouette = new Shader("VertexShader_silhouette.vert", "FragmentShader_silhouette.frag");
#pragma endregion 

#pragma region Pass light to shaders;
	shader_blinnPhong->use();
	shader_blinnPhong->SetUniform3f("lightD.pos", glm::vec3(lightD.position.x, lightD.position.y, lightD.position.z));
	shader_blinnPhong->SetUniform3f("lightD.color", glm::vec3(lightD.color.x, lightD.color.y, lightD.color.z));
	shader_blinnPhong->SetUniform3f("lightD.dirToLight", glm::vec3(lightD.direction.x, lightD.direction.y, lightD.direction.z));

	shader_cookTorrance->use();
	shader_cookTorrance->SetUniform3f("lightD.pos", glm::vec3(lightD.position.x, lightD.position.y, lightD.position.z));
	shader_cookTorrance->SetUniform3f("lightD.color", glm::vec3(lightD.color.x, lightD.color.y, lightD.color.z));
	shader_cookTorrance->SetUniform3f("lightD.dirToLight", glm::vec3(lightD.direction.x, lightD.direction.y, lightD.direction.z));

	shader_nonPhysical->use();
	shader_nonPhysical->SetUniform3f("lightD.pos", glm::vec3(lightD.position.x, lightD.position.y, lightD.position.z));
	shader_nonPhysical->SetUniform3f("lightD.color", glm::vec3(lightD.color.x, lightD.color.y, lightD.color.z));
	shader_nonPhysical->SetUniform3f("lightD.dirToLight", glm::vec3(lightD.direction.x, lightD.direction.y, lightD.direction.z));
#pragma endregion

#pragma region Load Models

	// load a .obj model 
	Model bunny(exePath.substr(0, exePath.find_last_of('\\')) + "\\bunny\\bunny.obj");

	// manually load the data of the ground
	unsigned int VBO_ground;
	glGenBuffers(1, &VBO_ground);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_ground);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_ground), vertices_ground, GL_STATIC_DRAW);

	unsigned int VAO_ground;
	glGenVertexArrays(1, &VAO_ground);
	glBindVertexArray(VAO_ground);

	unsigned int EBO_ground;
	glGenBuffers(1, &EBO_ground);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_ground);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_ground), indices_ground, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
#pragma endregion

#pragma region Init and Load Texture;
	stbi_set_flip_vertically_on_load(true);
	unsigned int TexBuffer_ground;
	TexBuffer_ground = LoadTextureImageToGPU("ground.jpg", GL_RGB, GL_RGB, 0);
	//unsigned int TexBufferB;
	//TexBufferB = LoadTextureImageToGPU("awesomeface.png", GL_RGBA, GL_RGBA, 1);
#pragma endregion

#pragma region Prepare MVP matrices
	//glm::mat4 scale;  glm::scale(transMatrix, glm::vec3(100.0f, 100.0f, 100.0f));
	glm::mat4 trans;
	glm::mat4 modelMat;
	//modelMat = glm::rotate(modelMat, glm::radians(0.0f), glm::vec3(1.0f, 0, 0));
	glm::mat4 viewMat;
	glm::mat4 projMat;
	projMat = glm::perspective(glm::radians(45.0f), 1400.0f / 1000.0f, 0.1f, 300.0f);
#pragma endregion

	glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);

	// Start drawing
	while (!glfwWindowShouldClose(window))
	{
		// get current time
		float currentTime = glfwGetTime();

		// Mouse Action
		processInput_camera(window);

		// Clear Screen
		glClearColor(0.2f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, 1400, 1000);
		viewMat = camera.GetViewMatrix();

#pragma region Draw ground
		// in this demonstration, the ground is not used
		// so commented out

		// Ground
		/*shader_blinnPhong->use();
		trans = glm::scale(glm::mat4(1.0f), glm::vec3(1000.0f, 1000.0f, 1000.0f));
		modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		shader_blinnPhong->SetMatrix("transform", trans);
		shader_blinnPhong->SetMatrix("modelMat", modelMat);
		shader_blinnPhong->SetMatrix("viewMat", viewMat);
		shader_blinnPhong->SetMatrix("projMat", projMat);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TexBuffer_ground);
		shader_blinnPhong->SetUniform1i("groundTexture_diffuse", 0);

		glBindVertexArray(VAO_ground);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_ground);
		glDrawElements(GL_TRIANGLES, sizeof(indices_ground), GL_UNSIGNED_INT, 0);*/
#pragma endregion

#pragma region Draw Bunny BlinnPhong
		
		// in the shader of blinn-phong, two parameters were manuplated
		// one is the effect of the ambinent colour (change with i)
		// another one is the shininess (change with j)
		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				shader_blinnPhong->use();
				trans = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
				trans = glm::rotate(trans, currentTime, glm::vec3(0.0f, 1.0f, 0.0f));
				modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(-40.0f + i * 4.0f, 8.0f - j * 4.0f, -15.0f));
				shader_blinnPhong->SetMatrix("transform", trans);
				shader_blinnPhong->SetMatrix("modelMat", modelMat);
				shader_blinnPhong->SetMatrix("viewMat", viewMat);
				shader_blinnPhong->SetMatrix("projMat", projMat);
				shader_blinnPhong->SetUniform3f("viewPos", camera.Position);
				shader_blinnPhong->SetUniform1f("ambinent_weight", 0.1 * i);
				shader_blinnPhong->SetUniform1f("shininess", std::pow(2, j + 2));

				bunny.Draw(shader_blinnPhong);
			}
		}
#pragma endregion

#pragma region Draw Bunny CookTorrance

		// in the shader of PBR with cook-torrance BRDF, two parameters were manuplated
		// one is the property of metallic (change with i)
		// another one is the property of roughness (change with j)

		shader_cookTorrance->use();

		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				trans = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
				trans = glm::rotate(trans, currentTime, glm::vec3(0.0f, 1.0f, 0.0f));
				modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(-8.0f + i * 4.0f, 8.0f - j * 4.0f, -15.0f));
				shader_cookTorrance->SetMatrix("transform", trans);
				shader_cookTorrance->SetMatrix("modelMat", modelMat);
				shader_cookTorrance->SetMatrix("viewMat", viewMat);
				shader_cookTorrance->SetMatrix("projMat", projMat);
				shader_cookTorrance->SetUniform3f("viewPos", camera.Position);
				shader_cookTorrance->SetUniform3f("albedo", glm::vec3(0.5f, 0.5f, 0.5f));
				shader_cookTorrance->SetUniform1f("metallic", 0.01f + i * 0.2f);
				shader_cookTorrance->SetUniform1f("roughness", 0.1f + j * 0.2f);

				bunny.Draw(shader_cookTorrance);
			}
		}
#pragma endregion

#pragma region Draw Bunny NonPhysical

		// in the shader of non-photorealistic rendering, two parameters were manuplated
		// one is the value of the upper threshold of diffuse reflection (change with i)
		// another one is the threshold of specular reflection (change with j)

		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				shader_nonPhysical->use();
				trans = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
				trans = glm::rotate(trans, currentTime, glm::vec3(0.0f, 1.0f, 0.0f));
				modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(24.0f + i * 4.0f, 8.0f - j * 4.0f, -15.0f));
				shader_nonPhysical->SetMatrix("transform", trans);
				shader_nonPhysical->SetMatrix("modelMat", modelMat);
				shader_nonPhysical->SetMatrix("viewMat", viewMat);
				shader_nonPhysical->SetMatrix("projMat", projMat);
				shader_nonPhysical->SetUniform3f("viewPos", camera.Position);
				shader_nonPhysical->SetUniform1f("diffuse_upper_bound", 0.25f + 0.05f * i);
				shader_nonPhysical->SetUniform1f("diffuse_lower_bound", 0.20f);
				shader_nonPhysical->SetUniform1f("specular_bound", 0.5f + 0.1f * j);

				bunny.Draw(shader_nonPhysical);

				glCullFace(GL_FRONT);
				shader_silhouette->use();
				trans = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
				trans = glm::rotate(trans, currentTime, glm::vec3(0.0f, 1.0f, 0.0f));
				modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(24.0f + i * 4.0f, 8.0f - j * 4.0f, -15.0f));
				shader_silhouette->SetMatrix("transform", trans);
				shader_silhouette->SetMatrix("modelMat", modelMat);
				shader_silhouette->SetMatrix("viewMat", viewMat);
				shader_silhouette->SetMatrix("projMat", projMat);
				shader_silhouette->SetUniform3f("viewPos", camera.Position);


				bunny.Draw(shader_silhouette);
				glCullFace(GL_BACK);
			}
		}

#pragma endregion

		// Clean up, prepare for next render loop
		glfwSwapBuffers(window);
		glfwPollEvents();
		camera.UpdateCameraPos();
	}

}
