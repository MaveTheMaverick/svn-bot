#include "SvnConfig.h"


SvnConfig SvnConfig::LoadSvnConfig(const char* configFile)
{
	SvnConfig svnConfig;
	std::ifstream file = std::ifstream(configFile);
	if (file)
	{
		json j = Parse(file);
		svnConfig = j.template get<SvnConfig>();
	}
	else
	{
		std::ofstream configOut(configFile);

		json j = svnConfig; //Create default template for user to fill
		configOut << j;

		configOut.close();
	}

	return svnConfig;
}

//SvnConfig::SvnConfig()
//{
//
//}

SvnConfig::~SvnConfig()
{
	//Save();
}

void SvnConfig::Save()
{
	std::ofstream configOut(file);

	json j = *this;
	configOut << j;

	configOut.close();
}
