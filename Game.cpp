#include "Game.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include <random>
#include <stdio.h>      
#include <stdlib.h>    
#include "Mesh.h"
// This code assumes files are in "ImGui" subfolder!
// Adjust as necessary for your own folder structure and project setup
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"


// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

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
	tint{1.0f, 0.5f, 0.5f, 1.0f},
	offset{0,0,0}
{
#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif

	//color[4] = {};
	int number = 0;
	move = 1;
	bool show = false;

	std::shared_ptr<Mesh> triangle;
	std::shared_ptr<Mesh> square;
	std::shared_ptr<Mesh> polygon;
	std::shared_ptr<Mesh> house;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constBuffer;

	
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
		context->IASetInputLayout(inputLayout.Get());

		// Set the active vertex and pixel shaders
		//  - Once you start applying different shaders to different objects,
		//    these calls will need to happen multiple times per frame
		context->VSSetShader(vertexShader.Get(), 0, 0);
		context->PSSetShader(pixelShader.Get(), 0, 0);
	}
	
	

	unsigned int size = sizeof(BufferStruct);
	size = (size + 15) / 16 * 16;
	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.ByteWidth = size;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	device->CreateBuffer(&cbDesc, 0, constBuffer.GetAddressOf());

	cam.push_back(std::make_shared<Camera>(DirectX::XMFLOAT3(0, 0, -5), (float)this->windowWidth / this->windowHeight, 0.1, 1000));
	cam.push_back(std::make_shared<Camera>(DirectX::XMFLOAT3(-2, 0, -2), ((float)this->windowWidth / this->windowHeight)/2, 0.1, 1000));
	cam.push_back(std::make_shared<Camera>(DirectX::XMFLOAT3(3, 0, -3), ((float)this->windowWidth / this->windowHeight)/3, 0.1, 1000));
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

	XMFLOAT4 bColor(color[0], color[1], color[2], color[3]);
	// Color Picker
	ImGui::ColorEdit4("RGBA color editor", color);
	if (ImGui::Button("Show ImGui Demo Window")) 
	{
		show = !show;
	}
	//ImGui::SliderFloat3("Offset", &offset[0], 0, 1, 0);
	ImGui::ColorEdit4("Tint", tint);

	if (ImGui::TreeNode("Meshes")) {
		ImGui::Text("Mesh 1: %i triangle(s)", square->GetIndexCount() / 3);
		ImGui::Text("Mesh 2: %i triangle(s)", polygon->GetIndexCount() / 3);
		ImGui::Text("Mesh 3: %i triangle(s)", house->GetIndexCount() / 3);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Entities")) {
		EntitiesUI(entities);
		ImGui::TreePop();
	}

	CamerasUI(activeCam);

	
	

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

