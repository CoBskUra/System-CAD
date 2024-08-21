#include "QuaternionRotationImGui.h"

bool operator==(QuaternionRotationImGui const& q1, QuaternionRotationImGui const& q2)
{
    return q1.Get() == q2.Get();
}

bool operator!=(QuaternionRotationImGui const& q1, QuaternionRotationImGui const& q2)
{
    return !(q1 == q2);
}
