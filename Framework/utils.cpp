#include "utils.h"
extern"C" {
#include "stbi/stb_image.h"
}

#ifdef __WIN32
#pragma warning(disable : 4996)
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glew32.lib")
#endif

unsigned char * LoadFileContent(const char *path, int &filesize) {
	unsigned char*fileContent = nullptr;
	filesize = 0;
	FILE*pFile = fopen(path, "rb");
	if (pFile) {
		fseek(pFile, 0, SEEK_END);
		int nLen = ftell(pFile);
		if (nLen > 0) {
			rewind(pFile);
			fileContent = new unsigned char[nLen + 1];
			fread(fileContent, sizeof(unsigned char), nLen, pFile);
			fileContent[nLen] = '\0';
			filesize = nLen;
		}
		fclose(pFile);
	}
	return fileContent;
}

static const char* attributeNames[] = { "position","texcoord","normal"};
static const char* attributeNames2[] = { "position","texcoord","normal","tangent" };
const VertexFactory* GetVertexFactory() {
	static VertexFactory* sVertexFactory = nullptr;
	if (sVertexFactory == nullptr) {
		sVertexFactory = new VertexFactory(attributeNames, 3, sizeof(float) * 4 * 3);
	}
	return sVertexFactory;
}
const VertexFactory* GetVertexFactory2() {
	static VertexFactory* sVertexFactory = nullptr;
	if (sVertexFactory == nullptr) {
		sVertexFactory = new VertexFactory(attributeNames2, 4, sizeof(float) * 4 * 4);
	}
	return sVertexFactory;
}
GLuint BuildVAO(GLuint inProgram, GLuint inVBO, int inAttributeCount, const char** inAttributeNames, int inVertexSizeInBytes) {
	int attributeComponentCount = 4;
	GLuint vao = 0;
	OGL_CALL(glGenVertexArrays(1, &vao));
	OGL_CALL(glBindVertexArray(vao));
	OGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, inVBO));
	for (int i = 0; i < inAttributeCount; i++) {
		const char* attributeName = inAttributeNames[i];
		GLint location = glGetAttribLocation(inProgram, attributeName);
		if (location != -1) {
			glEnableVertexAttribArray(location);
			glVertexAttribPointer(location, attributeComponentCount, GL_FLOAT, false, inVertexSizeInBytes, (void*)(sizeof(float) * attributeComponentCount * i));
		}
	}
	OGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	OGL_CALL(glBindVertexArray(0));
	return vao;
}
GLuint BindUBO(GLuint inProgram, const char* inUBOName, GLuint inUBO, GLuint inSlot) {
	GLint location = glGetUniformBlockIndex(inProgram, inUBOName);
	if (location != -1) {
		OGL_CALL(glUniformBlockBinding(inProgram, location, inSlot));
		OGL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, inSlot, inUBO));
		return inSlot + 1;
	}
	return inSlot;
}
Mat4UniformBufferData::Mat4UniformBufferData(int inMat4Count) {
	mMat4Count = inMat4Count;
	mbNeedSyncData = true;
	mUBO = 0;
}
void Mat4UniformBufferData::SetMat4(int inIndex, float* inMat4) {
	int offset = inIndex * 16;
	memcpy(mData + offset, inMat4, sizeof(float) * 16);
	mbNeedSyncData = true;
}
void Mat4UniformBufferData::UpdateGPUData() {
	if (!mbNeedSyncData) {
		return;
	}
	if (mUBO == 0) {
		mUBO = CreateBufferObject(GL_UNIFORM_BUFFER, sizeof(mData), GL_DYNAMIC_DRAW, mData);
	}
	else {
		mbNeedSyncData = false;
		UpdateBufferObject(mUBO, GL_UNIFORM_BUFFER, mData, sizeof(mData), 0);
	}
}
Vec4UniformBufferData::Vec4UniformBufferData(int inVec4Count) {
	mVec4Count = inVec4Count;
	mbNeedSyncData = true;
	mUBO = 0;
}
void Vec4UniformBufferData::SetVec4(int inIndex, float* inVec4) {
	int offset = inIndex * 4;
	memcpy(mData + offset, inVec4, sizeof(float) * 4);
	mbNeedSyncData = true;
}
void Vec4UniformBufferData::UpdateGPUData() {
	if (!mbNeedSyncData) {
		return;
	}
	if (mUBO == 0) {
		mUBO = CreateBufferObject(GL_UNIFORM_BUFFER, sizeof(mData), GL_DYNAMIC_DRAW, mData);
	}
	else {
		mbNeedSyncData = false;
		UpdateBufferObject(mUBO, GL_UNIFORM_BUFFER, mData, sizeof(mData), 0);
	}
}
GLuint CompileShader(GLenum shaderType, const char*shaderCode) {
	GLuint shader = 0;
	OGL_CALL(shader = glCreateShader(shaderType));
	OGL_CALL(glShaderSource(shader, 1, &shaderCode, nullptr));
	OGL_CALL(glCompileShader(shader));
	GLint compileResult = GL_TRUE;
	OGL_CALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult));
	if (compileResult == GL_FALSE) {
		char szLog[10240] = { 0 };
		GLsizei logLen = 0;
		OGL_CALL(glGetShaderInfoLog(shader, 10240, &logLen, szLog));
		printf("Compile Shader fail error log : %s \nshader code :\n%s\n", szLog, shaderCode);
		OGL_CALL(glDeleteShader(shader));
		shader = 0;
	}
	return shader;
}
GLuint CreateProgram(GLuint vsShader, GLuint fsShader) {
	GLuint program = 0;
	OGL_CALL(program=glCreateProgram());
	OGL_CALL(glAttachShader(program, vsShader));
	OGL_CALL(glAttachShader(program, fsShader));
	OGL_CALL(glLinkProgram(program));
	OGL_CALL(glDetachShader(program, vsShader));
	OGL_CALL(glDetachShader(program, fsShader));
	GLint nResult;
	OGL_CALL(glGetProgramiv(program, GL_LINK_STATUS, &nResult));
	if (nResult == GL_FALSE) {
		char log[10240] = { 0 };
		GLsizei writed = 0;
		OGL_CALL(glGetProgramInfoLog(program, 10240, &writed, log));
		printf("create gpu program fail,link error : %s\n", log);
		OGL_CALL(glDeleteProgram(program));
		program = 0;
	}
	return program;
}
GLuint CreateProgramFromFile(const char* inVertexShaderPath, const char* inFragmentShaderPath) {
	int nFileSize = 0;
	const char* vsCode = (char*)LoadFileContent(inVertexShaderPath, nFileSize);
	const char* fsCode = (char*)LoadFileContent(inFragmentShaderPath, nFileSize);
	GLuint vsShader = CompileShader(GL_VERTEX_SHADER, vsCode);
	if (vsShader == 0) {
		printf("Compile vertex shader [%s] failed return <=", inVertexShaderPath);
		return 0;
	}
	GLuint fsShader = CompileShader(GL_FRAGMENT_SHADER, fsCode);
	if (fsShader == 0) {
		printf("Compile fragment shader [%s] failed return <=", inFragmentShaderPath);
		return 0;
	}
	GLuint program = 0;
	OGL_CALL(program = CreateProgram(vsShader, fsShader));
	OGL_CALL(glDeleteShader(vsShader));
	OGL_CALL(glDeleteShader(fsShader));
	return program;
}
GLuint CreateTexture2D(void*pixelData, int width, int height, GLenum gpu_format, GLenum cpu_format, GLenum wrapMode, GLenum minFilter, GLenum magFilter) {
	GLuint texture;
	OGL_CALL(glGenTextures(1, &texture));
	OGL_CALL(glBindTexture(GL_TEXTURE_2D, texture));
	OGL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode));
	OGL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode));
	OGL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter));
	OGL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter));
	OGL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, gpu_format, width, height, 0, cpu_format, GL_UNSIGNED_BYTE, pixelData));
	OGL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
	return texture;
}
void* DecodeImageFile(const char* path, int& width, int& height, int& channelCount) {
	if (strcmp(path + (strlen(path) - 4), ".hdr") == 0) {
		float *pixel = stbi_loadf(path,&width, &height, &channelCount, 0);
		return pixel;
	}
	return stbi_load(path, &width, &height, &channelCount, 0);
}

