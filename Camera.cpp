#include"Camera.h"



Camera::Camera(int width, int height, glm::vec3 position)
{
	Position = position;
	updateMatrixes();
}

void Camera::updateMatrixes()
{
	UpdateViewMatrix();
	updateProjectionMatrix();
	SetScale(ScaleVec);

	cameraMatrix = projection * view * Scale;
	hasBeenUpdated = true;
}


void Camera::UpdateViewMatrix() {
	glm::vec3 target = Position + Orientation;
	glm::vec3 cameraDirection = glm::normalize(Position - target);
	glm::vec3 cameraRight = glm::normalize(glm::cross(Up, cameraDirection));

	glm::vec3 cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));

	glm::mat4 transform_invers = glm::mat4(1.0f);
	transform_invers[3][0] = -Position.x;
	transform_invers[3][1] = -Position.y;
	transform_invers[3][2] = -Position.z;

	glm::mat4 rotation_invers = glm::mat4(1.0f);
	rotation_invers[0][0] = cameraRight.x;
	rotation_invers[1][0] = cameraRight.y;
	rotation_invers[2][0] = cameraRight.z;

	rotation_invers[0][1] = cameraUp.x;
	rotation_invers[1][1] = cameraUp.y;
	rotation_invers[2][1] = cameraUp.z;

	rotation_invers[0][2] = cameraDirection.x;
	rotation_invers[1][2] = cameraDirection.y;
	rotation_invers[2][2] = cameraDirection.z;

	view = rotation_invers * transform_invers;
}

void Camera::updateProjectionMatrix() {

	projection[0][0] = ctg_FOVRad_0dot5 * aspect_invers;
	projection[1][1] = ctg_FOVRad_0dot5;
	projection[2][2] = (farPlane + nearPlane) * farPlane_minus_nearPlane_Invers;

	projection[2][3] = 1;

	projection[3][2] = (-2 * farPlane * nearPlane) * farPlane_minus_nearPlane_Invers;
}


// return true if any value has been changed
bool Camera::handelKeyboardInput(GLFWwindow* window)
{
	ImGuiIO& io = ImGui::GetIO();

	if (io.WantCaptureKeyboard)
		return false;

	bool updatePosition = false;
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	// Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		Position += deltaTime * keyboardSpeed * -Orientation;
		updatePosition = true;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Position += deltaTime * keyboardSpeed * -glm::normalize(glm::cross(Orientation, Up));
		updatePosition = true;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Position += deltaTime * keyboardSpeed * +Orientation;
		updatePosition = true;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Position += deltaTime * keyboardSpeed * glm::normalize(glm::cross(Orientation, Up));
		updatePosition = true;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		Position += deltaTime * keyboardSpeed * Up;
		updatePosition = true;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		Position += deltaTime * keyboardSpeed * -Up;
		updatePosition = true;
	}

	return updatePosition;
}

void Camera::SetScale(glm::vec3 vec)
{
	SetScale(vec.x, vec.y, vec.z);
}

void Camera::SetScale(float x, float y, float z)
{

	ScaleVec.x = x;
	ScaleVec.y = y;
	ScaleVec.z = z;

	Scale[0][0] = x;
	Scale[1][1] = y;
	Scale[2][2] = z;

	Scale_invers[0][0] = 1 / x;
	Scale_invers[1][1] = 1 / y;
	Scale_invers[2][2] = 1 / z;
}


// return true if any value has been changed
bool Camera::handelMouseInput(GLFWwindow* window)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS )
	{
		if (firstClick)
		{
			glfwGetCursorPos(window, &lastMousePosition.x, &lastMousePosition.y);
			firstClick = false;
			return false;
		}

		glm::dvec2 mouseDelta;
		glfwGetCursorPos(window, &mouseDelta.x, &mouseDelta.y);
		mouseDelta = mouseDelta - lastMousePosition;
		int height, width;
		glfwGetWindowSize(window, &width, &height);
		mouseDelta.y /= height;
		mouseDelta.x /= width;
		mouseDelta *= mouseSensitivity;

		// Calculates upcoming vertical change in the Orientation
		glm::vec3 newOrientation = RotationAlongAxis(Orientation, glm::radians((float)-mouseDelta.y), glm::normalize(glm::cross(Orientation, Up)));

		// Decides whether or not the next vertical Orientation is legal or not
		if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			Orientation = newOrientation;
		}

		// Rotates the Orientation left and right
		Orientation = RotationAlongAxis(Orientation, glm::radians((float)-mouseDelta.x), Up);

		glfwGetCursorPos(window, &lastMousePosition.x, &lastMousePosition.y);
		return true;
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
		firstClick = true;
	}

	return false;
}

