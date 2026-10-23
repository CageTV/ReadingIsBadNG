#include "Menu.h"
#include "Settings.h"

namespace
{
	constexpr auto pluginName = "ReadingIsBad.esp"sv;
	constexpr RE::FormID spellLocalID = 0x800;  // 7ES_ReadSpell, "Absorb Knowledge"

	RE::FormID spellID{ 0 };

	void InitializeLog()
	{
		auto path = logger::log_directory();
		if (!path) {
			return;
		}
		*path /= "ReadingIsBad.log"sv;
		auto log = std::make_shared<spdlog::logger>("global log"s, std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true));
		log->set_level(spdlog::level::info);
		log->flush_on(spdlog::level::info);
		spdlog::set_default_logger(std::move(log));
		spdlog::set_pattern("[%H:%M:%S.%e] [%l] %v"s);
	}

	struct Result
	{
		int found{ 0 };
		int read{ 0 };
		int skills{ 0 };
		int spells{ 0 };
		int questSkipped{ 0 };
	};

	void ReadBook(RE::TESObjectBOOK* a_book, RE::PlayerCharacter* a_player, Result& a_result)
	{
		const bool skill = a_book->TeachesSkill();
		const bool spell = a_book->TeachesSpell();
		a_book->Read(a_player);
		++a_result.read;
		if (spell) {
			++a_result.spells;
		} else if (skill) {
			++a_result.skills;
		}
		if (Settings::Get().debugLog) {
			logger::debug("Read {} ({:08X}), now read: {}", a_book->GetName(), a_book->GetFormID(), a_book->IsRead());
		}
	}

	void Absorb(RE::PlayerCharacter* a_player)
	{
		const auto& s = Settings::Get();
		Result      result;

		std::vector<RE::TESObjectBOOK*> books;
		std::set<RE::FormID>            queued;  // the read flag belongs to the base form, one Read is enough

		const auto consider = [&](RE::TESObjectBOOK* a_book, const RE::TESObjectREFR* a_ref) {
			if (!a_book || a_book->IsRead()) {
				return;
			}
			if (a_book->TeachesSpell() && s.skipSpellTomes) {
				return;
			}
			if (a_ref && s.skipQuestItems && a_ref->HasQuestObject()) {
				++result.questSkipped;
				return;
			}
			if (queued.insert(a_book->GetFormID()).second) {
				books.push_back(a_book);
			}
		};

		RE::TES::GetSingleton()->ForEachReferenceInRange(a_player, s.spellRange, [&](RE::TESObjectREFR* a_ref) {
			if (!a_ref || a_ref == a_player || a_ref->IsDisabled() || a_ref->IsDeleted() || a_ref->IsMarkedForDeletion()) {
				return RE::BSContainer::ForEachResult::kContinue;
			}
			const auto base = a_ref->GetBaseObject();
			if (!base) {
				return RE::BSContainer::ForEachResult::kContinue;
			}
			if (const auto book = base->As<RE::TESObjectBOOK>()) {
				consider(book, a_ref);
			} else if (s.includeContainers && base->Is(RE::FormType::Container)) {
				for (auto& [item, data] : a_ref->GetInventory([](RE::TESBoundObject& a_object) { return a_object.Is(RE::FormType::Book); })) {
					consider(item->As<RE::TESObjectBOOK>(), nullptr);
				}
			}
			return RE::BSContainer::ForEachResult::kContinue;
		});

		result.found = static_cast<int>(books.size());

		// reading can change the scene (a spell tome is used up), so it happens after the search
		for (const auto book : books) {
			if (s.maxBooks > 0 && result.read >= s.maxBooks) {
				break;
			}
			ReadBook(book, a_player, result);
		}

		logger::info("Absorb Knowledge: {} unread books in range, read {} ({} skill books, {} spell tomes), {} left for a quest", result.found, result.read,
			result.skills, result.spells, result.questSkipped);

		if (s.notify && result.read > 0) {
			RE::SendHUDMessage::ShowHUDMessage(std::format("Absorbed the knowledge of {} {}", result.read, result.read == 1 ? "book" : "books").c_str());
		}
	}

	class SpellCastSink : public RE::BSTEventSink<RE::TESSpellCastEvent>
	{
	public:
		static SpellCastSink* Get()
		{
			static SpellCastSink instance;
			return std::addressof(instance);
		}

		RE::BSEventNotifyControl ProcessEvent(const RE::TESSpellCastEvent* a_event, RE::BSTEventSource<RE::TESSpellCastEvent>*) override
		{
			if (a_event && a_event->spell == spellID && a_event->object && a_event->object->IsPlayerRef()) {
				if (const auto player = RE::PlayerCharacter::GetSingleton()) {
					Absorb(player);
				}
			}
			return RE::BSEventNotifyControl::kContinue;
		}
	};

	void OnMessage(SKSE::MessagingInterface::Message* a_message)
	{
		if (a_message->type != SKSE::MessagingInterface::kDataLoaded) {
			return;
		}
		const auto data = RE::TESDataHandler::GetSingleton();
		const auto spell = data ? data->LookupForm<RE::SpellItem>(spellLocalID, pluginName) : nullptr;
		if (!spell) {
			logger::error("{} is not loaded (or has no Absorb Knowledge spell), nothing to do. Install the original mod's ESP.", pluginName);
			return;
		}
		spellID = spell->GetFormID();
		RE::ScriptEventSourceHolder::GetSingleton()->AddEventSink(SpellCastSink::Get());
		Menu::Register();
		logger::info("Watching for {} ({:08X}), range {}", spell->GetName(), spellID, Settings::Get().spellRange);
	}
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse);
	InitializeLog();

	Settings::Get().Load();
	if (Settings::Get().debugLog) {
		spdlog::set_level(spdlog::level::debug);
	}
	Settings::Get().Save();  // writes the ini with every key when there is none yet

	SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
	logger::info("Reading Is Bad loaded");
	return true;
}