GLuint CreateTextureFromFile(const char *path, GLenum wrapMode, GLenum minFilter, GLenum magFilter) {
	int image_width, image_height, channel_count;
	void* pixelData = DecodeImageFile(path, image_width, image_height, channel_count);
	GLuint texture=0;
	if (strcmp(path + (strlen(path) - 4), ".hdr") == 0) {
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, image_width, image_height, 0, GL_RGB, GL_FLOAT, pixelData); // note how we specify the texture's data value to be float

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(pixelData);
	} else {
		GLenum pixel_format = channel_count == 3 ? GL_RGB : GL_RGBA;
		texture = CreateTexture2D(pixelData, image_width, image_height, pixel_format, pixel_format, wrapMode, minFilter, magFilter);
		stbi_image_free(pixelData);
	}
	return texture;
}
void SaveTexture2DToFile(const char* inFilePathToSaveTexture2D, GLuint inTexture2D) {
	glBindTexture(GL_TEXTURE_2D, inTexture2D);
	GLint image_width, image_height;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &image_width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &image_height);
	unsigned char* pixel = new unsigned char[image_width * image_height * 4];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
	int channel_count = 4;
	//stbi_write_bmp(inFilePathToSaveTexture2D, image_width, image_height, channel_count, pixel);
	delete[] pixel;
}
GLuint CreateBufferObject(GLenum bufferType, GLsizeiptr size, GLenum usage, void*data /* = nullptr */) {
	GLuint object;
	OGL_CALL(glGenBuffers(1, &object));
	OGL_CALL(glBindBuffer(bufferType, object));
	OGL_CALL(glBufferData(bufferType, size, data, usage));
	OGL_CALL(glBindBuffer(bufferType, 0));
	return object;
}
void UpdateBufferObject(GLuint object, GLenum type, void * data, int size, int offset) {
	OGL_CALL(glBindBuffer(type, object));
	OGL_CALL(glBufferSubData(type, offset, size, data));//cpu -> gpu
	OGL_CALL(glBindBuffer(type, 0));
}