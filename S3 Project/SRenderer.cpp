#include "SRenderer.h"

SRenderer::SRenderer(){
	viewMatrix = Matrix4::Translation(Vector3(0,0,-10));

	width = 1280.0f;
	height = 720.0f;

	//Assume 16:9 aspect ratio!
	projMatrix = Matrix4::Perspective(1.0f,1000.0f,
		width/height,45.0f);
}

SRenderer::SRenderer(float w, float h, bool ortho){
	this->width = w;
	this->height = h;

	if (ortho){

		viewMatrix.ToIdentity();
		projMatrix = Matrix4::Orthographic(-1, 1, width, 0, 0, height);
	} else {

		viewMatrix = Matrix4::Translation(Vector3(0,0,-10));

		//Assume 16:9 aspect ratio!
		projMatrix = Matrix4::Perspective(1.0f,1000.0f,
			width/height,45.0f);

	}
}

SRenderer::SRenderer(const SRenderer& rhs){
	this->viewMatrix = rhs.GetView();
	this->projMatrix = rhs.GetProj();
}

SRenderer::~SRenderer(void)
{

}
