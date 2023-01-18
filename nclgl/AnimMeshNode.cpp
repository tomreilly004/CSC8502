#include "AnimMeshNode.h"
AnimMeshNode::AnimMeshNode(Shader* shader, Mesh* mesh, MeshAnimation* meshAnim, MeshMaterial* meshMat) : SceneNode(mesh)
{
	anim = meshAnim;
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
	currentFrame = 0; 
	frameTime = 0.0f;
}

AnimMeshNode::~AnimMeshNode()
{
}

void AnimMeshNode::Draw(const OGLRenderer& renderer)
{
	glUniform1i(glGetUniformLocation(skinningShader->GetProgram(), "diffuseTex"), 0);
	vector <Matrix4> frameMatrices;
	const Matrix4* invBindPose = mesh->GetInverseBindPose();
	const Matrix4* frameData = anim->GetJointData(currentFrame);


	for (unsigned int i = 0; i < mesh->GetJointCount(); ++i) {
		frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
	}

	int j = glGetUniformLocation(skinningShader->GetProgram(), "joints");
	glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());

	DrawMat();
}


void AnimMeshNode::DrawMat()
{
	for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, matTextures[i]);
		mesh->DrawSubMesh(i);
	}
}

void AnimMeshNode::Update(float dt)
{
	frameTime -= dt;
	while (frameTime < 0.0f) {
		currentFrame = (currentFrame + 1) % anim->GetFrameCount();
		frameTime += 1.0f / anim->GetFrameRate();
	}
	SceneNode::Update(dt);
}


