#pragma once

// Data/SKSE/Plugins/ReadingIsBad.ini. fSpellRange, bSkipSpellTomes and [Log] Debug are the keys of the original Reading Is Bad SKSE.
struct Settings
{
	float spellRange{ 512.0f };         // game units around the player
	bool  skipSpellTomes{ false };      // leave spell tomes alone, do not learn their spell
	bool  skipQuestItems{ true };       // a book a quest is tracking is left for the player to read
	bool  includeContainers{ false };   // also books lying inside chests, barrels, bookshelves...
	int   maxBooks{ 0 };                // per cast, 0 = no limit
	bool  notify{ true };               // say how many books were read
	bool  debugLog{ false };

	static Settings& Get();
	static constexpr const char* kPath = "Data/SKSE/Plugins/ReadingIsBad.ini";
	void Load();
	void Save() const;
	void Clamp();
};
