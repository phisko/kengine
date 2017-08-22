#include "OgreSystem.hpp"
#include "Export.hpp"

#include "EntityManager.hpp"
#include "common/systems/LuaSystem.hpp"
#include "OgreComponent.hpp"
#include "OgreCameraComponent.hpp"
#include "OgreLightComponent.hpp"
#include "OgreTextComponent.hpp"

#include "common/components/TransformComponent.hpp"
#include "common/components/MetaComponent.hpp"
#include "common/components/CameraComponent.hpp"
#include "common/components/GUIComponent.hpp"

#include "pogre/FreeFloatingStrategy.hpp"

EXPORT kengine::ISystem *getSystem(kengine::EntityManager &em)
{
    return new OgreSystem(em);
}

OgreSystem::OgreSystem(kengine::EntityManager &em)
        : putils::BaseModule(&em), _em(em)
{
    runTask([this]
            {
                _app.reset(new pogre::App("kengine"));
                _app->start();
                getMediator()->running = false;
            }
    );
    while (!_app);

    _app->addAction(
            [this]
            {
                _scnMgr = _app->getRoot()->createSceneManager(Ogre::ST_GENERIC);
                _scnMgr->setAmbientLight(Ogre::ColourValue{ 0.3, 0.3, 0.3 });
                _app->addPermanentAction(
                        [this]
                        {
                            {
                                // Spawn entities that were requested by main thread
                                ToSpawnLock _(_toSpawnMutex);
                                for (const auto go : _toSpawn)
                                    createEntity(*go);
                                _toSpawn.clear();
                            }
                            {
                                // Update positions requested by main thread
                                ToMoveLock _(_toMoveMutex);
                                for (const auto & [go, func] : _toMove)
                                    func();
                                _toMove.clear();
                            }
                        }
                );
                initLua();
                _initDone = true;
            }
    );
    while (!_initDone);

    for (const auto go : _em.getGameObjects())
        registerGameObject(*go);
}

/*
 * System methods
 */

void OgreSystem::execute()
{
    // Attach components that were requested from Ogre thread
    {
        ToAttachLock _(_toAttachMutex);
        for (const auto & [go, func] : _toAttach)
        func();
        _toAttach.clear();
    }

    // Run handlers that were triggered from Ogre thread
    {
        ToRunLock _(_toRunMutex);
        for (const auto &f : _toRun)
            f();
        _toRun.clear();
    }

    ToMoveLock _(_toMoveMutex);

    // Update text
    for (const auto go : _em.getGameObjects<OgreTextComponent>())
    {
        auto &comp = go->getComponent<OgreTextComponent>();
        const auto &pos = go->getComponent<kengine::TransformComponent3d>().boundingBox.topLeft;
        _toMove.emplace_back(
                go,
                [this, pos, text = go->getComponent<kengine::GUIComponent>().text, &comp]
                {
                    comp.setPosition(pos);
                    comp.setText(text);
                }
        );
    }

    // Update cameras
    for (const auto go : _em.getGameObjects<OgreCameraComponent>())
    {
        const auto &cam = go->getComponent<kengine::CameraComponent3d>();
        const auto &pos = cam.frustrum.topLeft;

        auto &comp = go->getComponent<OgreCameraComponent>();
        _toMove.emplace_back(
                go,
                [this, &comp, pos, pitch = cam.pitch, yaw = cam.yaw]
                {
                    comp.setPosition(pos);
                    comp.getNode().setOrientation(Ogre::Quaternion::IDENTITY);
                    comp.getNode().pitch(Ogre::Radian((float)pitch), Ogre::Node::TS_WORLD);
                    comp.getNode().yaw(Ogre::Radian((float)yaw), Ogre::Node::TS_WORLD);
                }
        );
    }

    // Update lights
    for (const auto go : _em.getGameObjects<OgreLightComponent>())
    {
        const auto &transform = go->getComponent<kengine::TransformComponent3d>();
        const auto &pos = transform.boundingBox.topLeft;

        auto &comp = go->getComponent<OgreLightComponent>();
        _toMove.emplace_back(
                go,
                [this, &comp, pos] { comp.setPosition(pos); }
        );
    }

    // Update entities
    for (const auto go : _em.getGameObjects<OgreComponent>())
    {
        const auto &transform = go->getComponent<kengine::TransformComponent3d>();
        const auto &pos = transform.boundingBox.topLeft;
        const auto &size = transform.boundingBox.size;

        auto &comp = go->getComponent<OgreComponent>();
        _toMove.emplace_back(
                go,
                [this, &comp, pos, height = size.y, pitch = transform.pitch, yaw = transform.yaw]
                {
                    comp.setHeight(height);
                    comp.setPos(pos);
                    comp.setOrientation(pitch, yaw);
                }
        );
    }
}

