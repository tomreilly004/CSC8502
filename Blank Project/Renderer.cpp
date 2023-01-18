# include "Renderer.h";
# include "../nclgl/HeightMap.h";
# include "../nclgl/MeshAnimation.h"
# include "../nclgl/MeshMaterial.h"
# include < algorithm >

# define SHADOWSIZE 2048

using namespace std;

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {

	root = new SceneNode();
	quad = Mesh::GenerateQuad();
	skyboxNode = new SceneNode(quad);
	enableAutomatic = false;

	timePassed = 0;
	waterRotate = 0.0f;
	waterCycle = 0.0f;
	
	heightMap = new HeightMap(TEXTUREDIR "newHeighmap.png");
	Vector3 heightmapSize = heightMap->GetHeightmapSize();
	mapNode = new SceneNode(heightMap);
	root->AddChild(mapNode);
	root->AddChild(skyboxNode);

	grassShader = new Shader("GrassVertex.glsl", "GrassFragment.glsl", "GrassGeometry.glsl");
	TerrainShader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");
	reflectShader = new Shader("reflectVertex.glsl", "reflectFragment.glsl");
	skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	animationShader = new Shader("SkinningVertex.glsl", "skinningFragment.glsl");
	staticMeshShader = new Shader("staticVertex.glsl", "staticFragment.glsl");
	shadowShader = new Shader("shadowVert.glsl", "shadowFrag.glsl");
	reflectShader = new Shader("reflectVertex.glsl", "reflectFragment.glsl");
	skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");

	if (!grassShader->LoadSuccess() || !TerrainShader->LoadSuccess() 
		|| !reflectShader->LoadSuccess() || !skyboxShader->LoadSuccess()) {
		return;
	}

	blendMap = SOIL_load_OGL_texture(TEXTUREDIR "blendMap.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	grassTex = SOIL_load_OGL_texture(TEXTUREDIR "grass.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	snowTex = SOIL_load_OGL_texture(TEXTUREDIR "snow.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	rockTex = SOIL_load_OGL_texture(TEXTUREDIR "mountainRock.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	grassBump = SOIL_load_OGL_texture(TEXTUREDIR "grassBumpMap.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	snowBump = SOIL_load_OGL_texture(TEXTUREDIR "snowBumpMap.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	rockBump = SOIL_load_OGL_texture(TEXTUREDIR "MountainRockBumpMap.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	waterTex = SOIL_load_OGL_texture(TEXTUREDIR "water.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR "rusted_west.jpg", TEXTUREDIR "rusted_east.jpg", TEXTUREDIR "rusted_up.jpg", TEXTUREDIR "rusted_down.jpg", TEXTUREDIR "rusted_south.jpg", TEXTUREDIR "rusted_north.jpg", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);


	if (!grassTex || !rockTex || !snowTex || !blendMap || !waterTex || !cubeMap) {
		return;
	}

	SetTextureRepeating(grassTex, true);
	SetTextureRepeating(rockTex, true);
	SetTextureRepeating(snowTex, true);
	SetTextureRepeating(waterTex, true);
	SetTextureRepeating(blendMap, true);



	camera = new Camera(-45.0f, 0.0f, heightmapSize * Vector3(0.5f, 0.5f, 0.5f));
	light = new Light(Vector3(2900.0f, 950, 2900.0f), Vector4(0.7, 0.7, 1, 1), 20);
	sauronLight = new Spotlight(Vector3(2500.0f, 950, 2000.0f), Vector4(1, 0.5, 0, 0), 5, Vector3(0,-0.45,0), 5.0f);
	camera->SetPosition(light->GetPosition());

	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	animRoot = new SceneNode();
	staticRoot = new SceneNode();

	sauronEye = Mesh::LoadFromMeshFile("Sphere.msh");
	sauronMat = new MeshMaterial("Sphere.mat");
	sauronNode = new StaticMeshNode(staticMeshShader, sauronEye, sauronMat);
	sauronNode->SetTransform(Matrix4::Translation(Vector3(2610.0f, 680, 2378.0f)));
	sauronNode->SetTransform(sauronNode->GetTransform() * Matrix4::Rotation(180, Vector3(0, 1, 0)));
	sauronNode->SetTransform(sauronNode->GetTransform() * Matrix4::Scale(Vector3(30, 30, 30)));
	staticRoot->AddChild(sauronNode);

	spiderMesh = Mesh::LoadFromMeshFile("Black Widow.msh");
	spiderAnim = new MeshAnimation("Black Widow.anm");
	spiderMat = new MeshMaterial("Black Widow.mat");

	treeMesh = Mesh::LoadFromMeshFile("tree1.msh");
	treeMat = new MeshMaterial("tree1.mat");

	for (int j = 0; j < 5; ++j)
	{
		spiderNode = new AnimMeshNode(animationShader, spiderMesh, spiderAnim, spiderMat);
		spiderNode->SetTransform(Matrix4::Translation(Vector3((100 * j + rand() % 15) + 1250.0f, -30, (1750))));
		spiderNode->SetTransform(spiderNode->GetTransform() * Matrix4::Scale(Vector3(12, 12, 12)));
		spiderNode->SetTransform(spiderNode->GetTransform() * Matrix4::Rotation(180, Vector3(0, 1, 0)));
		animRoot->AddChild(spiderNode);
	}
	for (int i = 0; i < 5; ++i)
	{
		for (int j = 0; j < 6; ++j) {
			treeNode = new StaticMeshNode(staticMeshShader, treeMesh, treeMat);
			treeNode->SetTransform(Matrix4::Translation(Vector3( (100* i + rand()%100) + 1200.0f, -550, (100 * j + rand() % 100) -50 )));
			treeNode->SetTransform(treeNode->GetTransform() * Matrix4::Scale(Vector3(12, 12, 12)));
			staticRoot->AddChild(treeNode);
		}
	}
	for (int i = 0; i < 5; ++i)
	{
		for (int j = 0; j < 5; ++j) {
			treeNode = new StaticMeshNode(staticMeshShader, treeMesh, treeMat);
			treeNode->SetTransform(Matrix4::Translation(Vector3((100 * i + 1 + rand() % 100 ) + 1200.0f, -550, (100 * j + rand() % 100) + 600)));
			treeNode->SetTransform(treeNode->GetTransform() * Matrix4::Scale(Vector3(12, 12, 12)));
			staticRoot->AddChild(treeNode);
		}
	}
	root->AddChild(animRoot);
	root->AddChild(staticRoot);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	cameraMoving = false;

	futurePosition1 = Vector3(sauronNode->GetTransform().GetPositionVector() - Vector3(100,-200,1500));
	futurePosition2 = Vector3(1550.0f, 50, 1750);
	futurePosition3 = Vector3(heightMap->GetHeightmapSize() * 0.5 + Vector3(35.0f, -400.0f, -1100.0f));
	pitch1 = -35.0;
	pitch2 = 5.0;
	pitch3 = -40.0;
	yaw1 = 120.0;
	yaw2 = 15.0;
	yaw3 = 5.0;

	counter = 1;
	init = true;

}

Renderer ::~Renderer(void) {
	delete heightMap;
	delete camera;
	delete grassShader;
	delete TerrainShader;
	delete quad;
	delete reflectShader;
	delete skyboxShader;
	delete light;
	delete animRoot;
	delete staticRoot;
	delete sauronLight;

	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);
	delete shadowShader;
}

void Renderer::UpdateScene(float dt) {
	timePassed += dt;
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	waterRotate += dt * 2.0f; 
	waterCycle += dt * 0.25f; 
	staticRoot->Update(dt);
	animRoot->Update(dt);
	sauronLight->SetDirection(Vector3(sin(timePassed)/4 - 1, sin(-0.45), sin(timePassed/5)/4 ));
	if ((int)timePassed % 12 == 0) { counter = counter * -1; }
	sauronNode->SetTransform(sauronNode->GetTransform() * 
		Matrix4::Rotation(0.5, Vector3(sin(-0.45), counter * sin(timePassed) / 4, sin(timePassed/10) / 4)));
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1) || enableAutomatic == true) {
		enableAutomatic = true;
		CameraTrack(dt, futurePosition1, pitch1, yaw1, 500);
	}	
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2) || enableAutomatic == true) {
		enableAutomatic = true;
		CameraTrack(dt, futurePosition2, pitch2, yaw2, 500);
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_3) || enableAutomatic == true) {
		enableAutomatic = true;
		CameraTrack(dt, futurePosition3, pitch3, yaw3, 500);
	}
	
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::CameraTrack(float dt, Vector3 newPosition, float pitch, float yaw, float time)
{
	if (enableAutomatic)
	{
		Vector3 direction = newPosition - camera->GetPosition();
		float yawDelta = yaw - camera->GetYaw();
		float pitchDelta = pitch - camera->GetPitch();
		if (!cameraMoving) {
			timeTaken = 0.0f;
			pathToTake = direction;
			pitchToTake = pitchDelta;
			yawToTake = yawDelta;

			cameraMoving = true;
		}
		camera->SetPosition(camera->GetPosition() + (pathToTake / time));
		camera->SetPitch(camera->GetPitch() + (pitchToTake / time));
		camera->SetYaw(camera->GetYaw() + (yawToTake / time));

		timeTaken++;

		if (camera->GetPosition() == newPosition || timeTaken == time) {
			cameraMoving = false;
			enableAutomatic = false;
		}
	}
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);

	BindShader(skyboxShader);

	UpdateShaderMatrices();

	skyboxNode->GetMesh()->Draw();

	glDepthMask(GL_TRUE);

}

