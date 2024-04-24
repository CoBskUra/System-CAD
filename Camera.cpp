#include"Camera.h"


Camera::Camera(int width, int height, glm::vec3 position)
{
	Position = position;
	SetAspect(width / (float)height);
	updateMatrixes();
}

void Camera::updateMatrixes()
{
	UpdateViewMatrix();
	updateProjectionMatrix();

	cameraMatrix = projection * view * Scale;
	hasBeenUpdated = true;
}


void Camera::UpdateViewMatrix() {
	glm::mat4 transform_invers = TransformMatrixInvers();
	glm::mat4 rotation_invers = glm::transpose(RotationMatrix());

	view = rotation_invers * transform_invers;
}

void Camera::updateProjectionMatrix() {

	projection = ProjectionMatrix();
}

glm::mat4 Camera::GetCameraMatrixInvers() const {
	glm::mat4 transform = TransformMatrix();
	glm::mat4 rotation = RotationMatrix();
	glm::mat4 projectionInvers = ProjectionMatrixInvers();


	return Scale_invers * transform * rotation * projectionInvers;
}

glm::mat4 Camera::ProjectionMatrix() const {
	glm::mat4 proj{ 0.0f };
	proj[0][0] = ctg_FOVRad_0dot5 * aspect_invers;
	proj[1][1] = ctg_FOVRad_0dot5;
	proj[2][2] = (farPlane + nearPlane) * farPlane_minus_nearPlane_Invers;

	proj[2][3] = 1;

	proj[3][2] = (-2 * farPlane * nearPlane) * farPlane_minus_nearPlane_Invers;
	return proj;
}

glm::mat4 Camera::ProjectionMatrixInvers() const {
	glm::mat4 projectionInvers{ 0.0f };

	projectionInvers[0][0] = aspect / ctg_FOVRad_0dot5;

	projectionInvers[1][1] = 1 / ctg_FOVRad_0dot5;

	projectionInvers[2][2] = 0;
	projectionInvers[3][2] = 1;

	projectionInvers[2][3] = (farPlane - nearPlane) / (-2 * farPlane * nearPlane);
	projectionInvers[3][3] = (farPlane + nearPlane) / (2 * farPlane * nearPlane);
	return projectionInvers;
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

	updateMatrixes();
}

glm::mat4 Camera::RotationMatrix() const
{
	//glm::vec3 target = Position + Orientation;
	glm::vec3 cameraDirection = glm::normalize(-Orientation);
	glm::vec3 cameraRight = glm::normalize(glm::cross(Up, cameraDirection));
	glm::vec3 cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));


	glm::mat4 rotation = glm::mat4(1.0f);
	rotation[0][0] = cameraRight.x;
	rotation[0][1] = cameraRight.y;
	rotation[0][2] = cameraRight.z;

	rotation[1][0] = cameraUp.x;
	rotation[1][1] = cameraUp.y;
	rotation[1][2] = cameraUp.z;

	rotation[2][0] = cameraDirection.x;
	rotation[2][1] = cameraDirection.y;
	rotation[2][2] = cameraDirection.z;

	return rotation;
}

glm::mat4 Camera::TransformMatrixInvers() const
{
	glm::mat4 transform_invers = glm::mat4(1.0f);
	transform_invers[3][0] = -Position.x;
	transform_invers[3][1] = -Position.y;
	transform_invers[3][2] = -Position.z;

	return transform_invers;
}

glm::mat4 Camera::TransformMatrix() const
{
	glm::mat4 transform = glm::mat4(1.0f);
	transform[3][0] = Position.x;
	transform[3][1] = Position.y;
	transform[3][2] = Position.z;

	return transform;
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
		glm::vec3 newOrientation = MathOperations::RotationAlongAxis(Orientation,
			glm::radians((float)-mouseDelta.y), glm::normalize(glm::cross(Orientation, Up)));

		// Decides whether or not the next vertical Orientation is legal or not
		if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			Orientation = newOrientation;
		}

		// Rotates the Orientation left and right
		Orientation = MathOperations::RotationAlongAxis(Orientation, glm::radians((float)-mouseDelta.x), Up);

		glfwGetCursorPos(window, &lastMousePosition.x, &lastMousePosition.y);
		return true;
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
		firstClick = true;
	}

	return false;
}


bool Camera::HasBeenUpdated()
{
	return hasBeenUpdated;
}

void Camera::SaveMatrixToShader(const GLuint& ShaderID) const
{
	//hasBeenUpdated = false;
	glUniformMatrix4fv(glGetUniformLocation(ShaderID, "CAM_MATRIX"),
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
				SetScale(ScaleVec);
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
				SetAspect(aspect);
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

glm::vec3 Camera::GetPosition() const
{
	return Position;
}

glm::vec3 Camera::GetOrientation() const
{
	return Orientation;
}

glm::vec3 Camera::GetUp() const
{
	return Up;
}

glm::mat4 Camera::GetCameraMatrix() const
{
	return cameraMatrix;
}

float Camera::GetNearPlane() const {
	return nearPlane;
}

float Camera::GetFarPlane() const {
	return farPlane;
}

void Camera::SetAspect(float newAspect)
{
	hasBeenUpdated = true;
	aspect = newAspect;
	aspect_invers = 1/newAspect;
	updateMatrixes();
}
