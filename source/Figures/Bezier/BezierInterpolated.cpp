#include "BezierInterpolated.h"

BezierInterpolated::BezierInterpolated(const char* name) :BezierInterpolated( name,  FigureType::BezierInterpolated)
{
	//innerBezierC2.showVirtualPoints = false;
}

BezierInterpolated::BezierInterpolated():BezierInterpolated( "BezierInterpolated")
{
}

BezierInterpolated::BezierInterpolated(MG1::InterpolatedC2 b0, const Scene* scene, int idOffset) :BezierInterpolated() {
	for (auto refPoint : b0.controlPoints) {
		uint32_t id = refPoint.GetId() + idOffset;
		this->Add(scene->byID(id).get());
	}
	if (b0.name != "")
		this->SetName(b0.name.c_str());
	this->SetId(b0.GetId() + idOffset);
}

MG1::InterpolatedC2 BezierInterpolated::Serialize(int idOffset) const
{
	MG1::InterpolatedC2 bezier{};
	bezier.SetId(GetId() - idOffset);
	bezier.name = name;
	for (int i = 0; i < ContainerSize(); i++)
	{
		MG1::PointRef p{ At(i)->GetId() - idOffset };

		bezier.controlPoints.push_back(p);
	}
	return bezier;
}

void BezierInterpolated::Draw(GLFWwindow* window, const Camera& camera)
{
	if (IsSomethingChange())
		Update();

	innerBezierC0.Draw(window, camera);

	/*for (int i = 0; i < virtualPointsNumber; i++) {
		virtualPoints[i]->Draw(window, camera);
	}*/
}

BezierInterpolated::BezierInterpolated(const char* name, FigureType type) :
	BezierBase(name, type)
{
}


