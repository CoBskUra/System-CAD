#include"Camera.h"


Camera::Camera(int width, int height, glm::vec3 position)
{
	Position = position;
	SetAspect(width / (float)height);
	updateMatrixes();
}

Camera::Camera(const Camera& camera)
{
	hasBeenUpdated = false;
	Position = camera.Position;
	Orientation = camera.Orientation;
	Up = camera.Up;

	cameraMatrix = camera.cameraMatrix;
	view = camera.view;
	projection = camera.projection;

	firstClick = camera.firstClick;
	lastMousePosition = camera.lastMousePosition;

	keyboardSpeed = camera.keyboardSpeed;
	mouseSensitivity = camera.mouseSensitivity;

	deltaTime = camera.deltaTime;
	lastFrame = camera.lastFrame;

	FOVRad = camera.FOVRad;
	ctg_FOVRad_0dot5 = camera.ctg_FOVRad_0dot5;

	nearPlane = camera.nearPlane;
	farPlane = camera.farPlane;
	farPlane_minus_nearPlane_Invers = camera.farPlane_minus_nearPlane_Invers;

	aspect = camera.aspect;
	aspect_invers = camera.aspect_invers;

	ScaleVec = camera.ScaleVec;
	Scale = camera.Scale;
	Scale_invers = camera.Scale_invers;
}

void Camera::updateMatrixes()
{
	UpdateViewMatrix();
	updateProjectionMatrix();

	//cameraMatrix = projection * view * Scale;
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
	glm::vec3 newPos = Position;
	// Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		newPos += deltaTime * keyboardSpeed * -Orientation;
		updatePosition = true;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		newPos += deltaTime * keyboardSpeed * -glm::normalize(glm::cross(Orientation, Up));
		updatePosition = true;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		newPos += deltaTime * keyboardSpeed * +Orientation;
		updatePosition = true;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		newPos += deltaTime * keyboardSpeed * glm::normalize(glm::cross(Orientation, Up));
		updatePosition = true;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		newPos += deltaTime * keyboardSpeed * Up;
		updatePosition = true;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		newPos += deltaTime * keyboardSpeed * -Up;
		updatePosition = true;
	}

	if (updatePosition)
		SetPosition(newPos);

	return updatePosition;
}

glm::vec3 Camera::GetScale() const
{
	return ScaleVec;
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

void Camera::OrientationImgui()
{
	ImGui::BeginGroup();
	{
		ImGui::Text("Orientation");
		if (ImGui::DragFloat("x", &Orientation.x, 0.1f) ||
			ImGui::DragFloat("y", &Orientation.y, 0.1f) ||
			ImGui::DragFloat("z", &Orientation.z, 0.1f))
		{
			SetOrientation(Orientation);
			updateMatrixes();
		}
	}
	ImGui::EndGroup();
}

void Camera::PositionImgui()
{
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
}

void Camera::ScaleImgui()
{
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
}

void Camera::MoveSettingsImgui()
{
	ImGui::BeginGroup();
	{
		ImGui::Text("Properties");
		if (ImGui::DragFloat("Keyboard speed", &keyboardSpeed, 0.2f) ||
			ImGui::DragFloat("Mouse sensitivity", &mouseSensitivity, 10.0f))
		{
			updateMatrixes();
		}
	}
	ImGui::EndGroup();
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
	ImGui::PushID("Camera");
	ImGui::BeginGroup();
	{
		

		OrientationImgui();
		PositionImgui();
		ScaleImgui();
		MoveSettingsImgui();		

		ImGui::BeginGroup();
		{
			ImGui::Text("Perspective");
			if (
				ImGui::DragFloat("Near Plane", &nearPlane, 0.1f, M_ESP, farPlane) ||
				ImGui::DragFloat("Far Plane", &farPlane, 0.1f, nearPlane, M_FLOAT_MAX) 
				)
			{
				SetNearPlane(nearPlane);
				SetFarPlane(farPlane);
				updateMatrixes();
			}
			if (ImGui::DragFloat("Aspect", &aspect, 0.1f, M_ESP, M_FLOAT_MAX))
			{
				SetAspect(aspect);
			}
			if (ImGui::DragFloat("FOV Rad", &FOVRad, 0.1f, M_ESP, M_PI))
			{
				SetFov(FOVRad);
				updateMatrixes();
			}
		}
		ImGui::EndGroup();
	}
	ImGui::EndGroup();
	ImGui::PopID();
}

glm::vec3 Camera::GetPosition() const
{
	return Position;
}

glm::vec3 Camera::SetPosition(glm::vec3 newPos)
{
	glm::vec3 oldPos = Position;
	Position = newPos;

	return glm::vec3();
}

glm::vec3 Camera::GetOrientation() const
{
	return Orientation;
}

void Camera::SetOrientation(glm::vec3 newOrientation)
{
	if (abs(newOrientation.x) + abs(newOrientation.y) + abs(newOrientation.z) < M_ESP)
		newOrientation.x = newOrientation.y = newOrientation.z = 1;
	Orientation = glm::normalize(newOrientation);
}

glm::vec3 Camera::GetUp() const
{
	return Up;
}

glm::mat4 Camera::GetCameraMatrix() const
{
	return projection * view * Scale;
}

float Camera::GetNearPlane() const {
	return nearPlane;
}

float Camera::GetFarPlane() const {
	return farPlane;
}

void Camera::SetNearPlane(float newNearPlane)
{
	nearPlane = newNearPlane;
	farPlane_minus_nearPlane_Invers = 1 / (farPlane - nearPlane);
}

void Camera::SetFarPlane(float newFarPlane)
{
	farPlane = newFarPlane;
	farPlane_minus_nearPlane_Invers = 1 / (farPlane - nearPlane);
}

float Camera::GetAspect() const
{
	return aspect;
}

void Camera::SetAspect(float newAspect)
{
	hasBeenUpdated = true;
	aspect = newAspect;
	aspect_invers = 1/newAspect;
	updateMatrixes();
}

float Camera::GetFov() const
{
	return FOVRad;
}

void Camera::SetFov(float newFov)
{
	FOVRad = newFov;
	ctg_FOVRad_0dot5 = 1 / tanf(FOVRad * 0.5f);
}


void Camera::operator=(const Camera camera)
{
	hasBeenUpdated = false;
	Position = camera.Position;
	Orientation = camera.Orientation;
	Up = camera.Up;

	cameraMatrix = camera.cameraMatrix;
	view = camera.view;
	projection = camera.projection;

	firstClick = camera.firstClick;
	lastMousePosition = camera.lastMousePosition;

	keyboardSpeed = camera.keyboardSpeed;
	mouseSensitivity = camera.mouseSensitivity;

	deltaTime = camera.deltaTime;
	lastFrame = camera.lastFrame;

	FOVRad = camera.FOVRad;
	ctg_FOVRad_0dot5 = camera.ctg_FOVRad_0dot5;

	nearPlane = camera.nearPlane;
	farPlane = camera.farPlane;
	farPlane_minus_nearPlane_Invers = camera.farPlane_minus_nearPlane_Invers;

	aspect = camera.aspect;
	aspect_invers = camera.aspect_invers;

	ScaleVec = camera.ScaleVec;
	Scale = camera.Scale;
	Scale_invers = camera.Scale_invers;
}
