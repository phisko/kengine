#include "MagicaVoxelObjectManager.hpp"

#include <OgreManualObject.h>
#include <OgreSceneManager.h>
#include <OgreMesh.h>
#include <OgreMeshSerializer.h>
#include <OgreMaterialManager.h>
#include <OgreMaterialSerializer.h>

#include "components/GraphicsComponent.hpp"
#include "OgreObjectComponent.hpp"

#include <filesystem>
#include "MagicaVoxelToPolyvoxLoader.hpp"
#include "lengthof.hpp"

static void initMaterial() {
	auto & materialManager = Ogre::MaterialManager::getSingleton();
	if (materialManager.resourceExists("VoxelMaterial"))
		return;

	Ogre::String code;
	code += R"(material VoxelMaterial
			{
				technique
				{
					pass
					{
						diffuse vertexcolour
						specular vertexcolour
						ambient vertexcolour
						lighting on
					}
				}
			})";

	Ogre::DataStreamPtr stream(OGRE_NEW Ogre::MemoryDataStream(const_cast<void *>(static_cast<const void *>(code.c_str())),
		code.length() * sizeof(char), false));
	materialManager.parseScript(stream, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
}

MagicaVoxelObjectManager::MagicaVoxelObjectManager(kengine::EntityManager & em, Ogre::SceneManager & sceneManager)
	: _em(em), _sceneManager(sceneManager)
{
	initMaterial();
}

template<typename VertexType>
static Ogre::Vector3 getVertexPosition(const VertexType & vertex, const putils::Point3i & size) {
	return Ogre::Vector3(vertex.position.getX() - (float)size.x / 2.f, vertex.position.getY(), vertex.position.getZ() - (float)size.z / 2.f);
}

static void convert(const char * voxFile, const char * outputFile, Ogre::SceneManager & sceneManager) {
	putils::Point3i size;
	const auto polyvoxMesh = putils::MagicaVoxel::loadVoxFile(voxFile, &size);

	Ogre::ManualObject * manual = sceneManager.createManualObject();
	manual->begin("VoxelMaterial");

	using VertexType = decltype(polyvoxMesh)::VertexType;
	putils::vector<VertexType, 3> triangle;
	for (unsigned int i = 0; i < polyvoxMesh.getNoOfIndices(); ++i) {
		const auto & vertex = polyvoxMesh.getVertex(polyvoxMesh.getIndex(i));
		triangle.push_back(vertex);

		if (triangle.full()) {
			const auto normal = Ogre::Math::calculateFaceNormal(getVertexPosition(triangle[0], size), getVertexPosition(triangle[1], size), getVertexPosition(triangle[2], size)).xyz();
			for (const auto & vertex : triangle) {
				manual->position(getVertexPosition(vertex, size));
				manual->normal(normal);
				manual->colour(vertex.data.color[0], vertex.data.color[1], vertex.data.color[2]);
			}
			triangle.clear();
		}
	}

	for (unsigned int i = 0; i < polyvoxMesh.getNoOfIndices(); ++i)
		manual->index(i);

	manual->end();

	Ogre::MeshSerializer serializer;
	auto mesh = manual->convertToMesh(voxFile);
	serializer.exportMesh(mesh.get(), outputFile);
}

void MagicaVoxelObjectManager::registerEntity(kengine::Entity & e) noexcept {
	if (!e.has<kengine::GraphicsComponent>())
		return;

	const auto & graphics = e.get<kengine::GraphicsComponent>();

	Ogre::String finalName = graphics.appearance.c_str();
	Ogre::String extension, path;
	Ogre::StringUtil::splitFullFilename(graphics.appearance.c_str(), finalName, extension, path);

	if (extension != "vox")
		return;

	finalName = finalName + ".mesh";
	const auto fullPath = path + finalName;

	if (!std::filesystem::exists(fullPath.c_str()))
		convert(graphics.appearance, fullPath.c_str(), _sceneManager);

	auto & resourceGroupManager = Ogre::ResourceGroupManager::getSingleton();
	if (!resourceGroupManager.resourceGroupExists(graphics.appearance.c_str())) {
		resourceGroupManager.addResourceLocation(path, "FileSystem", graphics.appearance.c_str());
		resourceGroupManager.initialiseAllResourceGroups();
	}

	OgreObjectComponent comp;
	comp.entity = _sceneManager.createEntity(finalName);
	comp.node = _sceneManager.getRootSceneNode()->createChildSceneNode();
	comp.node->attachObject(comp.entity);
	e += comp;
}
