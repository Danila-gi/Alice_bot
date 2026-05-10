#include "bot.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <optional>

Bot::Bot(){

}

bool Bot::init(const std::string& filenameInput, int ironPrice, int goldPrice, int gemsPrice, int expPrice){
    std::string line;
    std::vector<Room> rooms;
    std::ifstream in(filenameInput);
    in >> mN;
    if (in.is_open())
    {
        while (std::getline(in, line))
        {
            if (line.empty()){
                continue;
            }
            if (rooms.size() == mN + 1){
                std::stringstream streamLine(line);
                std::string token;

                if (!std::getline(streamLine, token, ' ')){
                    std::cout << line << std::endl;
                    return false;
                }
                if (!Bot::getNumberFromStr(token, mM)){
                    std::cout << line << std::endl;
                    return false;
                }

                if (!std::getline(streamLine, token, ' ')){
                    std::cout << line << std::endl;
                    return false;
                }
                mTargetResource = Bot::getResourceFromStr(token);
                break;
            }
            Room room;
            int id;
            std::stringstream streamLine(line);
            std::string token;

            if (!std::getline(streamLine, token, ' ')){
                std::cout << line << std::endl;
                return false;
            }
            if (!Bot::getNumberFromStr(token, id)){
                mIsInit = false;
                std::cout << line << std::endl;
                return false;
            }

            if (!std::getline(streamLine, token, ' ')){
                std::cout << line << std::endl;
                return false;
            }
            {
                std::string tokenAdjacentRooms;
                std::stringstream streamTokenAdjacentRooms(token);
                while (std::getline(streamTokenAdjacentRooms, tokenAdjacentRooms, ','))
                {
                    int adjacentRoom;
                    if (!Bot::getNumberFromStr(tokenAdjacentRooms, adjacentRoom)){
                        mIsInit = false;
                        std::cout << line << std::endl;
                        return false;
                    }
                    room.adjacentRooms.push_back(adjacentRoom);
                }
            }
            if (!std::getline(streamLine, token, ' ')){
                std::cout << line << std::endl;
                return false;
            }

            if (!Bot::getNumberFromStr(token, room.resourcesCount[Resource::Iron])){
                mIsInit = false;
                std::cout << line << std::endl;
                return false;
            }

            if (!std::getline(streamLine, token, ' ')){
                std::cout << line << std::endl;
                return false;
            }
            if (!Bot::getNumberFromStr(token, room.resourcesCount[Resource::Gold])){
                mIsInit = false;
                std::cout << line << std::endl;
                return false;
            }

            if (!std::getline(streamLine, token, ' ')){
                std::cout << line << std::endl;
                return false;
            }
            if (!Bot::getNumberFromStr(token, room.resourcesCount[Resource::Gems])){
                mIsInit = false;
                std::cout << line << std::endl;
                return false;
            }

            if (!std::getline(streamLine, token, ' ')){
                std::cout << line << std::endl;
                return false;
            }
            if (!Bot::getNumberFromStr(token, room.resourcesCount[Resource::Exp])){
                mIsInit = false;
                std::cout << line << std::endl;
                return false;
            }
            
            mRooms.insert({id, room});
            rooms.push_back(room);
        }
    }
    else{
        mIsInit = false;
        std::cout << "error file open!" << std::endl;
        return false;
    }
    in.close();

    mResourcesPrices[Resource::Iron] = ironPrice;
    mResourcesPrices[Resource::Gold] = goldPrice;
    mResourcesPrices[Resource::Gems] = gemsPrice;
    mResourcesPrices[Resource::Exp] = expPrice;

    if (mTargetResource != Unknown){
        mResourcesPrices[mTargetResource] *= 2;
    }

    for (const auto& pair : mResourcesPrices) {
        mSortedResources.push_back(pair.first);
    }

    std::sort(mSortedResources.begin(), mSortedResources.end(),
        [this](Resource a, Resource b) {
            return mResourcesPrices[a] > mResourcesPrices[b];
        });

    for (const auto& room: mRooms){
        for (const auto& adjacentRoom: room.second.adjacentRooms){
            if (std::find(mRooms[adjacentRoom].adjacentRooms.begin(), mRooms[adjacentRoom].adjacentRooms.end(), room.first) 
                == mRooms[adjacentRoom].adjacentRooms.end()){
                mRooms[adjacentRoom].adjacentRooms.push_back(room.first);
            }
        }
    }
    for (const auto& room: mRooms){
        std::sort(mRooms[room.first].adjacentRooms.begin(), mRooms[room.first].adjacentRooms.end(),
            [this](int a, int b) {
                return a < b;
            });
    }
    mIsInit = true;
    mStartRoom = mRooms.begin()->first;
    return true; 
}

