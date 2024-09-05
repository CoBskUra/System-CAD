#include "GregoryPatch.h"


void GregoryPatch::FigureSpecificImGui()
{
	int counter = 0;
	ImGui::DragInt("Patch div. :##GregoryPatch", &patch_div);
	for (int i = 0; i < scene->Size(); i++) {
		if (scene->at(i)->GetType() != FigureType::BezierSurfaceC0) {
			continue;
		}
		std::shared_ptr<BezierSurfaceC0> surface =
			std::static_pointer_cast<BezierSurfaceC0>( scene->at(i));
		

		char buf[200];
		sprintf_s(buf, "%d. %s", counter, surface->name);
		if (ImGui::Selectable(buf, surfaces.contains(surface->GetId()))) {
			if (surfaces.contains(surface->GetId())) {
				surfaces.erase(surface->GetId());

				Update();
			}
			else if (surfaces.size() < 3) {
				surfaces[surface->GetId()] = surface;

				Update();
			}
		}

		counter++;
	}
}

GregoryPatch::GregoryPatch(Scene* scene, const char* name):GregoryPatch(name, FigureType::GregoryPatch)
{
	this->scene = scene;
}

GregoryPatch::GregoryPatch(Scene* scene):GregoryPatch(scene, "GregoryPatch")
{
}

Graph GregoryPatch::CreateGraph()
{
	Graph graph;
	for (auto pair : surfaces) {
		auto surface0 = pair.second;
		auto surfSize = surface0->SurfaceSize();
		int next_i = surfSize.x - 1;
		if (next_i == 0)
			next_i = 1;
		for (int i = 0; i < surfSize.x; i++) {
			for (int j = 0; j < surfSize.y; j++) {
				for (int k1 = 0; k1 < 4; k1 += 3)
				{
					for (int k2 = 0; k2 < 4; k2 += 3)
					{
						// po prostu chodzenie po p³atku i ³¹czenie 
						int v1 = surface0->TakePoint(i, j, k1, k2)->GetId();
						VertexInfo from{ v1, k1, k2 };
						int k1_a = (((k1 / 3) + 1) % 2) * 3, k2_a = k2;
						VertexInfo to_a{ surface0->TakePoint(i, j,k1_a, k2_a)->GetId(),
							k1_a, k2_a };
						Edge e_a{ surface0->GetId(), i, j,
							from,
							{surface0->TakePoint(i, j,abs(k1_a - 2), k2_a)->GetId(), abs(k1_a - 2), k2_a},
							{surface0->TakePoint(i, j,abs(k1_a - 1), k2_a)->GetId(), abs(k1_a - 1), k2_a},
							to_a
						};
						graph.Insert(v1, e_a);

						int k1_b = k1, k2_b = (((k2 / 3) + 1) % 2) * 3;
						VertexInfo to_b{ surface0->TakePoint(i, j, k1_b, k2_b)->GetId(),
							k1_b, k2_b };
						Edge e_b{ surface0->GetId(), i, j,
							from,
							{surface0->TakePoint(i, j,k1_b, abs(k2_b - 2))->GetId(), k1_b, abs(k2_b - 2)},
							{surface0->TakePoint(i, j,k1_b, abs(k2_b - 1))->GetId(), k1_b, abs(k2_b - 1)},
							to_b
						};
						graph.Insert(v1, e_b);
					}
				}
			}
		}
	}
	return graph;
}

bool GregoryPatch::IsOutsideEdge(const Edge& edge)
{
	auto surface0 = surfaces[edge.surfaceId];
	if (surface0->GetWrapType() == BezierSurface::CreationType::surface)
		return edge.patchV == 0 && edge.to.k1 == 0 ||
		edge.patchV == surface0->SurfaceSize().x - 1 && edge.to.k1 == 3 ||
		edge.pathH == 0 && edge.to.k2 == 0 ||
		edge.pathH == surface0->SurfaceSize().y - 1 && edge.to.k2 == 3;
	else if (surface0->GetWrapType() == BezierSurface::CreationType::cylinder)
	{
		return edge.patchV == 0 && edge.to.k1 == 0 ||
			edge.patchV == surface0->SurfaceSize().x - 1 && edge.to.k1 == 3;
	}
	return false;
}

std::vector<TriangleCycle> GregoryPatch::FindTriangleCycles(Graph graph)
{
	std::set<TriangleCycle> cycles;
	for (const auto& startV : graph.vertex) {
		for (const auto& firstVEdge : graph[startV]) {
			for (const auto& secondVEdge : graph[firstVEdge.to.v]) {
				if (secondVEdge.to.v == startV)
					continue;
				for (const auto& thirdVEdge : graph[secondVEdge.to.v]) {
					if (thirdVEdge.to.v == firstVEdge.to.v)
						continue;

					if (thirdVEdge.to.v == startV) {
						auto cycle = TriangleCycle{ firstVEdge, secondVEdge, thirdVEdge };
						if (IsOutsideEdge(cycle.GetEdge_1()) || IsOutsideEdge(cycle.GetEdge_2()) || IsOutsideEdge(cycle.GetEdge_3()))
							cycles.insert(cycle);
					}
				}
			}
		}
	}
	return std::vector<TriangleCycle>{cycles.begin(),cycles.end()};
}



