#include <Windows.h>
#include "auth.hpp"
#include <string>
#include <thread>
#include "utils.hpp"
#include "skStr.h"
std::string tm_to_readable_time(tm ctx);
static std::time_t string_to_timet(std::string timestamp);
static std::tm timet_to_tm(time_t timestamp);
const std::string compilation_date = (std::string)skCrypt(__DATE__);
const std::string compilation_time = (std::string)skCrypt(__TIME__);

using namespace KeyAuth;

// copy and paste from https://keyauth.cc/app/ and replace these string variables
// Please watch tutorial HERE 
std::string name = skCrypt("name").decrypt();
std::string ownerid = skCrypt("ownerid").decrypt();
std::string version = skCrypt("1.0").decrypt();
std::string url = skCrypt("https://keyauth.win/api/1.3/").decrypt(); // change if using KeyAuth custom domains feature
std::string path = skCrypt("").decrypt(); //optional, set a path if you're using the token validation setting

api KeyAuthApp(name, ownerid, version, url, path);

int main()
{
    std::string consoleTitle = skCrypt("Loader - Built at:  ").decrypt() + compilation_date + " " + compilation_time;
    SetConsoleTitleA(consoleTitle.c_str());
    std::cout << skCrypt("\n\n Connecting..");

    KeyAuthApp.init();
    if (!KeyAuthApp.response.success)
    {
        std::cout << skCrypt("\n Status: ") << KeyAuthApp.response.message;
        Sleep(1500);
        exit(1);
    }

    if (std::filesystem::exists("test.json")) //change test.txt to the path of your file :smile:
    {
        if (!CheckIfJsonKeyExists("test.json", "username"))
        {
            std::string key = ReadFromJson("test.json", "license");
            KeyAuthApp.license(key);
            if (!KeyAuthApp.response.success)
            {
                std::remove("test.json");
                std::cout << skCrypt("\n Status: ") << KeyAuthApp.response.message;
                Sleep(1500);
                exit(1);
            }
            std::cout << skCrypt("\n\n Successfully Automatically Logged In\n");
        }
        else
        {
            std::string username = ReadFromJson("test.json", "username");
            std::string password = ReadFromJson("test.json", "password");
            KeyAuthApp.login(username, password);
            if (!KeyAuthApp.response.success)
            {
                std::remove("test.json");
                std::cout << skCrypt("\n Status: ") << KeyAuthApp.response.message;
                Sleep(1500);
                exit(1);
            }
            std::cout << skCrypt("\n\n Successfully Automatically Logged In\n");
        }
    }
    else
    {
        std::cout << skCrypt("\n\n [1] Login\n [2] Register\n [3] Upgrade\n [4] License key only\n\n Choose option: ");

        int option;
        std::string username;
        std::string password;
        std::string key;

        std::cin >> option;
        switch (option)
        {
        case 1:
            std::cout << skCrypt("\n\n Enter username: ");
            std::cin >> username;
            std::cout << skCrypt("\n Enter password: ");
            std::cin >> password;
            KeyAuthApp.login(username, password);
            break;
        case 2:
            std::cout << skCrypt("\n\n Enter username: ");
            std::cin >> username;
            std::cout << skCrypt("\n Enter password: ");
            std::cin >> password;
            std::cout << skCrypt("\n Enter license: ");
            std::cin >> key;
            KeyAuthApp.regstr(username, password, key);
            break;
        case 3:
            std::cout << skCrypt("\n\n Enter username: ");
            std::cin >> username;
            std::cout << skCrypt("\n Enter license: ");
            std::cin >> key;
            KeyAuthApp.upgrade(username, key);
            break;
        case 4:
            std::cout << skCrypt("\n Enter license: ");
            std::cin >> key;
            KeyAuthApp.license(key);
            break;
        default:
            std::cout << skCrypt("\n\n Status: Failure: Invalid Selection");
            Sleep(3000);
            exit(1);
        }

        if (KeyAuthApp.response.message.empty()) exit(11);
        if (!KeyAuthApp.response.success)
        {
            std::cout << skCrypt("\n Status: ") << KeyAuthApp.response.message;
            Sleep(1500);
            exit(1);
        }

        if (username.empty() || password.empty())
        {
            WriteToJson("test.json", "license", key, false, "", "");
            std::cout << skCrypt("Successfully Created File For Auto Login");
        }
        else
        {
            WriteToJson("test.json", "username", username, true, "password", password);
            std::cout << skCrypt("Successfully Created File For Auto Login");
        }
    }

    /*
    * Do NOT remove this checkAuthenticated() function.
    * It protects you from cracking, it would be NOT be a good idea to remove it
    */
    std::thread run(checkAuthenticated, ownerid);
    // do NOT remove checkAuthenticated(), it MUST stay for security reasons

    if (KeyAuthApp.user_data.username.empty()) exit(10);
    std::cout << skCrypt("\n User data:");
    std::cout << skCrypt("\n Username: ") << KeyAuthApp.user_data.username;
    std::cout << skCrypt("\n IP address: ") << KeyAuthApp.user_data.ip;
    std::cout << skCrypt("\n Hardware-Id: ") << KeyAuthApp.user_data.hwid;
    std::cout << skCrypt("\n Create date: ") << tm_to_readable_time(timet_to_tm(string_to_timet(KeyAuthApp.user_data.createdate)));
    std::cout << skCrypt("\n Last login: ") << tm_to_readable_time(timet_to_tm(string_to_timet(KeyAuthApp.user_data.lastlogin)));
    std::cout << skCrypt("\n Subscription(s): ");

    for (int i = 0; i < KeyAuthApp.user_data.subscriptions.size(); i++) {
        auto sub = KeyAuthApp.user_data.subscriptions.at(i);
        std::cout << skCrypt("\n name: ") << sub.name;
        std::cout << skCrypt(" : expiry: ") << tm_to_readable_time(timet_to_tm(string_to_timet(sub.expiry)));
    }

    std::cout << skCrypt("\n\n Closing in five seconds...");
    Sleep(5000);

    return 0;
}

std::string tm_to_readable_time(tm ctx) {
    char buffer[80];

    strftime(buffer, sizeof(buffer), "%a %m/%d/%y %H:%M:%S %Z", &ctx);

    return std::string(buffer);
}

static std::time_t string_to_timet(std::string timestamp) {
    auto cv = strtol(timestamp.c_str(), NULL, 10); // long

    return (time_t)cv;
}

static std::tm timet_to_tm(time_t timestamp) {
    std::tm context;

    localtime_s(&context, &timestamp);

    return context;
}
