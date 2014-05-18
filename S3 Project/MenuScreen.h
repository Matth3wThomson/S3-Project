#pragma once

#include "Screen.h"
#include <list>
#include "RenderObject.h"


class MenuScreen :
	public Screen
{
public:
	MenuScreen(SRenderer& target, bool loadRes, Shader* s) : Screen(target, loadRes, s){ };
	~MenuScreen(void){ };

	virtual void RenderScreen(){
		for (list<RenderObject*>::const_iterator i = UIElements.begin(); i != UIElements.end(); ++i){
			(*i)->Render(target);
		}
	};

	virtual void LoadResources() = 0;

	virtual void DeleteResources(){

		list<RenderObject*>::iterator i = UIElements.begin();
		while (i != UIElements.end()){

			delete *i;
			UIElements.erase(i);
		}	
	};

	virtual void Update() = 0;

protected:
	list<RenderObject*> UIElements;
	float PADDING;
};

