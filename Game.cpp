#include "Game.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include <random>
#include <stdio.h>      
#include <stdlib.h>    
#include "Mesh.h"
#include "Material.h"

// This code assumes files are in "ImGui" subfolder!
// Adjust as necessary for your own folder structure and project setup
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"


// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>
#include "WICTextureLoader.h"

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// Direct3D itself, and our window, are not ready at this point!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,			// The application's handle
		L"DirectX Game",	// Text for the window's title bar (as a wide-character string)
		1280,				// Width of the window's client area
		720,				// Height of the window's client area
		false,				// Sync the framerate to the monitor refresh? (lock framerate)
		true),				// Show extra stats (fps) in title bar?
	color{ 0, 0, 0, 1 },
	hand(0),
	cpHand(0),
	tint{ 1.0f, 0.5f, 0.5f, 1.0f },
	offset{ 0,0,0 },
	ambientColor(0.1f, 0.1f, 0.1f)
{
#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif

	int number = 0;
	move = 1;
	bool show = false;

}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Delete all objects manually created within this class
//  - Release() all Direct3D objects created within this class
// --------------------------------------------------------
Game::~Game()
{
	// Call delete or delete[] on any objects or arrays you've
	// created using new or new[] within this class
	// - Note: this is unnecessary if using smart pointers

	// Call Release() on any Direct3D objects made within this class
	// - Note: this is unnecessary for D3D objects stored in ComPtrs

	// ImGui clean up
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

}

// --------------------------------------------------------
// Called once per program, after Direct3D and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Initialize ImGui itself & platform/renderer backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(device.Get(), context.Get());
	// Pick a style (uncomment one of these 3)
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	//ImGui::StyleColorsClassic();
	// 
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateGeometry();

	Light light1 = {};
	Light light2 = {};
	Light light3 = {};
	Light pointLight1 = {};
	Light pointLight2 = {};
	XMFLOAT3 directionalColor1 = XMFLOAT3(1, 1, 1);
	XMFLOAT3 directionalColor2 = XMFLOAT3(1, 1, 1);
	XMFLOAT3 directionalColor3 = XMFLOAT3(1, 1, 1);
	XMFLOAT3 pointColor1 = XMFLOAT3(1, 1, 1);
	XMFLOAT3 pointColor2 = XMFLOAT3(1, 1, 1);

	colors.push_back(directionalColor1);
	colors.push_back(directionalColor2);
	colors.push_back(directionalColor3);
	colors.push_back(pointColor1);
	colors.push_back(pointColor2);


	light1.Type = LIGHT_TYPE_DIRECTIONAL;
	light1.Direction = XMFLOAT3(1.0f, 0.0f, 0.0f);
	light1.Color = colors[0];
	light1.Intensity = 0.3f;

	light2.Type = LIGHT_TYPE_DIRECTIONAL;
	light2.Direction = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	light2.Color = colors[1];
	light2.Intensity = 0.3f;

	light3.Type = LIGHT_TYPE_DIRECTIONAL;
	light3.Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	light3.Color = colors[2];
	light3.Intensity = 0.3f;

	pointLight1.Type = LIGHT_TYPE_POINT;
	pointLight1.Position = XMFLOAT3(1.0, 0.0, -2.0);
	pointLight1.Color = colors[3];
	pointLight1.Intensity = 0.3f;
	pointLight1.Range = 5.0f;

	pointLight2.Type = LIGHT_TYPE_POINT;
	pointLight2.Position = XMFLOAT3(-2.0, 0.0, 0.0);
	pointLight2.Color = colors[4];
	pointLight2.Intensity = 0.3f;
	pointLight2.Range = 3.0f;

	lights.push_back(light1);
	lights.push_back(light2);
	lights.push_back(light3);
	lights.push_back(pointLight1);
	lights.push_back(pointLight2);

	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Ensure the pipeline knows how to interpret all the numbers stored in
		// the vertex buffer. For this course, all of your vertices will probably
		// have the same layout, so we can just set this once at startup.

		// Set the active vertex and pixel shaders
		//  - Once you start applying different shaders to different objects,
		//    these calls will need to happen multiple times per frame
	}



	//unsigned int size = sizeof(BufferStruct);
	//size = (size + 15) / 16 * 16;
	//D3D11_BUFFER_DESC cbDesc = {};
	//cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//cbDesc.ByteWidth = size;
	//cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//cbDesc.Usage = D3D11_USAGE_DYNAMIC;

	cam.push_back(std::make_shared<Camera>(DirectX::XMFLOAT3(0.0f, 0.0f, -5.0f), (float)this->windowWidth / this->windowHeight, 100.0f, 0.1f, 1000.0f));
	cam.push_back(std::make_shared<Camera>(DirectX::XMFLOAT3(-2.0f, 0.0f, -2.0f), ((float)this->windowWidth / this->windowHeight) / 2.0f, 60.0f, 0.1f, 1000.0f));
	cam.push_back(std::make_shared<Camera>(DirectX::XMFLOAT3(3.0f, 0.0f, -3.0f), ((float)this->windowWidth / this->windowHeight) / 3.0f, 150.0f, 0.1f, 1000.0f));
	activeCam = cam[0];
}