void Renderer::DrawHeightmap() {

	BindShader(TerrainShader);
	SetShaderLight(*light);
	SetShaderSpotlight(*sauronLight);

	viewMatrix = camera -> BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	glUniform3fv(glGetUniformLocation(TerrainShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(TerrainShader->GetProgram(), "blendMap"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, blendMap);

	glUniform1i(glGetUniformLocation(TerrainShader->GetProgram(), "grassTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, grassTex);

	glUniform1i(glGetUniformLocation(TerrainShader->GetProgram(), "grassBump"), 6);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, grassBump);

	glUniform1i(glGetUniformLocation(TerrainShader->GetProgram(), "snowTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, snowTex);

	glUniform1i(glGetUniformLocation(TerrainShader->GetProgram(), "snowBump"), 7);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, snowBump);

	glUniform1i(glGetUniformLocation(TerrainShader->GetProgram(), "rockTex"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, rockTex);

	glUniform1i(glGetUniformLocation(TerrainShader->GetProgram(), "rockBump"), 8);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, rockBump);

	glUniform1i(glGetUniformLocation(TerrainShader->GetProgram(), "shadowTex"), 9);
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	modelMatrix.ToIdentity(); // reset so when water shader changes these the heightmap does not also get affected
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();

	heightMap->Draw();

}

void Renderer::DrawWater() {

	BindShader(reflectShader);

	glUniform3fv(glGetUniformLocation(reflectShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "diffuseTex"), 4);
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "cubeTex"), 5);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, waterTex);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	Vector3 hSize = heightMap->GetHeightmapSize();
	Vector3 move = Vector3(30.0f, -450.0f, -1210.0f);
	modelMatrix =
		Matrix4::Translation(hSize * 0.5f + move) *
		Matrix4::Scale(hSize * 0.03) *
		Matrix4::Rotation(90, Vector3(1, 0, 0));

	textureMatrix =
		Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) *
		Matrix4::Scale(Vector3(10, 10, 10)) *
		Matrix4::Rotation(waterRotate, Vector3(0, 0, 1));

	UpdateShaderMatrices();

	skyboxNode->GetMesh()->Draw();

	modelMatrix.ToIdentity(); // reset so when water shader changes these the heightmap does not also get affected
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();
}

