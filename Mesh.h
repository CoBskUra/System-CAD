//#ifndef MESH_CLASS_H
//#define MESH_CLASS_H
//
//#include<string>
//
//#include"VAO.h"
//#include"EBO.h"
//#include"Camera.h"
//#include <vector>
//
//class Mesh
//{
//public:
//	std::vector <Vertex> vertices;
//	std::vector <GLuint> indices;
//	const int DrawElement = GL_TRIANGLES;
//	// Store VAO in public so it can be used in the Draw function
//	VAO VAO;
//
//	// Initializes the mesh
//	Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices);
//
//	// Draws the mesh
//	void Draw(Shader& shader, Camera& camera);
//};
//#endif