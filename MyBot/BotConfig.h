#pragma once

#include "Config.h"



class BotConfig : public Config
{
public:
	static BotConfig LoadBotConfig(const char* configFile = "BotConfig.json");


	//BotConfig();
	~BotConfig();

	virtual void Save() override;

	std::string environmentToken = "Put Environment Variable name here";;
	std::string footerEmbedURL;

public:
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(BotConfig, environmentToken, footerEmbedURL)
};