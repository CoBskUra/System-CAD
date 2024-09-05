#pragma once
#include "Figure.h"
#include "Figures/Bezier/Surface/BezierSurfaceC0.h"
struct VertexInfo {
	uint32_t v;
	int k1;
	int k2;
};

// from->between_v1->between_v2->to
struct Edge {
	uint32_t surfaceId;
	int patchV;
	int pathH;
	VertexInfo from;
	VertexInfo between_v1, between_v2;
	VertexInfo to;

	void ChangeDirection() {
		std::swap(from, to);
		std::swap(between_v1, between_v2);
	}

	std::vector<Figure*> FigureFromScene(Scene* scene) {
		return std::vector<Figure*>{
				scene->byID(from.v).get(),
				scene->byID(between_v1.v).get(),
				scene->byID(between_v2.v).get(),
				scene->byID(to.v).get(),
		};
	}
};

struct Graph {
	std::vector<uint32_t> vertex{};
	std::map<uint32_t, std::vector<Edge>> graph{};

	void Insert(uint32_t id, Edge edge) {
		if (id == edge.to.v)
			return;
		if (!graph.contains(id)) {
			graph[id] = std::vector<Edge>{};
			vertex.push_back(id);
		}
		graph[id].push_back(edge);
	}

	const std::vector<Edge>& operator[](uint32_t id) {
		return graph[id];
	}

	const std::vector<uint32_t>& Vertexes() const {
		return vertex;
	}
	
};

struct TriangleCycle {
private:
	Edge e1, e2, e3;
public:
	Edge GetEdge_1() { return e1; }
	Edge GetEdge_2() { return e2; }
	Edge GetEdge_3() { return e3; }

	TriangleCycle(Edge a, Edge b, Edge c) {
		e1 = a; e2 = b; e3 = c;
		if (e1.from.v > e2.from.v)
			std::swap(e1, e2);
		if(e1.from.v > e3.from.v)
			std::swap(e1, e3);
		if (e1.to.v != e2.from.v)
			std::swap(e2, e3);

		if (e3.from.v < e1.to.v)
		{
			std::swap(e1, e3);
			e1.ChangeDirection();
			e2.ChangeDirection();
			e3.ChangeDirection();
		}
	}
	
	friend bool operator==(const TriangleCycle& c1, const TriangleCycle& c2) {

		return c1.e1.from.v == c2.e1.from.v &&
			c1.e2.from.v == c2.e2.from.v &&
			c1.e3.from.v == c2.e3.from.v;
	}

	friend bool operator!=(const TriangleCycle& c1, const TriangleCycle& c2) {

		return c1 != c2;
	}

	friend bool operator>(const TriangleCycle & c1, const TriangleCycle & c2) {

		if (c1.e1.from.v != c2.e1.from.v)
			return c1.e1.from.v > c2.e1.from.v;
		if (c1.e2.from.v != c2.e2.from.v)
			return c1.e2.from.v > c2.e2.from.v;
		if (c1.e3.from.v != c2.e3.from.v)
			return c1.e3.from.v > c2.e3.from.v;
		return false;
	}

	friend bool operator<(const TriangleCycle& c1, const TriangleCycle& c2) {

		if (c1 == c2)
			return false;

		return !(c1 > c2);
	}
};

class GregoryPatch : public Figure, public FigureContainer {
public:
	std::map < uint32_t, std::shared_ptr<BezierSurfaceC0>> surfaces;
	std::vector<TriangleCycle> triangleCycle{};
	Scene* scene;

	void FigureSpecificImGui() override;
	GregoryPatch(Scene* scene, const char* name);
	GregoryPatch(Scene* scene);
	Graph CreateGraph();
	bool IsOutsideEdge(const Edge& edge);
	std::vector<TriangleCycle> FindTriangleCycles(Graph graph);
	
	void CreateGregoryPatchVAO();
	void Draw(GLFWwindow* window, const Camera& camera) override;

	bool IsValid(Figure* figure) override;
	void Update() override;
	void UnmarkFigure(Figure* figure) override;

	
protected:

	int patch_div = 4;
	VAO vao;
	const Shader& shader = StaticShaders::GetGregoryPatch();

	VAO vaoArrows;
	const Shader& shaderArrows = StaticShaders::GetBezierCurve();
	bool canInsertFigureToFigureContainer;

	GregoryPatch(const char* name, FigureType type);
};

