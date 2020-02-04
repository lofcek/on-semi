#include <functional>
#include <iostream>
#include <iomanip>
#include <map>
#include <string>

//
// supporting tools and software
//
// Validate and test your json commands
// https://jsonlint.com/

// RapidJSON : lots and lots of examples to help you use it properly
// https://github.com/Tencent/rapidjson
//

// std::function
// std::bind
// std::placeholders
// std::map
// std::make_pair

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"


using namespace rapidjson;
using namespace std;

bool g_done = false;

//
// TEST COMMANDS
//
auto help_command = R"({"command":"help", "payload": {}})";
auto exit_command = R"({"command": "exit", "payload": {"reason":"Exiting program on user request."}})";
auto now_command = R"({"command": "now", "payload": {}})";
auto hello_command = R"({"command":"hello", "payload": {"user": "username"}})";
auto times_command = R"({"command":"times", "payload": {}})";

class Controller {
public:
    bool help(rapidjson::Value &/*payload*/)
    {
        //cout << "Controller::help: command: ";

        cout << "Known commands:" << endl;
        cout << "  " << help_command << endl;
        cout << "  " << exit_command << endl;
        cout << "  " << now_command << endl;
        cout << "  " << times_command << endl;

        return true;
    }

    bool exit(rapidjson::Value &payload)
    {
        //cout << "Controller::exit: command: \n";
    
        // implement
        auto reason = payload.FindMember("reason");
        const char* reason_str = "unknown reason";
        if (reason != payload.MemberEnd() && reason->value.IsString()) {
            reason_str = reason->value.GetString();
        }
        
        g_done = true;
        cout << "exiting: reason " << reason_str << endl;
        return true;
    }

    bool now(rapidjson::Value &/*payload*/)
    {
        //cout << "Controller::now: command: \n";
        auto time = std::time(nullptr);
        cout << std::put_time(std::localtime(&time), "%F %T%z") << endl; // ISO 8601 format
        return true;
    }

    bool hello(rapidjson::Value &payload)
    {
        //cout << "Controller::hello: command: \n";
        auto user = payload.FindMember("user");
        const char* user_str = "world";
        if (user != payload.MemberEnd() && user->value.IsString()) {
            user_str = user->value.GetString();
        }
        
        cout << "hello " << user_str << endl;
        return true;
    }

    bool times(rapidjson::Value &/*payload*/)
    {
        //cout << "Controller::times: command: \n";
        cout << "command times was already called " << (times_++) << " time(s)" << endl; // ISO 8601 format
        return true;
    }
private:
    int times_ = 0;
};

// gimme ... this is actually tricky
// Bonus Question: why did I type cast this?
// Although `bool(rapidjson::Value &)` maybe looks like a cast, there is not.
// It only mean function which argument has type `rapidjson::Value &` and return bool.
typedef std::function<bool(rapidjson::Value &)> CommandHandler;

class CommandDispatcher final {
public:
    // ctor - need impl
    CommandDispatcher()
    {
    }

    // dtor - need impl
    virtual ~CommandDispatcher()
    {
        // question why is it virtual? Is it needed in this case?
        // It is a good custom to a virtual dtor if:
        //  - at least one function is virtual
        //  - or is intention to inherit from a class this class.
        // In our class virtual is not required. To emphasize that inherit is not intended I added final into class definition.
    }

    bool addCommandHandler(const std::string& command, CommandHandler handler)
    {
        //cout << "CommandDispatcher: addCommandHandler: " << command << std::endl;
        auto [_, inserted] = command_handlers_.insert(make_pair(command, handler));
        return inserted;
    }

    bool dispatchCommand(const std::string& command_json)
    {
        cout << "COMMAND: " << command_json << endl;
        Document d;
        if (d.Parse(command_json.c_str()).HasParseError()) {
            cerr << "Error(offset " << (unsigned)d.GetErrorOffset() << " ): " << rapidjson::GetParseError_En(d.GetParseError()) << std::endl;
            return false;
        }
        
        auto cmd = d.FindMember("command");
        if (cmd == d.MemberEnd() || !cmd->value.IsString()) {
            cerr << "Command is not a string!" << endl;
            return false;
        }
        const char* cmd_str = cmd->value.GetString();
        auto it = command_handlers_.find(cmd_str);
        if (it==command_handlers_.end()) {
            cerr << "Unknown command: " << cmd_str << endl;
            return false;
        }

        auto payload = d.FindMember("payload");
        if (payload == d.MemberEnd() || !payload->value.IsObject()) {
            cerr << "Payload is not an object!" << endl;
            return false;
        }

        return it->second(d["payload"]);
    }

private:

    // gimme ...
    std::map<std::string, CommandHandler> command_handlers_;

    // another gimme ...
    // Question: why delete these?
    // unused copy ctor and operator= forbid you to create a copies of CommandDispatcher.
    // This good practice allows you to avoid bug hard to find, when is created several copies of
    // the same container, although it was not intended.
    // This is especially usefull because constructor as you defined is not explicit.
    // Therefore `void fn(CommandDispatcher cd)` would create a copy (probably forgot reference).

    // delete unused constructors
    CommandDispatcher (const CommandDispatcher&) = delete;
    CommandDispatcher& operator= (const CommandDispatcher&) = delete;
};

int main()
{
    cout << "COMMAND DISPATCHER: STARTED" << std::endl;

    CommandDispatcher command_dispatcher;
    Controller controller;                 // controller class of functions to "dispatch" from Command Dispatcher

    // Implement
    // add command handlers in Controller class to CommandDispatcher using addCommandHandler
    // addCommand is helper that adopt Controller::*memberFn 
    auto addCommand = [&](const char* name, bool (Controller::*memberFn)(rapidjson::Value &)){
        using namespace std::placeholders;
        bool added = command_dispatcher.addCommandHandler(name, std::bind(memberFn,controller,_1));
        if (!added) {
            cerr << "handler " << name << " was not added." << endl;
        }
    };
    addCommand("help", &Controller::help);
    addCommand("exit", &Controller::exit);
    addCommand("now", &Controller::now);
    addCommand("hello", &Controller::hello);
    addCommand("times", &Controller::times);

    // gimme ...
    // command line interface
    string command;
    while( ! g_done ) {
        cout << R"(Please start with: {"command": "help", "payload":{}})" << endl;
        cout << "\tenter command : ";
        getline(cin, command);
        command_dispatcher.dispatchCommand(command);
    }

    cout << "COMMAND DISPATCHER: ENDED" << std::endl;
    return 0;
}