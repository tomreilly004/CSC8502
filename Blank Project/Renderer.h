# pragma once
# include "../nclgl/OGLRenderer.h"
# include "../nclgl/SceneNode.h"
#include "../nclgl/AnimMeshNode.h"
#include "../nclgl/StaticMeshNode.h"
#include "../nclgl/Camera.h"


class HeightMap;
class Camera;
class Mesh;
class MeshAnimation;
class MeshMaterial;

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);
	void RenderScene() override;
	void UpdateScene(float dt) override;

	Camera* camera;
protected:

	void DrawHeightmap();
	void DrawWater();
	void DrawSkybox();
	void DrawGrass();
	void DrawAssets();
	void DrawNode(SceneNode* n, Shader* shader);
	void DrawShadowScene();
	void BuildNodeLists(SceneNode * from);
	void SortNodeLists();
	void ClearNodeLists();

	void CameraTrack(float dt, Vector3 newPosition, float pitch, float yaw, float time);

	Shader* grassShader;
	Shader* TerrainShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* animationShader;
	Shader* staticMeshShader;

	bool cameraMoving;
	float timeTaken;
	Vector3 pathToTake;
	float pitchToTake;
	float yawToTake;

	SceneNode* root;
	SceneNode* animRoot;
	SceneNode* staticRoot;
	Mesh* spiderMesh;
	MeshAnimation* spiderAnim;
	MeshMaterial* spiderMat;
	AnimMeshNode* spiderNode;
	
	Mesh* treeMesh;
	MeshMaterial* treeMat;
	StaticMeshNode* treeNode;

	Mesh* sauronEye;
	MeshMaterial* sauronMat;
	StaticMeshNode* sauronNode;

	SceneNode* skyboxNode;
	SceneNode* mapNode;
	HeightMap* heightMap;
	Mesh* quad;

	Light* light;
	Spotlight* sauronLight;

	int counter;

	float timePassed;

	GLuint grassTex;
	GLuint rockTex;
	GLuint snowTex;
	GLuint blendMap;
	GLuint cubeMap;
	GLuint waterTex;
	GLuint earthTex;
	GLuint grassBump;
	GLuint snowBump;
	GLuint rockBump;

	GLuint shadowTex;
	GLuint shadowFBO;
	GLuint sceneDiffuse;
	GLuint sceneBump;
	Shader * shadowShader;

	float waterRotate;
	float waterCycle;

	bool enableAutomatic;

	Vector3 futurePosition1;
	Vector3 futurePosition2;
	Vector3 futurePosition3;

	vector < SceneNode* > transparentNodeList;
	vector < SceneNode* > nodeList;


	float pitch1;
	float pitch2;
	float pitch3;

	float yaw1;
	float yaw2;
	float yaw3;

};