#pragma once

class QcObject;
class QiAnimator;
class QcSceneNode;
class QcBullet;
class QcFish;
class QcPlayer;
class QcThread;
class QcTexture;
class QcResImageDesc;
class QcResPlayerDesc;
class QcResDigitalDesc;
class QcRegion;
class QiSceneNodeMonitor;
class QcSceneAnimation;
class QcColor;
class QcSceneDigital;
template<class T> class QcSharedPtr;
template<class T> class QcList;


#include "FileStorageSystem\Crypt_RC4.h"
#include "RenderSystem\QcColor.hpp"
#include "QcObjType.hpp"
#include "Scene\QcSceneNodePointerList.hpp"

//SceneNode
typedef QcSharedPtr<QcSceneNode> QcSceneNodePtr;
typedef QcSceneNodePointerList<QcSceneNode> QcSceneNodePtrList;
typedef QcSceneNodePtrList::iterator QcSceneNodeListIter;
typedef QcSceneNodePtrList::const_iterator QcSceneNodeListConstIter;

typedef QcSharedPtr<QcSceneAnimation> QcSceneAnimationPtr;
typedef QcSceneNodePointerList<QcSceneAnimation> QcSceneAnimationPtrList;
typedef QcSceneAnimationPtrList::iterator QcSceneAnimationPtrListIter;
typedef QcSceneAnimationPtrList::const_iterator QcSceneAnimationPtrListConstIter;

typedef QcSharedPtr<QcSceneDigital> QcSceneDigitalPtr;

typedef QcSharedPtr<QcFish> QcFishPtr;
typedef QcSceneNodePointerList<QcFish> QcFishList;
typedef QcFishList::iterator QcFishListIter;
typedef QcFishList::const_iterator QcFishListConstIter;

typedef QcSharedPtr<QcBullet> QcBulletPtr;
typedef QcSceneNodePointerList<QcBullet> QcBulletPtrList;
typedef QcBulletPtrList::iterator QcBulletPtrListIter;
typedef QcBulletPtrList::const_iterator QcBulletPtrListConstIter;

typedef QcSharedPtr<QcPlayer> QcPlayerPtr;
typedef QcSceneNodePointerList<QcPlayer> QcPlayerPtrList;
typedef QcPlayerPtrList::iterator QcPlayerPtrListIter;

//desc
typedef QcSharedPtr<QcResImageDesc> QcResImageDescPtr;
typedef std::map<QcObjType, QcResImageDescPtr, QcNoCaseCompare> QcResImageDescMap;
//typedef stdext::hash_map<QcObjType, QcResImageDescPtr, stdext::hash_compare<QcObjType, QcNoCaseCompare>> QcResImageDescMap;
typedef QcResImageDescMap::iterator QcResImageDescMapIter;
typedef QcResImageDescMap::const_iterator QcResImageDescMapConstIter;

class QcResPathDesc;
typedef QcSharedPtr<QcResPathDesc> QcResPathDescPtr;

typedef QcSharedPtr<QcResDigitalDesc> QcResDigitalDescPtr;
typedef QcSharedPointerList<QcResDigitalDesc> QcResDigitalDescList;
typedef QcResDigitalDescList::iterator QcResDigitalDescListIter;
typedef QcResDigitalDescList::const_iterator QcResDigitalDescListConstIter;

//Region
typedef QcSharedPtr<QcRegion> QcRegionAutoPtr;
typedef QcSharedPointerList<QcRegion> QcRegionList;
typedef QcRegionList::iterator QcRegionListIter;
typedef QcRegionList::const_iterator QcRegionListConstIter;

//Animator
typedef QcSharedPtr<QiAnimator> QiAnimatorPtr;
typedef QcSharedPointerList<QiAnimator> QcAnimatorPtrList;
typedef QcAnimatorPtrList::iterator QcAnimatorPtrListIter;
typedef QcAnimatorPtrList::const_iterator QcAnimatorPtrListConstIter;

//Texture
typedef QcSharedPtr<QcTexture> QcTexturePtr;
typedef std::map<QcString, QcTexturePtr, QcNoCaseCompare> QcTexturePtrMap;
//typedef stdext::hash_map<QcString, QcTexturePtr, stdext::hash_compare<QcString, QcNoCaseCompare>> QcTexturePtrMap;
typedef QcTexturePtrMap::iterator QcTexturePtrMapIter;
typedef QcTexturePtrMap::const_iterator QcTexturePtrMapConstIter;

typedef QcList<QcTexture*> QcTexturePtrList;
typedef QcTexturePtrList::iterator QcTexturePtrListIter;
typedef QcTexturePtrList::const_iterator QcTexturePtrListConstIter;

//Thread
typedef QcSharedPtr<QcThread> QcThreadAutoPtr;
typedef std::set<QcThreadAutoPtr> QcThreadSet;
typedef QcThreadSet::iterator QcThreadSetIter;
typedef QcThreadSet::const_iterator QcThreadSetConstIter;