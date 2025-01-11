#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"

#include "Walnut/Timer.h"

#include "Renderer.h"


using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	float r = 1, g = 1, b = 1, a = 1, x, z = -1.0, y;
	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Sphere Color");
		ImGui::SliderFloat("Set Red", &r, 0.0f, 1.0f);
		ImGui::SliderFloat("Set Green", &g, 0.0f, 1.0f);
		ImGui::SliderFloat("Set Blue", &b, 0.0f, 1.0f);
		ImGui::SliderFloat("Set Trans", &a, 0.0f, 1.0f);

		ImGui::Text("Lighting Direction");
		ImGui::SliderFloat("X", &x, -1.0f, 1.0f);
		ImGui::SliderFloat("Y", &y, -1.0f, 1.0f);
		ImGui::SliderFloat("Z", &z, -1.0f, 1.0f);
		glm::vec4 sphereColor(r, g, b, a);
		glm::vec3 lightDir = glm::normalize(glm::vec3(x,y,z));
		if (ImGui::Button("Reset Values"))
		{
			r = 1;
			g = 1;
			b = 1;
			a = 1;
			x = 0;
			y = 0;
			z = -1;
			//Render(sphereColor);
		}
		ImGui::Text("Last Render: %.3fms", m_LastRenderTime);
		
		
		
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");
		
		m_ViewPortWidth = ImGui::GetContentRegionAvail().x;
		m_ViewPortHeight = ImGui::GetContentRegionAvail().y;

		auto image = m_Renderer.GetFinalImage();
		if (image)
		{
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() }, 
				ImVec2(0,1), ImVec2(1,0));
		}
		

		ImGui::End();
		ImGui::PopStyleVar();

		
		Render(sphereColor, lightDir);
	}

	void Render(glm::vec4 rgba, glm::vec3 lightDir) {
		Timer timer;

		m_Renderer.OnResize(m_ViewPortWidth, m_ViewPortHeight);
		m_Renderer.Render(rgba, lightDir);

		m_LastRenderTime = timer.ElapsedMillis();
	}


	void Render()
	{
		Timer timer;

		m_Renderer.OnResize(m_ViewPortWidth, m_ViewPortHeight);
		m_Renderer.Render();

		m_LastRenderTime = timer.ElapsedMillis();
	}
private:
	Renderer m_Renderer;
	Renderer n_Renderer;
	uint32_t m_ViewPortWidth = 0, m_ViewPortHeight = 0;
	float m_LastRenderTime = 0.0f;
	float n_LastRenderTime = 0.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}