void Bot::run(){
    if (!mIsInit){
        return;
    }
    mCurrentRoomId = mStartRoom;
    int currentM = mM;
    std::unordered_map<Resource, int> resourcesCountRunResult;
    resourcesCountRunResult[Resource::Iron] = 0;
    resourcesCountRunResult[Resource::Gold] = 0;
    resourcesCountRunResult[Resource::Gems] = 0;
    resourcesCountRunResult[Resource::Exp] = 0;
    while (currentM > mM / 2 + (mM % 2)){
        mRooms[mCurrentRoomId].state = RoomState::Visited;
        bool isGo = false;
        for (const auto& nextRoom: mRooms[mCurrentRoomId].adjacentRooms){
            if (mRooms[nextRoom].state == RoomState::Visited){
                continue;
            }
            mCurrentRoomId = nextRoom;
            std::cout << "go " << mCurrentRoomId << std::endl;
            printCurrentState();
            isGo = true;
            currentM--;
            mRooms[mCurrentRoomId].state = RoomState::Visited;
            break;
        }
        if (!isGo){
            int currentRoomId = mCurrentRoomId;
            for (const auto& room: findNearestRoom(currentRoomId)){
                if (room == currentRoomId){
                    continue;
                }
                mCurrentRoomId = room;
                std::cout << "go " << mCurrentRoomId << std::endl;
                isGo = true;
                printCurrentState();
                currentM--;
                mRooms[mCurrentRoomId].state = RoomState::Visited;
                if (!mRooms[mCurrentRoomId].isWasCollected){
                    collectResources(resourcesCountRunResult);
                    mRooms[mCurrentRoomId].isWasCollected = true;
                }
                if (currentM <= mM / 2 + (mM % 2)){
                    break;
                }
            }
            if (!isGo){
                break;
            }
        }
        else{
            collectResources(resourcesCountRunResult);
            mRooms[mCurrentRoomId].isWasCollected = true;
        }
    }
    int currentRoomId = mCurrentRoomId;
    const auto path = dijkstra(currentRoomId, mStartRoom);
    int stepsForCollecting = currentM - (path.size() - 1);
    for (const auto& room: dijkstra(currentRoomId, mStartRoom)){
        if (currentM < 0){
            std::cout << "died" << std::endl;
        }
        if (room == currentRoomId){
            continue;
        }
        mCurrentRoomId = room;
        std::cout << "go " << mCurrentRoomId << std::endl;
        if (mCurrentRoomId != mStartRoom){
            printCurrentState();
        }
        currentM--;
        if (stepsForCollecting > 0){
            stepsForCollecting--;
            collectResources(resourcesCountRunResult);
        }
    }

    printResult(resourcesCountRunResult);
}

void Bot::collectResources(std::unordered_map<Resource, int>& resourcesCountRunResult){
    for (const auto& res: mSortedResources){
        if (mRooms[mCurrentRoomId].resourcesCount[res] <= 0){
            continue;
        }
        resourcesCountRunResult[res] += mRooms[mCurrentRoomId].resourcesCount[res];
        const auto resStr = Bot::getResourceAsStr(res);
        mRooms[mCurrentRoomId].resourcesCount[res] = -1;
        if (resStr.has_value()){
            std::cout << "collect " << resStr.value() << std::endl;
            printCurrentState();
        }
        break;
    }
}

std::vector<int> Bot::dijkstra(int startRoom, int targetRoom) {
    const int INF = INT32_MAX;
    std::unordered_map<int, int> dist;
    std::unordered_map<int, int> previousRooms;
    std::unordered_map<int, bool> used;
    
    for (const auto& room : mRooms) {
        used[room.first] = false;
        dist[room.first] = INF;
        previousRooms[room.first] = -1;
    }
    
    dist[startRoom] = 0;
    
    for (size_t i = 0; i < mRooms.size(); ++i) {
        int node = -1;
        int minDist = INF;
        
        for (const auto& room : mRooms) {
            int roomId = room.first;
            if ((mRooms[roomId].state == RoomState::Visited) && !used[roomId] && dist[roomId] < minDist) {
                minDist = dist[roomId];
                node = roomId;
            }
        }
        
        if (node == -1 || dist[node] == INF) {
            break;
        }
        
        used[node] = true;
        
        for (int neighbor : mRooms[node].adjacentRooms) {
            if (mRooms[neighbor].state == RoomState::Visited) {
                if (dist[node] + 1 < dist[neighbor]) {
                    dist[neighbor] = dist[node] + 1;
                    previousRooms[neighbor] = node;
                }
            }
        }
    }
    
    std::vector<int> path = {};
    if (dist[targetRoom] == INF) {
        return {};
    }

    int iterRoomId = targetRoom;
    while (iterRoomId != -1){
        path.insert(path.begin(), iterRoomId);
        iterRoomId = previousRooms[iterRoomId];
    }
    
    return path;
}

