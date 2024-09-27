#include "IntersectionCurve.h"
#include "Figures/Intersection/IntersectionAble.h"

void IntersectionCurve::ConvertToInterpolated()
{
	for (int i = 0; i < points.size(); i++) {
		scene->AddFigure(points[i]);
	}
	scene->AddFigure(curve);
	scene->DeleteFigureById(GetId());
}

glm::vec3 IntersectionCurve::Derivative(float t)
{
	return curve->Derivative(t);
}

glm::vec3 IntersectionCurve::PositionOnCurve(float t)
{
	return curve->PositionOnCurve(t);
}

IntersectionCurve::IntersectionCurve(std::vector<glm::vec2> parametryzationVector, std::shared_ptr<Figure> figure, Scene* scene):BezierCurve("IntersectionCurve", FigureType::IntersectionCurve)
{
	auto intersection = dynamic_cast<IntersectionAble*>(figure.get());

	intersectedFigure = figure;
	curve = std::make_shared<BezierInterpolated>("BezierInterpolated-Intersection");
	this->scene = scene;

	if(!curve->showBezierPol)
		curve->ChangeShowBezierPol();
	if (curve->showBezierCurve)
		curve->ChangeShowBezierCurve();
	

	points.reserve(parametryzationVector.size());
	params = parametryzationVector;
	for (glm::vec2 pos : parametryzationVector) {
		std::shared_ptr<Point> point = std::make_shared<Point>();
		point->transpose->SetObjectPosition(intersection->Parametrization(pos.x, pos.y));
		curve->Add(point.get());
		points.push_back(point);
	}

	curve->Unmark();
	SetUnmarkColor({ 0.215, 1, 0.129f, 1 });
	
	auto intersectionAble = dynamic_cast<IntersectionAble*>(figure.get());
	glm::vec2 field_v = intersectionAble->Field_v();
	glm::vec2 field_u = intersectionAble->Field_u();
	float length_v = field_v.y - field_v.x;
	float length_u = field_u.y - field_u.x;

	data = std::vector<float>(N * N );
	for(int k = 0; k < parametryzationVector.size() - 1; k++)
	{
		auto pos = parametryzationVector[k];
		int y1 = (N - 1) * (pos.x - field_v.x) / length_v;
		int x1 = (N - 1) * (pos.y - field_u.x) / length_u;

		auto pos2 = parametryzationVector[k + 1];
		int y2 = (N - 1) * (pos2.x - field_v.x) / length_v;
		int x2 = (N - 1) * (pos2.y - field_u.x) / length_u;

        glm::vec2 vector{ x2 - x1, y2 - y1};
        glm::vec2 vectorWrapped{ x1 + (N - x2), y1 + ( N - y2) };
        if (abs(x2 - x1) >  abs(x1 + (N - x2)) || abs(x2 - x1) > abs(x2 + (N - x1)))
        {
            if (x2 < x1)
                x2 += N;
            else
                x2 -= N;
            
        }

        if (abs(y2 - y1) > abs(y1 + (N - y2)) || abs(y2 - y1) > abs(y2 + (N - y1)))
        {
            if (y2 < y1)
                y2 += N;
            else
                y2 -= N;
        }

        BresenhamLineWraped(data, x1, y1, x2, y2);
	}

    FloodFill(data);
	texture.Recreat();
	texture.Bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, N, N, 0, GL_RED, GL_FLOAT, data.data());
	texture.Unbind();
}

void IntersectionCurve::Draw(GLFWwindow* window, const Camera& camera)
{
	if (intersectedFigure.expired()) {
		scene->DeleteFigureById(GetId());
		return;
	}

	curve->Draw(window, camera);
}

void IntersectionCurve::ActiveImGui()
{
    if (intersectedFigure.expired()) {
        scene->DeleteFigureById(GetId());
        return;
    }
	Figure::ActiveImGui();
}

void IntersectionCurve::FigureSpecificImGui()
{
    if (intersectedFigure.expired()) {
        scene->DeleteFigureById(GetId());
        return;
    }

	if (ImGui::Button("Convert to Interpolated##IntersectionCurve")) {
		ConvertToInterpolated();
	}
	ImGui::Text("pointer = %x", texture.ID);

    ImGui::Text("figure: %s", intersectedFigure.lock()->name);
	ImGui::Text("size = %d x %d", N, N);
    ImGui::Image((void*)(intptr_t)texture.ID, ImVec2(N, N));

    bool refresh = false;
    if (ImGui::RadioButton("Show red##IntersectionCurve", showRed)) {
        showRed = !showRed;
        refresh = true;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Show black##IntersectionCurve", !showRed)) {
        showRed = !showRed;
        refresh = true;
        
    }

    if (ImGui::RadioButton("Include intersection##IntersectionCurve", includeIntersection)) {
        includeIntersection = !includeIntersection;
        refresh = true;
        
    }

    if (refresh)
    {
        auto inter = dynamic_cast<IntersectionAble*>(intersectedFigure.lock().get());

        float min = showRed ? 0.5f : 0.0f;
        float max = showRed ? 1.0f : 0.49f;
        if (includeIntersection)
            inter->AddActiveTexture(this, min, max);
        else
            inter->RemoveActiveTexture(TextureId());
    }

}

