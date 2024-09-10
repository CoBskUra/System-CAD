enum class FigureType {
	UnchangeObject,
	Figure, // notVisable
	Cursor,
	CenterPoint,
	VirtualPoint,
	Any, // visable
	Point,
	Torus,
	BezierC0,
	BezierC2,
	BezierInterpolated,
	BezierSurfaceC0, 
	BezierSurfaceC2,
	GregoryPatch,
	COUNT
};

inline const char* ToString(FigureType v)
{
	switch (v)
	{
	case FigureType::Any: return "Any";
	case FigureType::Figure: return "Figure";
	case FigureType::Torus: return "Torus";
	case FigureType::Cursor: return "Cursor";
	case FigureType::CenterPoint: return "CenterPoint";
	case FigureType::Point: return "Point";
	case FigureType::BezierC0: return "BezierC0";
	case FigureType::BezierC2: return "BezierC2";
	case FigureType::BezierInterpolated: return "BezierInterpolated";
	case FigureType::BezierSurfaceC0: return "BezierSurfaceC0";
	case FigureType::BezierSurfaceC2: return "BezierSurfaceC2";
	case FigureType::VirtualPoint: return "VirtualPoint";
	case FigureType::GregoryPatch: return "GregoryPatch";
	case FigureType::COUNT: 
	default: return "";
	}
}