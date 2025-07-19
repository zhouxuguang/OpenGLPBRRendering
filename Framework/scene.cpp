#include "scene.h"
#include "ggl.h"
#include "StaticMesh.h"
#include "Material.h"
#include "GameObject.h"
#include "Camera.h"
#include "framebufferobject.h"
#include "stbi/stb_image.h"
#include "BoxCapture.h"

// mesh
FullScreenQuadMesh* gFullScreenQuadMesh = nullptr;
StaticMesh* gSphereMesh = nullptr;
StaticMesh* gSkyBoxmesh = nullptr;

// shader
GLuint gToneMappingShader = 0;
GLuint gPBRShader = 0;
GLuint gTexture2D2CubeMapShader = 0;
GLuint gCaptureDiffuseIrradianceShader = 0;
GLuint gCapturePrefilteredColorShader = 0;
GLuint gSkyBoxShader = 0;
GLuint gHDRTexture = 0;
GLuint gGenerateBRDFShader = 0;

//material
Material* gToneMappingMaterial = nullptr;
Material* gPBRMaterial = nullptr;
Material* gTexture2D2CubeMapMaterial = nullptr;
Material* gCaptureDiffuseIrradianceMaterial = nullptr;
Material* gCapturePrefilteredColorMaterial = nullptr;
Material* gSkyBoxMaterial = nullptr;
Material* gGenerateBRDFMaterial = nullptr;

// gameobject
GameObject* gToneMappingGameObject = nullptr;
GameObject* gSphereGameObject = nullptr;
GameObject* gTexture2D2CubeMapObject = nullptr;
GameObject* gSkyBoxObject = nullptr;
GameObject* gCaptureDiffuseIrradianceGameObject = nullptr;
GameObject* gCapturePrefilteredColorGameObject = nullptr;
GameObject* gGenerateBRDFGameObject = nullptr;


FrameBufferObject* gHDRFbo = nullptr;
FrameBufferObject* gGenerateBRDFFbo = nullptr;
glm::mat4 gProjectionMatrix;
Camera gMainCamera;

