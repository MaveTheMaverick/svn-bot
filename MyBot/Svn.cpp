#include "Svn.h"


#include <regex>                        //std::regex_replace()
#include <thread>                       //std::this_thread::sleep_for()
using namespace std::chrono_literals;   //ms

Svn::Svn(SvnConfig& _svnConfig)
	: svnConfig(_svnConfig)
{
	assert(GetCurrentDirectoryW(MAX_PATH, originalDirectory));
}

void Svn::StartUpdateTimer(dpp::Bot* bot)
{
	timerInfoUpdate.bot = bot;
	timerInfoUpdate.self = this;
	CreateTimerQueueTimer(&handleUpdate, NULL, &TimerCallbackUpdate, static_cast<PVOID>(&timerInfoUpdate), 1000, 61000, WT_EXECUTELONGFUNCTION);
}

void Svn::StartBuildTimer(dpp::Bot* bot)
{
	timerInfoBuild.bot = bot;
	timerInfoBuild.self = this;
	CreateTimerQueueTimer(&handleBuild, NULL, &TimerCallbackBuild, static_cast<PVOID>(&timerInfoBuild), 30000, 86400000, WT_EXECUTELONGFUNCTION);
}

bool Svn::Update()
{
	if (!updating && !building)
	{
		updating = true;
		std::string svnUpdate = svnConfig.svnUpdateCommand;
		svnUpdate += '\"';
		svnUpdate += svnConfig.svnRoot;
		svnUpdate += '\"';
		system(svnUpdate.c_str());
		updating = false;
		return true;
	}
	if (building)
	{
		std::cout << "[WARNING] Cannot update while building" << std::endl;
		return false;
	}
	else
	{
		int attempts = 0;
		while (updating && attempts < 150) //Block for up to 15 seconds, while the other thread that's updating updates
		{
			std::this_thread::sleep_for(100ms);
			attempts++;
		}
		if (!updating)
		{
			return true;
		}
	}
	return false;
}

std::string Svn::GetRevisionMessage(int revNum)
{
	//svn log -r "XXX" > NewestRevisionLog.txt
	std::string command = svnConfig.svnLogCommand;
	command += '\"';
	command += svnConfig.svnRoot;
	command += "\" -r ";
	command += std::to_string(revNum);
	command += " > ";
	command += NEWEST_REVISION_LOG;
	
	std::cout << "[RUNNING] " << command << std::endl;
	int r = system(command.c_str());
	if (r != 0)
	{
		//std::cout << "[INFO] No revision with number " << revNum << std::endl;
		return std::string();
	}

	//Read the file we just output the svn log command to
	std::ifstream nextRevisionFile(NEWEST_REVISION_LOG);

	std::stringbuf b;
	nextRevisionFile.get(b, '\0');
	nextRevisionFile.close();

	svnConfig.Save();

	return b.str();
}

