
#include "Scene.h"

Scene::Scene(
	ID3D11Device* dxdevice,
	ID3D11DeviceContext* dxdevice_context,
	int window_width,
	int window_height) :
	dxdevice(dxdevice),
	dxdevice_context(dxdevice_context),
	window_width(window_width),
	window_height(window_height)
{
}

void Scene::WindowResize(
	int window_width,
	int window_height)
{
	this->window_width = window_width;
	this->window_height = window_height;
}

OurTestScene::OurTestScene(
	ID3D11Device* dxdevice,
	ID3D11DeviceContext* dxdevice_context,
	int window_width,
	int window_height) :
	Scene(dxdevice, dxdevice_context, window_width, window_height)
{
	InitTransformationBuffer();
	InitLightCamBuffer();
	InitMaterialBuffer();
	// + init other CBuffers
}

//
// Called once at initialization
//
void OurTestScene::Init()
{
	camera = new Camera(
		45.0f * fTO_RAD,		// field-of-view (radians)
		(float)window_width / window_height,	// aspect ratio
		1.0f,					// z-near plane (everything closer will be clipped/removed)
		500.0f);				// z-far plane (everything further will be clipped/removed)

	// Move camera to (0,0,5)
	camera->moveTo({ 0, 0, 5 });

	light = new LightPoint();
	light->moveTo({ 0, 5, 0 });

	// Create objects
	quad = new QuadModel(dxdevice, dxdevice_context);
	cube = new Cube("assets/sphere/sphere.mtl", dxdevice, dxdevice_context);
	cube2 = new Cube("assets/WoodenCrate/WoodenCrate.mtl", dxdevice, dxdevice_context);
	cube3 = new Cube("assets/WoodenCrate/WoodenCrate.mtl", dxdevice, dxdevice_context);

	sponza = new OBJModel("assets/crytek-sponza/sponza.obj", dxdevice, dxdevice_context);
	sphere = new OBJModel("assets/sphere/sphere.obj", dxdevice, dxdevice_context);
	sphere->Ka = { 1, 0.5, 0 };
	sphere->Kd = { 0, 1, 0 };
	sphere->Ks = { 1, 0, 0 };

	sponza->Ka = { 0, 0.5, 0 };
	sponza->Kd = { 0, 0.5, 0 };
	sponza->Ks = { 1, 1, 1 };
}