void GregoryPatch::Update()
{
	triangleCycle = FindTriangleCycles(CreateGraph());

	FigureContainer::Clear();
	canInsertFigureToFigureContainer = true;
	for (TriangleCycle& cycle : triangleCycle) {
		for (auto figure : cycle.GetEdge_1().FigureFromScene(scene)) { FigureContainer::Add(figure); }
		for (auto figure : cycle.GetEdge_2().FigureFromScene(scene)) { FigureContainer::Add(figure); }
		for (auto figure : cycle.GetEdge_3().FigureFromScene(scene)) { FigureContainer::Add(figure); }
	}
	canInsertFigureToFigureContainer = false;

	CreateGregoryPatchVAO();
	FigureContainer::Update();
}

void GregoryPatch::UnmarkFigure(Figure* figure)
{
}

void GregoryPatch::CreateGregoryPatchVAO()
{
	std::vector<float> vs{};

	// tymczasowo jak bezier C0 p³atek
	for (TriangleCycle& cycle : triangleCycle) {
		for (auto figure : cycle.GetEdge_1().FigureFromScene(scene)) { 
			OpenGLHelper::AddVecToVector(vs, figure->transpose->GetPosition());
		}
		for (auto figure : cycle.GetEdge_2().FigureFromScene(scene)) {
			OpenGLHelper::AddVecToVector(vs, figure->transpose->GetPosition());
		};
		for (auto figure : cycle.GetEdge_3().FigureFromScene(scene)) {
			OpenGLHelper::AddVecToVector(vs, figure->transpose->GetPosition());
		};
		for (auto figure : cycle.GetEdge_3().FigureFromScene(scene)) { // tymczasowo póŸniej bêd¹ tylko 3 fory
			OpenGLHelper::AddVecToVector(vs, figure->transpose->GetPosition());
		};
	}


	vao.Reactive();
	vao.Bind();
	{
		VBO vbo(vs, GL_DYNAMIC_DRAW);

		vao.LinkAttrib(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);

		vao.Unbind(); vbo.Unbind();

		glPatchParameteri(GL_PATCH_VERTICES, 16); // zmien póŸniej na 12
	}

	vs.clear();

	//for (auto cycle : triangleCycle) {
	//	std::vector<Edge> edges;
	//	edges.push_back(cycle.GetEdge_1());
	//	edges.push_back(cycle.GetEdge_2());
	//	edges.push_back(cycle.GetEdge_3());
	//	for (Edge& edge : edges) {
	//		glm::vec3 start = surfaces[edge.surfaceId]->
	//			TakePoint(edge.patchV, edge.pathH, edge.from.k1, edge.from.k2)->transpose->GetPosition();
	//		glm::vec3 end = start + surfaces[edge.surfaceId]->DerivativeVU(edge.patchV, edge.pathH, edge.from.k1 / 3, edge.from.k2 / 3);
	//		//end *= 5;
	//		OpenGLHelper::AddVecToVector(vs, start);
	//		OpenGLHelper::AddVecToVector(vs, end);

	//	}
	//}

	for (auto surf : surfaces) {
		for (int i = 0; i < 4; i += 3) {
			for (int j = 0; j < 4; j += 3) {
				glm::vec3 start = surfaces[surf.first]->
					TakePoint(0, 0, i,j)->transpose->GetPosition();
				glm::vec3 end = start + surfaces[surf.first]->DerivativeVU(0, 0, i / 3, j / 3);
				//end *= 5;
				OpenGLHelper::AddVecToVector(vs, start);
				OpenGLHelper::AddVecToVector(vs, end);

			}
		}
	}

	vaoArrows.Reactive();
	vaoArrows.Bind();
	{
		VBO vbo(vs, GL_DYNAMIC_DRAW);

		vaoArrows.LinkAttrib(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);

		vaoArrows.Unbind(); vbo.Unbind();
	}
}

void GregoryPatch::Draw(GLFWwindow* window, const Camera& camera)
{
	if (IsSomethingChange())
		Update();
	CreateGregoryPatchVAO();
	//tymczasowo na testy tylko
	auto showColor = GetShowColor();
	shader.Activate();
	vao.Bind();
	{
		glPatchParameteri(GL_PATCH_VERTICES, 16);

		camera.SaveMatrixToShader(shader.ID);
		glUniform4f(glGetUniformLocation(shader.ID, "COLOR"),
			showColor.x, showColor.y, showColor.z, showColor.w);
		glUniform1i(glGetUniformLocation(shader.ID, "PATCH_DIV"), patch_div);


		glUniform1i(glGetUniformLocation(shader.ID, "VERTICAL_DRAW"), false);
		glDrawArrays(GL_PATCHES, 0, triangleCycle.size()*16);

		glUniform1i(glGetUniformLocation(shader.ID, "VERTICAL_DRAW"), true);
		glDrawArrays(GL_PATCHES, 0, triangleCycle.size() * 16);
	}
	vao.Unbind();

	shaderArrows.Activate();
	vaoArrows.Bind();
	{
		glUniform4f(glGetUniformLocation(shaderArrows.ID, "COLOR"),
			0.5f, 0.3f, 0.1f, 1);
		camera.SaveMatrixToShader(shaderArrows.ID);
		glDrawArrays(GL_LINES, 0, surfaces.size()*8); //ContainerSize() + (ContainerSize() - 4) / 3
	}
	vaoArrows.Unbind();

}

bool GregoryPatch::IsValid(Figure* figure)
{
	return canInsertFigureToFigureContainer;
}

GregoryPatch::GregoryPatch(const char* name, FigureType type):Figure(name, type)
{
	SetUnmarkColor({ 0.8f, 1, 0.5f, 1 });
}
