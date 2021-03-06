/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/app/ViewController.h>
#include <gamebase/impl/app/InputRegister.h>
#include <gamebase/impl/graphics/Window.h>
#include <gamebase/impl/tools/Counter.h>
#include <map>

namespace gamebase { namespace impl {

class CanvasLayout;

class GAMEBASE_API Application : public ViewController {
public:
    Application();
    ~Application();

    void setWindowTitle(const std::string& title);
    const std::string& configName() const { return m_configName; }
    void setConfigName(const std::string& name) { m_configName = name; }
    bool init(int* argc, char** argv);
    bool initOverrideConfig(int* argc, char** argv);
    void setMode(GraphicsMode::Enum mode);
    GraphicsMode::Enum mode() const { return m_window.mode(); }
    void setWindowSize(unsigned int width, unsigned int height);
    Size windowSize() const;
    void setMinWindowSize(unsigned int w, unsigned int h);
    void setMaxWindowSize(unsigned int w, unsigned int h);
    void hideConsole();
    void showConsole();
	void hideCursor();
	void showCursor();
	void maximizeWindow();
    void resetResourceCaches();
    const Window& window() const { return m_window; }

    void run();
    void close();

    void displayFunc();
    void resizeFunc(Size size);
    void keyboardFunc(int key);
    void keyboardUpFunc(int key);
    void textFunc(uint32_t unicodeKey);
    void motionFunc(int x, int y);
    void mouseFunc(int button, int state, int x, int y);
    void wheelFunc(int wheel, float offset, int x, int y);

    virtual void render() {}

    void deactivateAllControllers();
    void deactivateControllerByName(const std::string& controllerName);
    void deactivateController(ViewController* controller);
    void activateControllerByName(const std::string& controllerName);
    void activateController(ViewController* controller);

    virtual void onResize(const Size& size) {}
    virtual bool onClose() { return true; }
    virtual void onTerminate() {}
    virtual void processKeyDown(InputKey::Enum key) {}
    virtual void processKeyUp(InputKey::Enum key) {}
    virtual void processMouseMotion(const Vec2& pos) {}
    virtual void processMouseButtonDown(InputKey::Enum button) {}
    virtual void processMouseButtonUp(InputKey::Enum button) {}

    const InputRegister& input() { return m_inputRegister; }
    const std::shared_ptr<CanvasLayout>& topViewLayout() { return m_topViewLayout; }

protected:
    void setView(const std::shared_ptr<IObject>& obj);
    bool initApplication();
    void registerController(const std::shared_ptr<ViewController>& controller);
    void sortControllers();
    ViewController* currentController();
    void setFocus(ViewController* controller);
    void filterControllers();

    void processMouseActions();
    void processMouseActions(const std::shared_ptr<IObject>& curObject);
    void changeSelectionState(SelectionState::Enum state);
    void loadResourcesImpl();
    void resetResourceCachesImpl();

    bool m_isRunning;
    Window m_window;
    uint64_t m_frameNum;
    Time m_loadTime;
    std::string m_configName;
	std::shared_ptr<CanvasLayout> m_topViewLayout;
    std::unique_ptr<Counter> m_fpsCounter;
    InputRegister m_inputRegister;
    boost::optional<Size> m_pendingWindowSize;
    bool m_pendingCacheReset;
	bool m_pendingMaximizeWindow;
	bool m_isCursorVisible;

    std::weak_ptr<IObject> m_mouseOnObject;
    std::weak_ptr<IObject> m_selectedObject;
    std::weak_ptr<IObject> m_associatedSelectable;

    std::map<std::string, std::shared_ptr<ViewController>> m_controllers;
    std::vector<ViewController*> m_activeControllers;
    ViewController* m_focusedController;

    std::unique_ptr<IRegistrable> m_registerRoot;
};

GAMEBASE_API extern Application* app;

} }
