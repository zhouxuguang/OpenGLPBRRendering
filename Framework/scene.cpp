#include "scene.h"
#include "ggl.h"
#include "StaticMesh.h"
#include "Material.h"
#include "GameObject.h"
#include "Camera.h"
#include "framebufferobject.h"
#include "stbi/stb_image.h"
#include "BoxCapture.h"

FullScreenQuadMesh* gFullScreenQuadMesh = nullptr;
StaticMesh* gSphereMesh = nullptr;
StaticMesh* gSkyBoxmesh = nullptr;
GLuint gToneMappingShader = 0;
GLuint gPBRShader = 0;
GLuint gTexture2D2CubeMapShader = 0;
GLuint gHDRTexture = 0;
Material* gToneMappingMaterial = nullptr;
Material* gPBRMaterial = nullptr;
Material* gTexture2D2CubeMapMaterial = nullptr;
GameObject* gToneMappingGameObject = nullptr;
GameObject* gSphereGameObject = nullptr;
GameObject* gTexture2D2CubeMapObject = nullptr;
FrameBufferObject* gHDRFbo = nullptr;
glm::mat4 gProjectionMatrix;
Camera gMainCamera;
BoxCapture* gCaptureTexture2D2CubeMap = nullptr;

glm::mat4 gCaptureProjectionMatrix;
Camera gCaptureCameras[6];


void Init() 
{
	//init runtime assets
    gCaptureProjectionMatrix= glm::perspective(90.0f, 1.0f, 0.1f, 100.0f);
    gCaptureCameras[0].mViewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    gCaptureCameras[1].mViewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    gCaptureCameras[2].mViewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    gCaptureCameras[3].mViewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    gCaptureCameras[4].mViewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    gCaptureCameras[5].mViewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    
    gFullScreenQuadMesh = new FullScreenQuadMesh;
    gFullScreenQuadMesh->Init();

    gToneMappingShader = CreateProgramFromFile("Res/Shader/FSQ.vs", "Res/Shader/ToneMapping.fs");
    gToneMappingMaterial = new Material(gToneMappingShader);
    
    
    gSphereMesh = new StaticMesh();
    gSphereMesh->LoadFileFile("res/Model/sphere.staticmesh");
    
    // 加载天空盒的mesh
    gSkyBoxmesh = new StaticMesh();
    gSkyBoxmesh->LoadFileFile("res/Model/skybox.staticmesh");
    
    // 加载天空盒的shader
    gTexture2D2CubeMapShader = CreateProgramFromFile("Res/Shader/SkyBox.vs", "Res/Shader/Texture2D2CubeMap.fs");
    gTexture2D2CubeMapMaterial = new Material(gTexture2D2CubeMapShader);
    
    // 加载天空盒图片，2d纹理格式
    stbi_set_flip_vertically_on_load(true);
    gHDRTexture = CreateTextureFromFile("res/Image/1.hdr");
    
    // 天空盒的物体
    gTexture2D2CubeMapObject = new GameObject;
    gTexture2D2CubeMapObject->mStaticMesh = gSkyBoxmesh;
    gTexture2D2CubeMapObject->mMaterial = gTexture2D2CubeMapMaterial->Clone();
    gTexture2D2CubeMapObject->mMaterial->SetTexture("U_Texture", gHDRTexture);
    
    //天空盒环境捕捉器
    gCaptureTexture2D2CubeMap = new BoxCapture();
    gCaptureTexture2D2CubeMap->Init(512, GL_RGB32F, GL_FLOAT);
    
    gCaptureTexture2D2CubeMap->Bind();
    for (int i = 0; i < 6; i ++)
    {
        gCaptureTexture2D2CubeMap->BeginRenderFace(i);
        gTexture2D2CubeMapObject->Render(gCaptureProjectionMatrix, gCaptureCameras + i);
    }
    gCaptureTexture2D2CubeMap->Unbind();
    
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
