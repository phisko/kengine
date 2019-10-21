// Ogre dll warnings
#pragma warning(disable : 4251 4275) 

/*
-----------------------------------------------------------------------------
This source file is part of
                                    _
  ___   __ _ _ __ ___  __ _ ___ ___(_)_ __ ___  _ __
 / _ \ / _` | '__/ _ \/ _` / __/ __| | '_ ` _ \| '_ \
| (_) | (_| | | |  __/ (_| \__ \__ \ | | | | | | |_) |
 \___/ \__, |_|  \___|\__,_|___/___/_|_| |_| |_| .__/
       |___/                                   |_|

For the latest info, see https://bitbucket.org/jacmoe/ogreassimp

Copyright (c) 2011 Jacob 'jacmoe' Moen

Licensed under the MIT license:

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#include "AssimpLoader.h"
#include "assimp/Importer.hpp"
//#include "assimp/DefaultLogger.h"
#include "assimp/DefaultLogger.hpp"
#include "OgreDataStream.h"
#include "OgreImage.h"
#include "OgreTexture.h"
#include "OgreTextureManager.h"
#include "OgreMaterial.h"
#include "OgreMaterialManager.h"
#include "OgreLog.h"
#include "OgreLogManager.h"
#include "OgreHardwareBuffer.h"
#include "OgreMesh.h"
#include "OgreSubMesh.h"
#include "OgreDefaultHardwareBufferManager.h"
#include "OgreMeshManager.h"
#include "OgreSceneManager.h"
#include <OgreStringConverter.h>
#include <OgreSkeletonManager.h>
#include "OgreMeshSerializer.h"
#include "OgreSkeletonSerializer.h"
#include "OgreAnimation.h"
#include "OgreAnimationTrack.h"
#include "OgreKeyFrame.h"
#include "OgreLodConfig.h"
#include "OgreLodStrategyManager.h"
#include "OgreDistanceLodStrategy.h"
#include "Ogre.h"
#include <tuple>
//#include "OgreXMLSkeletonSerializer.h"

#if OGRE_VERSION < (1 << 16 | 11 << 8 | 0)
    typedef Ogre::Matrix4 Affine3;
#else
    typedef Ogre::Affine3 Affine3;
#endif

Ogre::String toString(const aiColor4D& colour)
{
    return Ogre::StringConverter::toString(Ogre::Real(colour.r)) + " " +
        Ogre::StringConverter::toString(Ogre::Real(colour.g)) + " " +
        Ogre::StringConverter::toString(Ogre::Real(colour.b)) + " " +
        Ogre::StringConverter::toString(Ogre::Real(colour.a));
}

int AssimpLoader::msBoneCount = 0;

AssimpLoader::AssimpLoader()
{
    //mSkeletonRootNode = NULL;
}

AssimpLoader::~AssimpLoader()
{
}

bool AssimpLoader::convert(const AssOptions options, Ogre::MeshPtr *meshPtr,  Ogre::SkeletonPtr *skeletonPtr)
{
    mAnimationSpeedModifier = options.animationSpeedModifier;
    mLoaderParams = options.params;
    mQuietMode = ((mLoaderParams & LP_QUIET_MODE) == 0) ? false : true;
    mCustomAnimationName = options.customAnimationName;
    if ((mLoaderParams & LP_USE_LAST_RUN_NODE_DERIVED_TRANSFORMS) == false)
    {
        mNodeDerivedTransformByName.clear();
    }

    Ogre::String extension;
    Ogre::StringUtil::splitFullFilename(options.source, mBasename, extension, mPath);
    mBasename = mBasename + "_" + extension;

    if(!options.dest.empty())
    {
        mPath = options.dest + "/";
    }

    Assimp::DefaultLogger::create("asslogger.log",Assimp::Logger::VERBOSE);
    Assimp::DefaultLogger::get()->info("Logging asses");

    if(!mQuietMode)
    {
        Ogre::LogManager::getSingleton().logMessage("*** Loading ass file... ***");
        Ogre::LogManager::getSingleton().logMessage("Filename " + options.source);
    }
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mPath, "FileSystem");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("./resources", "FileSystem");

    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    const aiScene *scene;

    Assimp::Importer importer;
    scene = importer.ReadFile(options.source.c_str(), aiProcessPreset_TargetRealtime_Quality | aiProcess_TransformUVCoords | aiProcess_FlipUVs);

    // If the import failed, report it
    if( !scene)
    {
        if(!mQuietMode)
        {
            Ogre::LogManager::getSingleton().logMessage("AssImp importer failed with the following message:");
            Ogre::LogManager::getSingleton().logMessage(importer.GetErrorString() );
        }
        return false;
    }

    grabNodeNamesFromNode(scene, scene->mRootNode);
    grabBoneNamesFromNode(scene, scene->mRootNode);

    computeNodesDerivedTransform(scene, scene->mRootNode, scene->mRootNode->mTransformation);

    if(mBonesByName.size())
    {
        mSkeleton = Ogre::SkeletonManager::getSingleton().create("conversion", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

        msBoneCount = 0;
        createBonesFromNode(scene, scene->mRootNode);
        msBoneCount = 0;
        createBoneHiearchy(scene, scene->mRootNode);

        if(scene->HasAnimations())
        {
            for(unsigned int i = 0; i < scene->mNumAnimations; ++i)
            {
                parseAnimation(scene, i, scene->mAnimations[i]);
            }
        }
    }

    loadDataFromNode(scene, scene->mRootNode, mPath);

    if(!mQuietMode)
    {
        Ogre::LogManager::getSingleton().logMessage("*** Finished loading ass file ***");
    }
    Assimp::DefaultLogger::kill();

    if(mSkeleton)
    {

        if(!mQuietMode)
        {
            Ogre::LogManager::getSingleton().logMessage("Root bone: " + mSkeleton->getRootBones()[0]->getName());
        }

        unsigned short numBones = mSkeleton->getNumBones();
        unsigned short i;
        for (i = 0; i < numBones; ++i)
        {
            Ogre::Bone* pBone = mSkeleton->getBone(i);
            assert(pBone);
        }

		if(skeletonPtr)
			(*skeletonPtr) = mSkeleton;
		else
		{
			Ogre::SkeletonSerializer binSer;
			binSer.exportSkeleton(mSkeleton.get(), mPath + mBasename + ".skeleton");
		}
    }

    Ogre::MeshSerializer meshSer;
    for(MeshVector::iterator it = mMeshes.begin(); it != mMeshes.end(); ++it)
    {
        Ogre::MeshPtr mMesh = *it;
        if(mBonesByName.size())
        {
            mMesh->setSkeletonName(mBasename + ".skeleton");
        }

        Ogre::Mesh::SubMeshIterator smIt = mMesh->getSubMeshIterator();
        while (smIt.hasMoreElements())
        {
            Ogre::SubMesh* sm = smIt.getNext();
            if (!sm->useSharedVertices)
            {
#if (OGRE_VERSION >  ((1 << 16) | (7 << 8) | 0))
                Ogre::VertexDeclaration* newDcl =
                    sm->vertexData->vertexDeclaration->getAutoOrganisedDeclaration(mMesh->hasSkeleton(), mMesh->hasVertexAnimation(), false);
#else
                Ogre::VertexDeclaration* newDcl =
                    sm->vertexData->vertexDeclaration->getAutoOrganisedDeclaration(mMesh->hasSkeleton(), mMesh->hasVertexAnimation());
#endif
                if (*newDcl != *(sm->vertexData->vertexDeclaration))
                {
                    // Usages don't matter here since we're only exporting
                    Ogre::BufferUsageList bufferUsages;
                    for (size_t u = 0; u <= newDcl->getMaxSource(); ++u)
                        bufferUsages.push_back(Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
                    sm->vertexData->reorganiseBuffers(newDcl, bufferUsages);
                }
            }
        }

        if (options.numLods > 0)
        {
            unsigned short numLod;
            Ogre::LodConfig lodConfig;
            lodConfig.levels.clear();
            lodConfig.mesh = mMesh->clone(mMesh->getName());
#if (OGRE_VERSION < ((1 << 16) | (9 << 8) | 0))
            lodConfig.strategy = Ogre::DistanceLodStrategy::getSingletonPtr();
#else
            lodConfig.strategy = Ogre::DistanceLodBoxStrategy::getSingletonPtr();
#endif

            Ogre::LodLevel lodLevel;
            lodLevel.reductionMethod = Ogre::LodLevel::VRM_PROPORTIONAL;

            numLod = options.numLods;
            if (options.usePercent)
            {
                lodLevel.reductionMethod = Ogre::LodLevel::VRM_PROPORTIONAL;
                lodLevel.reductionValue = options.lodPercent * 0.01f;
            }
            else
            {
                lodLevel.reductionMethod = Ogre::LodLevel::VRM_CONSTANT;
                lodLevel.reductionValue = (Ogre::Real)options.lodFixed;
            }
            Ogre::Real currDist = 0;
            for (unsigned short iLod = 0; iLod < numLod; ++iLod)
            {
                currDist += options.lodValue;
                Ogre::Real currDistSq = Ogre::Math::Sqr(currDist);
                lodLevel.distance = currDistSq;
                lodConfig.levels.push_back(lodLevel);
            }

            mMesh->setLodStrategy(Ogre::LodStrategyManager::getSingleton().getStrategy(options.lodStrategy));
            //Ogre::ProgressiveMeshGenerator pm;
            //pm.generateLodLevels(lodConfig);
        }

		if(meshPtr)
			(*meshPtr) = mMesh;
		else
			meshSer.exportMesh(mMesh.get(), mPath + mBasename + ".mesh");
    }


    // serialise the materials
    if((mLoaderParams & LP_GENERATE_MATERIALS_AS_CODE) == 0)
    {
        Ogre::MaterialSerializer ms;
        std::vector<Ogre::String> exportedNames;

        for(MeshVector::iterator it = mMeshes.begin(); it != mMeshes.end(); ++it)
        {
            Ogre::MeshPtr mMesh = *it;

            // queue up the materials for serialise
            Ogre::MaterialManager *mmptr = Ogre::MaterialManager::getSingletonPtr();
            Ogre::Mesh::SubMeshIterator smIt = mMesh->getSubMeshIterator();
            while(smIt.hasMoreElements())
            {
                Ogre::SubMesh* sm = smIt.getNext();
                Ogre::String matName(sm->getMaterialName());
                if (std::find(exportedNames.begin(), exportedNames.end(), matName) == exportedNames.end())
                {
                    Ogre::MaterialPtr materialPtr = mmptr->getByName(matName);
                    ms.queueForExport(materialPtr);
                    exportedNames.push_back(matName);
                }
            }
        }

        if(exportedNames.size()&&!meshPtr)
            ms.exportQueued(mPath + mBasename + ".material", true);
    }
    else
    {
		if(!meshPtr)
		{
			std::ofstream stream;
			stream.open( (mPath + mBasename + ".material").c_str(), std::ios::out | std::ios::binary);
			//stream << "import * from base.material\n\n";
			stream << mMaterialCode;
			stream.close();
		}
    }

    for(auto mesh: mMeshes)
        mesh->load();

    // clean up
    mMeshes.clear();
    mMaterialCode = "";
    mBonesByName.clear();
    mBoneNodesByName.clear();
    boneMap.clear();
#if (OGRE_VERSION < ((1 << 16) | (9 << 8) | 0))
    mSkeleton = Ogre::SkeletonPtr(NULL);
#else
    mSkeleton = Ogre::SkeletonPtr();
#endif
    mCustomAnimationName = "";
    // etc...

    Ogre::MeshManager::getSingleton().removeUnreferencedResources();
    Ogre::SkeletonManager::getSingleton().removeUnreferencedResources();

    return true;
}

/** translation, rotation, scale */
typedef std::tuple< aiVectorKey*, aiQuatKey*, aiVectorKey* > KeyframeData;
typedef std::map< Ogre::Real, KeyframeData > KeyframesMap;