// box capture
BoxCapture* gCaptureTexture2D2CubeMap = nullptr;
BoxCapture* gCaptureDiffuseIrradiance = nullptr;
BoxCapture* gCapturePrefilteredColor = nullptr;

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
    
    // diffuse irraidance
    gCaptureDiffuseIrradianceShader = CreateProgramFromFile("Res/Shader/SkyBox.vs", "Res/Shader/CaptureDiffuseIrradiance.fs");
    gCaptureDiffuseIrradianceMaterial = new Material(gCaptureDiffuseIrradianceShader);
    
    // PrefilteredColor
    gCapturePrefilteredColorShader = CreateProgramFromFile("Res/Shader/SkyBox.vs", "Res/Shader/CapturePrefilteredColor.fs");
    gCapturePrefilteredColorMaterial = new Material(gCapturePrefilteredColorShader);
    
    gGenerateBRDFShader = CreateProgramFromFile("Res/Shader/FSQ.vs", "Res/Shader/GenerateBRDF.fs");
    gGenerateBRDFMaterial = new Material(gGenerateBRDFShader);
    
    // 加载天空盒图片，2d纹理格式
    stbi_set_flip_vertically_on_load(true);
    gHDRTexture = CreateTextureFromFile("res/Image/1.hdr");
    
    // 天空盒的物体
    gTexture2D2CubeMapObject = new GameObject;
    gTexture2D2CubeMapObject->mStaticMesh = gSkyBoxmesh;
    gTexture2D2CubeMapObject->mMaterial = gTexture2D2CubeMapMaterial->Clone();
    gTexture2D2CubeMapObject->mMaterial->mbEnableCullFace = false;
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
    
    // 加载正常天空盒的信息
    gSkyBoxShader = CreateProgramFromFile("Res/Shader/SkyBox.vs", "Res/Shader/SkyBox.fs");
    gSkyBoxMaterial = new Material(gSkyBoxShader);
    gSkyBoxObject = new GameObject;
    gSkyBoxObject->mStaticMesh = gSkyBoxmesh;
    gSkyBoxObject->mMaterial = gSkyBoxMaterial->Clone();
    gSkyBoxObject->mMaterial->mbEnableDepthTest = false;
    gSkyBoxObject->mMaterial->mbEnableCullFace = false;
    gSkyBoxObject->mMaterial->SetTextureCube("U_SkyBox", gCaptureTexture2D2CubeMap->mCubeMap);
    
    gPBRShader = CreateProgramFromFile("Res/Shader/PBR.vs", "Res/Shader/PBR.fs");
    gPBRMaterial = new Material(gPBRShader);

    gToneMappingGameObject = new GameObject;
    gToneMappingGameObject->mStaticMesh = gFullScreenQuadMesh;
    gToneMappingGameObject->mMaterial = gToneMappingMaterial->Clone();
    
    gSphereGameObject = new GameObject;
    gSphereGameObject->mStaticMesh = gSphereMesh;
    gSphereGameObject->mMaterial = gPBRMaterial->Clone();
    
    //生成difuse irraidance的天空盒
    gCaptureDiffuseIrradianceGameObject = new GameObject;
    gCaptureDiffuseIrradianceGameObject->mStaticMesh = gSkyBoxmesh;
    gCaptureDiffuseIrradianceGameObject->mMaterial = gCaptureDiffuseIrradianceMaterial->Clone();
    gCaptureDiffuseIrradianceGameObject->mMaterial->mbEnableCullFace = false;
    gCaptureDiffuseIrradianceGameObject->mMaterial->SetTextureCube("U_SkyBox", gCaptureTexture2D2CubeMap->mCubeMap);
    
    //天空盒环境捕捉器
    gCaptureDiffuseIrradiance = new BoxCapture();
    gCaptureDiffuseIrradiance->Init(512, GL_RGB32F, GL_FLOAT);
    
    gCaptureDiffuseIrradiance->Bind();
    for (int i = 0; i < 6; i ++)
    {
        gCaptureDiffuseIrradiance->BeginRenderFace(i);
        gCaptureDiffuseIrradianceGameObject->Render(gCaptureProjectionMatrix, gCaptureCameras + i);
    }
    gCaptureDiffuseIrradiance->Unbind();
    
    // PrefilteredColor
    gCapturePrefilteredColorGameObject = new GameObject;
    gCapturePrefilteredColorGameObject->mStaticMesh = gSkyBoxmesh;
    gCapturePrefilteredColorGameObject->mMaterial = gCapturePrefilteredColorMaterial->Clone();
    gCapturePrefilteredColorGameObject->mMaterial->mbEnableCullFace = false;
    gCapturePrefilteredColorGameObject->mMaterial->SetTextureCube("U_SkyBox", gCaptureTexture2D2CubeMap->mCubeMap);
    
    //天空盒环境捕捉器
    gCapturePrefilteredColor = new BoxCapture();
    gCapturePrefilteredColor->Init(512, GL_RGB32F, GL_FLOAT, true);
    
    gCapturePrefilteredColor->Bind();
    for (int mipmapLevel = 0; mipmapLevel < 5; mipmapLevel++) 
    {
        float roughness = float(mipmapLevel) / float(4);
        for (int i = 0; i < 6; ++i) 
        {
            gCapturePrefilteredColor->BeginRenderFace(i, mipmapLevel);
            gCapturePrefilteredColorGameObject->mMaterial->SetVec4(0, roughness, 0.0f, 0.0f, 0.0f);
            gCapturePrefilteredColorGameObject->Render(gCaptureProjectionMatrix, &gCaptureCameras[i]);
        }
    }
    gCapturePrefilteredColor->Unbind();
    
    //brdf相关
    gGenerateBRDFGameObject = new GameObject;
    gGenerateBRDFGameObject->mStaticMesh = gFullScreenQuadMesh;
    gGenerateBRDFGameObject->mMaterial = gGenerateBRDFMaterial->Clone();
    
    gGenerateBRDFFbo = new FrameBufferObject;
    gGenerateBRDFFbo->AttachColorBuffer("color", GL_COLOR_ATTACHMENT0, 512, 512, GL_RG32F, GL_RG, GL_FLOAT);
    gGenerateBRDFFbo->Finish();
    
    gMainCamera.mPosition = glm::vec3(0.0f, 0.0f, 3.0f);
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
    
    // render sky
    glDepthMask(GL_FALSE);
    gSkyBoxObject->mModelMatrix = glm::translate(gMainCamera.mPosition);
    gSkyBoxObject->Render(gProjectionMatrix, &gMainCamera);
    glDepthMask(GL_TRUE);
    
    gSphereGameObject->mMaterial->SetCameraWorldPosition(0, 0, 3);
    gSphereGameObject->Render(gProjectionMatrix, &gMainCamera);
    
    gHDRFbo->Unbind();
	//hdr rendering pipeline
	glClearColor(0.1f, 0.4f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    gToneMappingGameObject->mMaterial->SetTexture("U_Texture", gHDRFbo->GetBuffer("color"));
    
    gToneMappingGameObject->Render(gProjectionMatrix, &gMainCamera);
}
