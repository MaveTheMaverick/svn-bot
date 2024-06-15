#include "BotConfig.h"


BotConfig BotConfig::LoadBotConfig(const char* configFile)
{
	BotConfig botConfig;
	std::ifstream file = std::ifstream(configFile);
	if (file)
	{
		json j = Parse(file);
		botConfig = j.template get<BotConfig>();
	}
	else
	{
		std::ofstream configOut(configFile);

		json j = botConfig; //Create default template for user to fill
		configOut << j;

		configOut.close();
	}

	botConfig.fileName = configFile;
	return botConfig;
}

//BotConfig::BotConfig()
//{	
//}

BotConfig::~BotConfig()
{
	//Save();
}

void BotConfig::Save()
{
	std::ofstream configOut(fileName);

	json j = *this;
	configOut << j;

	configOut.close();
}
