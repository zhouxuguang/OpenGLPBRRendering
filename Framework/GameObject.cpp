#include "GameObject.h"
#include "StaticMesh.h"
#include "Material.h"
#include "utils.h"
#include "Camera.h"

GameObject::GameObject() {
    mMat4UniformBufferData = new Mat4UniformBufferData(1024);
	mVAO = 0;
}
void GameObject::Render(glm::mat4& inProjectionMatrix, Camera* camera){
    if(mMaterial->mProgram==0){
        return ;
	}
	mMaterial->Active();
	if (mVAO == 0) {
		mVAO=BuildVAO(mMaterial->mProgram,mStaticMesh->mVBO, mStaticMesh->mVertexFactory->mAttributeCount, mStaticMesh->mVertexFactory->mAttributes, mStaticMesh->mVertexFactory->mVertexSizeInBytes);
	}
	OGL_CALL(glBindVertexArray(mVAO));

	if (camera != nullptr) {
		mMat4UniformBufferData->SetMat4(0, glm::value_ptr(camera->mViewMatrix));
	}
	mMat4UniformBufferData->SetMat4(1, glm::value_ptr(inProjectionMatrix));
	mMat4UniformBufferData->SetMat4(2, glm::value_ptr(mModelMatrix));
	glm::mat4 itModelMatrix = glm::inverseTranspose(mModelMatrix);
	mMat4UniformBufferData->SetMat4(3, glm::value_ptr(itModelMatrix));
	mMat4UniformBufferData->UpdateGPUData();

    GLuint uboSlot = 0;
	uboSlot=BindUBO(mMaterial->mProgram, "DefaultVertexMatrixs", mMat4UniformBufferData->mUBO, uboSlot);
	mMaterial->mVec4s->UpdateGPUData();
	uboSlot = BindUBO(mMaterial->mProgram, "DefaultVec4s", mMaterial->mVec4s->mUBO, uboSlot);
    mStaticMesh->Draw();
	OGL_CALL(glBindVertexArray(0));
}