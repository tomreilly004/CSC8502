#pragma once
#include "SceneNode.h"
#include "OGLRenderer.h"
#include "MeshAnimation.h"
#include "MeshMaterial.h"

class AnimMeshNode : public SceneNode {

public:
	AnimMeshNode(Shader* shader, Mesh* mesh, MeshAnimation* meshAnim, MeshMaterial* meshMat);

	~AnimMeshNode();

	void Draw(const OGLRenderer& renderer) override;


	void DrawMat();

	void Update(float dt) override;

protected:

	vector <GLuint> matTextures;
	int currentFrame;
	float frameTime;
	Shader* skinningShader;
	MeshAnimation* anim;
	MeshMaterial* material;

};