template <int v>
struct Int2Type
{
    enum { value = v };
};

// T should be a Loki::Int2Type<>
template< typename T > void GetInterpolationIterators(KeyframesMap& keyframes,
                                                    KeyframesMap::iterator it,
                                                    KeyframesMap::reverse_iterator& front,
                                                    KeyframesMap::iterator& back)
{
    front = KeyframesMap::reverse_iterator(it);

    front++;
    for(front; front != keyframes.rend(); front++)
    {
        if(std::get< T::value >(front->second) != NULL)
        {
            break;
        }
    }

    back = it;
    back++;
    for(back; back != keyframes.end(); back++)
    {
        if(std::get< T::value >(back->second) != NULL)
        {
            break;
        }
    }
}

aiVector3D getTranslate(aiNodeAnim* node_anim, KeyframesMap& keyframes, KeyframesMap::iterator it, Ogre::Real ticksPerSecond)
{
    aiVectorKey* translateKey = std::get<0>(it->second);
    aiVector3D vect;
    if(translateKey)
    {
        vect = translateKey->mValue;
    }
    else
    {
        KeyframesMap::reverse_iterator front;
        KeyframesMap::iterator back;


        GetInterpolationIterators< Int2Type<0> > (keyframes, it, front, back);

        KeyframesMap::reverse_iterator rend = keyframes.rend();
        KeyframesMap::iterator end = keyframes.end();
        aiVectorKey* frontKey = NULL;
        aiVectorKey* backKey = NULL;

        if(front != rend)
            frontKey = std::get<0>(front->second);

        if(back != end)
            backKey = std::get<0>(back->second);

        // got 2 keys can interpolate
        if(frontKey && backKey)
        {
            float prop = (float)(((double)it->first - frontKey->mTime) / (backKey->mTime - frontKey->mTime));
            prop /= ticksPerSecond;
            vect = ((backKey->mValue - frontKey->mValue) * prop) + frontKey->mValue;
        }

        else if(frontKey)
        {
            vect = frontKey->mValue;
        }
        else if(backKey)
        {
            vect = backKey->mValue;
        }
    }

    return vect;
}

