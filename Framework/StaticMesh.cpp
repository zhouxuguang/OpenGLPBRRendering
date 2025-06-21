#include "StaticMesh.h"
#include "utils.h"
#pragma warning(disable : 4996)
StaticMesh::StaticMesh() {
	mPrimitiveType = GL_TRIANGLES;
	mVBO = 0;
}
void StaticMesh::SetPosition(int index, float x, float y, float z, float w) {
	mVertices[index].mPosition[0] = x;
	mVertices[index].mPosition[1] = y;
	mVertices[index].mPosition[2] = z;
	mVertices[index].mPosition[3] = w;
}
void StaticMesh::SetTexcoord(int index, float x, float y, float z, float w) {
	mVertices[index].mTexcoord[0] = x;
	mVertices[index].mTexcoord[1] = y;
	mVertices[index].mTexcoord[2] = z;
	mVertices[index].mTexcoord[3] = w;
}
void StaticMesh::SetNormal(int index, float x, float y, float z, float w) {
	mVertices[index].mNormal[0] = x;
	mVertices[index].mNormal[1] = y;
	mVertices[index].mNormal[2] = z;
	mVertices[index].mNormal[3] = w;
}
void StaticMesh::LoadFileFile(const char* inFilePath) {
	mVertexFactory = GetVertexFactory();
	FILE* file = fopen(inFilePath, "rb");
	if (file != NULL) {
		int vertice_count;
		fread(&vertice_count, 1, sizeof(int), file);
		mVertexCount = vertice_count;
		mVertices = new StaticMeshVertexData[vertice_count];
		fread(mVertices, 1, sizeof(StaticMeshVertexData) * vertice_count, file);
		mVBO = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(StaticMeshVertexData) * vertice_count, GL_DYNAMIC_DRAW, mVertices);
		while (!feof(file)) {
			int nameLen = 0, indexCount = 0;
			fread(&nameLen, 1, sizeof(int), file);
			if (feof(file)) {
				break;
			}
			char name[256] = { 0 };
			fread(name, 1, sizeof(char) * nameLen, file);
			fread(&indexCount, 1, sizeof(int), file);
			OGLSubMesh* submesh = new OGLSubMesh;
			submesh->mIndexCount = indexCount;
			submesh->mIndexes = new unsigned int[indexCount];
			fread(submesh->mIndexes, 1, sizeof(unsigned int) * indexCount, file);
			submesh->mIBO = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indexCount, GL_DYNAMIC_DRAW, submesh->mIndexes);
			mSubMeshes.insert(std::pair<std::string, OGLSubMesh*>(name, submesh));
			printf("Load StaticMesh [%s] : vertex count[%d] submesh[%s] Index[%d]\n", inFilePath, vertice_count, name, indexCount);
		}
		fclose(file);
	}
}
void StaticMesh::LoadFileFile2(const char* inFilePath) {
	mVertexFactory = GetVertexFactory2();
	FILE* file = fopen(inFilePath, "rb");
	if (file != NULL) {
		int vertice_count;
		fread(&vertice_count, 1, sizeof(int), file);
		mVertexCount = vertice_count;
		mVertices = new StaticMeshVertexDataEx[vertice_count];
		fread(mVertices, 1, sizeof(StaticMeshVertexDataEx) * vertice_count, file);
		mVBO = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(StaticMeshVertexDataEx) * vertice_count, GL_STATIC_DRAW, mVertices);
		while (!feof(file)) {
			int nameLen = 0, indexCount = 0;
			fread(&nameLen, 1, sizeof(int), file);
			if (feof(file)) {
				break;
			}
			char name[256] = { 0 };
			fread(name, 1, sizeof(char) * nameLen, file);
			fread(&indexCount, 1, sizeof(int), file);
			OGLSubMesh* submesh = new OGLSubMesh;
			submesh->mIndexCount = indexCount;
			submesh->mIndexes = new unsigned int[indexCount];
			fread(submesh->mIndexes, 1, sizeof(unsigned int) * indexCount, file);
			submesh->mIBO = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indexCount, GL_STATIC_DRAW, submesh->mIndexes);
			mSubMeshes.insert(std::pair<std::string, OGLSubMesh*>(name, submesh));
			printf("Load StaticMesh [%s] : vertex count[%d] submesh[%s] Index[%d]\n", inFilePath, vertice_count, name, indexCount);
		}
		fclose(file);
	}
}
void StaticMesh::Draw() {
	if (mSubMeshes.empty()) {
		OGL_CALL(glDrawArrays(mPrimitiveType, 0, mVertexCount));
	}
	else {
		std::unordered_map<std::string, OGLSubMesh*>::iterator iter = mSubMeshes.begin();
		for (; iter != mSubMeshes.end(); iter++) {
			OGL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iter->second->mIBO));
			OGL_CALL(glDrawElements(mPrimitiveType, iter->second->mIndexCount, GL_UNSIGNED_INT, 0));
		}
	}
}
void FullScreenQuadMesh::Init() {
	mVertexCount = 4;
	mVertices = new StaticMeshVertexData[4];
	mPrimitiveType = GL_TRIANGLE_STRIP;
	SetPosition(0, -1.0f, -1.0f, 0.0f);
	SetPosition(1, 1.0f, -1.0f, 0.0f);
	SetPosition(2, -1.0f, 1.0f, 0.0f);
	SetPosition(3, 1.0f, 1.0f, 0.0f);
	SetTexcoord(0, 0.0f, 0.0f);
	SetTexcoord(1, 1.0f, 0.0f);
	SetTexcoord(2, 0.0f, 1.0f);
	SetTexcoord(3, 1.0f, 1.0f);
	mVBO = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(StaticMeshVertexData) * mVertexCount, GL_DYNAMIC_DRAW, mVertices);
	mVertexFactory = GetVertexFactory();
}