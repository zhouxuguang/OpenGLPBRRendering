#include "scene.h"
#include "ggl.h"
#include "StaticMesh.h"
#include "Material.h"
#include "GameObject.h"
#include "Camera.h"
#include "framebufferobject.h"

FullScreenQuadMesh* gFullScreenQuadMesh = nullptr;
GLuint gToneMappingShader = 0;
Material* gToneMappingMaterial = nullptr;
GameObject* gToneMappingGameObject = nullptr;
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

    gToneMappingGameObject = new GameObject;
    gToneMappingGameObject->mStaticMesh = gFullScreenQuadMesh;
    gToneMappingGameObject->mMaterial = gToneMappingMaterial->Clone();
}
void SetViewPortSize(int inWidth, int inHeight) 
{
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
    
    gHDRFbo->Unbind();
	//hdr rendering pipeline
	glClearColor(0.1f, 0.4f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    gToneMappingGameObject->mMaterial->SetTexture("U_Texture", gHDRFbo->GetBuffer("color"));
    
    gToneMappingGameObject->Render(gProjectionMatrix, &gMainCamera);
}
