#include "BezierInterpolated.h"

BezierInterpolated::BezierInterpolated(const char* name) :BezierInterpolated( name, "##BezierInterpolated", FigureType::BezierInterpolated)
{
	innerBezierC2.showVirtualPoints = false;
}

BezierInterpolated::BezierInterpolated():BezierInterpolated( "BezierInterpolated")
{
}

void BezierInterpolated::Draw(GLFWwindow* window, const Camera& camera)
{
	if (IsSomethingChange())
		Update();

	innerBezierC2.Draw(window, camera);
}

BezierInterpolated::BezierInterpolated(const char* name, const char* uniqueName, FigureType type) :
	BezierBase(name, uniqueName, type)
{
}


void BezierInterpolated::CreateBezier()
{
	innerBezierC2.Clear();
	virtualPointsNumber = 0;
	if (ContainerSize() < 2)
		return;

	int id = 0;
	while (virtualPoints.size() < ContainerSize() + 2) {
		VirtualPoint* p = new VirtualPoint();
		virtualPoints.push_back(p);
		p->SetColor({ 0, 0, 0, 1 });
		p->SetName(std::to_string(id).c_str());
		id++;
	}
	

	VirtualPoint* p = virtualPoints.at(virtualPointsNumber);
	virtualPointsNumber++;
	p->connectedPoints.clear();
	p->transpose->SetObjectPosition(At(0)->transpose->GetPosition());

	innerBezierC2.Add(p);

	int n = ContainerSize();
	float* a = new float[n];
	float* b = new float[n];
	float* c = new float[n];

	glm::vec3* interpolatedPointsPositions = new glm::vec3[n];
	glm::vec3* virtualPointsPositions = new glm::vec3[n];
	for (int i = 0; i < n; i++) {
		virtualPointsPositions[i] = interpolatedPointsPositions[i] = 
			orderdFigures[i]->transpose->GetPosition();
		a[i] = 1.0f / 6.0f;
		b[i] = 4.0f / 6.0f;
		c[i] = 1.0f / 6.0f;
	}

	a[0] = 0; a[n - 1] = 0;
	b[0] = 1; b[n - 1] = 1;
	c[0] = 0; c[n - 1] = 0;

	MathOperations::InversThreeDiagonalMatrix(n, virtualPointsPositions, a, b, c, interpolatedPointsPositions);

	for (int i = 0; i < n; i++) {
		p = virtualPoints.at(virtualPointsNumber);
		virtualPointsNumber++;
		p->connectedPoints.clear();
		p->transpose->SetObjectPosition(virtualPointsPositions[i]);

		innerBezierC2.Add(p);
	}
	p = virtualPoints.at(virtualPointsNumber);
	virtualPointsNumber++;
	p->connectedPoints.clear();
	p->transpose->SetObjectPosition(At(ContainerSize() - 1)->transpose->GetPosition());
	
	innerBezierC2.Add(p);

}

void BezierInterpolated::ChangeShowBezierC0()
{
	innerBezierC2.ChangeShowBezierC0();
}

void BezierInterpolated::ChangeShowBezierPol()
{
	innerBezierC2.ChangeShowBezierPol();
}