//
// Called every frame
// dt (seconds) is time elapsed since the previous frame
//
void OurTestScene::Update(float dt, InputHandler* input_handler)
{
	mousedx = input_handler->GetMouseDeltaX();
	mousedy = input_handler->GetMouseDeltaY();

#pragma region CameraControl

	// Basic camera control
	if (input_handler->IsKeyPressed(Keys::W))
	{

		camera->move({ 0.0f, 0.0f, -camera_vel * dt });
		light->move({ 0.0f, 0.0f, -camera_vel * dt });

	}
	if (input_handler->IsKeyPressed(Keys::S))
	{

		camera->move({ 0.0f, 0.0f, camera_vel * dt });
		//light->move({ 0.0f, 0.0f, camera_vel * dt });

	}
	if (input_handler->IsKeyPressed(Keys::D))
	{
		camera->move({ camera_vel * dt, 0.0f, 0.0f });
		//light->move({ camera_vel * dt, 0.0f, 0.0f });

	}
	if (input_handler->IsKeyPressed(Keys::A))
	{
		camera->move({ -camera_vel * dt, 0.0f, 0.0f });
		//light->move({ -camera_vel * dt, 0.0f, 0.0f });
	}
	if (input_handler->IsKeyPressed(Keys::Ctrl))
	{
		camera->move({ 0.0f,-camera_vel * dt ,0.0f });
		//light->move({ 0.0f,-camera_vel * dt ,0.0f });
	}
	if (input_handler->IsKeyPressed(Keys::Shift))
	{
		camera->move({ 0.0f,camera_vel * dt ,0.0f });
		//light->move({ 0.0f,camera_vel * dt ,0.0f });
	}
	if (input_handler->IsKeyPressed(Keys::Space))
	{
		camera->rotateAroundY(camera_vel * mousedx * dt * 0.3f);
		camera->rotateAroundX(camera_vel * mousedy * dt * 0.3f);
		/*light->rotateAroundY(camera_vel * mousedx * dt * 0.3f);
		light->rotateAroundX(camera_vel * mousedy * dt * 0.3f);*/
	}
#pragma endregion

#pragma region LightControl

	// Basic camera control
	if (input_handler->IsKeyPressed(Keys::Up))
	{

		light->move({ 0.0f, 0.0f, -camera_vel * dt });

	}
	if (input_handler->IsKeyPressed(Keys::Down))
	{

		light->move({ 0.0f, 0.0f, camera_vel * dt });

	}
	if (input_handler->IsKeyPressed(Keys::Right))
	{
		light->move({ camera_vel * dt, 0.0f, 0.0f });

	}
	if (input_handler->IsKeyPressed(Keys::Left))
	{
		light->move({ -camera_vel * dt, 0.0f, 0.0f });
	}
	//if (input_handler->IsKeyPressed(Keys::Space))
	//{
	//	light->rotateAroundY(camera_vel * mousedx * dt * 0.3f);
	//	light->rotateAroundX(camera_vel * mousedy * dt * 0.3f);
	//}
//#pragma endregion


	// Now set/update object transformations
	// This can be done using any sequence of transformation matrices,
	// but the T*R*S order is most common; i.e. scale, then rotate, and then translate.
	// If no transformation is desired, an identity matrix can be obtained 
	// via e.g. Mquad = linalg::mat4f_identity; 

	// Quad model-to-world transformation
	Mquad = mat4f::translation(0, 0, 0) *			// No translation
		mat4f::rotation(-angle, 0.0f, 1.0f, 0.0f) *	// Rotate continuously around the y-axis
		mat4f::scaling(1.5, 1.5, 1.5);				// Scale uniformly to 150%

	Mcube = mat4f::translation(0, -1, -4) *			// No translation
		/*mat4f::rotation(-angle, 0.0f, 1.0f, 0.0f) **/	// Rotate continuously around the y-axis
		mat4f::scaling(1.5, 1.5, 1.5);				// Scale uniformly to 150%

	Mcube2 = Mcube * mat4f::translation(-3, 0, 0) *			// No translation
		mat4f::rotation(-angle, 0.0f, 1.0f, 0.0f) *	// Rotate continuously around the y-axis
		mat4f::scaling(0.7, 0.7, 0.7);				// Scale uniformly to 150%

	Mcube3 = Mcube2 * mat4f::translation(-1, 0, 0) *			// No translation
		mat4f::rotation(angle, 0.0f, 1.0f, 0.0f) *	// Rotate continuously around the y-axis
		mat4f::scaling(0.5, 0.5, 0.5);				// Scale uniformly to 150%


	// Sponza model-to-world transformation
	Msponza = mat4f::translation(0, -5, 0) *		 // Move down 5 units
		mat4f::rotation(fPI / 2, 0.0f, 1.0f, 0.0f) * // Rotate pi/2 radians (90 degrees) around y
		mat4f::scaling(1.0f);						 // The scene is quite large so scale it down to 5%



	Msphere = mat4f::translation(0, 0, 0) *			// No translation
		/*mat4f::rotation(-angle, 0.0f, 1.0f, 0.0f) **/	// Rotate continuously around the y-axis
		mat4f::scaling(1.5, 1.5, 1.5);				// Scale uniformly to 150%

	// Increment the rotation angle.
	angle += angle_vel * dt;

	// Print fps
	fps_cooldown -= dt;
	if (fps_cooldown < 0.0)
	{
		std::cout << "fps " << (int)(1.0f / dt) << std::endl;
		//		printf("fps %i\n", (int)(1.0f / dt));
		fps_cooldown = 2.0;
	}
}

//
// Called every frame, after update
//
void OurTestScene::Render()
{

	// Bind transformation_buffer to slot b0 of the VS
	dxdevice_context->VSSetConstantBuffers(0, 1, &transformation_buffer);

	// Bind lightCam to slot b0 of the PS
	dxdevice_context->PSSetConstantBuffers(0, 1, &lightCam_buffer);

	// Bind materialBuffer to slot b1 of the PS
	dxdevice_context->PSSetConstantBuffers(1, 1, &material_buffer);


	// Obtain the matrices needed for rendering from the camera
	Mview = camera->get_WorldToViewMatrix();
	Mproj = camera->get_ProjectionMatrix();

	UpdateLightCamBuffer(camera->position.xyz0(), light->position.xyz0());

	// Load matrices + the Quad's transformation to the device and render it
	//UpdateTransformationBuffer(Mquad, Mview, Mproj);
	//quad->Render();
	//UpdateTransformationBuffer(Mcube, Mview, Mproj);

	//
	//cube->Render();

	//UpdateTransformationBuffer(Mcube2, Mview, Mproj);
	//cube2->Render();

	//UpdateTransformationBuffer(Mcube3, Mview, Mproj);
	//cube3->Render();
	vec4f Ka = { sphere->Ka,0 };
	vec4f Kd = { sphere->Kd,0 };
	vec4f Ks = { sphere->Ks,0 };
	UpdateMaterialBuffer(Ka, Kd, Ks);
	UpdateTransformationBuffer(Msphere, Mview, Mproj);
	sphere->Render();
	vec4f Ka2 = { sponza->Ka,0 };
	vec4f Kd2 = { sponza->Kd,0 };
	vec4f Ks2 = { sponza->Ks,0 };
	UpdateMaterialBuffer(Ka2, Kd2, Ks2);
	// Load matrices + Sponza's transformation to the device and render it
	UpdateTransformationBuffer(Msponza, Mview, Mproj);
	sponza->Render();
}

