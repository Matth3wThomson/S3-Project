#include "HealthHUD.h"


HealthHUD::HealthHUD(Shader* shader, const Vector2& position, const Vector2& size)
{
	this->position = Matrix4::Translation(Vector3(position.x, position.y, 0.0f));
	this->size = size;

	redBar = new RenderObject(Mesh::GenerateQuad(Vector4(1.0f, 0.0f, 0.0f, 1.0f)), shader);
	greenBar = new RenderObject(Mesh::GenerateQuad(Vector4(0.0f, 1.0f, 0.0f, 1.0f)), shader);

	//Needs testing!
	//Red bar at the back
	redBar->SetModelMatrix(this->position
		* Matrix4::Scale(Vector3(size.x, size.y, 0)));
	redBar->Update(0.1f);

	//Green bar at the front!
	greenBar->SetModelMatrix(this->position
		* Matrix4::Scale(Vector3(size.x, size.y, 1)));
	greenBar->Update(0.1f);

}


HealthHUD::~HealthHUD(void)
{

}

void HealthHUD::Update(const FreqCircle& fq){
	float health = fq.GetHealth();

	//Scale the green bar appropriately according to health.
	greenBar->SetModelMatrix(position * Matrix4::Scale(Vector3(size.x * (health * 1/Settings::MAX_HEALTH), size.y, 1)));
	greenBar->Update(0.1f);
}

void HealthHUD::Render(const SRenderer& ortho){
	greenBar->Render(ortho);
	redBar->Render(ortho);
}