void Renderer::DrawGrass() {

	heightMap->SetType(GL_POINTS);
	BindShader(grassShader);

	glUniform1i(glGetUniformLocation(grassShader->GetProgram(), "grassTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, grassTex);
	glUniform1i(glGetUniformLocation(grassShader->GetProgram(), "shadowTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glUniform1f(glGetUniformLocation(grassShader->GetProgram(), "time"), timePassed);

	UpdateShaderMatrices();
	heightMap->Draw();
	heightMap->SetType(GL_TRIANGLES);
}

void Renderer::DrawAssets() {

	BindShader(animationShader);
	UpdateShaderMatrices();
	DrawNode(animRoot, animationShader);
	

	BindShader(staticMeshShader);
	UpdateShaderMatrices();
	DrawNode(staticRoot, staticMeshShader);
	


}

void Renderer::DrawNode(SceneNode* n, Shader* shader) {
	if (n->GetMesh()) {
		Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());

		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "modelMatrix"), 1, false, model.values);

		n->Draw(*this);
	}

	for (vector < SceneNode* >::const_iterator i = n->GetChildIteratorStart(); i != n->GetChildIteratorEnd(); ++i) {
		DrawNode(*i, shader);
	}
}

void Renderer::DrawShadowScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	BindShader(shadowShader);

	viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3(heightMap->GetHeightmapSize().x/2, 0, heightMap->GetHeightmapSize().z / 2));
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	shadowMatrix = projMatrix * viewMatrix; // used later
	UpdateShaderMatrices();
	heightMap->Draw();
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::BuildNodeLists(SceneNode* from)
{

	Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
	from->SetCameraDistance(Vector3::Dot(dir, dir));

	if (from->GetColour().w < 1.0f) {
		transparentNodeList.push_back(from);

	}
	else {
		nodeList.push_back(from);
	}

	for (vector < SceneNode* >::const_iterator i = from->GetChildIteratorStart();
		i != from->GetChildIteratorEnd(); ++i)
	{
		BuildNodeLists((*i));
	}
}

void Renderer::SortNodeLists()
{
	 std::sort(transparentNodeList.rbegin(), // note the r!
	 transparentNodeList.rend(), // note the r!
	 SceneNode::CompareByCameraDistance);
	 std::sort(nodeList.begin(),
	 nodeList.end(),
	 SceneNode::CompareByCameraDistance);
}

void Renderer::ClearNodeLists()
{
	transparentNodeList.clear();
	nodeList.clear();
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	BuildNodeLists ( root );
	SortNodeLists();
	DrawSkybox();
	DrawWater();
	DrawShadowScene();
	DrawHeightmap();
	DrawGrass();	
	DrawAssets();
	ClearNodeLists();
}