dpp::embed Svn::GetRevisionEmbed(int revNum)
{
	std::string nextRevisionText = GetRevisionMessage(revNum);

	if (nextRevisionText.empty() || strncmp(nextRevisionText.c_str(), "svn:", 4) == 0) // If we got an error code instead of a log
	{
		std::string s = "[INFO] No revision with number ";
		s += std::to_string(revNum);
		throw(std::exception(s.c_str()));
	}
	else
	{
		std::cout << "[INFO] Posting new revision" << std::endl;

		//Format string

		std::cout << nextRevisionText << std::endl;
		//remove bottom/top line
		nextRevisionText = std::regex_replace(nextRevisionText, std::regex("------------------------------------------------------------------------"), "");
		//get revision number
		size_t revStart = nextRevisionText.find_first_of('r') + 1;
		size_t revEnd = nextRevisionText.find_first_of('|');
		std::string revisionNumber = nextRevisionText.substr(revStart, revEnd - 1 - revStart);
		//format revision number
		revisionNumber = "__Revision: **#" + revisionNumber + "**__";
		//remove revision number from main text
		nextRevisionText = nextRevisionText.substr(revEnd + 2);
		//get author
		size_t nameEnd = nextRevisionText.find_first_of(' ');
		std::string revisionAuthor = nextRevisionText.substr(0, nameEnd);
		//remove author from main text
		nextRevisionText = nextRevisionText.substr(nextRevisionText.find_first_of('|') + 2);
		size_t lineEnd = nextRevisionText.find_first_of('\n');
		//extract remainder of first line to use as the footer/date
		std::string footer = nextRevisionText.substr(0, lineEnd);
		size_t dateEnd = footer.find_first_of(' ');
		footer = footer.substr(dateEnd + 1, footer.find_last_of('|') - (dateEnd + 1));
		footer = std::regex_replace(footer, std::regex("-0700 "), ""); //west coast
		footer = std::regex_replace(footer, std::regex("-0800 "), ""); //west coast daylight savings
		nextRevisionText = nextRevisionText.substr(lineEnd + 2);

		try {
			json j = Parse(std::ifstream(SVN_MEMBERS_FILE));

			for (json::const_iterator it = j.begin(); it != j.end(); ++it) {
				if (it.value().is_string())
				{
					//replace iterator key with iterator value
					revisionAuthor = std::regex_replace(revisionAuthor, std::regex(it.key()), it.value().get<std::string>());
				}
				else
				{
					std::cout << "[WARNING] Garbage field of type \"" << it.value().type_name() << "\" found in \"" << SVN_MEMBERS_FILE << "\"" << std::endl;
				}
			}
		}
		catch (...)
		{

		}

		std::cout << nextRevisionText << std::endl;


		dpp::embed embed = dpp::embed()
			.set_title(revisionNumber + "  |  " + revisionAuthor)
			.set_description(nextRevisionText)
			.set_footer(footer, "https://cdn.prod.website-files.com/6257adef93867e50d84d30e2/665786b8a93285bff36e194e_discord-mark-white%202.webp");
		return embed;
	}

	return dpp::embed();
}

void Svn::UpdateConfig(SvnConfig& _svnConfig)
{
	svnConfig = _svnConfig;
}

void Svn::TimerCallbackUpdate(void* lpParameter, BOOLEAN TimerOrWaitFired)
{
	TimerInfo* timerInfo = static_cast<TimerInfo*>(lpParameter);
	assert(timerInfo && timerInfo->self);

	if (timerInfo->self->building)
	{
		std::cout << "[WARNING] Cannot update during build" << std::endl;
		return;
	}

	//svn update
	try {
		timerInfo->self->Update();

		SvnConfig& svnConfig = timerInfo->self->svnConfig;
		dpp::embed embed = timerInfo->self->GetRevisionEmbed(svnConfig.lastRevision + 1);


		dpp::message m(svnConfig.updateMessageChannel, embed);
		//m.set_guild_id(1146177411004768336ULL);

		dpp::Bot* bot = timerInfo->bot;
		assert(bot);

		//const std::string& footerEmbedURL = bot->GetBotConfig().footerEmbedURL;
		//if (!footerEmbedURL.empty())
		//{
		//	
		//}

		try {
			dpp::message mReturn = bot->Cluster->message_create_sync(m);

			svnConfig.lastRevision++;

			//std::cout << "Channel: " << mReturn.channel_id << std::endl;
			//std::cout << "Guild: " << mReturn.channel_id << std::endl;


			svnConfig.Save();
		}
		catch (dpp::rest_exception e)
		{
			std::cout << "[WARNING] Could not send update embed: " << e.what() << std::endl;
		}

	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
}

void Svn::TimerCallbackBuild(void* lpParameter, BOOLEAN TimerOrWaitFired)
{
	TimerInfo* timerInfo = static_cast<TimerInfo*>(lpParameter);
	assert(timerInfo && timerInfo->self);

	if (timerInfo->self->building)
	{
		std::cout << "[WARNING] Cannot build, already building" << std::endl;
		return;
	}

	//TODO
}
