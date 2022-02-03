
#pragma once
#ifndef SCENE_H
#define SCENE_H

#include "stdafx.h"
#include "InputHandler.h"
#include "Camera.h"
#include "LightPoint.h"
#include "Model.h"
#include "Texture.h"

// New files
// Material
// Texture <- stb

// TEMP


class Scene
{
protected:
	ID3D11Device* dxdevice;
	ID3D11DeviceContext* dxdevice_context;
	int						window_width;
	int						window_height;

public:
	long mousedx, mousedy;
	Scene(
		ID3D11Device* dxdevice,
		ID3D11DeviceContext* dxdevice_context,
		int window_width,
		int window_height);

	virtual void Init() = 0;

	virtual void Update(
		float dt,
		InputHandler* input_handler) = 0;

	virtual void Render() = 0;

	virtual void Release() = 0;

	virtual void WindowResize(
		int window_width,
		int window_height);
};

class OurTestScene : public Scene
{
	//
	// Constant buffers (CBuffers) for data that is sent to shaders
	//

	// CBuffer for transformation matrices
	ID3D11Buffer* transformation_buffer = nullptr;
	// + other CBuffers
	ID3D11Buffer* lightCam_buffer = nullptr;
	ID3D11Buffer* material_buffer = nullptr;


	// 
	// CBuffer client-side definitions
	// These must match the corresponding shader definitions 
	//

	struct TransformationBuffer
	{
		mat4f ModelToWorldMatrix;
		mat4f WorldToViewMatrix;
		mat4f ProjectionMatrix;
	};


	struct LightCamBuffer
	{
		vec4f cameraPos;
		vec4f lightPos;
	};


	struct MaterialBuffer
	{
		vec4f Ka, Kd, Ks;
	};


	//
	// Scene content
	//
	Camera* camera;
	LightPoint* light;

	QuadModel* quad;
	Cube* cube;
	Cube* cube2;
	Cube* cube3;
	Cube* cube4;
	OBJModel* sponza;
	OBJModel* sphere;

	// Model-to-world transformation matrices
	mat4f Msponza;
	mat4f Mquad;
	mat4f Mcube;
	mat4f Mcube2;
	mat4f Mcube3;
	mat4f Mcube4;

	mat4f Msphere;

	// World-to-view matrix
	mat4f Mview;
	// Projection matrix
	mat4f Mproj;

	// Misc
	float angle = 0;			// A per-frame updated rotation angle (radians)...
	float angle_vel = fPI / 2;	// ...and its velocity (radians/sec)
	float camera_vel = 5.0f;	// Camera movement velocity in units/s
	float fps_cooldown = 0;

	void InitTransformationBuffer();
	void InitLightCamBuffer();
	void InitMaterialBuffer();

	void UpdateTransformationBuffer(
		mat4f ModelToWorldMatrix,
		mat4f WorldToViewMatrix,
		mat4f ProjectionMatrix);

	void UpdateLightCamBuffer(
		vec4f cameraPos,
		vec4f lightPos);

	void UpdateMaterialBuffer(vec4f Ka, vec4f Kd, vec4f Ks);

public:
	OurTestScene(
		ID3D11Device* dxdevice,
		ID3D11DeviceContext* dxdevice_context,
		int window_width,
		int window_height);

	void Init() override;

	void Update(
		float dt,
		InputHandler* input_handler) override;

	void Render() override;

	void Release() override;

	void WindowResize(
		int window_width,
		int window_height) override;
};

#endif