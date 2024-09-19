#include "Bot.h"

#define EXIT_CODE_NO_TOKEN -2

dpp::Bot::Bot(BotConfig& _botConfig)
    : Cluster(nullptr)
	, botConfig(_botConfig)
{
    char* tokenBuffer;
    size_t tokenBufferSize;
    errno_t error = _dupenv_s(&tokenBuffer, &tokenBufferSize, botConfig.environmentToken.c_str());
    if (tokenBufferSize == 0 || !tokenBuffer)
    {
        std::cout << "Failed to read token. Ensure environment variable \"" << botConfig.environmentToken.c_str() << "\" is set to the bot's token" << std::endl;
        exit(EXIT_CODE_NO_TOKEN);
    }
    else
    {
        Cluster = new cluster(tokenBuffer);
        delete tokenBuffer;
    }
    assert(Cluster);
}

dpp::Bot::~Bot()
{
    if (Cluster)
        delete Cluster;
}
