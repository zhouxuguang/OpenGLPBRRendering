#pragma once
#include "ggl.h"
struct SubMesh {
	unsigned int* mIndexes;
	int mIndexCount;
};
struct OGLSubMesh : public SubMesh {
	GLuint mIBO;
};
struct Mat4UniformBufferData {
	int mMat4Count;
	float mData[16384];
	GLuint mUBO;
	bool mbNeedSyncData;
	Mat4UniformBufferData(int inMat4Count);
	void SetMat4(int inIndex, float* inMat4);
	void UpdateGPUData();
};
struct Vec4UniformBufferData {
	int mVec4Count;
	float mData[4096];
	GLuint mUBO;
	bool mbNeedSyncData;
	Vec4UniformBufferData(int inVec4Count);
	void SetVec4(int inIndex, float* inVec4);
	void UpdateGPUData();
};
struct VertexFactory {
	const char** mAttributes;
	int mAttributeCount;
	int mVertexSizeInBytes;
	VertexFactory(const char** inAttributes, int inAttributeCount, int inVertexSize) {
		mAttributes = inAttributes;
		mAttributeCount = inAttributeCount;
		mVertexSizeInBytes = inVertexSize;
	}
};
struct StaticMeshVertexData {
	float mPosition[4];
	float mTexcoord[4];
	float mNormal[4];
};
struct StaticMeshVertexDataEx :public StaticMeshVertexData {
	float mTangent[4];
};
const VertexFactory* GetVertexFactory();
const VertexFactory* GetVertexFactory2();
GLuint BindUBO(GLuint inProgram, const char* inUBOName, GLuint inUBO, GLuint inSlot);
GLuint BuildVAO(GLuint inProgram, GLuint inVBO, int inAttributeCount, const char** inAttributeNames, int inVertexSizeInBytes);
unsigned char * LoadFileContent(const char*path, int&filesize);
GLuint CompileShader(GLenum shaderType, const char*shaderCode);
GLuint CreateProgram(GLuint vsShader, GLuint fsShader);
GLuint CreateProgramFromFile(const char* inVertexShaderPath, const char* inFragmentShaderPath);
float GetFrameTime();
unsigned char * DecodeBMP(unsigned char*bmpFileData, int&width, int&height);
void* DecodeImageFile(const char* path, int& width, int& height, int& channelCount);
GLuint CreateTexture2D(void*pixelData, int width, int height,GLenum gpu_format=GL_RGB,GLenum cpu_format=GL_RGB, 
	GLenum wrapMode=GL_CLAMP_TO_EDGE, GLenum minFilter=GL_LINEAR, GLenum magFilter = GL_LINEAR);
GLuint CreateTextureFromFile(const char *path,GLenum wrapMode=GL_CLAMP_TO_EDGE,GLenum minFilter=GL_LINEAR,GLenum magFilter = GL_LINEAR);
GLuint CreateBufferObject(GLenum bufferType, GLsizeiptr size, GLenum usage, void*data = nullptr);
void UpdateBufferObject(GLuint object, GLenum type, void * data, int size, int offset);
#define OGL_CALL(x) do { x; } while(0)