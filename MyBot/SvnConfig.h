#pragma once

#include "Config.h"

#define NEWEST_REVISION_LOG "NewestRevisionLog.txt"
//#define LAST_REVISION_LOG   "LastRevisionNum.txt"
#define SVN_MEMBERS_FILE    "TeamMembers.json"

#define EXCEPTION_NO_REVISION "Revision number does not exist"

#define SVN_UPDATE          "svn update "
#define SVN_LOG				"svn log "

class SvnConfig : public Config
{
public:
	static SvnConfig LoadSvnConfig(const char* configFile = "SvnConfig.json");

	//SvnConfig();
	~SvnConfig();

	virtual void Save() override;

	//Relative location of the SVN root directory
	std::string svnRoot = "..\\..\\";

	//Relative location/name of the actual project folder within the svn (Optional for auto build only)
	std::string svnProjectFolder;

	std::string svnUpdateCommand = SVN_UPDATE;

	std::string svnLogCommand = SVN_LOG;

	int lastRevision = 0;

	unsigned long long updateMessageChannel = 0;

private:

public:
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(SvnConfig, svnRoot, svnProjectFolder, svnUpdateCommand, svnLogCommand, lastRevision, updateMessageChannel)
};
