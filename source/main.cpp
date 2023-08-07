// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sstream>
#include <iostream>
#include <unistd.h>
// Include the main libnx system header, for Switch development
#include <switch.h>
// Include custom webdav libs
#include "webdav.hpp"
#include <inih/cpp/INIReader.h>

using namespace std;

// Main program entrypoint
int main(int argc, char *argv[])
{
    // This example uses a text console, as a simple way to output text to the screen.
    // If you want to write a software-rendered graphics application,
    //   take a look at the graphics/simplegfx example, which uses the libnx Framebuffer API instead.
    // If on the other hand you want to write an OpenGL based application,
    //   take a look at the graphics/opengl set of examples, which uses EGL instead.
    consoleInit(NULL);

    // Configure our supported input layout: a single player with standard controller styles
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);

    // Initialize the default gamepad (which reads handheld mode inputs as well as the first connected controller)
    PadState pad;
    padInitializeDefault(&pad);

    socketInitializeDefault();

    printf(CONSOLE_BLUE "Switch Nexcloud\n\nPress A to start.\n" CONSOLE_RESET);
    consoleUpdate(NULL);
    while (appletMainLoop())
    {
        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);
        if (kDown & HidNpadButton_A)
        {
            break;
        }
    }

    INIReader reader("/switch/NXDavSync.ini");
    vector<string> bad_config;
    vector<pair<string, WebDavClient *>> clients;
    if (reader.ParseError() < 0)
    {
        printf("Error loading configuration file at /switch/NXDavSync.ini");
        consoleUpdate(NULL);
    }
    else
    {
        string enabled = reader.Get("General", "Enabled", "");
        string buf;
        stringstream ss(enabled);

        while (ss >> buf)
        {
            if (buf == "")
            {
                break;
            }
            // Load server config
            string url = reader.Get(buf, "Url", "");
            string local_path = reader.Get(buf, "LocalPath", "");
            string username = reader.Get(buf, "Username", "");
            string password = reader.Get(buf, "Password", "");
            if (url.size() == 0 || local_path.size() == 0)
            {
                bad_config.push_back(buf);
            }
            else
            {
                WebDavClient *c = new WebDavClient(url, local_path);
                if (username.size() != 0)
                {
                    c->set_basic_auth(username, password);
                }
                c->set_pad_state(&pad);
                clients.push_back(make_pair(buf, c));
            }
        }
    }

    if (!bad_config.empty())
    {
        for (auto name : bad_config)
        {
            printf("Malformed config for server %s", name.c_str());
            consoleUpdate(NULL);
        }
    }
    else
    {
        vector<pair<string, bool>> results;
        for (auto [name, client] : clients)
        {
            printf(CONSOLE_BLUE "\nSyncing %s\n", name.c_str());
            printf(CONSOLE_RESET);
            consoleUpdate(NULL);
            bool this_result = client->compareAndUpdate();
            results.push_back(make_pair(name, this_result));
        }
        // Print summary
        printf(CONSOLE_BLUE "\n\n===== Sync Summary =====\n\n" CONSOLE_RESET);
        consoleUpdate(NULL);
        for (auto [name, result] : results)
        {
            if (result)
            {
                printf(CONSOLE_GREEN "SUCCESS " CONSOLE_RESET);
            }
            else
            {
                printf(CONSOLE_RED "FAILED  " CONSOLE_RESET);
            }
            printf("%s\n", name.c_str());
            consoleUpdate(NULL);
        }
    }

    printf(CONSOLE_GREEN "\n Press A to exit.\n" CONSOLE_RESET);
    consoleUpdate(NULL);
    while (appletMainLoop())
    {
        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);
        if (kDown & HidNpadButton_A)
        {
            break;
        }
    }

    socketExit();

    // Deinitialize and clean up resources used by the console (important!)
    consoleExit(NULL);
    return 0;
}
