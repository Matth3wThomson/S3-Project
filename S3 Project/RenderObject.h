#pragma once


#include "Shader.h"
#include "Mesh.h"
#include "../nclgl/Matrix4.h"
#include "SRenderer.h"
#include "Settings.h"

//Thread Safety
#include <boost/interprocess/sync/interprocess_upgradable_mutex.hpp>
#include <boost/interprocess/sync/interprocess_sharable_mutex.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>

using namespace boost::interprocess;

class SRenderer;

class RenderObject	{
public:
	RenderObject(void);
	RenderObject(Mesh*m, Shader*s, GLuint t = 0);
	~RenderObject(void);

	//Get Methods (Ensure correct when retrieved
	Mesh*	GetMesh()	const {
		sharable_lock<interprocess_upgradable_mutex> lock(mutex);
		return mesh;
	}

	Shader* GetShader()		const		{
		sharable_lock<interprocess_upgradable_mutex> lock(mutex);
		return shader;
	}

	GLuint	GetTexture()		const	{
		sharable_lock<interprocess_upgradable_mutex> lock(mutex);
		return texture;
	}

	Matrix4 GetModelMatrix()	const	{
		sharable_lock<interprocess_upgradable_mutex> lock(mutex);
		return modelMatrix;
	}

	Matrix4 GetWorldTransform() const {
		sharable_lock<interprocess_upgradable_mutex> lock(mutex);
		return worldTransform;
	};

	//NOT THREAD SAFE!
	const vector<RenderObject*>& GetChildren() const  {
		sharable_lock<interprocess_upgradable_mutex> lock(mutex);
		return children;
	};

	//Set Methods (Ensure unique access)
	void	SetMesh(Mesh*m){
		scoped_lock<interprocess_upgradable_mutex> lock(mutex);
		mesh = m;
	}

	
	void	SetShader(Shader*s){
		scoped_lock<interprocess_upgradable_mutex> lock(mutex);
		shader = s;
	}

	void	SetTexture(GLuint tex){
		scoped_lock<interprocess_upgradable_mutex> lock(mutex);
		texture = tex;
	}

	void	SetModelMatrix(const Matrix4& mat){
		scoped_lock<interprocess_upgradable_mutex> lock(mutex);
		modelMatrix = mat;
	}

	virtual void Update(float msec);

	virtual void Render(const SRenderer& r);
	virtual void UpdateShaderMatrices(const GLuint& proogram, const SRenderer& r);

	void	AddChild(RenderObject* child){
		scoped_lock<interprocess_upgradable_mutex> lock(mutex);
		children.push_back(child);
		child->parent = this;
	};

protected:
	//Object properties
	Mesh*	mesh;
	Shader*	shader;

	GLuint	texture;

	Matrix4 modelMatrix;
	Matrix4 worldTransform;

	//RenderObject children
	RenderObject*			parent;
	vector<RenderObject*>	children;

	//Protected Draw Method
	virtual void Draw() const;

	//Thread safety
	mutable interprocess_upgradable_mutex mutex;

	inline void LockUpgradable(){
		mutex.lock_upgradable();
	}

	inline void UnlockUpgradable(){
		mutex.unlock_upgradable();
	}

	inline void UpgradeLock(){
		mutex.unlock_upgradable_and_lock();
	}

	inline void DowngradeLock(){
		mutex.unlock_and_lock_upgradable();
	}

	inline void ReadLock(){
		mutex.lock_sharable();
	}

	inline void ReadUnlock(){
		mutex.unlock_sharable();
	}

	inline void Lock(){
		mutex.lock();
	}

	inline void Unlock(){
		mutex.unlock();
	}
};

