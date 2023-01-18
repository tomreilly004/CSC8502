#pragma once
# include "Vector4.h"
# include "Vector3.h"
#include "Light.h"


class Spotlight : public Light {
public:
	Spotlight() {}

	Spotlight(const Vector3& position, const Vector4& colour, float radius, Vector3 direction, float angle) {
		this->position = position;
		this->colour = colour;
		this->radius = radius;
		this->direction = direction;
		this->angle = angle;
	}

	~Spotlight(void) {};

	Vector3 GetDirection() const { return direction; }
	void SetDirection(const Vector3& val) { direction = val; }

	float GetAngle() const { return cos(angle * (PI / 180)); }


	Vector3 GetPosition() const { return position; }
	void SetPosition(const Vector3& val) { position = val; }

	float GetRadius() const { return radius; }
	void SetRadius(float val) { radius = val; }

	Vector4 GetColour() const { return colour; }
	void SetColour(const Vector4& val) { colour = val; }

protected:
	Vector3 position;
	float radius;
	Vector4 colour;
	Vector3 direction;
	float angle;
};