#pragma once

#include <string>

class GuiListener {

public:
    virtual void TranslationAccepted()=0;
    virtual void RotationAccepted()=0;

    virtual void ModelAdded(const std::string& filename)=0;

};
