#include "Menu.h"

#include "Settings.h"

#pragma warning(push)
#pragma warning(disable : 4099 5054)
#include "SKSEMenuFramework.h"
#pragma warning(pop)

namespace Menu
{
	namespace
	{
		using namespace ImGuiMCP;

		bool dirty = false;

		void Tip(const char* a_text)
		{
			if (IsItemHovered()) {
				SetTooltip("%s", a_text);
			}
		}

		void __stdcall OnMenuEvent(SKSEMenuFramework::Model::EventType a_type)
		{
			if (a_type == SKSEMenuFramework::Model::kCloseMenu && dirty) {
				Settings::Get().Clamp();
				Settings::Get().Save();
				dirty = false;
			}
		}

		void __stdcall RenderSettings()
		{
			auto& s = Settings::Get();
			bool  c = false;

			SeparatorText("Absorb Knowledge");
			c |= SliderFloat("Range", &s.spellRange, 64.0f, 2048.0f, "%.0f units");
			Tip("Books this close to you are read. About 70 units are 1 metre.");
			c |= SliderInt("Most books per cast", &s.maxBooks, 0, 100, "%d");
			Tip("0 = no limit.");
			c |= Checkbox("Also read books inside containers", &s.includeContainers);
			Tip("Chests, barrels and bookshelves that hold books. Their books count as read from then on.");
			c |= Checkbox("Leave spell tomes alone", &s.skipSpellTomes);
			Tip("Do not learn the spells of spell tomes.");
			c |= Checkbox("Leave quest books alone", &s.skipQuestItems);
			Tip("Books a quest is tracking are left for you to read, so the quest keeps working.");
			c |= Checkbox("Say how many books were read", &s.notify);

			Spacing();
			if (Button("Save now")) {
				s.Clamp();
				s.Save();
				dirty = false;
			}
			SameLine();
			if (Button("Reload from disk")) {
				s.Load();
				dirty = false;
			}
			TextDisabled(dirty ? "Unsaved changes. They are saved when this menu closes." : "Saved to Data/SKSE/Plugins/ReadingIsBad.ini");
			dirty |= c;
		}
	}

	void Register()
	{
		if (!SKSEMenuFramework::IsInstalled()) {
			return;
		}
		SKSEMenuFramework::SetSection("Reading Is Bad");
		SKSEMenuFramework::AddSectionItem("Settings", RenderSettings);
		SKSEMenuFramework::AddEvent(OnMenuEvent, 0.0f);  // never freed on purpose, like the framework's own examples
	}
}
