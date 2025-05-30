#include "pch.h"


Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification specification;
	specification.Name = "monsters";

	Walnut::Application* application = new Walnut::Application(specification);

	application->PushLayer<menulayer>();
	application->PushLayer<playlayer>();

	return application;
}