void Game::Helper(float deltaTime) {
	// Put this all in a helper method that is called from Game::Update()
// Feed fresh data to ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)this->windowWidth;
	io.DisplaySize.y = (float)this->windowHeight;
	// Reset the frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// Determine new input capture
	Input& input = Input::GetInstance();
	input.SetKeyboardCapture(io.WantCaptureKeyboard);
	input.SetMouseCapture(io.WantCaptureMouse);
	// Show the demo window
	//ImGui::ShowDemoWindow();

}

void Game::BuildUI()
{
	ImGui::Begin("Some Stats");

	if (show) {
		ImGui::ShowDemoWindow();
	}


	// Framerate
	ImGui::Text("Framerate: %f fps", ImGui::GetIO().Framerate);

	// Dimensions
	ImGui::Text("Window Resolution: %dx%d", windowWidth, windowHeight);

	// Color Picker
	ImGui::ColorEdit4("RGBA color editor", color);
	if (ImGui::Button("Show ImGui Demo Window"))
	{
		show = !show;
	}
	//ImGui::SliderFloat3("Offset", &offset[0], 0, 1, 0);
	ImGui::ColorEdit4("Tint", tint);


	if (ImGui::TreeNode("Entities")) {
		EntitiesUI(entities);
		ImGui::TreePop();
	}

	CamerasUI();


	if (ImGui::TreeNode("Lights")) {
		LightsUI();
		ImGui::TreePop();
	}
	ImGui::End();
}

void Game::EntitiesUI(std::vector<std::shared_ptr<GameEntity>> entities)
{
	for (int i = 0; i < entities.size(); i++) {
		ImGui::PushID(i);
		if (ImGui::TreeNode("Entity %d", "Entity %d:", i)) {
			Transforms* transform = entities[i]->GetTransform();
			XMFLOAT3 pos = transform->GetPosition();
			XMFLOAT3 rot = transform->GetPitchYawRoll();
			XMFLOAT3 scale = transform->GetScale();

			if (ImGui::DragFloat3("Position", &pos.x, 0.1f)) {
				transform->SetPosition(pos.x, pos.y, pos.z);
			}
			if (ImGui::DragFloat3("Rotation (Radians)", &rot.x, 0.1f)) {
				transform->SetRotation(rot.x, rot.y, rot.z);
			}
			if (ImGui::DragFloat3("Scale", &scale.x, 0.1f)) {
				transform->SetScale(scale.x, scale.y, scale.z);
			}
			ImGui::TreePop();
		}
		ImGui::PopID();

	}
}

void Game::CamerasUI()
{
	if (ImGui::Button("Camera 1")) {
		activeCam = cam[0];
	}
	if (ImGui::Button("Camera 2")) {
		activeCam = cam[1];
	}
	if (ImGui::Button("Camera 3")) {
		activeCam = cam[2];
	}
	if (ImGui::TreeNode("Active Camera", "Active Camera")) {
		DirectX::XMFLOAT3 pos = activeCam->GetTransform()->GetPosition();
		float fov = activeCam->GetFieldOfView();
		ImGui::Text("X Position: %f", pos.x);
		ImGui::Text("Y Position: %f", pos.y);
		ImGui::Text("Z Position: %f", pos.z);

		ImGui::Text("Field Of View: %f", fov);
		ImGui::TreePop();
	}




}

