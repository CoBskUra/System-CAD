#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include "Libraries/include/ImGui/imgui.h"
#include "Libraries/include/ImGui/imgui_impl_glfw.h"
#include "Libraries/include/ImGui/imgui_impl_opengl3.h"

#include"Shader.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"Camera.h"
#include "Torus.h"
#include "Manager.h"
#include "Point.h"

unsigned int width = 1600;
unsigned int height = 1024;

Camera camera(width, height, glm::vec3(0.0f, 1.0f, -2.0f));
std::vector<float> infinityGrid {
	1, 1, 0,
	-1, -1, 0,
	-1, 1, 0,

	-1, -1, 0,
	1, 1, 0,
	1, -1, 0
};

GLFWwindow* Init();
void ResizeCallBack(GLFWwindow* window, int w, int h);

int main()
{
	GLFWwindow* window = Init();
	if (window == NULL)
		return -1;

	// infinity Grid
	VAO vaoInfinityGrid;
	vaoInfinityGrid.Bind();
	VBO vboInfinityGrid(infinityGrid, GL_STATIC_DRAW);

	vaoInfinityGrid.LinkAttrib(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);

	vaoInfinityGrid.Unbind(); vboInfinityGrid.Unbind();
	Shader infinityGridShader("infinityGrid_vert.glsl", "infinityGrid_frag.glsl");

	////////////////////////////////
	Manager manader(&camera, window);
	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		

		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		


		manader.MenuInterferes();
		manader.Draw();
		manader.ProcesInput();

		camera.Inputs(window);
		camera.ActiveInterferes();


		// inifinity Grid
		infinityGridShader.Activate();
		vaoInfinityGrid.Bind();
		glUniformMatrix4fv(glGetUniformLocation(infinityGridShader.ID, "camMatrixInvers"),
			1, GL_FALSE, glm::value_ptr(camera.GetCameraMatrixInvers()));
		camera.SaveMatrixToShader(infinityGridShader.ID);
		glUniform1f(glGetUniformLocation(infinityGridShader.ID, "near"), camera.GetNearPlane());
		glUniform1f(glGetUniformLocation(infinityGridShader.ID, "far"), camera.GetFarPlane());
		glDrawArrays(GL_TRIANGLES, 0, 6);
		vaoInfinityGrid.Unbind();


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	// Delete all the objects we've created

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	

	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}


GLFWwindow* Init() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "MKMG_lab_1", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, ResizeCallBack);

	gladLoadGL();
	glViewport(0, 0, width, height);


	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return window;
}

// PóŸniej tutaj dam resize, mam tylko problem, ¿e po risie Ÿle 
// kursor po klikniêciu ustawia mi siê w z³ym miejscu
// 
void ResizeCallBack(GLFWwindow* window, int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, width, height);
	camera.SetAspect(width / (float)height);
}
