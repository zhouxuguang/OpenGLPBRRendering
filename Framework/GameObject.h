#pragma once
#include "utils.h"
class StaticMesh;
class Material;
class Camera;
class GameObject{
public:
	GLuint mVAO;
    glm::mat4 mModelMatrix;
    StaticMesh* mStaticMesh;
    Material * mMaterial;
    Mat4UniformBufferData* mMat4UniformBufferData;
    GameObject();
	void Render(glm::mat4& inProjectionMatrix, Camera* camera);
};