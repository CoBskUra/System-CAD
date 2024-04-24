#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

#include <ImGui/imgui.h>

#include"Shader.h"
#include "MathOperations.h"
#include "Constants.h"

class Camera
{
public:
	Camera(int width, int height, glm::vec3 position);
	glm::mat4 GetCameraMatrix() const;
	glm::mat4 GetCameraMatrixInvers() const;

	bool Inputs(GLFWwindow* window);
	void ActiveInterferes();
	bool HasBeenUpdated();
	void SaveMatrixToShader(const GLuint& ShaderID) const;

	glm::vec3 GetPosition() const;
	glm::vec3 GetOrientation() const;
	glm::vec3 GetUp() const;
	float GetNearPlane() const;
	float GetFarPlane() const;
	void SetAspect(float newAspect);

private:
	bool hasBeenUpdated = true;
	glm::vec3 Position;
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 cameraMatrix = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	bool firstClick = true;
	glm::dvec2 lastMousePosition;

	float keyboardSpeed = 5.0f;
	float mouseSensitivity = 200.0f;

	float deltaTime = 0.0f;	
	float lastFrame = 0.0f; 

	float FOVRad = M_PI*0.5;
	float ctg_FOVRad_0dot5 = 1 / tanf(M_PI * 0.25);

	float nearPlane = 0.1f; 
	float farPlane = 100; 
	float farPlane_minus_nearPlane_Invers = 1 / (100 - 0.1f);

	float aspect = 1;
	float aspect_invers = 1;

	glm::vec3 ScaleVec = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::mat4 Scale = glm::mat4(1.0f);
	glm::mat4 Scale_invers = glm::mat4(1.0f);

	void updateMatrixes();
	void UpdateViewMatrix();
	void updateProjectionMatrix();

	glm::mat4 RotationMatrix() const;
	glm::mat4 TransformMatrix() const;
	glm::mat4 TransformMatrixInvers() const;
	glm::mat4 ProjectionMatrix() const;
	glm::mat4 ProjectionMatrixInvers() const;



	void SetScale(glm::vec3 vec);
	void SetScale(float x, float y, float z);

	bool handelKeyboardInput(GLFWwindow* window);
	bool handelMouseInput(GLFWwindow* window);


};
#endif