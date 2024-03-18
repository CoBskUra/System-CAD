//#include "Mesh.h"
//
//Mesh::Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices)
//{
//	Mesh::vertices = vertices;
//	Mesh::indices = indices;
//
//	VAO.Bind();
//	// Generates Vertex Buffer Object and links it to vertices
//	VBO VBO(vertices);
//	// Generates Element Buffer Object and links it to indices
//	EBO EBO(indices);
//	// Links VBO attributes such as coordinates
//	VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
//
//	// Unbind all to prevent accidentally modifying them
//	VAO.Unbind();
//	VBO.Unbind();
//	EBO.Unbind();
//}
//
//
//void Mesh::Draw(Shader& shader, Camera& camera)
//{
//	// Bind shader to be able to access uniforms
//	shader.Activate();
//	VAO.Bind();
//
//	// Keep track of how many of each type of textures we have
//	unsigned int numDiffuse = 0;
//	unsigned int numSpecular = 0;
//
//	// Take care of the camera Matrix
//	glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
//	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(camera.GetCameraMatrix()));
//	glUniform4f(glGetUniformLocation(shader.ID, "color"), 1, 1, 1, 1);
//
//	// Draw the actual mesh
//	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
//	//glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
//}