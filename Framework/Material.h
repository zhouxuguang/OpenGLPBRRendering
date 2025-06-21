#pragma once
#include "utils.h"
class Camera;
struct TextureProperty{
    GLint mLocation;
    GLuint mTexture;
    TextureProperty(){
        mTexture=0;
    }
};
class Material{
public:
    GLuint mProgram;
    std::unordered_map<std::string,TextureProperty*> mTextures,mTextureCubes;
    Vec4UniformBufferData* mVec4s;
    GLenum mBlendSrcFunc,mBlendDstFunc;
    bool mbEnableBlend;
    bool mbEnableDepthTest;
	bool mbEnableCullFace;
    Material(GLuint inProgram);
	void SetTexture(const char * property_name, GLuint texture);
	void SetTextureCube(const char * property_name, GLuint texture);
	void SetVec4(int inIndex, float*v);
	void SetVec4(int inIndex, float x, float y, float z, float w=0.0f);
    void SetCameraWorldPosition(float inX, float inY, float inZ, float inW = 1.0f);
    void SetIOR(float inR, float inG, float inB, float inUseIOR = 1.0f);
	void EnableAOTexture(bool inIsEnable);
	void EnableEmissiveTexture(bool inIsEnable);
	void SetLightCount(int inCount);
	void SetTime(float inTime);
	void SetLightPosition(int inIndex, float inX, float inY, float inZ, float inW = 0.0f);
	void SetLightColor(int inIndex, float inR, float inG, float inB, float inIntensity = 1.0f);
    void Active();
	Material*Clone();
};