aiQuaternion getRotate(aiNodeAnim* node_anim, KeyframesMap& keyframes, KeyframesMap::iterator it, Ogre::Real ticksPerSecond)
{
    aiQuatKey* rotationKey = std::get<1>(it->second);
    aiQuaternion rot;
    if(rotationKey)
    {
        rot = rotationKey->mValue;
    }
    else
    {
        KeyframesMap::reverse_iterator front;
        KeyframesMap::iterator back;

        GetInterpolationIterators< Int2Type<1> > (keyframes, it, front, back);

        KeyframesMap::reverse_iterator rend = keyframes.rend();
        KeyframesMap::iterator end = keyframes.end();
        aiQuatKey* frontKey = NULL;
        aiQuatKey* backKey = NULL;

        if(front != rend)
            frontKey = std::get<1>(front->second);

        if(back != end)
            backKey = std::get<1>(back->second);

        // got 2 keys can interpolate
        if(frontKey && backKey)
        {
            float prop = (float)(((double)it->first - frontKey->mTime) / (backKey->mTime - frontKey->mTime));
            prop /= ticksPerSecond;
            aiQuaternion::Interpolate(rot, frontKey->mValue, backKey->mValue, prop);
        }

        else if(frontKey)
        {
            rot = frontKey->mValue;
        }
        else if(backKey)
        {
            rot = backKey->mValue;
        }
    }

    return rot;
}

aiVector3D getScale(aiNodeAnim* node_anim, KeyframesMap& keyframes, KeyframesMap::iterator it, Ogre::Real ticksPerSecond)
{
    aiVectorKey* scaleKey = std::get<2>(it->second);
    aiVector3D vect;
    if(scaleKey)
    {
        vect = scaleKey->mValue;
    }
    else
    {
        KeyframesMap::reverse_iterator front;
        KeyframesMap::iterator back;


        GetInterpolationIterators< Int2Type<2> > (keyframes, it, front, back);

        KeyframesMap::reverse_iterator rend = keyframes.rend();
        KeyframesMap::iterator end = keyframes.end();
        aiVectorKey* frontKey = NULL;
        aiVectorKey* backKey = NULL;

        if(front != rend)
            frontKey = std::get<0>(front->second);

        if(back != end)
            backKey = std::get<0>(back->second);

        // got 2 keys can interpolate
        if(frontKey && backKey)
        {
            float prop = (float)(((double)it->first - frontKey->mTime) / (backKey->mTime - frontKey->mTime));
            prop /= ticksPerSecond;
            vect = ((backKey->mValue - frontKey->mValue) * prop) + frontKey->mValue;
        }

        else if(frontKey)
        {
            vect = frontKey->mValue;
        }
        else if(backKey)
        {
            vect = backKey->mValue;
        }
    }

    return vect;
}

