#pragma once

#include "Bot.h"
#include "SvnConfig.h"

typedef unsigned char   BYTE;
typedef BYTE			BOOLEAN;

class Svn
{
public:
	Svn(SvnConfig& _svnConfig);

	void StartUpdateTimer(dpp::Bot* bot);
	void StartBuildTimer(dpp::Bot* bot);

	bool Update();

	//TODO
	//void Build();

	std::string GetRevisionMessage(int revNum);
	dpp::embed  GetRevisionEmbed(int revNum);

private:
	SvnConfig svnConfig;
	bool updating = false;
	bool building = false;
	wchar_t originalDirectory[MAX_PATH];

	struct TimerInfo
	{
		dpp::Bot* bot;
		Svn* self;
	};

	void* handleUpdate = nullptr;
	TimerInfo timerInfoUpdate;
	static void TimerCallbackUpdate(void* lpParameter, BOOLEAN TimerOrWaitFired);

	void* handleBuild = nullptr;
	TimerInfo timerInfoBuild;
	static void TimerCallbackBuild(void* lpParameter, BOOLEAN TimerOrWaitFired);
};