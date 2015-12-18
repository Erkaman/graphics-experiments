#include "tuhu_application.hpp"

#include "ewa/config.hpp"

int main (int argc, char *argv[]) {

    Config& config = Config::GetInstance();

    config.SetGui(true);

    TuhuApplication app(argc, argv);
    app.Start();
    return 0;
}