void AssimpLoader::parseAnimation (const aiScene* mScene, int index, aiAnimation* anim)
{
    // DefBonePose a matrix that represents the local bone transform (can build from Ogre bone components)
    // PoseToKey a matrix representing the keyframe translation
    // What assimp stores aiNodeAnim IS the decomposed form of the transform (DefBonePose * PoseToKey)
    // To get PoseToKey which is what Ogre needs we'ed have to build the transform from components in
    // aiNodeAnim and then DefBonePose.Inverse() * aiNodeAnim(generated transform) will be the right transform

    Ogre::String animName;
    if(mCustomAnimationName != "")
    {
        animName = mCustomAnimationName;
        if(index >= 1)
        {
            animName += Ogre::StringConverter::toString(index);
        }
    }
    else
    {
        animName = Ogre::String(anim->mName.data);
    }
    if(animName.length() < 1)
    {
        animName = "Animation" + Ogre::StringConverter::toString(index);
    }

    if(!mQuietMode)
    {
        Ogre::LogManager::getSingleton().logMessage("Animation name = '" + animName + "'");
        Ogre::LogManager::getSingleton().logMessage("duration = " + Ogre::StringConverter::toString(Ogre::Real(anim->mDuration)));
        Ogre::LogManager::getSingleton().logMessage("tick/sec = " + Ogre::StringConverter::toString(Ogre::Real(anim->mTicksPerSecond)));
        Ogre::LogManager::getSingleton().logMessage("channels = " + Ogre::StringConverter::toString(anim->mNumChannels));
    }
    Ogre::Animation* animation;
    mTicksPerSecond = (Ogre::Real)((0 == anim->mTicksPerSecond) ? 24 : anim->mTicksPerSecond);
    mTicksPerSecond *= mAnimationSpeedModifier;

    Ogre::Real cutTime = 0.0;
    if(mLoaderParams & LP_CUT_ANIMATION_WHERE_NO_FURTHER_CHANGE)
    {
        for (int i = 1; i < (int)anim->mNumChannels; i++)
        {
            aiNodeAnim* node_anim = anim->mChannels[i];

            // times of the equality check
            Ogre::Real timePos = 0.0;
            Ogre::Real timeRot = 0.0;

            for(unsigned int i = 1; i < node_anim->mNumPositionKeys; i++)
            {
                if( node_anim->mPositionKeys[i] != node_anim->mPositionKeys[i-1])
                {
                    timePos = (Ogre::Real)node_anim->mPositionKeys[i].mTime;
                    timePos /= mTicksPerSecond;
                }
            }

            for(unsigned int i = 1; i < node_anim->mNumRotationKeys; i++)
            {
                if( node_anim->mRotationKeys[i] != node_anim->mRotationKeys[i-1])
                {
                    timeRot = (Ogre::Real)node_anim->mRotationKeys[i].mTime;
                    timeRot /= mTicksPerSecond;
                }
            }

            if(timePos > cutTime){ cutTime = timePos; }
            if(timeRot > cutTime){ cutTime = timeRot; }
        }

        animation = mSkeleton->createAnimation(Ogre::String(animName), cutTime);
    }
    else
    {
        cutTime = Ogre::Math::POS_INFINITY;
        animation = mSkeleton->createAnimation(Ogre::String(animName), Ogre::Real(anim->mDuration/mTicksPerSecond));
    }

    animation->setInterpolationMode(Ogre::Animation::IM_LINEAR); //FIXME: Is this always true?

    if(!mQuietMode)
    {
        Ogre::LogManager::getSingleton().logMessage("Cut Time " + Ogre::StringConverter::toString(cutTime));
    }

    for (int i = 0; i < (int)anim->mNumChannels; i++)
    {
        Ogre::TransformKeyFrame* keyframe;

        aiNodeAnim* node_anim = anim->mChannels[i];
        if(!mQuietMode)
        {
            Ogre::LogManager::getSingleton().logMessage("Channel " + Ogre::StringConverter::toString(i));
            Ogre::LogManager::getSingleton().logMessage("affecting node: " + Ogre::String(node_anim->mNodeName.data));
            //Ogre::LogManager::getSingleton().logMessage("position keys: " + Ogre::StringConverter::toString(node_anim->mNumPositionKeys));
            //Ogre::LogManager::getSingleton().logMessage("rotation keys: " + Ogre::StringConverter::toString(node_anim->mNumRotationKeys));
            //Ogre::LogManager::getSingleton().logMessage("scaling keys: " + Ogre::StringConverter::toString(node_anim->mNumScalingKeys));
        }

        Ogre::String boneName = Ogre::String(node_anim->mNodeName.data);

        if(mSkeleton->hasBone(boneName))
        {
            Ogre::Bone* bone = mSkeleton->getBone(boneName);
            Affine3 defBonePoseInv;
            defBonePoseInv.makeInverseTransform(bone->getPosition(), bone->getScale(), bone->getOrientation());

            Ogre::NodeAnimationTrack* track = animation->createNodeTrack(i, bone);

            // Ogre needs translate rotate and scale for each keyframe in the track
            KeyframesMap keyframes;

            for(unsigned int i = 0; i < node_anim->mNumPositionKeys; i++)
            {
                keyframes[ (Ogre::Real)node_anim->mPositionKeys[i].mTime / mTicksPerSecond ] = KeyframeData( &(node_anim->mPositionKeys[i]), NULL, NULL);
            }

            for(unsigned int i = 0; i < node_anim->mNumRotationKeys; i++)
            {
                KeyframesMap::iterator it = keyframes.find((Ogre::Real)node_anim->mRotationKeys[i].mTime / mTicksPerSecond);
                if(it != keyframes.end())
                {
                    std::get<1>(it->second) = &(node_anim->mRotationKeys[i]);
                }
                else
                {
                    keyframes[ (Ogre::Real)node_anim->mRotationKeys[i].mTime / mTicksPerSecond ] = KeyframeData( NULL, &(node_anim->mRotationKeys[i]), NULL );
                }
            }

            for(unsigned int i = 0; i < node_anim->mNumScalingKeys; i++)
            {
                KeyframesMap::iterator it = keyframes.find((Ogre::Real)node_anim->mScalingKeys[i].mTime / mTicksPerSecond);
                if(it != keyframes.end())
                {
                    std::get<2>(it->second) = &(node_anim->mScalingKeys[i]);
                }
                else
                {
                    keyframes[ (Ogre::Real)node_anim->mRotationKeys[i].mTime / mTicksPerSecond ] = KeyframeData( NULL, NULL, &(node_anim->mScalingKeys[i]) );
                }
            }

            KeyframesMap::iterator it = keyframes.begin();
            KeyframesMap::iterator it_end = keyframes.end();
            for(it; it != it_end; ++it)
            {
                if(it->first < cutTime)	// or should it be <=
                {
                    aiVector3D aiTrans = getTranslate( node_anim, keyframes, it, mTicksPerSecond);

                    Ogre::Vector3 trans(aiTrans.x, aiTrans.y, aiTrans.z);

                    aiQuaternion aiRot = getRotate(node_anim, keyframes, it, mTicksPerSecond);
                    Ogre::Quaternion rot(aiRot.w, aiRot.x, aiRot.y, aiRot.z);

                    aiVector3D aiScale = getScale(node_anim, keyframes, it, mTicksPerSecond);
                    Ogre::Vector3 scale(aiScale.x, aiScale.y, aiScale.z);
                    
                    Ogre::Vector3 transCopy = trans;

                    Affine3 fullTransform;
                    fullTransform.makeTransform(trans, scale, rot);

                    Affine3 poseTokey = defBonePoseInv * fullTransform;
                    poseTokey.decomposition(trans, scale, rot);

                    keyframe = track->createNodeKeyFrame(Ogre::Real(it->first));

                    // weirdness with the root bone, But this seems to work
                    if(mSkeleton->getRootBones()[0]->getName() == boneName)
                    {
                        trans = transCopy - bone->getPosition();
                    }

                    keyframe->setTranslate(trans);
                    keyframe->setRotation(rot);
                    keyframe->setScale(scale);
                }
            }

        } // if bone exists

    } // loop through channels

    mSkeleton->optimiseAllAnimations();
}



void AssimpLoader::markAllChildNodesAsNeeded(const aiNode *pNode)
{
    flagNodeAsNeeded(pNode->mName.data);
    // Traverse all child nodes of the current node instance
    for ( unsigned int childIdx=0; childIdx<pNode->mNumChildren; ++childIdx )
    {
        const aiNode *pChildNode = pNode->mChildren[ childIdx ];
        markAllChildNodesAsNeeded(pChildNode);
    }
}

void AssimpLoader::grabNodeNamesFromNode(const aiScene* mScene, const aiNode* pNode)
{
    boneNode bNode;
    bNode.node = const_cast<aiNode*>(pNode);
    if(NULL != pNode->mParent)
    {
        bNode.parent = const_cast<aiNode*>(pNode->mParent);
    }
    bNode.isNeeded = false;
    boneMap.insert(std::pair<Ogre::String, boneNode>(Ogre::String(pNode->mName.data), bNode));
    mBoneNodesByName[pNode->mName.data] = pNode;
    if(!mQuietMode)
    {
        Ogre::LogManager::getSingleton().logMessage("Node " + Ogre::String(pNode->mName.data) + " found.");
    }

    // Traverse all child nodes of the current node instance
    for ( unsigned int childIdx=0; childIdx<pNode->mNumChildren; ++childIdx )
    {
        const aiNode *pChildNode = pNode->mChildren[ childIdx ];
        grabNodeNamesFromNode(mScene, pChildNode);
    }
}


void AssimpLoader::computeNodesDerivedTransform(const aiScene* mScene,  const aiNode *pNode, const aiMatrix4x4 accTransform)
{
    if(mNodeDerivedTransformByName.find(pNode->mName.data) == mNodeDerivedTransformByName.end())
    {
        mNodeDerivedTransformByName[pNode->mName.data] = accTransform;
    }
    for ( unsigned int childIdx=0; childIdx<pNode->mNumChildren; ++childIdx )
    {
        const aiNode *pChildNode = pNode->mChildren[ childIdx ];
        computeNodesDerivedTransform(mScene, pChildNode, accTransform * pChildNode->mTransformation);
    }
}

