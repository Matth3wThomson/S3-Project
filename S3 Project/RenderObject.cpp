#include "RenderObject.h"


RenderObject::RenderObject(void){
	mesh	= NULL;
	shader	= NULL;
	texture = NULL;
	parent  = NULL;
}

RenderObject::RenderObject(Mesh*m, Shader*s, GLuint t){
	mesh	= m;
	shader	= s;
	texture = t;
	parent  = NULL;
}


RenderObject::~RenderObject(void)
{
}


void RenderObject::Update(float msec) {
	//Acquire a write lock here
	Lock();

	//Parent should already be locked if this resolves to true
	if(parent) {
		worldTransform = parent->modelMatrix * modelMatrix;
	}
	else {
		worldTransform = modelMatrix;
	}

	//Update all children
	for(vector<RenderObject*>::const_iterator i = children.begin(); i != children.end(); ++i ) {
		(*i)->Update(msec);
	}

	//release the write lock
	Unlock();
}

void RenderObject::Draw() const {
	//Method is protected, so locks not necessary
	if(mesh) {
		mesh->Draw();
	}
}

void RenderObject::Render(const SRenderer& r){

	glDisable(GL_CULL_FACE);

	//Acquire a Read lock
	ReadLock();

	//Draw the RenderObject
	if(shader && mesh){
		GLuint program = shader->GetShaderProgram();
		glUseProgram(program);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		UpdateShaderMatrices(program, r);
		//glUniform1i(glGetUniformLocation(program, "heightMap"), 0);

		Draw();
	}

	//Draw all of the children (GET CHILDREN IS TRYING TO REACQUIRE THE READ LOCK!)
	for(vector<RenderObject*>::const_iterator i = children.begin(); i != children.end(); ++i ) {
		(*i)->Render(r);
	}

	//Release read lock
	ReadUnlock();
}

void RenderObject::UpdateShaderMatrices(const GLuint& program, const SRenderer& r){
	//Send the world transform matrix... not the model matrix! (world transform is compiled model matricies)
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix")  , 1, false, (float*)&worldTransform);
	glUniformMatrix4fv(glGetUniformLocation(program, "viewMatrix")   , 1, false, (float*)&r.GetView());
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix")   , 1, false, (float*)&r.GetProj());
}