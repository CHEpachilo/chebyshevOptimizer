#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <string>
#include <format>

#include "chebCosSum.cpp"
#define MAX_POINTS 32

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

static void glfw_error_callback(int error, const char* description) {
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Main code
int WinMain(void*, void*, int, char**) {
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100 (WebGL 1.0)
	const char* glsl_version = "#version 100";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
	// GL ES 3.0 + GLSL 300 es (WebGL 2.0)
	const char* glsl_version = "#version 300 es";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);			// Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);			  // 3.0+ only
#endif

	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(800, 600, "Cosine sum resolver", nullptr, nullptr);
	if (window == nullptr)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	  // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	  // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
	ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
	ImGui_ImplOpenGL3_Init(glsl_version);

	bool show_demo_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Main loop
#ifdef __EMSCRIPTEN__
	// For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
	// You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
	io.IniFilename = nullptr;
	EMSCRIPTEN_MAINLOOP_BEGIN
#else
	while (!glfwWindowShouldClose(window))
#endif
	{
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();
		if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
			ImGui_ImplGlfw_Sleep(10);
			continue;
		}

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		int width, height;
		glfwGetWindowSize(window, &width, &height);
		ImGui::SetNextWindowSize(ImVec2(width, height)); // ensures ImGui fits the GLFW window
		ImGui::SetNextWindowPos(ImVec2(0, 0));

		static ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove			|
										ImGuiWindowFlags_NoResize		|
										ImGuiWindowFlags_NoCollapse		|
										ImGuiWindowFlags_NoDecoration;

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		{
			static int size = 2;
			static double inputs[MAX_POINTS] = {};
			static double outputs[MAX_POINTS] = {};

			ImGui::Begin("Main area", 0, flags);
			
			if (ImGui::Button("+")) {
				size = (size < MAX_POINTS) ? size + 1: size;
				chebCosSum (size, inputs, outputs);
			}
			ImGui::SameLine();
			if (ImGui::Button("-")) {
				size = (size > 2) ? size - 1 : size;
				chebCosSum (size, inputs, outputs);
			}
			ImGui::SameLine();
			if (size < 10)
				ImGui::Text("size = %d   ", size);
			else
				ImGui::Text("size = %d  ", size);

			ImGui::SameLine();
			ImGui::Text("Demos:");

			ImGui::SameLine();
			if (ImGui::Button("\"Good enough\" Blackman window")) {
				size = 3;
				inputs [0] = 0.42;
				inputs [1] = -0.5;
				inputs [2] = 0.08;
				chebCosSum (size, inputs, outputs);
			}
			ImGui::SameLine();
			if (ImGui::Button("Blackman-Nutall window")) {
				size = 4;
				inputs [0] = 3.635819267707608e-001;
				inputs [1] = -4.891774371450171e-001;
				inputs [2] = 1.365995139786921e-001;
				inputs [3] = -1.064112210553003e-002;
				chebCosSum (size, inputs, outputs);
			}
			ImGui::SameLine();
			if (ImGui::Button("Blackman-Harris window")) {
				size = 4;
				inputs [0] = 3.58750287312166e-001;
				inputs [1] = -4.88290107472600e-001;
				inputs [2] = 1.41279712970519e-001;
				inputs [3] = -1.16798922447150e-002;
				chebCosSum (size, inputs, outputs);
			}

			ImGui::SameLine();
			if (ImGui::Button("Clear")) {
				for (int i = 0; i < MAX_POINTS; i++) {
					inputs[i] = 0;
					outputs[i] = 0;
				}
			}

			ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
			if (ImGui::BeginTabBar("Input format", tab_bar_flags)) {
				if (ImGui::BeginTabItem("Default")) {
					ImGui::Text("Default input method\n(ex. 123.45467)");
					for (int i = 0; i < size; i++) {
						std::string ss;
						if (i > 1) {
							ss = std::format("cos({0:}x)", i);
						}
						else if (i == 1) {
							ss = "cos(x)";
						}
						else {
							ss = "scalar";
						}
						if(ImGui::InputDouble(ss.c_str(), &inputs[i], 0.0, 0.0, "%.20lf")) {
							chebCosSum (size, inputs, outputs);
						}
					}
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Scientific")) {
					ImGui::Text("Exponential input\n(ex. 1.234e-567)");
					for (int i = 0; i < size; i++) {
						std::string ss;
						if (i > 1) {
							ss = std::format("cos({0:}x)", i);
						}
						else if (i == 1) {
							ss = "cos(x)";
						}
						else {
							ss = "scalar";
						}
						if(ImGui::InputDouble(ss.c_str(), &inputs[i], 0.0, 0.0, "%e")) {
							chebCosSum (size, inputs, outputs);
						}
					}
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}

			ImGui::Separator();

			ImGui::Text("Calculated coefficients:");
			for (int i = 0; i < size; i++) {
				std::string ss;
				if (i > 1) {
					ss = std::format("cos(x)^{0:}", i);
				}
				else if (i == 1) {
					ss = "cos(x)";
				}
				else {
					ss = "scalar";
				}
				ImGui::InputDouble(ss.c_str(), &outputs[i], 0.0, 0.0, "%.20lf", ImGuiInputTextFlags_ReadOnly);
			}

			ImGui::Separator();

			ImGui::Text("Typical usage:");
			//Generating code example
			std::string ss;
			ss = "double window_function (const double x) {\n\tconst double c = std::cos(2.0 * pi * x);\n\treturn ";
			for (int i = 1; i < size; i++)
				ss += "(";
			ss += std::format(" {:.20f}", outputs[size - 1]);
			for (int i = size - 2; i >= 0; i--) {
				if (outputs[i] >= 0)
					ss += std::format("\n\t\t\t\t* c + {:.20f} )", outputs[i]);
				else
					ss += std::format("\n\t\t\t\t* c - {:.20f} )", -outputs[i]);
			}
			ss += ";\n}";

			char* txt = strdup(ss.c_str());
			ImGui::InputTextMultiline("##source", txt, strlen(txt), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_ReadOnly);
			//ImGui::Checkbox("Demo Window", &show_demo_window);			// Edit bools storing our window open/close state
			ImGui::End();
			delete [] txt;
		}

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}
#ifdef __EMSCRIPTEN__
	EMSCRIPTEN_MAINLOOP_END;
#endif

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
