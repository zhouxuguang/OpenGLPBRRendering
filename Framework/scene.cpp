#include "scene.h"
#include "ggl.h"
#include "StaticMesh.h"
#include "Material.h"
#include "GameObject.h"
#include "Camera.h"
#include "framebufferobject.h"

FullScreenQuadMesh* gFullScreenQuadMesh = nullptr;
StaticMesh* gSphereMesh = nullptr;
GLuint gToneMappingShader = 0;
GLuint gPBRShader = 0;
Material* gToneMappingMaterial = nullptr;
Material* gPBRMaterial = nullptr;
GameObject* gToneMappingGameObject = nullptr;
GameObject* gSphereGameObject = nullptr;
FrameBufferObject* gHDRFbo = nullptr;
glm::mat4 gProjectionMatrix;
Camera gMainCamera;

void Init() 
{
	//init runtime assets
    gFullScreenQuadMesh = new FullScreenQuadMesh;
    gFullScreenQuadMesh->Init();

    gToneMappingShader = CreateProgramFromFile("Res/Shader/FSQ.vs", "Res/Shader/ToneMapping.fs");
    gToneMappingMaterial = new Material(gToneMappingShader);
    
    
    gSphereMesh = new StaticMesh();
    gSphereMesh->LoadFileFile("res/Model/sphere.staticmesh");
    
    gPBRShader = CreateProgramFromFile("Res/Shader/PBR.vs", "Res/Shader/PBR.fs");
    gPBRMaterial = new Material(gPBRShader);

    gToneMappingGameObject = new GameObject;
    gToneMappingGameObject->mStaticMesh = gFullScreenQuadMesh;
    gToneMappingGameObject->mMaterial = gToneMappingMaterial->Clone();
    
    gSphereGameObject = new GameObject;
    gSphereGameObject->mStaticMesh = gSphereMesh;
    gSphereGameObject->mMaterial = gPBRMaterial->Clone();
    
    gMainCamera.mViewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,1.0f,0.0f));
}
void SetViewPortSize(int inWidth, int inHeight) 
{
    gProjectionMatrix = glm::perspective(60.0f, float(inWidth) / float(inHeight), 0.1f, 1000.0f);
	//rt
    gHDRFbo = new FrameBufferObject;
    gHDRFbo->AttachColorBuffer("color", GL_COLOR_ATTACHMENT0, inWidth, inHeight, GL_RGB32F, GL_RGB, GL_FLOAT);
    gHDRFbo->AttachDepthBuffer("depth", inWidth, inHeight);
    gHDRFbo->Finish();
}
void Draw() 
{
    gHDRFbo->Bind();
    
    glClearColor(0.1f, 0.4f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    gSphereGameObject->mMaterial->SetCameraWorldPosition(0, 0, 3);
    gSphereGameObject->Render(gProjectionMatrix, &gMainCamera);
    
    gHDRFbo->Unbind();
	//hdr rendering pipeline
	glClearColor(0.1f, 0.4f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    gToneMappingGameObject->mMaterial->SetTexture("U_Texture", gHDRFbo->GetBuffer("color"));
    
    gToneMappingGameObject->Render(gProjectionMatrix, &gMainCamera);
}
