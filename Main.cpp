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

Camera camera(width, height, glm::vec3(0.0f, 0.0f, -2.0f));

GLFWwindow* Init();
void ResizeCallBack(GLFWwindow* window, int w, int h);

int main()
{
	GLFWwindow* window = Init();
	if (window == NULL)
		return -1;

	auto shader = Shader("simple_vert.glsl", "simple_frag.glsl");
	bool showTorus = true;

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
