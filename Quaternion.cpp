#include "Quaternion.h"

Quaternion::Quaternion(){
    r(1);
}

Quaternion::Quaternion(float r, float i, float j, float k)
{
    this->r(r);
    this->i(i);
    this->j(j);
    this->k(k);

}

Quaternion::Quaternion(float angle, glm::vec3 axis)
{
    SetRotationAlong(angle, axis);
}

Quaternion::Quaternion(glm::vec3 values)
{
    this->values = glm::vec4(0, values);
}

Quaternion::Quaternion(glm::vec4 values)
{
    this->values = values;
}

Quaternion::Quaternion(glm::mat3 RotationMatrix)
{
    glm::vec4 q;
    float t;

    if (RotationMatrix[2][2] < 0)
    {
        if (RotationMatrix[0][0] > RotationMatrix[1][1])
        {
            t = 1 + RotationMatrix[0][0] - RotationMatrix[1][1] - RotationMatrix[2][2];
            q = { 
                t,
                RotationMatrix[0][1] + RotationMatrix[1][0],
                RotationMatrix[2][0] + RotationMatrix[0][2],
                -RotationMatrix[1][2] + RotationMatrix[2][1] };
        }
        else
        {
            t = 1 - RotationMatrix[0][0] + RotationMatrix[1][1] - RotationMatrix[2][2];
            q = { 
                RotationMatrix[0][1] + RotationMatrix[1][0],
                t,
                RotationMatrix[1][2] + RotationMatrix[2][1],
                -RotationMatrix[2][0] + RotationMatrix[0][2] };
        }
    }
    else
    {
        if (RotationMatrix[0][0] < -RotationMatrix[1][1])
        {
            t = 1 - RotationMatrix[0][0] - RotationMatrix[1][1] + RotationMatrix[2][2];
            q = {
                RotationMatrix[2][0] + RotationMatrix[0][2],
                RotationMatrix[1][2] + RotationMatrix[2][1],
                t,
                -RotationMatrix[0][1] + RotationMatrix[1][0] };
        }
        else
        {
            t = 1 + RotationMatrix[0][0] + RotationMatrix[1][1] + RotationMatrix[2][2];
            q = {
                -RotationMatrix[1][2] + RotationMatrix[2][1],
                -RotationMatrix[2][0] + RotationMatrix[0][2],
                -RotationMatrix[0][1] + RotationMatrix[1][0],
                t };
        }
    }
    q *= 0.5 / sqrtf(t);
    values = q;
}

Quaternion::Quaternion(glm::mat4 RotationMatrix): Quaternion(glm::mat3(RotationMatrix))
{

}

void Quaternion::SetRotationAlong(float angle, glm::vec3 axis)
{
    axis = glm::normalize(axis);
    values = glm::vec4(cosf(angle * 0.5f), sinf(angle*0.5f) * axis);
}

glm::vec4 Quaternion::GetAngleRepresentation()
{
    glm::vec4 result{ 0 };
    auto tmp = glm::normalize(values);
    float angle = acosf(tmp.x);
    float s = sinf(angle);
    if (s < 0.0000000000001)
    {
        return { 2 * angle, 0, 0, 0 };
    }
    result = glm::vec4(2 * angle, tmp.y / s, tmp.z / s, tmp.w / s);
    return result;
}

Quaternion Quaternion::RotatedAlong(float angle, glm::vec3 axis) const
{
    Quaternion r;
    r.SetRotationAlong(angle, axis);
    auto q = *this;
    q.Normalize();
    auto result = r* q;
    result.Normalize();
    return result;
}

glm::mat4 Quaternion::GetEulerRotation() const
{
    glm::mat4 R{ 0.0f };
    R[0][0] = 1 - 2 * powf(j(), 2) - 2 * powf(k(), 2);
    R[0][1] = 2 * i() * j() + 2 * r() * k();
    R[0][2] = 2 * i() * k() - 2 * r() * j();
    
    R[1][0] = 2 * i() * j() - 2 * r() * k();
    R[1][1] = 1 - 2 * powf(i(), 2) - 2 * powf(k(), 2);
    R[1][2] = 2 * j() * k() + 2 * r() * i();

    R[2][0] = 2 * i() * k() + 2 * r() * j();
    R[2][1] = 2 * j() * k() - 2 * r() * i();
    R[2][2] = 1 - 2 * powf(i(), 2) - 2 * powf(j(), 2);

    R[3][3] = 1;

    return R;
}

void Quaternion::Normalize()
{
    values = glm::normalize(values);
}

Quaternion Quaternion::Invers() const
{
    Quaternion result;
    result.values = -values;
    result.r(r());
    return result;
}

glm::vec3 Quaternion::RotatePoint(glm::vec3 point) const
{
    Quaternion tmp = *this;
    tmp.Normalize();
    tmp = tmp * point* tmp.Invers();
    return glm::vec3(tmp.values.y, tmp.values.z, tmp.values.w);
}

Quaternion operator*(Quaternion const& q1, Quaternion const& q2) {
    Quaternion result;
    result.r(q1.r() * q2.r() - q1.i() * q2.i() - q1.j() * q2.j() - q1.k() * q2.k());
    result.i(q1.r() * q2.i() + q1.i() * q2.r() + q1.j() * q2.k() - q1.k() * q2.j());
    result.j(q1.r() * q2.j() - q1.i() * q2.k() + q1.j() * q2.r() + q1.k() * q2.i());
    result.k(q1.r() * q2.k() + q1.i() * q2.j() - q1.j() * q2.i() + q1.k() * q2.r());
    return result;
}

Quaternion operator+(Quaternion const& q1, Quaternion const& q2) {
    Quaternion result;
    result.values = q1.values + q2.values;
    return result;
}

bool operator==(Quaternion const& q1, Quaternion const& q2)
{
    return q1.values == q2.values;
}

bool operator!=(Quaternion const& q1, Quaternion const& q2)
{
    return !(q1 == q2);
}