void AssimpLoader::createBonesFromNode(const aiScene* mScene,  const aiNode *pNode)
{
    if(isNodeNeeded(pNode->mName.data))
    {
        Ogre::Bone* bone = mSkeleton->createBone(Ogre::String(pNode->mName.data), msBoneCount);

        aiQuaternion rot;
        aiVector3D pos;
        aiVector3D scale;

        /*
        aiMatrix4x4 aiM = mNodeDerivedTransformByName.find(pNode->mName.data)->second;

        const aiNode* parentNode = NULL;
        {
            boneMapType::iterator it = boneMap.find(pNode->mName.data);
            if(it != boneMap.end())
            {
                parentNode = it->second.parent;
            }
        }
        if(parentNode)
        {
            aiMatrix4x4 aiMParent = mNodeDerivedTransformByName.find(parentNode->mName.data)->second;
            aiM = aiMParent.Inverse() * aiM;
        }
        */

        // above should be the same as
        aiMatrix4x4 aiM = pNode->mTransformation;

        aiM.Decompose(scale, rot, pos);


        /*
        // debug render
        Ogre::SceneNode* sceneNode = NULL;
        if(parentNode)
        {
            Ogre::SceneNode* parent = static_cast<Ogre::SceneNode*>(
                GOOF::NodeUtils::GetNodeMatch(getSceneManager()->getRootSceneNode(), parentNode->mName.data, false));
            assert(parent);
            sceneNode = parent->createChildSceneNode(pNode->mName.data);
        }
        else
        {
            sceneNode = getSceneManager()->getRootSceneNode()->createChildSceneNode(pNode->mName.data);
        }

        sceneNode->setScale(scale.x, scale.y, scale.z);
        sceneNode->setPosition(pos.x, pos.y, pos.z);
        sceneNode->setOrientation(rot.w, rot.x, rot.y, rot.z);

        sceneNode = sceneNode->createChildSceneNode();
        sceneNode->setScale(0.01, 0.01, 0.01);
        sceneNode->attachObject(getSceneManager()->createEntity("Box1m.mesh"));
        */

        if (!aiM.IsIdentity())
        {
            bone->setPosition(pos.x, pos.y, pos.z);
            bone->setOrientation(rot.w, rot.x, rot.y, rot.z);
        }

        if(!mQuietMode)
        {
            Ogre::LogManager::getSingleton().logMessage(Ogre::StringConverter::toString(msBoneCount) + ") Creating bone '" + Ogre::String(pNode->mName.data) + "'");
        }
        msBoneCount++;
    }
    // Traverse all child nodes of the current node instance
    for ( unsigned int childIdx=0; childIdx<pNode->mNumChildren; ++childIdx )
    {
        const aiNode *pChildNode = pNode->mChildren[ childIdx ];
        createBonesFromNode(mScene, pChildNode);
    }
}

void AssimpLoader::createBoneHiearchy(const aiScene* mScene,  const aiNode *pNode)
{
    if(isNodeNeeded(pNode->mName.data))
    {
        Ogre::Bone* parent = 0;
        Ogre::Bone* child = 0;
        if(pNode->mParent)
        {
            if(mSkeleton->hasBone(pNode->mParent->mName.data))
            {
                parent = mSkeleton->getBone(pNode->mParent->mName.data);
            }
        }
        if(mSkeleton->hasBone(pNode->mName.data))
        {
            child = mSkeleton->getBone(pNode->mName.data);
        }
        if(parent && child)
        {
            parent->addChild(child);
        }
    }
    // Traverse all child nodes of the current node instance
    for ( unsigned int childIdx=0; childIdx<pNode->mNumChildren; childIdx++ )
    {
        const aiNode *pChildNode = pNode->mChildren[ childIdx ];
        createBoneHiearchy(mScene, pChildNode);
    }
}

void AssimpLoader::flagNodeAsNeeded(const char* name)
{
    boneMapType::iterator iter = boneMap.find(Ogre::String(name));
    if( iter != boneMap.end())
    {
        iter->second.isNeeded = true;
    }
}

bool AssimpLoader::isNodeNeeded(const char* name)
{
    boneMapType::iterator iter = boneMap.find(Ogre::String(name));
    if( iter != boneMap.end())
    {
        return iter->second.isNeeded;
    }
    return false;
}

void AssimpLoader::grabBoneNamesFromNode(const aiScene* mScene,  const aiNode *pNode)
{
    static int meshNum = 0;
    meshNum++;
    if(pNode->mNumMeshes > 0)
    {
        for ( unsigned int idx=0; idx<pNode->mNumMeshes; ++idx )
        {
            aiMesh *pAIMesh = mScene->mMeshes[ pNode->mMeshes[ idx ] ];

            if(pAIMesh->HasBones())
            {
                for ( Ogre::uint32 i=0; i < pAIMesh->mNumBones; ++i )
                {
                    aiBone *pAIBone = pAIMesh->mBones[ i ];
                    if ( NULL != pAIBone )
                    {
                        mBonesByName[pAIBone->mName.data] = pAIBone;

                        if(!mQuietMode)
                        {
                            Ogre::LogManager::getSingleton().logMessage(Ogre::StringConverter::toString(i) + ") REAL BONE with name : " + Ogre::String(pAIBone->mName.data));
                        }

                        // flag this node and all parents of this node as needed, until we reach the node holding the mesh, or the parent.
                        aiNode* node = mScene->mRootNode->FindNode(pAIBone->mName.data);
                        while(node)
                        {
                            if(node->mName.data == pNode->mName.data)
                            {
                                flagNodeAsNeeded(node->mName.data);
                                break;
                            }
                            if(node->mName.data == pNode->mParent->mName.data)
                            {
                                flagNodeAsNeeded(node->mName.data);
                                break;
                            }

                            // Not a root node, flag this as needed and continue to the parent
                            flagNodeAsNeeded(node->mName.data);
                            node = node->mParent;
                        }

                        // Flag all children of this node as needed
                        node = mScene->mRootNode->FindNode(pAIBone->mName.data);
                        markAllChildNodesAsNeeded(node);

                    } // if we have a valid bone
                } // loop over bones
            } // if this mesh has bones
        } // loop over meshes
    } // if this node has meshes

    // Traverse all child nodes of the current node instance
    for ( unsigned int childIdx=0; childIdx<pNode->mNumChildren; childIdx++ )
    {
        const aiNode *pChildNode = pNode->mChildren[ childIdx ];
        grabBoneNamesFromNode(mScene, pChildNode);
    }
}

Ogre::String ReplaceSpaces(const Ogre::String& s)
{
    Ogre::String res(s);
    replace(res.begin(), res.end(), ' ', '_');

    return res;
}

