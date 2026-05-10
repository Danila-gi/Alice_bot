#include <iostream>
#include <memory>
#include "bot.h"

int main(int argc, char* argv[])
{
    std::string filenameInput;

    if (argc >= 2){
        filenameInput = argv[1];
    }

    std::unique_ptr<Bot> bot = std::make_unique<Bot>();
    const auto result = bot->init(filenameInput, 7, 11, 23, 1);
    std::cout << "Result init: " << result << std::endl;
    if (result){
        bot->run();
    }

    return 0;
}