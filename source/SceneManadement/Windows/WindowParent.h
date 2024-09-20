#pragma once
#include <imgui.h>
#include <SceneManadement/Scene.h>

class WindowParent {
protected:
	const char* name = "Transform Surface with sceneObject_2";
public:

	bool open = false;

	void CreateMyWindow(Scene* scene) {
		if (!open)
			return;

		ImGui::Begin(name, &open);
		{
			Interferes(scene);
		}
		ImGui::End();
	}

	virtual void Interferes(Scene* scene) = 0;
};