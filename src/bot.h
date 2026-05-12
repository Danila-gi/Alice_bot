#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <optional>

enum Resource : int{
    Unknown = 0,
    Iron = 1,
    Gold = 2,
    Gems = 3,
    Exp = 4,
};

enum RoomState : int{
    Unvisited = 0,
    Visited = 1,
};

struct Room{
    std::vector<int> adjacentRooms;
    std::unordered_map<Resource, int> resourcesCount;
    RoomState state {RoomState::Unvisited};
    bool isWasCollected {false};

    Room(){
        resourcesCount[Resource::Iron] = 0;
        resourcesCount[Resource::Gold] = 0;
        resourcesCount[Resource::Gems] = 0;
        resourcesCount[Resource::Exp] = 0;
    }
};

class Bot{
public:
    Bot(const std::string& fileResults);
    ~Bot();
    bool init(const std::string& filenameInput, int ironPrice, int goldPrice, int gemsPrice, int expPrice);
    void run();

private:
    void collectResources(std::unordered_map<Resource, int>& resourcesCountRunResult);
    std::vector<int> dijkstra(int startRoom, int targetRoom);
    std::vector<int> findNearestRoom(int currentRoom);
    static bool getNumberFromStr(std::string& token, int& result);
    static std::optional<std::string> getResourceAsStr(const Resource& res);
    static Resource getResourceFromStr(const std::string& str);
    void printCurrentState();
    void printResult(std::unordered_map<Resource, int>& resourcesCountRunResult);

private:
    bool mIsInit {false};
    int mN {0};
    int mM {0};
    int mCurrentRoomId {0};
    int mStartRoom {0};
    std::string mFileResults;
    std::ofstream mResults;
    Resource mTargetResource {Unknown};
    std::map<int, Room> mRooms;
    std::unordered_map<Resource, int> mResourcesPrices;
    std::vector<Resource> mSortedResources;
};