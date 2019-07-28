#include "skse64_common/skse_version.h"

#include "version.h"

#include "RE/Skyrim.h"
#include "SKSE/API.h"


namespace
{
	void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
	{
		switch (a_msg->type) {
		case SKSE::MessagingInterface::kDataLoaded:
			{
				auto dataHandler = RE::TESDataHandler::GetSingleton();
				auto keywords = dataHandler->GetFormArray<RE::BGSKeyword>();
				auto factory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::BGSListForm>();
				auto list = factory->Create();
				for (auto& keyword : keywords) {
					list->forms.push_back(keyword);
				}

				auto enchantments = dataHandler->GetFormArray<RE::EnchantmentItem>();
				for (auto& ench : enchantments) {
					ench->data.wornRestrictions = list;
				}
			}
			break;
		}
	}
}


extern "C" {
	bool SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
	{
		SKSE::Logger::OpenRelative(FOLDERID_Documents, L"\\My Games\\Skyrim Special Edition\\SKSE\\NoEnchantmentRestrictionsSSE.log");
		SKSE::Logger::SetPrintLevel(SKSE::Logger::Level::kDebugMessage);
		SKSE::Logger::SetFlushLevel(SKSE::Logger::Level::kDebugMessage);
		SKSE::Logger::UseLogStamp(true);

		_MESSAGE("NoEnchantmentRestrictionsSSE v%s", ENCH_VERSION_VERSTRING);

		a_info->infoVersion = SKSE::PluginInfo::kVersion;
		a_info->name = "NoEnchantmentRestrictionsSSE";
		a_info->version = ENCH_VERSION_MAJOR;

		if (a_skse->IsEditor()) {
			_FATALERROR("Loaded in editor, marking as incompatible!\n");
			return false;
		}

		switch (a_skse->RuntimeVersion()) {
		case RUNTIME_VERSION_1_5_73:
		case RUNTIME_VERSION_1_5_80:
			break;
		default:
			_FATALERROR("Unsupported runtime version %08X!\n", a_skse->RuntimeVersion());
			return false;
		}

		return true;
	}


	bool SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
	{
		_MESSAGE("NoEnchantmentRestrictionsSSE loaded");

		if (!SKSE::Init(a_skse)) {
			return false;
		}

		auto messaging = SKSE::GetMessagingInterface();
		if (messaging->RegisterListener("SKSE", MessageHandler)) {
			_MESSAGE("Messaging interface registration successful");
		} else {
			_FATALERROR("Messaging interface registration failed!\n");
			return false;
		}

		return true;
	}
};
