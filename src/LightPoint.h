
//
//  LightPoint.h
//
//	Basic light class
//

#pragma once
#ifndef LIGHTPOINT_H
#define LIGHTPOINT_H

#include "vec\mat.h"
#include "vec\mat.h"

using namespace linalg;

class LightPoint
{
public:

	float angle = 0;			// rad
	float angle_vel = fPI / 4;	// rad/s
	float yaw, pitch;

	vec3f position;

	LightPoint()
	{
		position = { 0.0f, 0.0f, 0.0f };
		yaw = 0;
		pitch = 0;
	}

	// Move to an absolute position
	//
	void moveTo(const vec3f& p)
	{
		position = p;
	}

	// Move relatively
	//
	//void move(const vec3f& v)
	//{
	//	position += v;
	//}

	void move(const vec3f& v)
	{
		vec4f M = get_ViewToWorldMatrix() * vec4f(v, 0);

		position += M.xyz();
	}

	void rotateAroundX(const float v)
	{
		pitch -= v;

		pitch = CLAMP(pitch, -PI / 2, PI / 2);
	}

	void rotateAroundY(const float v)
	{
		yaw -= v;
	}

	mat4f get_ViewToWorldMatrix() const
	{
		mat4f R, T, S, M;
		R = mat4f::rotation(0, yaw, pitch);
		T = mat4f::translation(position);
		M = T * R;
		return M;
	}

	// Return World-to-View matrix for this camera
	//
	mat4f get_WorldToViewMatrix()
	{
		// Assuming a camera's position and rotation is defined by matrices T(p) and R,
		// the View-to-World transform is T(p)*R (for a first-person style camera).
		//
		// World-to-View then is the inverse of T(p)*R;
		//		inverse(T(p)*R) = inverse(R)*inverse(T(p)) = transpose(R)*T(-p)
		// Since now there is no rotation, this matrix is simply T(-p)

		//return mat4f::translation(-position);

		mat4f T, R, S, M;
		R = linalg::transpose(mat4f::rotation(0, yaw, pitch));
		T = mat4f::translation(-position);
		M = R * T;
		return M;
	}

};

#endif