void Game::CamerasUI(std::shared_ptr<Camera>activeCam)
{
	if (ImGui::TreeNode("Active Camera", "Active Camera:")) {
		DirectX::XMFLOAT3 pos = activeCam->GetTransform()->GetPosition();
		ImGui::Text("Position: %i", pos.x);
		ImGui::TreePop();
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
	// BLOBs (or Binary Large OBjects) for reading raw data from external files
	// - This is a simplified way of handling big chunks of external data
	// - Literally just a big array of bytes read from a file
	ID3DBlob* pixelShaderBlob;
	ID3DBlob* vertexShaderBlob;

	// Loading shaders
	//  - Visual Studio will compile our shaders at build time
	//  - They are saved as .cso (Compiled Shader Object) files
	//  - We need to load them when the application starts
	{
		// Read our compiled shader code files into blobs
		// - Essentially just "open the file and plop its contents here"
		// - Uses the custom FixPath() helper from Helpers.h to ensure relative paths
		// - Note the "L" before the string - this tells the compiler the string uses wide characters
		D3DReadFileToBlob(FixPath(L"PixelShader.cso").c_str(), &pixelShaderBlob);
		D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(), &vertexShaderBlob);

		// Create the actual Direct3D shaders on the GPU
		device->CreatePixelShader(
			pixelShaderBlob->GetBufferPointer(),	// Pointer to blob's contents
			pixelShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			pixelShader.GetAddressOf());			// Address of the ID3D11PixelShader pointer

		device->CreateVertexShader(
			vertexShaderBlob->GetBufferPointer(),	// Get a pointer to the blob's contents
			vertexShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			vertexShader.GetAddressOf());			// The address of the ID3D11VertexShader pointer
	}

	// Create an input layout 
	//  - This describes the layout of data sent to a vertex shader
	//  - In other words, it describes how to interpret data (numbers) in a vertex buffer
	//  - Doing this NOW because it requires a vertex shader's byte code to verify against!
	//  - Luckily, we already have that loaded (the vertex shader blob above)
	{
		D3D11_INPUT_ELEMENT_DESC inputElements[2] = {};

		// Set up the first element - a position, which is 3 float values
		inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
		inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
		inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element

		// Set up the second element - a color, which is 4 more float values
		inputElements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// 4x 32-bit floats
		inputElements[1].SemanticName = "COLOR";							// Match our vertex shader input!
		inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

		// Create the input layout, verifying our description against actual shader code
		device->CreateInputLayout(
			inputElements,							// An array of descriptions
			2,										// How many elements in that array?
			vertexShaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
			vertexShaderBlob->GetBufferSize(),		// Size of the shader code that uses this layout
			inputLayout.GetAddressOf());			// Address of the resulting ID3D11InputLayout pointer
	}


}



// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateGeometry()
{


	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red	= XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green	= XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue	= XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

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

	unsigned int indices4[] = { 0, 1, 2, 3, 4, 5, 4 , 6, 5 };
	

	triangle = std::make_shared<Mesh>(vert1, indices1, 3, 3, device);
	square = std::make_shared<Mesh>(vert2, indices2, 6, 4, device);
	polygon = std::make_shared<Mesh>(vert3, indices3, 9, 6, device);
	house = std::make_shared<Mesh>(vert4, indices4, 9, 7, device);

	meshes.push_back(triangle);
	meshes.push_back(square);
	meshes.push_back(polygon);
	meshes.push_back(house);
	

	//Entities list

	std::shared_ptr<GameEntity> entityOne = std::make_shared<GameEntity>(triangle);
	std::shared_ptr<GameEntity> entityTwo = std::make_shared<GameEntity>(square);
	std::shared_ptr<GameEntity> entityThree = std::make_shared<GameEntity>(polygon);
	std::shared_ptr<GameEntity> entityFour = std::make_shared<GameEntity>(triangle);
	std::shared_ptr<GameEntity> entityFive = std::make_shared<GameEntity>(square);

	entityOne->GetTransform()->Rotate(0, 0, .3f);
	entityTwo->GetTransform()->MoveAbsolute(0.0, 0, 0);
	entityThree->GetTransform()->MoveAbsolute(-0.7, 0, 0);
	//entityFour->GetTransform()->MoveAbsolute(-0.7, 0, 0);

	entities.push_back(entityOne);
	entities.push_back(entityTwo);
	entities.push_back(entityThree);
	entities.push_back(entityFour);
	entities.push_back(entityFive);
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
	activeCam->UpdateProjectionMatrix((float)this->windowWidth / this->windowHeight);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	Helper(deltaTime);
	BuildUI();
	activeCam->Update(deltaTime);
	
	for (int i = 0; i < 2; i++) {
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
	float scale = (float)cos(totalTime * 2) * 1.0f + 2;

	
	
	entities[4]->GetTransform()->SetScale(scale,scale,scale);
	entities[4]->GetTransform()->Rotate(0, 0, deltaTime * 1.0f);
	
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
		entities[i]->Draw(context, constBuffer, activeCam);
	}

	//entities[0]->Draw(context, constBuffer);

	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{

		ImGui::Render(); // Turns this frame�s UI into renderable triangles
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