glm::vec4 IntersectionCurve::SetUnmarkColor(glm::vec4 color)
{
	curve->SetUnmarkColor(color);
	return Figure::SetUnmarkColor(color);
}

void IntersectionCurve::SetMarkColor(glm::vec4 color)
{
	curve->SetUnmarkColor(color);
	Figure::SetMarkColor(color);
}

void IntersectionCurve::SetShowColor(glm::vec4 color)
{
	curve->SetShowColor(color);
	Figure::SetShowColor(color);
}

void IntersectionCurve::BresenhamLineWraped(std::vector<float>& data, const int x1, const int y1, const int x2, const int y2)
{
    int d, dx, dy, ai, bi, xi, yi;
    int x = x1, y = y1;
    int xWrap, yWrap;
   
    // ustalenie kierunku rysowania
    if (x1 < x2)
    {
        xi = 1;
        dx = x2 - x1;
    }
    else
    {
        xi = -1;
        dx = x1 - x2;
    }
    // ustalenie kierunku rysowania
    if (y1 < y2)
    {
        yi = 1;
        dy = y2 - y1;
    }
    else
    {
        yi = -1;
        dy = y1 - y2;
    }
    // pierwszy piksel
    yWrap = MathOperations::Wrap(y, 0, N);
    xWrap = MathOperations::Wrap(x, 0, N);
    data[yWrap * N + xWrap] = 1;
    // oœ wiod¹ca OX
    if (dx > dy)
    {
        ai = (dy - dx) * 2;
        bi = dy * 2;
        d = bi - dx;
        // pêtla po kolejnych x
        while (x != x2)
        {
            // test wspó³czynnika
            if (d >= 0)
            {
                x += xi;
                y += yi;
                d += ai;
            }
            else
            {
                d += bi;
                x += xi;
            }
            yWrap = MathOperations::Wrap(y, 0, N);
            xWrap = MathOperations::Wrap(x, 0, N);
            data[yWrap * N + xWrap] = 1;
        }
    }
    // oœ wiod¹ca OY
    else
    {
        ai = (dx - dy) * 2;
        bi = dx * 2;
        d = bi - dy;
        // pêtla po kolejnych y
        while (y != y2)
        {
            // test wspó³czynnika
            if (d >= 0)
            {
                x += xi;
                y += yi;
                d += ai;
            }
            else
            {
                d += bi;
                y += yi;
            }
            
            yWrap = MathOperations::Wrap(y, 0, N);
            xWrap = MathOperations::Wrap(x, 0, N);
            data[yWrap * N + xWrap] = 1;
        }
    }
}

void IntersectionCurve::FloodFill(std::vector<float>& data)
{

    std::queue<std::pair<int, int>> nodes = std::queue<std::pair<int, int>>();
    int x = -1, y = -1;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (data[N * i + j] == 0)
            {
                x = j;
                y = i;
                break;
            }
        }
        if (x != -1)
            break;
    }
    nodes.push({ x, y });

    while (!nodes.empty()) {
        auto node = nodes.front();
        nodes.pop();

        if (data[N * node.second + node.first] > 0) {
            continue;
        }

        data[N * node.second + node.first] = 0.5f;
        int x = node.first;
        int y = node.second;

        nodes.push({ (x + 1) % N, y });
        nodes.push({ (x - 1 + N) % N, y });
        nodes.push({ x, (y + 1) % N });
        nodes.push({ x, (y - 1 + N) % N });
    }
}

const std::vector<float>& IntersectionCurve::GetResultData()
{
    return data;
}

GLuint IntersectionCurve::TextureId()
{
    return texture.ID;
}

IntersectionCurve::~IntersectionCurve()
{
    if (intersectedFigure.expired())
        return;

    auto inter = dynamic_cast<IntersectionAble*>(intersectedFigure.lock().get());
    inter->RemoveActiveTexture(texture.ID);
}


