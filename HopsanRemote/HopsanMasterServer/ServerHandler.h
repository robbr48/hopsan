#ifndef SERVERHANDLER_H
#define SERVERHANDLER_H

#include <string>
#include <chrono>
#include <map>
#include <mutex>
#include <list>

class ServerHandler;

class ServerInfo
{
    friend class ServerHandler;
private:
    size_t mId;

public:
    std::string ip;
    std::string port;
    double benchmarkTime=1e100;
    std::chrono::steady_clock::time_point lastCheckTime;
    bool bussyProcessing=false;
    bool isReady;

    size_t id() const
    {
        return mId;
    }
};

class ServerHandler
{
private:
    typedef std::list<size_t> idlist_t;
    idlist_t mFreeIds;
    idlist_t mServerAgeList;
    idlist_t mServerRefreshList;
    std::map<size_t, ServerInfo> mServerMap;
    std::mutex mMutex;
public :
    void addServer(ServerInfo server);
    void updateServerInfo(ServerInfo server);
    void removeServer(size_t id);
    size_t numServers();

    ServerInfo getServer(size_t id);
    int getServerMatching(std::string ip, std::string port);
    std::chrono::steady_clock::time_point getServerAge(size_t id);
    idlist_t getServersFasterThen(double maxTime, int maxNum=-1);
    idlist_t getServersToRefresh(double maxAge, int maxNumServers=-1);
    idlist_t getOldestServers(size_t maxNum);
};

#endif // SERVERHANDLER_H
