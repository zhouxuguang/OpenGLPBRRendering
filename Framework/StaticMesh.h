#pragma once
#include "ggl.h"
#include "utils.h"
class StaticMesh {
public:
	const VertexFactory* mVertexFactory;
	StaticMeshVertexData* mVertices;
	int mVertexCount;
	GLuint mVBO;
	std::unordered_map<std::string, OGLSubMesh*> mSubMeshes;
	GLenum mPrimitiveType;
	StaticMesh();
	void SetPosition(int index, float x, float y, float z, float w = 1.0f);
	void SetTexcoord(int index, float x, float y, float z=1.0f, float w = 1.0f);
	void SetNormal(int index, float x, float y, float z, float w = 0.0f);
	void LoadFileFile(const char* inFilePath);
	void LoadFileFile2(const char* inFilePath);
	virtual void Draw();
};
class FullScreenQuadMesh : public StaticMesh {
public:
	void Init();
};