void BezierInterpolated::CreateBezier()
{
	innerBezierC0.Clear();
	virtualPointsNumber = 0;
	if (virtualPoints.size() > 8 * ContainerSize()) {
		auto ite_3div4 = std::next(virtualPoints.begin(), 3 * virtualPoints.size() / 4);
		virtualPoints.erase(ite_3div4, virtualPoints.end());
	}

	if (ContainerSize() < 2)
		return;

	int id = 0;
	while (virtualPoints.size() < 4 * ContainerSize() ) {
		VirtualPoint* p = new VirtualPoint();
		virtualPoints.push_back(p);
		p->SetUnmarkColor({ 0, 0, 0, 1 });
		p->SetName(std::to_string(id).c_str());
		id++;
	}

	float minDelta = 0.01;
	std::vector<glm::vec3> interpolatedPoints;
	interpolatedPoints.push_back(At(0)->transpose->GetPosition());
	for (int i = 1; i < ContainerSize(); i++) {
		auto tmp = interpolatedPoints.at(interpolatedPoints.size() - 1) - At(i)->transpose->GetPosition();

		if (sqrtf(glm::dot(tmp, tmp)) >= minDelta)
		{
			interpolatedPoints.push_back(At(i)->transpose->GetPosition());
		}
	}

	if (interpolatedPoints.size() == 2) {

		glm::vec3 next_a = At(0)->transpose->GetPosition();
		glm::vec3 next_b = { 0, 0, 0 };
		glm::vec3 next_c = { 0, 0, 0 };
		glm::vec3 next_d = At(1)->transpose->GetPosition();

		AddPoint(innerBezierC0, next_a);
		AddPoint(innerBezierC0, next_a);
		AddPoint(innerBezierC0, next_a);
		AddPoint(innerBezierC0, next_d);
	}
	else {
		int n = interpolatedPoints.size() - 2;
		if (n <= 0)
			return;
		

		float* belowDiagonal = new float[n];
		float* diagonal = new float[n] {2.0f};
		float* aboveDiagonal = new float[n];

		glm::vec3* beizer_c_values = new glm::vec3[n + 2];
		glm::vec3* scratch = new glm::vec3[n];

		for (int i = 0; i < n; i++) {
			int k = i + 1;
			auto tmp = 
				3.0f*(
					(interpolatedPoints.at(k + 1) - interpolatedPoints.at(k)) / TakePointDelta(interpolatedPoints, k) -
					(interpolatedPoints.at(k) - interpolatedPoints.at(k - 1)) / TakePointDelta(interpolatedPoints, k - 1)
				) / (TakePointDelta(interpolatedPoints, k - 1) + TakePointDelta(interpolatedPoints, k));
			beizer_c_values[k] = tmp;
		}

		for (int i = 1; i < n; i++) {
			belowDiagonal[i] = TakePointDelta(interpolatedPoints, i - 1) /
				(TakePointDelta(interpolatedPoints, i - 1) + TakePointDelta(interpolatedPoints, i));
			diagonal[i] = 2.0f ;
			aboveDiagonal[i - 1] = TakePointDelta(interpolatedPoints, i) /
				(TakePointDelta(interpolatedPoints, i - 1) + TakePointDelta(interpolatedPoints, i));
		}
		diagonal[0] = 2.0f;

		MathOperations::InversThreeDiagonalMatrix(
			n, &beizer_c_values[1], belowDiagonal, diagonal, aboveDiagonal, scratch);
		beizer_c_values[n + 1] = glm::vec3{ 0, 0 , 0 };
		beizer_c_values[0] = glm::vec3{ 0, 0 , 0 };

		AddPoint(innerBezierC0, interpolatedPoints.at(0));
		for (int i = 0; i < n + 1; i++) {

			float functionRange = TakePointDelta(interpolatedPoints, i);
			glm::vec3 Pi = interpolatedPoints.at(i );
			glm::vec3 Pi_1 = interpolatedPoints.at(i + 1);
			glm::vec3 a = Pi;
			glm::vec3 c = beizer_c_values[i];
			//glm::vec3 d = (beizer_c_values[i + 1] - beizer_c_values[i]) / (3.0f * functionRange);
			glm::vec3 b = (Pi_1 - Pi - ((beizer_c_values[i + 1] + 2.0f * beizer_c_values[i]) / 3.0f  )* functionRange * functionRange);

			c *= functionRange * functionRange;
			//d *= functionRange * functionRange * functionRange;

			AddPoint(innerBezierC0, a + b / 3.0f);
			AddPoint(innerBezierC0, a + (2.0f * b + c) / 3.0f);
			AddPoint(innerBezierC0, Pi_1);
		}

		delete[] belowDiagonal;
		belowDiagonal = 0;
		delete[] diagonal;
		diagonal = 0;
		delete[] aboveDiagonal;
		aboveDiagonal = 0;
		delete[] beizer_c_values;
		beizer_c_values = 0;
		delete[] scratch;
		scratch = 0;
	}
}

void BezierInterpolated::ChangeShowBezierCurve()
{
	BezierBase::ChangeShowBezierCurve();
	innerBezierC0.ChangeShowBezierCurve();
}

void BezierInterpolated::ChangeShowBezierPol()
{
	BezierBase::ChangeShowBezierPol();
	innerBezierC0.ChangeShowBezierPol();
}

float BezierInterpolated::TakePointDelta(int i)
{
	glm::vec3 tmp = At(i + 1)->transpose->GetPosition() - At(i)->transpose->GetPosition();
	return sqrtf(glm::dot(tmp, tmp));
}

float BezierInterpolated::TakePointDelta(std::vector<glm::vec3> points, int i)
{
	glm::vec3 tmp = points.at(i + 1) - points.at(i);
	return sqrtf(glm::dot(tmp, tmp));
}

void BezierInterpolated::AddPoint(BezierC0& bezier, glm::vec3 pos)
{
	VirtualPoint* p = virtualPoints.at(virtualPointsNumber);
	virtualPointsNumber++;
	p->connectedPoints.clear();
	p->transpose->SetObjectPosition(pos);

	bezier.Add(p);
}