Ogre::MaterialPtr AssimpLoader::createMaterialByScript(const Ogre::String & name, int index, const aiMaterial* mat)
{
    // Create a material in code as using script inheritance variable substitution and other goodies

    Ogre::MaterialManager* matMgr = Ogre::MaterialManager::getSingletonPtr();
    Ogre::String materialName = name + "#" + Ogre::StringConverter::toString(index);
    if(matMgr->resourceExists(materialName, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME))
    {
        Ogre::MaterialPtr matPtr = matMgr->getByName(materialName);
        if(matPtr->isLoaded())
        {
            return matPtr;
        }
    }

    Ogre::String code;

    aiColor4D c;
    if(aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT,  &c) == aiReturn_SUCCESS)
        code += "\t\t\tambient " + toString(c) + "\n";

    if(aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &c) == aiReturn_SUCCESS)
        code += "\t\t\tdiffuse " + toString(c) + "\n";

    if(aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &c) == aiReturn_SUCCESS)
        code += "\t\t\tspecular " + toString(c) + "\n";

    if(aiGetMaterialColor(mat, AI_MATKEY_COLOR_EMISSIVE, &c) == aiReturn_SUCCESS)
        code += "\t\t\temissive " + toString(c) + "\n";

    int shade = aiShadingMode_NoShading;
    if (AI_SUCCESS == mat->Get(AI_MATKEY_SHADING_MODEL, shade) && shade != aiShadingMode_NoShading) { 
        switch (shade) {
            case aiShadingMode_Phong: // Phong shading mode was added to opengl and directx years ago to be ready for gpus to support it (in fixed function pipeline), but no gpus ever did, so it has never done anything. From directx 10 onwards it was removed again.
            case aiShadingMode_Gouraud:
                code += "\t\t\tshading gouraud\n";
                break;
            case aiShadingMode_Flat:
                code += "\t\t\tshading flat\n";
                break;
            default:
                break;
        }
    }

    // Specifies the type of the texture to be retrieved ( e.g. diffuse, specular, height map ...)
    enum aiTextureType type = aiTextureType_DIFFUSE;

    // Index of the texture to be retrieved. The function fails if there is no texture of that type with this index.
    // GetTextureCount() can be used to determine the number of textures per texture type.

    // Receives the path to the texture. NULL is a valid value.
    aiString path;

    // The texture mapping. NULL is allowed as value.
    aiTextureMapping mapping = aiTextureMapping_UV;

    // Receives the UV index of the texture. NULL is a valid value.
    unsigned int uvindex = 0;

    // Receives the blend factor for the texture NULL is a valid value.
    float blend = 1.0f;

    // Receives the texture operation to be performed between this texture and the previous texture. NULL is allowed as value.
    aiTextureOp op = aiTextureOp_Multiply;

    // Receives the mapping modes to be used for the texture. The parameter may be NULL but if it is a valid pointer it
    // MUST point to an array of 3 aiTextureMapMode's (one for each axis: UVW order (=XYZ)).
    aiTextureMapMode mapmode[3] =  { aiTextureMapMode_Wrap, aiTextureMapMode_Wrap, aiTextureMapMode_Wrap };    // mapmode

    // For now assuming at most that only one diffuse texture exists
    if (mat->GetTexture(type, 0, &path, &mapping, &uvindex, &blend, &op, mapmode) == AI_SUCCESS)
    {
        Ogre::String texBasename, texExtention, texPath;
        Ogre::StringUtil::splitFullFilename(Ogre::String(path.data), texBasename, texExtention, texPath);

        Ogre::String texName = texBasename + "." + texExtention;

        int twoSided = 0;
        mat->Get(AI_MATKEY_TWOSIDED, twoSided);
        if(twoSided != 0)
        {
            code += "\t\t\t\tcull_hardware none\n";
        }

        //code += "\tset $diffuse_map " + texName + "\n";
        code += "\n\t\t\ttexture_unit\n\t\t\t{\n\t\t\t\ttexture " + texName + "\n";

        // no infomation on the alpha channel in the texture will have to load the texture and look at it
        code += "\t\t\t}\n";
    }

    code = "\ttechnique\n\t{\n\t\tpass\n\t\t{\n" + code + "\t\t}\n\t}\n";

    //code = "material " + materialName + " : base\n{\n" + code + "}\n\n";
    code = "material " + materialName + "\n{\n" + code + "}\n\n";
    mMaterialCode += code;

    // compile the material
    //code = "import * from base.material\n" + code;

/*    std::cout << "-------------------------------------------code" << std::endl;
    std::cout << code << std::endl;
    std::cout << "-------------------------------------------code" << std::endl;*/

    Ogre::DataStreamPtr stream(OGRE_NEW Ogre::MemoryDataStream(const_cast<void*>(static_cast<const void*>(code.c_str())),
                                                code.length() * sizeof(char), false));
    Ogre::MaterialManager::getSingleton().parseScript(stream, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    Ogre::MaterialPtr omat = Ogre::MaterialManager::getSingleton().getByName(materialName);
    //omat->compile(false);
    //omat->load();

    return omat;
}

