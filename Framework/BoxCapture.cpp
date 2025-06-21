#include "BoxCapture.h"
#include "utils.h"
#include "StaticMesh.h"
BoxCapture::BoxCapture() {
	mCubeMapResolution = 512;
}
void BoxCapture::Init(int resolution, GLenum gpu_format, GLenum data_type,bool inGenerateMipmap) {
	mCubeMapResolution = resolution;
	mGPUFormat = gpu_format;
	mGPUDataType = data_type;
	OGL_CALL(glGenFramebuffers(1, &mCaptureFBO));
	OGL_CALL(glGenRenderbuffers(1, &mCaptureRBO));
	OGL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, mCaptureFBO));
	OGL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, mCaptureRBO));
	OGL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mCubeMapResolution, mCubeMapResolution));
	OGL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mCaptureRBO));
	OGL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mCaptureRBO));
	OGL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	OGL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, 0));

	OGL_CALL(glGenTextures(1, &mCubeMap));
	OGL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, mCubeMap));
	for (unsigned int i = 0; i < 6; ++i) {
		OGL_CALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, mGPUFormat, mCubeMapResolution, mCubeMapResolution, 0, GL_RGB, mGPUDataType, nullptr));
	}
	OGL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	OGL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	OGL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
	OGL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	if (inGenerateMipmap) {
		OGL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	} else {
		OGL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	}
	OGL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}
void BoxCapture::Bind() {
	OGL_CALL(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &mPrevFrameBuffer));
	OGL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, mCaptureFBO));
	OGL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
}
void BoxCapture::BeginRenderFace(int inFaceIndex, int inTargetMipMapLevel) {
	switch (inFaceIndex)
	{
	case 0:
		OGL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, mCubeMap, inTargetMipMapLevel));
		break;
	case 1:
		OGL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, mCubeMap, inTargetMipMapLevel));
		break;
	case 2:
		OGL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, mCubeMap, inTargetMipMapLevel));
		break;
	case 3:
		OGL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, mCubeMap, inTargetMipMapLevel));
		break;
	case 4:
		OGL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, mCubeMap, inTargetMipMapLevel));
		break;
	case 5:
		OGL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, mCubeMap, inTargetMipMapLevel));
		break;
	}
	unsigned int mipWidth = static_cast<unsigned int>(mCubeMapResolution * std::pow(0.5, inTargetMipMapLevel));
	unsigned int mipHeight = static_cast<unsigned int>(mCubeMapResolution * std::pow(0.5, inTargetMipMapLevel));
	OGL_CALL(glViewport(0, 0, mipWidth, mipHeight));
	OGL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}
void BoxCapture::Unbind() {
	OGL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, mPrevFrameBuffer));
}