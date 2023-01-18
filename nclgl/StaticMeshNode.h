#pragma once
#include "SceneNode.h"
#include "OGLRenderer.h"
#include "MeshMaterial.h"

class StaticMeshNode : public SceneNode {

public:
	StaticMeshNode(Shader* shader, Mesh* mesh, MeshMaterial* meshMat);

	~StaticMeshNode();

	void Draw(const OGLRenderer& renderer) override;

	void Update(float dt) override;


protected:

	vector <GLuint> matTextures;
	Shader* skinningShader;
	MeshMaterial* material;

};