#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

#include <imgui.h>

#include"ShaderManadement/Shader.h"
#include "Helper/MathOperations.h"
#include "Constants.h"
//#include "StereoscoicView.h"

class Camera
{
public:
	Camera(int width, int height, glm::vec3 position);
	Camera(const Camera& camera);
	glm::mat4 GetCameraMatrix() const;
	glm::mat4 GetCameraMatrixInvers() const;

	bool Inputs(GLFWwindow* window);
	virtual void ActiveInterferes();
	bool HasBeenUpdated();
	void SaveMatrixToShader(const GLuint& ShaderID) const;

	glm::vec3 GetPosition() const;
	virtual glm::vec3 SetPosition(glm::vec3 newPos);
	glm::vec3 GetOrientation() const;
	void SetOrientation(glm::vec3 newOrientation);
	glm::vec3 GetUp() const;

	float GetNearPlane() const;
	float GetFarPlane() const;

	void SetNearPlane(float newNearPlane);
	void SetFarPlane(float newFarPlane);

	glm::vec3 GetScale() const;
	void SetScale(glm::vec3 vec);
	void SetScale(float x, float y, float z);

	glm::mat4 RotationMatrix() const;
	
	glm::mat4 TransformMatrix() const;
	glm::mat4 TransformMatrixInvers() const;

	virtual glm::mat4 ProjectionMatrix() const;
	virtual glm::mat4 ProjectionMatrixInvers() const;

	float GetAspect() const;
	void SetAspect(float newAspect);
	float GetFov() const;
	void SetFov(float newFov);
	void operator=(const Camera camera);

	virtual void updateMatrixes();
	void UpdateViewMatrix();
	void updateProjectionMatrix();


protected:
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
	float ctg_FOVRad_0dot5 = 1.0f / tanf(M_PI * 0.25);

	float nearPlane = 0.1f; 
	float farPlane = 100; 
	float farPlane_minus_nearPlane_Invers = 1 / (100 - 0.1f);

	float aspect = 1;
	float aspect_invers = 1;

	glm::vec3 ScaleVec = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::mat4 Scale = glm::mat4(1.0f);
	glm::mat4 Scale_invers = glm::mat4(1.0f);



	bool handelKeyboardInput(GLFWwindow* window);
	bool handelMouseInput(GLFWwindow* window);

	void OrientationImgui();
	void PositionImgui();
	void ScaleImgui();
	void MoveSettingsImgui();
};
#endif