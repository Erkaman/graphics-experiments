#include "ao_application.hpp"

#include "ewa/config.hpp"

int main (int argc, char *argv[]) {


    Config& config = Config::GetInstance();

    config.SetGui(true);

    AoApplication app(argc, argv);
    app.Start();
    return 0;

}
