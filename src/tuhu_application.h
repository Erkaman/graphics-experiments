#pragma once

#include "application.h"

class TuhuApplication : public Application{

public:

    TuhuApplication() {}

    void Init() override;
    void Render() override;
    void Update() override;


};
