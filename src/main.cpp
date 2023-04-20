#include "hcp/Application.hpp"

int main(int argc, char** argv)
{
    HCPApplication app("Hydroponics Control Panel");
    app.setup();

    while(!app.shouldClose())
    {
        app.loop();
    }

    app.terminate();
    return 0;
}