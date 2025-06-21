#include "scene.h"
#include "ggl.h"

void Init() {
	//init runtime assets
}
void SetViewPortSize(int inWidth, int inHeight) {
	//rt
}
void Draw() {
	//hdr rendering pipeline
	glClearColor(0.1f, 0.4f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}