std::vector<int> Bot::findNearestRoom(int currentRoom) {
    if (mRooms[currentRoom].state != RoomState::Visited) {
        return {currentRoom};
    }

    std::queue<std::vector<int>> queue;
    queue.push({currentRoom});
    std::unordered_map<int, bool> visited;
    for (const auto& room: mRooms){
        visited[room.first] = false;
    }
    visited[currentRoom] = true;
    
    while (!queue.empty()) {
        std::vector<int> path = queue.front();
        queue.pop();
        
        int current = path.back();
        
        for (int neighbor : mRooms[current].adjacentRooms) {
            if (visited[neighbor]) {
                continue;
            }
            
            std::vector<int> newPath = path;
            newPath.push_back(neighbor);
            
            if (mRooms[neighbor].state != RoomState::Visited) {
                return newPath;
            }
            
            visited[neighbor] = true;
            queue.push(newPath);
        }
    }
    
    return {};
}

bool Bot::getNumberFromStr(std::string& token, int& result){
    try {
        size_t resultConvertPose = 0;
        result = std::stoi(token, &resultConvertPose);
        return resultConvertPose == token.length();
    } catch (const std::exception&) {
        return false;
    }
}

std::optional<std::string> Bot::getResourceAsStr(const Resource& res){
    if (res == Resource::Iron){
        return "iron";
    }
    if (res == Resource::Gold){
        return "gold";
    }
    if (res == Resource::Gems){
        return "gems";
    }
    if (res == Resource::Exp){
        return "exp";
    }
    return std::nullopt;
}

Resource Bot::getResourceFromStr(const std::string& str){
    if (str == "iron"){
        return Resource::Iron;
    }
    else if (str == "gold"){
        return Resource::Gold;
    }
    else if (str == "gems"){
        return Resource::Gems;
    }
    else if (str == "exp"){
        return Resource::Exp;
    }
    return Resource::Unknown;
}

void Bot::printCurrentState(){
    std::cout << "state " 
                << mCurrentRoomId << " "
                << (mRooms[mCurrentRoomId].resourcesCount[Resource::Iron] >= 0 ? std::to_string(mRooms[mCurrentRoomId].resourcesCount[Resource::Iron]) : "_") << " "
                << (mRooms[mCurrentRoomId].resourcesCount[Resource::Gold] >= 0 ? std::to_string(mRooms[mCurrentRoomId].resourcesCount[Resource::Gold]) : "_") << " "
                << (mRooms[mCurrentRoomId].resourcesCount[Resource::Gems] >= 0 ? std::to_string(mRooms[mCurrentRoomId].resourcesCount[Resource::Gems]) : "_") << " "
                << (mRooms[mCurrentRoomId].resourcesCount[Resource::Exp] >= 0 ? std::to_string(mRooms[mCurrentRoomId].resourcesCount[Resource::Exp]) : "_")
                << std::endl;
}

void Bot::printResult(std::unordered_map<Resource, int>& resourcesCountRunResult){
    int sum = 0;
    sum += resourcesCountRunResult[Resource::Iron] * mResourcesPrices[Resource::Iron];
    sum += resourcesCountRunResult[Resource::Gold] * mResourcesPrices[Resource::Gold];
    sum += resourcesCountRunResult[Resource::Gems] * mResourcesPrices[Resource::Gems];
    sum += resourcesCountRunResult[Resource::Exp] * mResourcesPrices[Resource::Exp];
    std::cout << "result " 
                << resourcesCountRunResult[Resource::Iron] << " "
                << resourcesCountRunResult[Resource::Gold] << " "
                << resourcesCountRunResult[Resource::Gems] << " "
                << resourcesCountRunResult[Resource::Exp] << " "
                << sum
                << std::endl;
}