void Game::LightsUI() {

	for (int i = 0; i < lights.size(); i++) {
		ImGui::PushID(i);
		if (ImGui::TreeNode("Light", "Light %i", i)) {
			if (lights[i].Type == 0) {
				ImGui::Text("Directional Light");
				ImGui::Spacing();
				ImGui::DragFloat3("Direction", &lights[i].Direction.x, 0.1f);
				ImGui::Spacing();
			}
			else if (lights[i].Type == 1) {
				ImGui::Text("Point Light");
				ImGui::Spacing();
				ImGui::DragFloat3("Position", &lights[i].Position.x, 0.1f);
				ImGui::Spacing();
				ImGui::SliderFloat("Range", &lights[i].Range, 0, 10);
				ImGui::Spacing();
			}
			ImGui::SliderFloat("Intensity", &lights[i].Intensity, 0, 1);
			ImGui::Spacing();

			ImGui::ColorEdit3("Color", &lights[i].Color.x);


			ImGui::TreePop();
		}

		ImGui::PopID();
	}
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = std::make_shared<SimpleVertexShader>(device, context,
		FixPath(L"VertexShader.cso").c_str());
	pixelShader = std::make_shared<SimplePixelShader>(device, context,
		FixPath(L"PixelShader.cso").c_str());
	pixelShaderNew = std::make_shared<SimplePixelShader>(device, context,
		FixPath(L"PixelShaderNew.cso").c_str());

}



// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateGeometry()
{
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
	D3D11_SAMPLER_DESC sampDesc = {};

	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&sampDesc, sampler.GetAddressOf());

	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT3 red = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3 green = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 blue = XMFLOAT3(0.0f, 0.0f, 1.0f);

	// Set up the vertices of the triangle we would like to draw
	// - We're going to copy this array, exactly as it exists in CPU memory
	//    over to a Direct3D-controlled data structure on the GPU (the vertex buffer)
	// - Note: Since we don't have a camera or really any concept of
	//    a "3d world" yet, we're simply describing positions within the
	//    bounds of how the rasterizer sees our screen: [-1 to +1] on X and Y
	// - This means (0,0) is at the very center of the screen.
	// - These are known as "Normalized Device Coordinates" or "Homogeneous 
	//    Screen Coords", which are ways to describe a position without
	//    knowing the exact size (in pixels) of the image/window/etc.  
	// - Long story short: Resizing the window also resizes the triangle,
	//    since we're describing the triangle in terms of the window itself
	Vertex vertices[] =
	{

		{ XMFLOAT3(+0.0f, +0.5f, +0.0f), red },
		{ XMFLOAT3(+0.5f, -0.5f, +0.0f), blue },
		{ XMFLOAT3(-0.5f, -0.5f, +0.0f), green },
	};

	// Set up indices, which tell us which vertices to use and in which order
	// - This is redundant for just 3 vertices, but will be more useful later
	// - Indices are technically not required if the vertices are in the buffer 
	//    in the correct order and each one will be used exactly once
	// - But just to see how it's done...
	unsigned int indices[] = { 0, 1, 2 };

	{
		// Create a VERTEX BUFFER
	// - This holds the vertex data of triangles for a single object
	// - This buffer is created on the GPU, which is where the data needs to
	//    be if we want the GPU to act on it (as in: draw it to the screen)
	//{
	//	// First, we need to describe the buffer we want Direct3D to make on the GPU
	//	//  - Note that this variable is created on the stack since we only need it once
	//	//  - After the buffer is created, this description variable is unnecessary
	//	D3D11_BUFFER_DESC vbd	= {};
	//	vbd.Usage				= D3D11_USAGE_IMMUTABLE;	// Will NEVER change
	//	vbd.ByteWidth			= sizeof(Vertex) * 3;       // 3 = number of vertices in the buffer
	//	vbd.BindFlags			= D3D11_BIND_VERTEX_BUFFER; // Tells Direct3D this is a vertex buffer
	//	vbd.CPUAccessFlags		= 0;	// Note: We cannot access the data from C++ (this is good)
	//	vbd.MiscFlags			= 0;
	//	vbd.StructureByteStride = 0;

	//	// Create the proper struct to hold the initial vertex data
	//	// - This is how we initially fill the buffer with data
	//	// - Essentially, we're specifying a pointer to the data to copy
	//	D3D11_SUBRESOURCE_DATA initialVertexData = {};
	//	initialVertexData.pSysMem = vertices; // pSysMem = Pointer to System Memory

	//	// Actually create the buffer on the GPU with the initial data
	//	// - Once we do this, we'll NEVER CHANGE DATA IN THE BUFFER AGAIN
	//	device->CreateBuffer(&vbd, &initialVertexData, vertexBuffer.GetAddressOf());
	//}

	// Create an INDEX BUFFER
	// - This holds indices to elements in the vertex buffer
	// - This is most useful when vertices are shared among neighboring triangles
	// - This buffer is created on the GPU, which is where the data needs to
	//    be if we want the GPU to act on it (as in: draw it to the screen)
	//{
	//	// Describe the buffer, as we did above, with two major differences
	//	//  - Byte Width (3 unsigned integers vs. 3 whole vertices)
	//	//  - Bind Flag (used as an index buffer instead of a vertex buffer) 
	//	D3D11_BUFFER_DESC ibd	= {};
	//	ibd.Usage				= D3D11_USAGE_IMMUTABLE;	// Will NEVER change
	//	ibd.ByteWidth			= sizeof(unsigned int) * 3;	// 3 = number of indices in the buffer
	//	ibd.BindFlags			= D3D11_BIND_INDEX_BUFFER;	// Tells Direct3D this is an index buffer
	//	ibd.CPUAccessFlags		= 0;	// Note: We cannot access the data from C++ (this is good)
	//	ibd.MiscFlags			= 0;
	//	ibd.StructureByteStride = 0;

	//	// Specify the initial data for this buffer, similar to above
	//	D3D11_SUBRESOURCE_DATA initialIndexData = {};
	//	initialIndexData.pSysMem = indices; // pSysMem = Pointer to System Memory

	//	// Actually create the buffer with the initial data
	//	// - Once we do this, we'll NEVER CHANGE THE BUFFER AGAIN
	//	device->CreateBuffer(&ibd, &initialIndexData, indexBuffer.GetAddressOf());
	//}

	}

	// Shape #1
	Vertex vert1[] =
	{
		//					Position || Color
		{ XMFLOAT3(+0.4f, +0.3f, +0.0f), green },
		{ XMFLOAT3(+0.6f, +0.0f, +0.0f), blue },
		{ XMFLOAT3(+0.3f, +0.0f, +0.0f), red },
	};

	unsigned int indices1[] = { 0, 1, 2 };

	// Shape #2
	Vertex vert2[] = {
		{XMFLOAT3(-0.3f, -0.3f, +0.0f), red}, //Bottom Left
		{XMFLOAT3(-0.3f, +0.0f, +0.0f), blue}, //Top Left
		{XMFLOAT3(+0.0f, +0.0f, +0.0f), green}, // Top RIght
		{XMFLOAT3(+0.0f, -0.3f, +0.0f), blue} //Bottom Right
	};

	unsigned int indices2[] = { 0, 1, 2, 0, 2, 3 };


	// Shape #3
	Vertex vert3[] = {
		{XMFLOAT3(+0.65f, +0.65f, +0.0f), blue}, //0
		{XMFLOAT3(+0.7f, +0.7f, +0.0f), green}, //1
		{XMFLOAT3(+0.75f, +0.65f, +0.0f), blue}, //2
		{XMFLOAT3(+0.8f, +0.7f, +0.0f), red}, //3
		{XMFLOAT3(+0.85f, +0.65f, +0.0f), green}, //4
		{XMFLOAT3(+0.75f, +0.55f, +0.0f), red}, //5
	};

	unsigned int indices3[] = { 0, 1, 2, 2, 3, 4, 0 , 4, 5 };

	Vertex vert4[] = {
		{XMFLOAT3(+0.65f, -0.65f, +0.0f), blue}, //0
		{XMFLOAT3(+0.7f, -0.6f, +0.0f), green}, //1
		{XMFLOAT3(+0.75f, -0.65f, +0.0f), blue}, //2
		{XMFLOAT3(+0.675f, -0.65f, +0.0f), red}, //3
		{XMFLOAT3(+0.725f, -0.65f, +0.0f), red}, //4
		{XMFLOAT3(+0.675f, -0.75f, +0.0f), green}, //5
		{XMFLOAT3(+0.725f, -0.75f, +0.0f), green}, //6
	};

	// TEXTURES
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rustyMetalSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rustyMetalSpecularSRV;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> tilesSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> tilesSpecularSRV;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> brokenTilesSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> brokenTilesSpecularSRV;

	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/rustymetal.png").c_str(), 0, rustyMetalSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/rustymetal_specular.png").c_str(), 0, rustyMetalSpecularSRV.GetAddressOf());
	
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/tiles.png").c_str(), 0, tilesSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/tiles_specular.png").c_str(), 0, tilesSpecularSRV.GetAddressOf());
	
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/brokentiles.png").c_str(), 0, brokenTilesSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/brokentiles_specular.png").c_str(), 0, brokenTilesSpecularSRV.GetAddressOf());

	unsigned int indices4[] = { 0, 1, 2, 3, 4, 5, 4 , 6, 5 };
	std::shared_ptr<Mesh> cube = std::make_shared<Mesh>(FixPath("../../Assets/Models/cube.obj").c_str(), device);
	std::shared_ptr<Mesh> cylinder = std::make_shared<Mesh>(FixPath("../../Assets/Models/cylinder.obj").c_str(), device);
	std::shared_ptr<Mesh> helix = std::make_shared<Mesh>(FixPath("../../Assets/Models/helix.obj").c_str(), device);
	std::shared_ptr<Mesh> sphere = std::make_shared<Mesh>(FixPath("../../Assets/Models/sphere.obj").c_str(), device);
	
	// Set Materials
	std::shared_ptr<Material> material1 = std::make_shared<Material>(XMFLOAT3(1.0f, 1.0f, 1.0f), vertexShader, pixelShader, 0.0f, 1.0f);
	std::shared_ptr<Material> material2 = std::make_shared<Material>(XMFLOAT3(0.5f, 0.5f, 0.5f), vertexShader, pixelShader, 0.5f, 1.0f);
	std::shared_ptr<Material> material3 = std::make_shared<Material>(XMFLOAT3(1.0f, 1.0f, 1.0f), vertexShader, pixelShader, 0.0f, 1.0f);
	std::shared_ptr<Material> material4 = std::make_shared<Material>(XMFLOAT3(1.0f, 1.0f, 1.0f), vertexShader, pixelShader, 0.0f, 1.0f);
	
	std::shared_ptr<Material> metalMat = std::make_shared<Material>(XMFLOAT3(1.0f, 1.0f, 1.0f), vertexShader, pixelShader, 0.0f, 10.0f);
	
	metalMat->SetSampler("BasicSampler", sampler);
	metalMat->SetTextureSRV("SurfaceTexture", rustyMetalSRV);
	metalMat->SetTextureSRV("SpecularMap", rustyMetalSpecularSRV);

	std::shared_ptr<Material> tileMat = std::make_shared<Material>(XMFLOAT3(1.0f, 1.0f, 1.0f), vertexShader, pixelShader, 0.0f, 5.0f);

	tileMat->SetSampler("BasicSampler", sampler);
	tileMat->SetTextureSRV("SurfaceTexture", tilesSRV);
	tileMat->SetTextureSRV("SpecularMap", tilesSpecularSRV);

	std::shared_ptr<Material> brokenTileMat = std::make_shared<Material>(XMFLOAT3(1.0f, 1.0f, 1.0f), vertexShader, pixelShader, 0.0f, 7.0f);

	brokenTileMat->SetSampler("BasicSampler", sampler);
	brokenTileMat->SetTextureSRV("SurfaceTexture", brokenTilesSRV);
	brokenTileMat->SetTextureSRV("SpecularMap", brokenTilesSpecularSRV);

	materials.push_back(material1);
	materials.push_back(material2);
	materials.push_back(material3);
	materials.push_back(material4);
	materials.push_back(metalMat);
	materials.push_back(tileMat);
	materials.push_back(brokenTileMat);
	//Entities list

	std::shared_ptr<GameEntity> entityOne = std::make_shared<GameEntity>(cube, tileMat);
	std::shared_ptr<GameEntity> entityTwo = std::make_shared<GameEntity>(cylinder, brokenTileMat);
	std::shared_ptr<GameEntity> entityThree = std::make_shared<GameEntity>(helix, brokenTileMat);
	std::shared_ptr<GameEntity> entityFour = std::make_shared<GameEntity>(sphere, metalMat);
	
	//std::shared_ptr<GameEntity> entityFive = std::make_shared<GameEntity>(square, material3);

	//entityOne->GetTransform()->Rotate(0, 0, .3f);
	//entityTwo->GetTransform()->MoveAbsolute(0.0, 0, 0);
	entityThree->GetTransform()->MoveAbsolute(-4.0f, 0, 0);
	entityTwo->GetTransform()->MoveAbsolute(4.0f, 0, 0);
	entityFour->GetTransform()->MoveAbsolute(8.0f, 0.0f, 0);



	entities.push_back(entityOne);
	entities.push_back(entityTwo);
	entities.push_back(entityThree);
	entities.push_back(entityFour);
}


