#include "StaticMeshNode.h"

StaticMeshNode::StaticMeshNode(Shader* shader, Mesh* mesh, MeshMaterial* meshMat) : SceneNode(mesh)
{
	material = meshMat;
	skinningShader = shader;
	for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry = meshMat->GetMaterialForLayer(i);

		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		matTextures.emplace_back(texID);
	}
}

void StaticMeshNode::Draw(const OGLRenderer& renderer)
{
	for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, matTextures[i]);
		mesh->DrawSubMesh(i);
	}
	SceneNode::Draw(renderer);
}



void StaticMeshNode::Update(float dt)
{
	SceneNode::Update(dt);
}