#include "pch.h"

#include "Application.h"

#include "GLAD/glad.h"

namespace NXTN {
	Application* Application::s_Instance = nullptr;

	Application::Application() : m_Alive(true), m_Minimized(false)
	{
		NXTN_PROFILE_NEXT(60);
		NXTN_PROFILE_FUNCTION()

		if (s_Instance) Log::Warning("Duplicated Application: Application should be unique");

		s_Instance = this;

		m_Window.reset(Window::Create());
		//m_Window->SetEventCallback(std::bind(&Application::OnWindowEvent, this, std::placeholders::_1));

		Renderer::Init();

		Time::InitTime();

		//Input::Init(m_Window->GetNativeWindow());

		UI::Init(m_Window);
	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		while (m_Alive)
		{
			Update();
		}
	}

	void Application::Update()
	{
		NXTN_PROFILE_FUNCTION()

		EventBuffer::PushEvent(new ApplicationUpdateEvent());

		Time::Update();

		m_LayerStack.Update();

		UI::NewFrame();
		{
			m_LayerStack.UIUpdate();
		}
		UI::EndFrame();

		m_Window->Update();

		for (Event*& event_ptr : EventBuffer::GetEventBuffer())
		{
			switch (event_ptr->GetEventType())
			{
			case EventType::WindowClosed:
			{
				m_Alive = false;
				return;
			}
			case EventType::WindowResized:
			{
				WindowResizeEvent e = *(WindowResizeEvent*)(event_ptr);
				//Renderer::ResizeViewport(e.GetNewWidth(), e.GetNewHeight());
				m_Minimized = e.GetNewWidth() < 1 || e.GetNewHeight() < 1;
				break;
			}
			}

			m_LayerStack.OnEvent(event_ptr);

			UI::OnEvent(event_ptr);
		}

		EventBuffer::Clear();
	}

	//void Application::OnWindowEvent(Event& event)
	//{
	//	EventBuffer::PushEvent(event);
	//}
}