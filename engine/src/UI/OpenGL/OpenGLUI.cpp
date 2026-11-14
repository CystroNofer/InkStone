#include "pch.h"

#include "OpenGLUI.h"
#include "ECS/Scene.h"
#include <imgui.h>
#include <cmath>

#include "Input/Keycode.h"
#include "Event/ApplicationEvent.h"
#include "Event/KeyEvent.h"
#include "Event/MouseEvent.h"

namespace NXTN {
	OpenGLUI::OpenGLUI(unsigned int w, unsigned int h)
	{
		ImGui::CreateContext();

		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking

		io.DisplaySize = ImVec2(w, h);

		if (io.BackendRendererUserData == nullptr)
		{
			ImGui_ImplOpenGL3_Init("#version 450 core");
		}

		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;  // Enable cursor functionalities defined by backend
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;  // Allow manually setting cursor position

		// Per ImGui documentation, the docking branch of ImGui seems to not accept custom KeyMap
		//io.KeyMap[ImGuiMouseButton_Left] = NXTN_MOUSE_BUTTON_LEFT;
		//io.KeyMap[ImGuiMouseButton_Right] = NXTN_MOUSE_BUTTON_RIGHT;
		//io.KeyMap[ImGuiMouseButton_Middle] = NXTN_MOUSE_BUTTON_MIDDLE;
		//io.KeyMap[ImGuiKey_Tab] = NXTN_KEY_TAB;
		//io.KeyMap[ImGuiKey_LeftArrow] = NXTN_KEY_LEFT;
		//io.KeyMap[ImGuiKey_RightArrow] = NXTN_KEY_RIGHT;
		//io.KeyMap[ImGuiKey_UpArrow] = NXTN_KEY_UP;
		//io.KeyMap[ImGuiKey_DownArrow] = NXTN_KEY_DOWN;
		//io.KeyMap[ImGuiKey_PageUp] = NXTN_KEY_PAGE_UP;
		//io.KeyMap[ImGuiKey_PageDown] = NXTN_KEY_PAGE_DOWN;
		//io.KeyMap[ImGuiKey_Home] = NXTN_KEY_HOME;
		//io.KeyMap[ImGuiKey_End] = NXTN_KEY_END;
		//io.KeyMap[ImGuiKey_Insert] = NXTN_KEY_INSERT;
		//io.KeyMap[ImGuiKey_Delete] = NXTN_KEY_DELETE;
		//io.KeyMap[ImGuiKey_Backspace] = NXTN_KEY_BACKSPACE;
		//io.KeyMap[ImGuiKey_Space] = NXTN_KEY_SPACE;
		//io.KeyMap[ImGuiKey_Enter] = NXTN_KEY_ENTER;
		//io.KeyMap[ImGuiKey_Escape] = NXTN_KEY_ESCAPE;
		//io.KeyMap[ImGuiKey_A] = NXTN_KEY_A;  // Map for Ctrl+A
		//io.KeyMap[ImGuiKey_C] = NXTN_KEY_C;  // Map for Ctrl+C
		//io.KeyMap[ImGuiKey_V] = NXTN_KEY_V;  // Map for Ctrl+V
		//io.KeyMap[ImGuiKey_X] = NXTN_KEY_X;  // Map for Ctrl+X
		//io.KeyMap[ImGuiKey_Y] = NXTN_KEY_Y;  // Map for Ctrl+Y
		//io.KeyMap[ImGuiKey_Z] = NXTN_KEY_Z;  // Map for Ctrl+Z
	}