// --------------------------------------------------------
// Handle resizing to match the new window size.
//  - DXCore needs to resize the back buffer
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();
	for (int i = 0; i < cam.size(); i++) {
		cam[i]->UpdateProjectionMatrix((float)this->windowWidth / (float)this->windowHeight);
	}

}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	Helper(deltaTime);
	BuildUI();
	activeCam->Update(deltaTime);
	entities[0]->GetMaterial()->GetPixelShader()->SetFloat("time", totalTime);
	entities[0]->GetMaterial()->GetPixelShader()->SetFloat2("resolution", XMFLOAT2(windowWidth, windowHeight));

	/*for (int i = 0; i < 2; i++) {
		if (entities[i]->GetTransform()->GetPosition().x > 1.0f) {
			move = -1;
		}
		if (entities[i]->GetTransform()->GetPosition().x < -1.0f) {
			move = 1;
		}
		entities[i]->GetTransform()->MoveAbsolute(deltaTime * move, 0, 0);
		entities[i]->GetTransform()->Rotate(0, 0, deltaTime * 1.0f);
	}

	for (int i = 2; i < entities.size()-1; i++) {
		if (entities[i]->GetTransform()->GetPosition().y > 1.0f) {
			move = -1;
		}
		if (entities[i]->GetTransform()->GetPosition().y < -1.0f) {
			move = 1;
		}

		entities[i]->GetTransform()->MoveAbsolute(0, deltaTime * move, 0);
		entities[i]->GetTransform()->Rotate(0, 0, deltaTime * 1.0f);
	}
	float scale = (float)cos(totalTime * 2.0f) * 1.0f + 2.0f;



	entities[4]->GetTransform()->SetScale(scale,scale,scale);
	entities[4]->GetTransform()->Rotate(0, 0, deltaTime * 1.0f);*/

	// Example input checking: Quit if the escape key is pressed
	if (Input::GetInstance().KeyDown(VK_ESCAPE))
		Quit();
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erases what's on the screen)
		float bgColor[4] = { color[0], color[1], color[2], color[3] }; // Cornflower Blue

		context->ClearRenderTargetView(backBufferRTV.Get(), bgColor);


		// Clear the depth buffer (resets per-pixel occlusion information)
		context->ClearDepthStencilView(depthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	for (int i = 0; i < entities.size(); i++) {
		entities[i]->GetMaterial()->GetPixelShader()->SetFloat3("ambient", ambientColor);
		entities[i]->GetMaterial()->GetPixelShader()->SetData("lights", &lights[0], sizeof(Light) * (int)lights.size());
		entities[i]->Draw(context, activeCam);
	}

	//entities[0]->Draw(context, constBuffer);

	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{

		ImGui::Render(); // Turns this frame’s UI into renderable triangles
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // Draws it to the screen

		// Present the back buffer to the user
		//  - Puts the results of what we've drawn onto the window
		//  - Without this, the user never sees anything
		bool vsyncNecessary = vsync || !deviceSupportsTearing || isFullscreen;



		swapChain->Present(
			vsyncNecessary ? 1 : 0,
			vsyncNecessary ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Must re-bind buffers after presenting, as they become unbound
		context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthBufferDSV.Get());
	}
}