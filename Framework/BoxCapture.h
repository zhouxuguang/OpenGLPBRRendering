#pragma once
#include "ggl.h"
#include "framebufferobject.h"
#include "Material.h"
#include "StaticMesh.h"
class BoxCapture {
public:
	GLuint mCaptureFBO, mCaptureRBO;
	GLuint mCubeMap;
	GLint mPrevFrameBuffer;
	int mCubeMapResolution;
	GLenum mGPUFormat;
	GLenum mGPUDataType;
public:
	BoxCapture();
	void Init(int resolution = 512,GLenum gpu_format=GL_RGB16F,GLenum data_type=GL_FLOAT,bool inGenerateMipmap=false);
	void Bind();
	void Unbind();
	void BeginRenderFace(int i, int mipmap_level=0);
};