void OgreSystem::registerGameObject(kengine::GameObject &go)
{
    if (!go.hasComponent<kengine::MetaComponent>() && !go.hasComponent<kengine::CameraComponent3d>() && !go.hasComponent<kengine::GUIComponent>())
        return;
    ToSpawnLock _(_toSpawnMutex);
    _toSpawn.push_back(&go);
}

void OgreSystem::removeGameObject(kengine::GameObject &go)
{
    if (!go.hasComponent<OgreComponent>() && !go.hasComponent<OgreCameraComponent>() &&
            !go.hasComponent<OgreLightComponent>() && !go.hasComponent<OgreTextComponent>())
    {
        // Make sure I wasn't attaching a component for this entity
        {
            ToSpawnLock _(_toSpawnMutex);
            const auto it = std::find(_toSpawn.begin(), _toSpawn.end(), &go);
            if (it != _toSpawn.end())
                _toSpawn.erase(it);
        }

        {
            ToAttachLock _(_toAttachMutex);
            auto it = std::find_if(_toAttach.begin(), _toAttach.end(),
                                         [&go](const auto &p) { return p.first == &go; });
            while (it != _toAttach.end())
            {
                _toAttach.erase(it);
                it = std::find_if(_toAttach.begin(), _toAttach.end(),
                                  [&go](const auto &p) { return p.first == &go; });
            }
        }

        return;
    }

    // If I was moving entity, stop
    {
        ToMoveLock _(_toMoveMutex);
        auto it = std::find_if(_toMove.begin(), _toMove.end(),
                                     [&go](const auto &p) { return p.first == &go; }
        );
        while (it != _toMove.end())
        {
            _toMove.erase(it);
            it = std::find_if(_toMove.begin(), _toMove.end(),
                              [&go](const auto &p) { return p.first == &go; }
            );
        }
    }

    if (go.hasComponent<OgreComponent>()) // Remove entity
    {
        auto &comp = go.getComponent<OgreComponent>();
        _app->addAction(
                [this, e = &comp.getEntity(), node = &comp.getNode()]
                {
                    _scnMgr->destroyEntity(e);
                    _scnMgr->getRootSceneNode()->removeChild(node);
                }
        );
        go.detachComponent(comp);
    }
    if (go.hasComponent<OgreCameraComponent>()) // Remove camera
    {
        auto &comp = go.getComponent<OgreCameraComponent>();
        _app->addAction([this, name = go.getName()] { _app->removeCamera(name); });
        go.detachComponent(comp);
    }
    if (go.hasComponent<OgreLightComponent>()) // Remove light
    {
        _app->addAction([this, light = &go.getComponent<OgreLightComponent>().getLight()]
                        {
                            _scnMgr->destroyLight(light);
                        }
        );
    }
    if (go.hasComponent<OgreTextComponent>()) // Remove text
    {
        auto &comp = go.getComponent<OgreTextComponent>();
        _app->addAction(
                [this, text = &comp.getText(), node = &comp.getNode()]
                {
                    _scnMgr->getRootSceneNode()->removeChild(node);
                    delete text;
                }
        );
        go.detachComponent(comp);
    }
}

/*
 * Helpers
 */

void OgreSystem::createEntity(kengine::GameObject &go) noexcept
{
    if (go.hasComponent<kengine::CameraComponent3d>())
    {
        createCamera(go);
        return;
    }

    if (go.hasComponent<kengine::GUIComponent>())
    {
        createText(go);
        return;
    }

    const auto &appearance = go.getComponent<kengine::MetaComponent>().appearance;

    if (appearance == "light")
    {
        createLight(go);
        return;
    }

    auto e = _scnMgr->createEntity(appearance);
    auto node = _scnMgr->getRootSceneNode()->createChildSceneNode();
    node->attachObject(e);

    ToAttachLock _(_toAttachMutex);
    _toAttach.emplace_back(
            &go,
            [this, &go, e, node]
            {
                auto &comp = go.attachComponent<OgreComponent>(*e, *node, *_app);
                const auto &transform = go.getComponent<kengine::TransformComponent3d>();
                comp.setWidth(transform.boundingBox.size.x);
                comp.setPos(transform.boundingBox.topLeft);
            }
    );
}

