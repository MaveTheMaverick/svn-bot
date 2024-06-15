#include "SvnConfig.h"


SvnConfig SvnConfig::LoadSvnConfig(const char* configFile)
{
	SvnConfig svnConfig;
	std::ifstream file = std::ifstream(configFile);
	if (file)
	{
		json j = Parse(file);
		file.close();
		try
		{
			svnConfig = j.template get<SvnConfig>();
		}
		catch (nlohmann::json_abi_v3_11_2::detail::out_of_range e)
		{
			std::cout << "Malformed config file" << std::endl;

			//Save to another file just in case important information was there
			std::string configOutBrokenFile("old_");
			configOutBrokenFile += configFile;

			std::ofstream configOutBroken(configOutBrokenFile);
			configOutBroken << j;

			//rewrite the file with default info
			std::ofstream configOut(configFile);

			j = svnConfig; //Create default template for user to fill
			configOut << j;

			configOut.close();
		}
	}
	else
	{
		std::ofstream configOut(configFile);

		json j = svnConfig; //Create default template for user to fill
		configOut << j;

		configOut.close();
	}

	svnConfig.fileName = configFile;
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
	std::ofstream configOut(fileName);

	json j = *this;
	configOut << j;

	configOut.close();
}