Ogre::MaterialPtr AssimpLoader::createMaterial(int index, const aiMaterial* mat, const Ogre::String& mDir)
{
    static int dummyMatCount = 0;

    // extreme fallback texture -- 2x2 hot pink
    static Ogre::uint8 s_RGB[] = {128, 0, 255, 128, 0, 255, 128, 0, 255, 128, 0, 255};

    std::ostringstream matname;
    Ogre::MaterialManager* omatMgr =  Ogre::MaterialManager::getSingletonPtr();
    enum aiTextureType type = aiTextureType_DIFFUSE;
    static aiString path;
    aiTextureMapping mapping = aiTextureMapping_UV;       // the mapping (should be uv for now)
    unsigned int uvindex = 0;                             // the texture uv index channel
    float blend = 1.0f;                                   // blend
    aiTextureOp op = aiTextureOp_Multiply;                // op
    aiTextureMapMode mapmode[3] =  { aiTextureMapMode_Wrap, aiTextureMapMode_Wrap, aiTextureMapMode_Wrap };	// mapmode
    std::ostringstream texname;

    aiString szPath;
    if(AI_SUCCESS == aiGetMaterialString(mat, AI_MATKEY_TEXTURE_DIFFUSE(0), &szPath))
    {
        if(!mQuietMode)
        {
            Ogre::LogManager::getSingleton().logMessage("Using aiGetMaterialString : Found texture " + Ogre::String(szPath.data) + " for channel " + Ogre::StringConverter::toString(uvindex));
        }
    }
    if(szPath.length < 1)
    {
        if(!mQuietMode)
        {
            Ogre::LogManager::getSingleton().logMessage("Didn't find any texture units...");
        }
        szPath = Ogre::String("dummyMat" + Ogre::StringConverter::toString(dummyMatCount)).c_str();
        dummyMatCount++;
    }

    Ogre::String basename;
    Ogre::String outPath;
    Ogre::StringUtil::splitFilename(Ogre::String(szPath.data), basename, outPath);
    if(!mQuietMode)
    {
        Ogre::LogManager::getSingleton().logMessage("Creating " + basename);
    }

    Ogre::ResourceManager::ResourceCreateOrRetrieveResult status = omatMgr->createOrRetrieve(ReplaceSpaces(basename), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);
#if (OGRE_VERSION < ((1 << 16) | (9 << 8) | 0))
    Ogre::MaterialPtr omat = status.first;
#else
    Ogre::MaterialPtr omat = Ogre::static_pointer_cast<Ogre::Material>(status.first);
#endif
    if (!status.second)
        return omat;

    // ambient
    aiColor4D clr(1.0f, 1.0f, 1.0f, 1.0);
    //Ambient is usually way too low! FIX ME!
    if (mat->GetTexture(type, 0, &path) != AI_SUCCESS)
        aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT,  &clr);
    omat->setAmbient(clr.r, clr.g, clr.b);

    // diffuse
    clr = aiColor4D(1.0f, 1.0f, 1.0f, 1.0f);
    if(AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &clr))
    {
        omat->setDiffuse(clr.r, clr.g, clr.b, clr.a);
    }

    // specular
    clr = aiColor4D(1.0f, 1.0f, 1.0f, 1.0f);
    if(AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &clr))
    {
        omat->setSpecular(clr.r, clr.g, clr.b, clr.a);
    }

    // emissive
    clr = aiColor4D(1.0f, 1.0f, 1.0f, 1.0f);
    if(AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_EMISSIVE, &clr))
    {
        omat->setSelfIllumination(clr.r, clr.g, clr.b);
    }

    float fShininess;
    if(AI_SUCCESS == aiGetMaterialFloat(mat, AI_MATKEY_SHININESS, &fShininess))
    {
        omat->setShininess(Ogre::Real(fShininess));
    }

    int shade = aiShadingMode_NoShading;
    if (AI_SUCCESS == mat->Get(AI_MATKEY_SHADING_MODEL, shade) && shade != aiShadingMode_NoShading) { 
        switch (shade) {
        case aiShadingMode_Phong: // Phong shading mode was added to opengl and directx years ago to be ready for gpus to support it (in fixed function pipeline), but no gpus ever did, so it has never done anything. From directx 10 onwards it was removed again.
        case aiShadingMode_Gouraud:
            omat->setShadingMode(Ogre::SO_GOURAUD);
            break;
        case aiShadingMode_Flat:
            omat->setShadingMode(Ogre::SO_FLAT);
            break;
        default:
            break;
        }
    }

    if (mat->GetTexture(type, 0, &path) == AI_SUCCESS)
    {
        if(!mQuietMode)
        {
            Ogre::LogManager::getSingleton().logMessage("Found texture " + Ogre::String(path.data) + " for channel " + Ogre::StringConverter::toString(uvindex));
        }
        if(AI_SUCCESS == aiGetMaterialString(mat, AI_MATKEY_TEXTURE_DIFFUSE(0), &szPath))
        {
            if(!mQuietMode)
            {
                Ogre::LogManager::getSingleton().logMessage("Using aiGetMaterialString : Found texture " + Ogre::String(szPath.data) + " for channel " + Ogre::StringConverter::toString(uvindex));
            }
        }

        // attempt to load the image
        Ogre::Image image;

        // possibly if we fail to actually find it, pop up a box?
        Ogre::String pathname(mDir + "/" + path.data);

        std::ifstream imgstream;
        imgstream.open(path.data, std::ios::binary);
        if(!imgstream.is_open())
            imgstream.open(Ogre::String(mPath + "/" + path.data).c_str(), std::ios::binary);
        //TODO: save this to materials/textures ?

        Ogre::TextureUnitState* texUnitState = omat->getTechnique(0)->getPass(0)->createTextureUnitState(basename);

    }

    //omat->load(); // would need a rendersystem

    return omat;
}