glm::vec3 Camera::RotationAlongAxis(glm::vec3 v, float rad, glm::vec3 axis)
{
	float s = sinf(rad);
	float c = cosf(rad);

	return (1 - c) * glm::dot(v, axis) * axis + c * v + s * glm::cross(axis, v);
}


bool Camera::HasBeenUpdated()
{
	return hasBeenUpdated;
}

void Camera::SaveMatrixToShader(const Shader& shader)
{
	hasBeenUpdated = false;
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "CAM_MATRIX"),
		1, GL_FALSE, glm::value_ptr(GetCameraMatrix()));
}

bool Camera::Inputs(GLFWwindow* window)
{
	bool getKeyboardInput	= handelKeyboardInput(window);
	bool getMouseInput		= handelMouseInput(window);
	if (getKeyboardInput || getMouseInput)
	{
		updateMatrixes();
	}

	return getMouseInput || getKeyboardInput;
}

void Camera::ActiveInterferes()
{
	ImGui::Begin("Camera Control");
	{
		ImGui::BeginGroup();
		{
			ImGui::Text("Orientation");
			if (ImGui::DragFloat("x", &Orientation.x, 0.1f) ||
				ImGui::DragFloat("y", &Orientation.y, 0.1f) ||
				ImGui::DragFloat("z", &Orientation.z, 0.1f)) 
			{
				if (abs(Orientation.x) + abs(Orientation.y) + abs(Orientation.z) < M_ESP)
					Orientation.x = Orientation.y = Orientation.z = 1;
				Orientation = glm::normalize(Orientation);
				updateMatrixes();
			}
		}
		ImGui::EndGroup();

		ImGui::BeginGroup();
		{
			ImGui::Text("Position");
			if (ImGui::DragFloat("x:", &Position.x, 0.1f) ||
				ImGui::DragFloat("y:", &Position.y, 0.1f) ||
				ImGui::DragFloat("z:", &Position.z, 0.1f))
			{
				updateMatrixes();
			}
		}
		ImGui::EndGroup();

		ImGui::BeginGroup();
		{
			ImGui::Text("Scale");
			if (ImGui::DragFloat("Scale x", &ScaleVec.x, 0.1f) ||
				ImGui::DragFloat("Scale y", &ScaleVec.y, 0.1f) ||
				ImGui::DragFloat("Scale z", &ScaleVec.z, 0.1f))
			{
				updateMatrixes();
			}
		}
		ImGui::EndGroup();

		ImGui::BeginGroup();
		{
			ImGui::Text("Properties");
			if (ImGui::DragFloat("Keyboard speed", &keyboardSpeed, 0.2f) ||
				ImGui::DragFloat("Mouse sensitivity", &mouseSensitivity, 10.0f) )
			{
				updateMatrixes();
			}
		}
		ImGui::EndGroup();

		ImGui::BeginGroup();
		{
			ImGui::Text("Perspective");
			if (
				ImGui::DragFloat("Near Plane", &nearPlane, 0.1f, M_ESP, farPlane) ||
				ImGui::DragFloat("Far Plane", &farPlane, 0.1f, nearPlane, M_FLOAT_MAX) 
				)
			{
				farPlane_minus_nearPlane_Invers = 1 / (farPlane - nearPlane);
				updateMatrixes();
			}
			if (ImGui::DragFloat("Aspect", &aspect, 0.1f, M_ESP, M_FLOAT_MAX))
			{
				aspect_invers = 1 / aspect;
				updateMatrixes();
			}
			if (ImGui::DragFloat("FOV Rad", &FOVRad, 0.1f, M_ESP, M_PI))
			{
				ctg_FOVRad_0dot5 = 1 / tanf(FOVRad * 0.5f);
				updateMatrixes();
			}
		}
		ImGui::EndGroup();
	}
	ImGui::End();
}

glm::vec3 Camera::GetPosition()
{
	return Position;
}

glm::vec3 Camera::GetOrientation()
{
	return Orientation;
}

glm::vec3 Camera::GetUp()
{
	return Up;
}

glm::mat4 Camera::GetCameraMatrix() const
{
	return cameraMatrix;
}