void OgreSystem::createCamera(kengine::GameObject &go) noexcept
{
    auto &cam = _app->addCamera(
            go.getName(),
            pogre::CameraMan(*_scnMgr, *_app->getRenderWindow(),
                             std::make_unique<pogre::FreeFloatingStrategy>()
            )
    );
    auto &strategy = cam.getStrategy<pogre::FreeFloatingStrategy>();
    strategy.setFlightSpeed(0);
    strategy.setMouseSpeed(0);

    ToAttachLock _(_toAttachMutex);
    _toAttach.emplace_back(
            &go,
            [this, &go, &cam, &strategy]
            {
                auto &comp = go.attachComponent<OgreCameraComponent>(cam, strategy);
            }
    );
}

void OgreSystem::createLight(kengine::GameObject &go) noexcept
{
    auto pointLight = _scnMgr->createLight();
    pointLight->setType(Ogre::Light::LT_POINT);

    const auto &transform = go.getComponent<kengine::TransformComponent3d>();
    const auto &pos = transform.boundingBox.topLeft;
    pointLight->setPosition((float)pos.x, (float)pos.y, (float)pos.z);

    go.attachComponent<OgreLightComponent>(*pointLight);
}

void OgreSystem::createText(kengine::GameObject &go) noexcept
{
    const auto &gui = go.getComponent<kengine::GUIComponent>();

    auto text = new Ogre::MovableText(go.getName().data(), gui.text);
    text->setTextAlignment(Ogre::MovableText::H_CENTER, Ogre::MovableText::V_CENTER);
    text->setCharacterHeight(gui.textSize);
    text->setFontName("StarWars");

    auto node = _scnMgr->getRootSceneNode()->createChildSceneNode();
    node->attachObject(text);

    ToAttachLock _(_toAttachMutex);
    _toAttach.emplace_back(
            &go,
            [this, &go, text, node]
            {
                auto &comp = go.attachComponent<OgreTextComponent>(*text, *node);
                const auto &transform = go.getComponent<kengine::TransformComponent3d>();
                comp.setPosition(transform.boundingBox.topLeft);
            }
    );
}

void OgreSystem::initLua() noexcept
{
    try
    {
        auto &lua = _em.getSystem<kengine::LuaSystem>();

        _app->addKeyHandler(
                [this](const OgreBites::KeyboardEvent &e)
                {
                    ToRunLock _(_toRunMutex);
                    _toRun.push_back([this, key = e.keysym.scancode] { _keyHandler.onPress(key); });
                },
                [this](const OgreBites::KeyboardEvent &e)
                {
                    ToRunLock _(_toRunMutex);
                    _toRun.push_back([this, key = e.keysym.scancode] { _keyHandler.onRelease(key); });
                }
        );
        lua.getState()["setKeyHandler"] =
                [this](const KeyHandler &onPress, const KeyHandler &onRelease)
                {
                    _keyHandler.onPress = onPress;
                    _keyHandler.onRelease = onRelease;
                };

        _app->addMouseButtonHandler(
                [this](const OgreBites::MouseButtonEvent &e)
                {
                    ToRunLock _(_toRunMutex);
                    _toRun.push_back([this, button = e.button, x = e.x, y = e.y] { _mouseButtonHandler.onPress(button, x, y); });
                },
                [this](const OgreBites::MouseButtonEvent &e)
                {
                    ToRunLock _(_toRunMutex);
                    _toRun.push_back([this, button = e.button, x = e.x, y = e.y] { _mouseButtonHandler.onRelease(button, x, y); });
                }
        );
        lua.getState()["setMouseButtonHandler"] =
                [this](const MouseButtonHandler &onPress, const MouseButtonHandler &onRelease)
                {
                    _mouseButtonHandler.onPress = onPress;
                    _mouseButtonHandler.onRelease = onRelease;
                };

        _app->addMouseMovedHandler(
                [this](const OgreBites::MouseMotionEvent &e)
                {
                    ToRunLock _(_toRunMutex);
                    _toRun.push_back([this, x = e.x, y = e.y] { _mouseMovedHandler(x, y); });
                }
        );
        lua.getState()["setMouseMovedHandler"] = [this](const std::function<void(int x, int y)> &onMoved) { _mouseMovedHandler = onMoved; };
    }
    catch (const std::out_of_range &) {}
}
