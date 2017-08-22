#pragma once

#include "System.hpp"
#include "pogre/App.hpp"

namespace kengine { class EntityManager; }

class OgreSystem : public kengine::System<OgreSystem>
{
public:
    OgreSystem(kengine::EntityManager &em);

    // System methods
public:
    void execute() final;
    void registerGameObject(kengine::GameObject &go) final;
    void removeGameObject(kengine::GameObject &go) final;

    // Helper
private:
    void createText(kengine::GameObject &go) noexcept;
    void createEntity(kengine::GameObject &go) noexcept;
    void createCamera(kengine::GameObject &go) noexcept;
    void createLight(kengine::GameObject &go) noexcept;
    void initLua() noexcept;

private:
    std::atomic<bool> _initDone = false;

private:
    kengine::EntityManager &_em;
    std::unique_ptr<pogre::App> _app;

private:
    Ogre::SceneManager *_scnMgr;

private:
    std::vector<kengine::GameObject *> _toSpawn;
    std::mutex _toSpawnMutex;
    using ToSpawnLock = std::unique_lock<decltype(_toSpawnMutex)>;

private:
    std::vector<std::pair<kengine::GameObject *, std::function<void()>>> _toMove;
    std::mutex _toMoveMutex;
    using ToMoveLock = std::unique_lock<decltype(_toMoveMutex)>;

private:
    std::vector<std::pair<kengine::GameObject *, std::function<void()>>> _toAttach;
    std::mutex _toAttachMutex;
    using ToAttachLock = std::unique_lock<decltype(_toAttachMutex)>;

private:
    std::vector<std::function<void()>> _toRun;
    std::mutex _toRunMutex;
    using ToRunLock = std::unique_lock<decltype(_toRunMutex)>;

private:
    using KeyHandler = std::function<void(SDL_Scancode keysym)>;
    struct
    {
        KeyHandler onPress = [](auto &&...) {};
        KeyHandler onRelease = [](auto &&...) {};
    } _keyHandler;

    using MouseButtonHandler = std::function<void(char button, int x, int y)>;
    struct
    {
        MouseButtonHandler onPress = [](auto &&...) {};
        MouseButtonHandler onRelease = [](auto &&...) {};
    } _mouseButtonHandler;

    std::function<void(int x, int y)> _mouseMovedHandler = [](auto &&...) {};
};
