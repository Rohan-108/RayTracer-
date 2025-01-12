#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"
#include "camera.h"
#include "global.h"
#include "hittable_list.h"
#include "sphere.h"
#include "material.h"
using namespace Walnut;
class ExampleLayer : public Walnut::Layer
{
public:
	hittable_list world;
	camera cam;
	void initialize(){
		auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
		world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));
		auto material1 = make_shared<dielectric>(1.5);
		for (int a = -11; a < 11; a++) {
			for (int b = -11; b < 11; b++) {
				auto choose_mat = random_double();
				point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

				if ((center - point3(4, 0.2, 0)).length() > 0.9) {
					shared_ptr<material> sphere_material;

					if (choose_mat < 0.8) {
						// diffuse
						auto albedo = color::random() * color::random();
						sphere_material = make_shared<lambertian>(albedo);
						world.add(make_shared<sphere>(center, 0.2, sphere_material));
					}
					else if (choose_mat < 0.95) {
						// metal
						auto albedo = color::random(0.5, 1);
						auto fuzz = random_double(0, 0.5);
						sphere_material = make_shared<metal>(albedo, fuzz);
						world.add(make_shared<sphere>(center, 0.2, sphere_material));
					}
					else {
						//                // glass
						sphere_material = make_shared<dielectric>(1.5);
						world.add(make_shared<sphere>(center, 0.2, sphere_material));
					}
				}
			}
		}

		world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

		auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
		world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

		auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
		world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));
		//camera control
		cam.aspect_ratio = 16.0/ 9.0;
		cam.samples_per_pixel = 10;
		cam.max_depth = 8;

		cam.vfov = 20;
		cam.lookfrom = point3(13, 2, 3);
		cam.lookat = point3(0, 0, 0);
		cam.vup = vec3(0, 1, 0);

		cam.defocus_angle = 0.6;
		cam.focus_dist = 10.0;
	}
	virtual void OnUIRender() override
	{
		ImGui::Begin("settings");
		ImGui::Text("Last Render Time: %.3f ms", m_LastRenderTime);
		if (ImGui::Button("Render")) {
			Render();
		}
		ImGui::End();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("ViewPort");
		m_ViewPortWidth = ImGui::GetContentRegionAvail().x;
		m_ViewPortHeight= ImGui::GetContentRegionAvail().y;
		auto image = cam.getImage();
		if (image) {
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(),(float)image->GetHeight() });
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}
	void Render() {
		Timer timer;
		initialize();
		cam.onResize(m_ViewPortWidth,m_ViewPortHeight);
		cam.render(world);
		m_LastRenderTime = timer.ElapsedMillis();
	}	
private:
	uint32_t m_ViewPortWidth, m_ViewPortHeight;
	float m_LastRenderTime = 0.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "RayTracing";

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