bool AssimpLoader::createSubMesh(const Ogre::String& name, int index, const aiNode* pNode, const aiMesh *mesh, const aiMaterial* mat, Ogre::MeshPtr mMesh, Ogre::AxisAlignedBox& mAAB, const Ogre::String& mDir)
{
    // if animated all submeshes must have bone weights
    if(mBonesByName.size() && !mesh->HasBones())
    {
        if(!mQuietMode)
        {
            Ogre::LogManager::getSingleton().logMessage("Skipping Mesh " + Ogre::String(mesh->mName.data) + "with no bone weights");
        }
        return false;
    }

    Ogre::MaterialPtr matptr;

    if((mLoaderParams & LP_GENERATE_MATERIALS_AS_CODE) == 0)
    {
        matptr = createMaterial(mesh->mMaterialIndex, mat, mDir);
    }
    else
    {
        matptr = createMaterialByScript(name, mesh->mMaterialIndex, mat);
    }

    // now begin the object definition
    // We create a submesh per material
    Ogre::SubMesh* submesh = mMesh->createSubMesh(name + Ogre::StringConverter::toString(index));

    // prime pointers to vertex related data
    aiVector3D *vec = mesh->mVertices;
    aiVector3D *norm = mesh->mNormals;
    aiVector3D *uv = mesh->mTextureCoords[0];
    //aiColor4D *col = mesh->mColors[0];

    // We must create the vertex data, indicating how many vertices there will be
    submesh->useSharedVertices = false;
    submesh->vertexData = new Ogre::VertexData();
    submesh->vertexData->vertexStart = 0;
    submesh->vertexData->vertexCount = mesh->mNumVertices;

    // We must now declare what the vertex data contains
    Ogre::VertexDeclaration* declaration = submesh->vertexData->vertexDeclaration;
    static const unsigned short source = 0;
    size_t offset = 0;
    offset += declaration->addElement(source,offset,Ogre::VET_FLOAT3,Ogre::VES_POSITION).getSize();

    //mLog->logMessage((std::format(" %d vertices ") % m->mNumVertices).str());
    if(!mQuietMode)
    {
        Ogre::LogManager::getSingleton().logMessage(Ogre::StringConverter::toString(mesh->mNumVertices) + " vertices");
    }
    if (norm)
    {
        if(!mQuietMode)
        {
            Ogre::LogManager::getSingleton().logMessage(Ogre::StringConverter::toString(mesh->mNumVertices) + " normals");
        }
        //mLog->logMessage((std::format(" %d normals ") % m->mNumVertices).str() );
        offset += declaration->addElement(source,offset,Ogre::VET_FLOAT3,Ogre::VES_NORMAL).getSize();
    }

    if (uv)
    {
        if(!mQuietMode)
        {
            Ogre::LogManager::getSingleton().logMessage(Ogre::StringConverter::toString(mesh->mNumVertices) + " uvs");
        }
        //mLog->logMessage((std::format(" %d uvs ") % m->mNumVertices).str() );
        offset += declaration->addElement(source,offset,Ogre::VET_FLOAT2,Ogre::VES_TEXTURE_COORDINATES).getSize();
    }

    /*
    if (col)
    {
        Ogre::LogManager::getSingleton().logMessage(Ogre::StringConverter::toString(mesh->mNumVertices) + " colours");
        //mLog->logMessage((std::format(" %d colours ") % m->mNumVertices).str() );
        offset += declaration->addElement(source,offset,Ogre::VET_FLOAT3,Ogre::VES_DIFFUSE).getSize();
    }
    */


    // We create the hardware vertex buffer
    Ogre::HardwareVertexBufferSharedPtr vbuffer =
        Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(declaration->getVertexSize(source), // == offset
        submesh->vertexData->vertexCount,   // == nbVertices
        Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

    aiMatrix4x4 aiM = mNodeDerivedTransformByName.find(pNode->mName.data)->second;

    aiMatrix4x4 normalMatrix = aiM;
    normalMatrix.a4 = 0;
    normalMatrix.b4 = 0;
    normalMatrix.c4 = 0;
    normalMatrix.Transpose().Inverse();

    // Now we get access to the buffer to fill it.  During so we record the bounding box.
    float* vdata = static_cast<float*>(vbuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD));
    for (size_t i=0;i < mesh->mNumVertices; ++i)
    {
        // Position
        aiVector3D vect;
        vect.x = vec->x;
        vect.y = vec->y;
        vect.z = vec->z;

        vect *= aiM;

        /*
        if(NULL != mSkeletonRootNode)
        {
            vect *= mSkeletonRootNode->mTransformation;
        }
        */

        Ogre::Vector3 position( vect.x, vect.y, vect.z );
        *vdata++ = vect.x;
        *vdata++ = vect.y;
        *vdata++ = vect.z;
        mAAB.merge(position);
        vec++;

        // Normal
        if (norm)
        {
            vect.x = norm->x;
            vect.y = norm->y;
            vect.z = norm->z;

            vect *= normalMatrix;
            vect = vect.Normalize();

            *vdata++ = vect.x;
            *vdata++ = vect.y;
            *vdata++ = vect.z;
            norm++;

            //*vdata++ = norm->x;
            //*vdata++ = norm->y;
            //*vdata++ = norm->z;
            //norm++;
        }

        // uvs
        if (uv)
        {
            *vdata++ = uv->x;
            *vdata++ = uv->y;
            uv++;
        }

        /*
        if (col)
        {
            *vdata++ = col->r;
            *vdata++ = col->g;
            *vdata++ = col->b;
            //*vdata++ = col->a;
            //col++;
        }
        */
    }

    vbuffer->unlock();
    submesh->vertexData->vertexBufferBinding->setBinding(source,vbuffer);

    if(!mQuietMode)
    {
        Ogre::LogManager::getSingleton().logMessage(Ogre::StringConverter::toString(mesh->mNumFaces) + " faces");
    }
    aiFace *faces = mesh->mFaces;

    // Creates the index data
    submesh->indexData->indexStart = 0;
    submesh->indexData->indexCount = mesh->mNumFaces * 3;

    if (submesh->indexData->indexCount >= 65536) // 32 bit index buffer
    {
            submesh->indexData->indexBuffer = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
                    Ogre::HardwareIndexBuffer::IT_32BIT, submesh->indexData->indexCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

            Ogre::uint32* indexData = static_cast<Ogre::uint32*>(submesh->indexData->indexBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD));

            for (size_t i=0; i < mesh->mNumFaces;++i)
            {
                    // this is a quick hack to filter lines, which are currently not supported
                    if(faces->mNumIndices != 3) {
                            *indexData++ = 0;
                            *indexData++ = 0;
                            *indexData++ = 0;
                            faces++;
                            continue;
                    }
                    *indexData++ = faces->mIndices[0];
                    *indexData++ = faces->mIndices[1];
                    *indexData++ = faces->mIndices[2];

                    faces++;
            }
    }
    else // 16 bit index buffer
    {
            submesh->indexData->indexBuffer = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
            Ogre::HardwareIndexBuffer::IT_16BIT, submesh->indexData->indexCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

            Ogre::uint16* indexData = static_cast<Ogre::uint16*>(submesh->indexData->indexBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD));

            for (size_t i=0; i < mesh->mNumFaces;++i)
            {
                    // this is a quick hack to filter lines, which are currently not supported
                    if(faces->mNumIndices != 3) {
                            *indexData++ = 0;
                            *indexData++ = 0;
                            *indexData++ = 0;
                            faces++;
                            continue;
                    }
                    *indexData++ = faces->mIndices[0];
                    *indexData++ = faces->mIndices[1];
                    *indexData++ = faces->mIndices[2];

                    faces++;
            }
    }

    submesh->indexData->indexBuffer->unlock();

    // set bone weigths
    if(mesh->HasBones())
    {
        for ( Ogre::uint32 i=0; i < mesh->mNumBones; i++ )
        {
            aiBone *pAIBone = mesh->mBones[ i ];
            if ( NULL != pAIBone )
            {
                Ogre::String bname = pAIBone->mName.data;
                for ( Ogre::uint32 weightIdx = 0; weightIdx < pAIBone->mNumWeights; weightIdx++ )
                {
                    aiVertexWeight aiWeight = pAIBone->mWeights[ weightIdx ];

                    Ogre::VertexBoneAssignment vba;
                    vba.vertexIndex = aiWeight.mVertexId;
                    vba.boneIndex = mSkeleton->getBone(bname)->getHandle();
                    vba.weight= aiWeight.mWeight;

                    submesh->addBoneAssignment(vba);
                }
            }
        }
    } // if mesh has bones

    // Finally we set a material to the submesh
    if (matptr)
        submesh->setMaterialName(matptr->getName());

    return true;
}

void AssimpLoader::loadDataFromNode(const aiScene* mScene,  const aiNode *pNode, const Ogre::String& mDir)
{
    if(pNode->mNumMeshes > 0)
    {
        Ogre::MeshPtr mesh;
        Ogre::AxisAlignedBox mAAB;

        if(mLoaderParams & LP_GENERATE_SINGLE_MESH)
        {
            if(mMeshes.size() == 0)
            {
                mesh = Ogre::MeshManager::getSingleton().createManual("ROOTMesh", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

                mMeshes.push_back(mesh);
            }
            else
            {
                mesh = mMeshes[0];
                mAAB = mesh->getBounds();
            }
        }

        for ( unsigned int idx=0; idx<pNode->mNumMeshes; ++idx )
        {
            aiMesh *pAIMesh = mScene->mMeshes[ pNode->mMeshes[ idx ] ];
            if(!mQuietMode)
            {
                Ogre::LogManager::getSingleton().logMessage("SubMesh " + Ogre::StringConverter::toString(idx) + " for mesh '" + Ogre::String(pNode->mName.data) + "'");
            }

            // Create a material instance for the mesh.
            const aiMaterial *pAIMaterial = mScene->mMaterials[ pAIMesh->mMaterialIndex ];
            createSubMesh(pNode->mName.data, idx, pNode, pAIMesh, pAIMaterial, mesh, mAAB, mDir);
        }

        // We must indicate the bounding box
        mesh->_setBounds(mAAB);
        mesh->_setBoundingSphereRadius((mAAB.getMaximum()- mAAB.getMinimum()).length()/2);
    }

    // Traverse all child nodes of the current node instance
    for ( unsigned int childIdx=0; childIdx<pNode->mNumChildren; childIdx++ )
    {
        const aiNode *pChildNode = pNode->mChildren[ childIdx ];
        loadDataFromNode(mScene, pChildNode, mDir);
    }
}
