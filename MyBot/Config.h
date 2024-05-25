#pragma once

#include <dpp/dpp.h>                    //D++ discord bot library
#include <string>

// for convenience
using json = nlohmann::json;

#define Parse(i) json::parse(i, nullptr, true, true); //allow exceptions, allow comments


class Config
{
public:

	virtual void Save() = 0;

	json jsonData;

protected:
	std::string file;
};