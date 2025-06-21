#include "Material.h"
Material::Material(GLuint inProgram) {
    mbEnableBlend=false;
    mbEnableDepthTest=true;
	mbEnableCullFace = true;
    mBlendSrcFunc=GL_SRC_ALPHA;
    mBlendDstFunc=GL_ONE_MINUS_SRC_ALPHA;
	mProgram = inProgram;
	mVec4s = new Vec4UniformBufferData(1024);
}
void Material::SetTexture(const char *property_name, GLuint texture) {
    auto iter=mTextures.find(property_name);
    TextureProperty*tp= nullptr;
    if(iter==mTextures.end()){
		GLint location = -1;
		OGL_CALL(location=glGetUniformLocation(mProgram, property_name));
        if(location!=-1){
            tp=new TextureProperty;
            tp->mLocation=location;
            tp->mTexture=texture;
            mTextures.insert(std::pair<std::string,TextureProperty*>(property_name,tp));
        }
    }else{
        tp=iter->second;
        tp->mTexture=texture;
    }
}
void Material::SetTextureCube(const char *property_name, GLuint texture) {
	auto iter = mTextureCubes.find(property_name);
	TextureProperty*tp = nullptr;
	if (iter == mTextureCubes.end()) {
		GLint location = -1;
		OGL_CALL(location = glGetUniformLocation(mProgram, property_name));
		if (location != -1) {
			tp = new TextureProperty;
			tp->mLocation = location;
			mTextureCubes.insert(std::pair<std::string, TextureProperty*>(property_name, tp));
		}
	}
	else {
		tp = iter->second;
	}
	if (tp != nullptr) {
		tp->mTexture = texture;
	}
}
void Material::SetVec4(int inIndex, float *inVec4){
	mVec4s->SetVec4(inIndex, inVec4);
}
void Material::SetVec4(int inIndex, float x, float y, float z, float w) {
	float v[4];
	v[0] = x;
	v[1] = y;
	v[2] = z;
	v[3] = w;
	SetVec4(inIndex, v);
}
void Material::SetCameraWorldPosition(float inX, float inY, float inZ, float inW /* = 1.0f */) {
	SetVec4(0, inX, inY, inZ, inW);
}
void Material::SetIOR(float inR, float inG, float inB, float inUseIOR /* = 1.0f */) {
	SetVec4(1, inR, inG, inB, inUseIOR);
}
void Material::EnableAOTexture(bool inIsEnable) {
	int offset = 2 * 4;
	mVec4s->mData[offset] = inIsEnable ? 1.0f : 0.0f;
	mVec4s->mbNeedSyncData = true;
}
void Material::EnableEmissiveTexture(bool inIsEnable) {
	int offset = 2 * 4;
	mVec4s->mData[offset+1] = inIsEnable ? 1.0f : 0.0f;
	mVec4s->mbNeedSyncData = true;
}
void Material::SetLightCount(int inCount) {
	int offset = 2 * 4;
	mVec4s->mData[offset + 2] = float(inCount);
	mVec4s->mbNeedSyncData = true;
}
void Material::SetTime(float inTime) {
	int offset = 2 * 4;
	mVec4s->mData[offset + 3] = float(inTime);
	mVec4s->mbNeedSyncData = true;
}
void Material::SetLightPosition(int inIndex, float inX, float inY, float inZ, float inW /* = 0.0f */) {
	SetVec4(3+inIndex, inX, inY, inZ, inW);
}
void Material::SetLightColor(int inIndex, float inR, float inG, float inB, float inIntensity /* = 1.0f */) {
	SetVec4(7 + inIndex, inR, inG, inB, inIntensity);
}
void Material::Active() {
    if(mbEnableDepthTest){
		OGL_CALL(glEnable(GL_DEPTH_TEST));
    }else{
		OGL_CALL(glDisable(GL_DEPTH_TEST));
    }
    if(mbEnableBlend){
		OGL_CALL(glEnable(GL_BLEND));
		OGL_CALL(glBlendFunc(mBlendSrcFunc,mBlendDstFunc));
    }else{
		OGL_CALL(glDisable(GL_BLEND));
    }
	if (mbEnableCullFace){
		OGL_CALL(glEnable(GL_CULL_FACE));
	}
	else {
		OGL_CALL(glDisable(GL_CULL_FACE));
	}
	OGL_CALL(glUseProgram(mProgram));
    int texture_unit_index=0;
    for (auto iter = mTextures.begin(); iter!=mTextures.end() ; ++iter) {
        OGL_CALL(glActiveTexture(GL_TEXTURE0+texture_unit_index));
        OGL_CALL(glBindTexture(GL_TEXTURE_2D,iter->second->mTexture));
        OGL_CALL(glUniform1i(iter->second->mLocation,texture_unit_index++));
	}
	for (auto iter = mTextureCubes.begin(); iter != mTextureCubes.end(); ++iter) {
		OGL_CALL(glActiveTexture(GL_TEXTURE0 + texture_unit_index));
		OGL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, iter->second->mTexture));
		OGL_CALL(glUniform1i(iter->second->mLocation, texture_unit_index++));
	}
}
Material*Material::Clone() {
	Material*ret = new Material(mProgram);
	ret->mBlendSrcFunc = mBlendSrcFunc;
	ret->mBlendDstFunc = mBlendDstFunc;
	ret->mbEnableBlend = mbEnableBlend;
	ret->mbEnableDepthTest = mbEnableDepthTest;
	ret->mbEnableCullFace = mbEnableCullFace;
	for (auto iter = mTextures.begin(); iter != mTextures.end(); ++iter) {
		ret->SetTexture(iter->first.c_str(), iter->second->mTexture);
	}
	for (auto iter = mTextureCubes.begin(); iter != mTextureCubes.end(); ++iter) {
		ret->SetTextureCube(iter->first.c_str(), iter->second->mTexture);
	}
	memcpy(ret->mVec4s->mData, mVec4s->mData, sizeof(mVec4s->mData));
	return ret;
}