void OurTestScene::Release()
{
	SAFE_DELETE(quad);
	SAFE_DELETE(cube);
	SAFE_DELETE(cube2);
	SAFE_DELETE(cube3);
	SAFE_DELETE(sponza);
	SAFE_DELETE(sphere);
	SAFE_DELETE(camera);
	SAFE_DELETE(light);

	SAFE_RELEASE(transformation_buffer);
	SAFE_RELEASE(lightCam_buffer);
	SAFE_RELEASE(material_buffer);
	// + release other CBuffers
}

void OurTestScene::WindowResize(
	int window_width,
	int window_height)
{
	if (camera)
		camera->aspect = float(window_width) / window_height;

	Scene::WindowResize(window_width, window_height);
}

void OurTestScene::InitTransformationBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC MatrixBuffer_desc = { 0 };
	MatrixBuffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	MatrixBuffer_desc.ByteWidth = sizeof(TransformationBuffer);
	MatrixBuffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	MatrixBuffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	MatrixBuffer_desc.MiscFlags = 0;
	MatrixBuffer_desc.StructureByteStride = 0;
	ASSERT(hr = dxdevice->CreateBuffer(&MatrixBuffer_desc, nullptr, &transformation_buffer));
}

void OurTestScene::InitLightCamBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC LightCamBuffer_desc = { 0 };
	LightCamBuffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	LightCamBuffer_desc.ByteWidth = sizeof(LightCamBuffer);
	LightCamBuffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	LightCamBuffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	LightCamBuffer_desc.MiscFlags = 0;
	LightCamBuffer_desc.StructureByteStride = 0;
	ASSERT(hr = dxdevice->CreateBuffer(&LightCamBuffer_desc, nullptr, &lightCam_buffer));
}

void OurTestScene::InitMaterialBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC MaterialBuffer_desc = { 0 };
	MaterialBuffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	MaterialBuffer_desc.ByteWidth = sizeof(MaterialBuffer);
	MaterialBuffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	MaterialBuffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	MaterialBuffer_desc.MiscFlags = 0;
	MaterialBuffer_desc.StructureByteStride = 0;
	ASSERT(hr = dxdevice->CreateBuffer(&MaterialBuffer_desc, nullptr, &material_buffer));
}

void OurTestScene::UpdateTransformationBuffer(
	mat4f ModelToWorldMatrix,
	mat4f WorldToViewMatrix,
	mat4f ProjectionMatrix)
{
	// Map the resource buffer, obtain a pointer and then write our matrices to it
	D3D11_MAPPED_SUBRESOURCE resource;
	dxdevice_context->Map(transformation_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	TransformationBuffer* matrix_buffer_ = (TransformationBuffer*)resource.pData;
	matrix_buffer_->ModelToWorldMatrix = ModelToWorldMatrix;
	matrix_buffer_->WorldToViewMatrix = WorldToViewMatrix;
	matrix_buffer_->ProjectionMatrix = ProjectionMatrix;
	dxdevice_context->Unmap(transformation_buffer, 0);
}


void OurTestScene::UpdateLightCamBuffer(
	vec4f cameraPos,
	vec4f lightPos)
{
	// Map the resource buffer, obtain a pointer and then write our matrices to it
	D3D11_MAPPED_SUBRESOURCE resource;
	dxdevice_context->Map(lightCam_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	LightCamBuffer* lightCam_buffer_ = (LightCamBuffer*)resource.pData;
	lightCam_buffer_->cameraPos = cameraPos;
	lightCam_buffer_->lightPos = lightPos;
	dxdevice_context->Unmap(lightCam_buffer, 0);
}

void OurTestScene::UpdateMaterialBuffer(vec4f Ka, vec4f Kd, vec4f Ks)
{
	// Map the resource buffer, obtain a pointer and then write our matrices to it
	D3D11_MAPPED_SUBRESOURCE resource;
	dxdevice_context->Map(material_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	MaterialBuffer* Material_buffer_ = (MaterialBuffer*)resource.pData;
	Material_buffer_->Ka = Ka;
	Material_buffer_->Kd = Kd;
	Material_buffer_->Ks = Ks;
	dxdevice_context->Unmap(material_buffer, 0);
}