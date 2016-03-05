#pragma once

#include <functional>

namespace gamebase {

#define connect0(OBJECT_NAME, METHOD_NAME) \
    OBJECT_NAME->setCallback(std::bind(&MyApp::METHOD_NAME, this))

#define connect1(OBJECT_NAME, METHOD_NAME, ARG1) \
    OBJECT_NAME->setCallback(std::bind(&MyApp::METHOD_NAME, this, ARG1))

#define connect2(OBJECT_NAME, METHOD_NAME, ARG1, ARG2) \
    OBJECT_NAME->setCallback(std::bind(&MyApp::METHOD_NAME, this, ARG1, ARG2))

#define connect3(OBJECT_NAME, METHOD_NAME, ARG1, ARG2, ARG3) \
    OBJECT_NAME->setCallback(std::bind(&MyApp::METHOD_NAME, this, ARG1, ARG2, ARG3))

#define connect4(OBJECT_NAME, METHOD_NAME, ARG1, ARG2, ARG3, ARG4) \
    OBJECT_NAME->setCallback(std::bind(&MyApp::METHOD_NAME, this, ARG1, ARG2, ARG3, ARG4))

}