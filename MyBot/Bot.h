#pragma once

#include <dpp/dpp.h>                    //D++ discord bot library
#include "BotConfig.h"


namespace dpp
{
	class Bot
	{
	public:

		Bot(BotConfig& _botConfig);

		//void SetSvn(Svn* _svn);
		cluster* operator->() { return Cluster; };

		cluster* Cluster;
	private:
		BotConfig botConfig;
		//Svn* svn;

	};
}
