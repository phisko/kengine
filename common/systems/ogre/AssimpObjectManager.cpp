// Ogre dll warnings
#pragma warning(disable : 4251 4275) 

#include "AssimpObjectManager.hpp"
#include "AssimpLoader.h"
#include "EntityManager.hpp"

#include <OgreSceneManager.h>

#include "OgreObjectComponent.hpp"
#include "components/GraphicsComponent.hpp"

#include <filesystem>

static AssimpLoader g_assimp;

AssimpObjectManager::AssimpObjectManager(kengine::EntityManager & em, Ogre::SceneManager & sceneManager)
	: _em(em), _sceneManager(sceneManager)
{
}

void AssimpObjectManager::registerEntity(kengine::Entity & e) noexcept {
	if (!e.has<kengine::GraphicsComponent>())
		return;

	const auto & graphics = e.get<kengine::GraphicsComponent>();

	OgreObjectComponent comp;

	Ogre::String finalName = graphics.appearance.c_str();
	Ogre::String extension, path;
	Ogre::StringUtil::splitFullFilename(graphics.appearance.c_str(), finalName, extension, path);

	if (extension == "mesh")
		return;

	finalName = finalName + "_" + extension + ".mesh";
	const auto fullPath = path + finalName;

	auto & resourceGroupManager = Ogre::ResourceGroupManager::getSingleton();

	if (!std::filesystem::exists(fullPath.c_str())) {
		AssimpLoader::AssOptions options;
		options.source = graphics.appearance;
		if (!g_assimp.convert(options))
			return;
	}

	resourceGroupManager.removeResourceLocation(path);
	resourceGroupManager.removeResourceLocation("./resources");

	if (!resourceGroupManager.resourceLocationExists(path)) {
		resourceGroupManager.addResourceLocation(path, "FileSystem");
		resourceGroupManager.initialiseAllResourceGroups();
	}


	comp.entity = _sceneManager.createEntity(finalName);
	comp.node = _sceneManager.getRootSceneNode()->createChildSceneNode();
	comp.node->attachObject(comp.entity);

	e += comp;
}