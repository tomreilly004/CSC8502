#include "../nclgl/window.h"
#include "Renderer.h"

int main()	{

	string notLOTR = "Definitely Not LOTR";
	Window w(notLOTR, 1280, 720, false);
	
	if(!w.HasInitialised()) {
		return -1;
	}
	
	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		return -1;
	}
	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while(w.UpdateWindow()  && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		renderer.RenderScene();
		renderer.SwapBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
			Shader::ReloadAllShaders();
		}
		float fps = w.GetTimer()->GetTimeDeltaSeconds();
		w.SetTitle(notLOTR + " fps: " + std::to_string(1/fps));
	}
	return 0;
}