	void OpenGLUI::NewFrameImpl()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();
	}

	void OpenGLUI::EndFrameImpl()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	std::optional<Entity> OpenGLUI::HierarchyTreeImpl(SceneHierarchyView hierarchy, std::optional<Entity> selected)
	{
		std::vector<Entity> roots = hierarchy.GetRoots();
		if (roots.empty()) {
			ImGui::TextUnformatted("No entity in hierarchy");
			return selected;
		}

		// Explicit traversal stack avoids recursion for deeply nested scenes
		struct TraversalFrame {
			std::span<const Entity> entities;
			size_t next = 0;
		};
		std::vector<TraversalFrame> pending{ { std::span<const Entity>(roots) } };
		ImGui::PushID(hierarchy.GetIdentity());
		while (!pending.empty()) {
			if (pending.back().next == pending.back().entities.size()) {
				pending.pop_back();
				if (!pending.empty()) { ImGui::Unindent(); }
				continue;
			}
			Entity entity = pending.back().entities[pending.back().next++];
			std::span<const Entity> children = hierarchy.GetChildren(entity);
			std::string_view name = hierarchy.GetName(entity);
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			if (children.empty()) { flags |= ImGuiTreeNodeFlags_Leaf; }
			if (selected && *selected == entity) { flags |= ImGuiTreeNodeFlags_Selected; }

			ImGui::PushID(static_cast<int>(entity.GetID()));
			ImGui::PushID(static_cast<int>(entity.GetGeneration()));
			bool expanded = ImGui::TreeNodeEx("entity", flags, "%.*s", static_cast<int>(name.size()), name.data());
			if (ImGui::IsItemClicked()) { selected = entity; }
			ImGui::PopID();
			ImGui::PopID();
			if (expanded && !children.empty()) {
				pending.push_back({ children });
				ImGui::Indent();
			}
		}
		ImGui::PopID();
		return selected;
	}

	bool OpenGLUI::TransformEditorImpl(Transform& draft)
	{
		static bool s_UniformScale = false;

		bool changed = false;
		float position[3] = { draft.position.x, draft.position.y, draft.position.z };
		if (ImGui::DragFloat3("Position", position, 0.05f) &&
			std::isfinite(position[0]) &&
			std::isfinite(position[1]) &&
			std::isfinite(position[2]))
		{
			draft.position = vec3(position[0], position[1], position[2]);
			changed = true;
		}

		float lengthSquared =
			draft.rotation.x * draft.rotation.x +
			draft.rotation.y * draft.rotation.y +
			draft.rotation.z * draft.rotation.z + 
			draft.rotation.w * draft.rotation.w;
		if (std::isfinite(lengthSquared) && lengthSquared > EPSILON_SQ) {
			float inverseLength = 1.0f / std::sqrt(lengthSquared);
			float x = draft.rotation.x * inverseLength;
			float y = draft.rotation.y * inverseLength;
			float z = draft.rotation.z * inverseLength;
			float w = draft.rotation.w * inverseLength;
			// XYZ Euler display uses R = Rz * Ry * Rx
			vec3 degrees = vec3(
				std::atan2(2.0f * (w * x + y * z), 1.0f - 2.0f * (x * x + y * y)),
				std::asin(std::clamp(2.0f * (w * y - z * x), -1.0f, 1.0f)),
				std::atan2(2.0f * (w * z + x * y), 1.0f - 2.0f * (y * y + z * z))
			) * NXTN_RAD_2_DEG_FLOAT;
			ImGui::Text("Rotation (Euler, read-only): %.1f, %.1f, %.1f", degrees.x, degrees.y, degrees.z);
		}
		else { ImGui::TextUnformatted("Rotation: invalid quaternion"); }

		float scale[3] = { draft.scale.x, draft.scale.y, draft.scale.z };
		if (ImGui::DragFloat3("Scale", scale, 0.05f) &&
			std::isfinite(scale[0]) &&
			std::isfinite(scale[1]) &&
			std::isfinite(scale[2]))
		{
			if (s_UniformScale) {
				if (scale[0] != draft.scale.x) { scale[1] = scale[2] = scale[0]; }
				else if (scale[1] != draft.scale.y) { scale[0] = scale[2] = scale[1]; }
				else if (scale[2] != draft.scale.z) { scale[0] = scale[1] = scale[2]; }
			}
			draft.scale = vec3(scale[0], scale[1], scale[2]);
			changed = true;
		}
		ImGui::SameLine();
		ImGui::Checkbox("Uniform Scale", &s_UniformScale);
		return changed;
	}

	void OpenGLUI::OnEventImpl(Event*& event_ptr)
	{
		ImGuiIO& io = ImGui::GetIO();

		switch (event_ptr->GetEventType())
		{
		case EventType::WindowResized:
		{
			WindowResizeEvent e = *((WindowResizeEvent*)event_ptr);
			io.DisplaySize = ImVec2(e.GetNewWidth(), e.GetNewHeight());
			break;
		}
		case EventType::KeyPressed:
			break;
		case EventType::KeyReleased:
			break;
		case EventType::MouseButtonPressed:
		{
			int mouseBtnCode = (int)(((MouseButtonPressEvent*)event_ptr)->GetButton());
			io.MouseDown[mouseBtnCode] = true;
			break;
		}
		case EventType::MouseButtonReleased:
		{
			int mouseBtnCode = (int)(((MouseButtonPressEvent*)event_ptr)->GetButton());
			io.MouseDown[mouseBtnCode] = false;
			break;
		}
		case EventType::MouseMove:
		{
			MouseMoveEvent e = *(MouseMoveEvent*)event_ptr;
			io.MousePos = ImVec2(e.GetX(), e.GetY());
			break;
		}
		case EventType::MouseScroll:
		{
			MouseScrollEvent e = *(MouseScrollEvent*)event_ptr;
			io.MouseWheelH += e.GetX();
			io.MouseWheel += e.GetY();
			break;
		}
		}

		return;
	}
}
