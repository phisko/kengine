#include "SkyBoxManager.hpp"
#include "EntityManager.hpp"

#include <OgreSceneManager.h>
#include <OgreMaterialManager.h>
#include <OgreMaterialSerializer.h>

#include "components/SkyBoxComponent.hpp"

#include <filesystem>
#include "file_extension.hpp"

SkyBoxManager::SkyBoxManager(kengine::EntityManager & em, Ogre::SceneManager & sceneManager)
	: _em(em), _sceneManager(sceneManager)
{

}

void SkyBoxManager::registerEntity(kengine::Entity & e) noexcept {
	if (!e.has<kengine::SkyBoxComponent>())
		return;

	const auto & comp = e.get<kengine::SkyBoxComponent>();

	const auto extension = putils::file_extension(comp.front);

	using string = kengine::SkyBoxComponent::string;
	const string pathNoExtension = putils::remove_extension(comp.front);

	const auto ogrePath = pathNoExtension + "_ogre";
	const auto front = ogrePath + "_fr." + extension;
	if (!std::filesystem::exists(front.c_str())) {
		std::filesystem::copy(comp.front.c_str(), front.c_str());
		std::filesystem::copy(comp.back.c_str(), (ogrePath + "_bk." + extension).c_str());
		std::filesystem::copy(comp.left.c_str(), (ogrePath + "_lf." + extension).c_str());
		std::filesystem::copy(comp.right.c_str(), (ogrePath + "_rt." + extension).c_str());
		std::filesystem::copy(comp.bottom.c_str(), (ogrePath + "_dn." + extension).c_str());
		std::filesystem::copy(comp.top.c_str(), (ogrePath + "_up." + extension).c_str());
	}

	auto dir = std::filesystem::path(comp.front.c_str());
	dir.remove_filename();
	const auto dirStr = dir.string();
	
	auto & resourceGroupManager = Ogre::ResourceGroupManager::getSingleton();
	if (!resourceGroupManager.resourceLocationExists(dirStr)) {
		resourceGroupManager.addResourceLocation(dirStr, "FileSystem", dirStr);
		resourceGroupManager.initialiseAllResourceGroups();
	}

	auto & materialManager = Ogre::MaterialManager::getSingleton();
	if (!materialManager.resourceExists(ogrePath.c_str())) {
		Ogre::String code;
		code += "material "; code += ogrePath; code += R"(
			{
				technique
				{
					pass
					{
						lighting off
						depth_write off
						texture_unit
						{
							texture )"; code += string(putils::remove_directory(pathNoExtension)) + "_ogre." + extension; code += R"( cubic
							tex_address_mode clamp
						}
					}
				}
			})";

		Ogre::DataStreamPtr stream(OGRE_NEW Ogre::MemoryDataStream(const_cast<void *>(static_cast<const void *>(code.c_str())),
			code.length() * sizeof(char), false));
		materialManager.parseScript(stream, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		Ogre::MaterialSerializer serializer;
		serializer.exportMaterial(materialManager.getByName(ogrePath.c_str()), (ogrePath + ".material").c_str());
	}

	_sceneManager.setSkyBox(true, ogrePath.c_str());
}

void SkyBoxManager::removeEntity(kengine::Entity & e) noexcept {
}
