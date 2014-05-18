#pragma once

#include "RenderObject.h"
#include "Singleton.h"

//Thread Safety
#include <boost/interprocess/sync/interprocess_sharable_mutex.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>

using namespace boost::interprocess;

class RenderObject;

class SRenderer
{
public:
	SRenderer(float width, float height, bool ortho);
	SRenderer(const SRenderer& rhs);
	~SRenderer(void);

	inline float GetWidth() const {
		sharable_lock<interprocess_sharable_mutex> lock(mutex);
		return width;
	}

	inline float GetHeight() const {
		sharable_lock<interprocess_sharable_mutex> lock(mutex);
		return height;
	}

	inline Matrix4 GetProj() const {
		//Scoped sharable lock
		sharable_lock<interprocess_sharable_mutex> lock(mutex);
		return projMatrix;
	}

	inline Matrix4 GetView() const {
		//Scoped sharable lock
		sharable_lock<interprocess_sharable_mutex> lock(mutex);
		return viewMatrix;
	}

	inline void SetProjectionMatrix(Matrix4 m){
		//Scoped write lock
		scoped_lock<interprocess_sharable_mutex> lock(mutex);
		projMatrix = m;
	}

	inline void SetViewMatrix(Matrix4 m){ 
		//Scoped write lock
		scoped_lock<interprocess_sharable_mutex> lock(mutex);
		viewMatrix = m; 
	}

	inline void LockRendering(){
		mutex.lock();
	}

	inline void UnlockRendering(){
		mutex.unlock();
	}

protected:
	SRenderer(void);

	Matrix4 viewMatrix;
	Matrix4 projMatrix;

	float width, height;

	mutable interprocess_sharable_mutex mutex;
};

