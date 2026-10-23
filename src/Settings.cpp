#include "Settings.h"

Settings& Settings::Get()
{
	static Settings instance;
	return instance;
}

void Settings::Clamp()
{
	spellRange = std::clamp(spellRange, 32.0f, 4096.0f);
	maxBooks = std::clamp(maxBooks, 0, 500);
}

void Settings::Load()
{
	CSimpleIniA ini;
	ini.SetUnicode();
	ini.LoadFile(kPath);

	spellRange = static_cast<float>(ini.GetDoubleValue("General", "fSpellRange", spellRange));
	skipSpellTomes = ini.GetBoolValue("General", "bSkipSpellTomes", skipSpellTomes);
	skipQuestItems = ini.GetBoolValue("General", "bSkipQuestItems", skipQuestItems);
	includeContainers = ini.GetBoolValue("General", "bIncludeContainers", includeContainers);
	maxBooks = static_cast<int>(ini.GetLongValue("General", "iMaxBooksPerCast", maxBooks));
	notify = ini.GetBoolValue("General", "bNotify", notify);
	debugLog = ini.GetBoolValue("Log", "Debug", debugLog);
	Clamp();
}

void Settings::Save() const
{
	CSimpleIniA ini;
	ini.SetUnicode();
	ini.LoadFile(kPath);

	ini.SetDoubleValue("General", "fSpellRange", spellRange, "; radius around the player, in game units (about 70 units = 1 m)");
	ini.SetBoolValue("General", "bSkipSpellTomes", skipSpellTomes);
	ini.SetBoolValue("General", "bSkipQuestItems", skipQuestItems, "; leave books a quest is tracking for the player to read");
	ini.SetBoolValue("General", "bIncludeContainers", includeContainers, "; also read books lying inside chests, barrels and bookshelves");
	ini.SetLongValue("General", "iMaxBooksPerCast", maxBooks, "; 0 = no limit");
	ini.SetBoolValue("General", "bNotify", notify);
	ini.SetBoolValue("Log", "Debug", debugLog);

	std::error_code ec;
	std::filesystem::create_directories(std::filesystem::path(kPath).parent_path(), ec);
	if (ini.SaveFile(kPath) < 0) {
		logger::error("Could not write {}", kPath);
	}
}
