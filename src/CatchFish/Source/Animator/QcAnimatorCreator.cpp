#include "pch.hpp"
#include "QcAnimatorCreator.hpp"
#include "QcAnimatorMoveCircle.hpp"
#include "QcAnimatorMoveLine.hpp"
#include "QcAnimatorMovePath.hpp"
#include "QcAnimatorMoveBoundBox.hpp"
#include "QcAnimatorMoveTrack.hpp"
#include "QcAnimatorRotation.hpp"
#include "QcAnimatorScale.hpp"
#include "QcAnimatorAlpha.hpp"
#include "QcAnimatorTexture.hpp"
#include "QiAnimatorDeque.hpp"
#include "QcAnimatorMoveOffset.hpp"
#include "QcAnimatorStretch.hpp"
#include "QcAnimatorFlash.hpp"
#include "QcAnimatorDither.hpp"
#include "QcAmimatorCyclone.hpp"
#include "QcAnimatorMoveParapola.hpp"

ImplRuntimeClass2(QcGravitySpeed, 500);
ImplRuntimeClass2(QiAnimatorDeque, 100);

QcAnimatorMove* QcAnimatorCreator::MakeCycloneAnimator(const QcPoint& center, f32 radSpeed, f32 fRadiuSpeed, f32 fRad, f32 fTime, f32 fMinRadiu, f32 fMaxRadiu, bool bAnticlockwise, bool bSelfRotate, bool bOutScreenDied)
{
	QcAnimatorCyclone* p = new QcAnimatorCyclone();
	p->Reset(center, radSpeed, fRadiuSpeed, fRad, fTime, fMinRadiu, fMaxRadiu, bAnticlockwise, bSelfRotate, bOutScreenDied);
	return p;
}
QcAnimatorMove* QcAnimatorCreator::MakeCircleAnimator(const QcPoint& center, f32 radSpeed, bool bAnticlockwise, bool bSelfRotate, f32 fRad)
{
	QcAnimatorMoveCircle* p = new QcAnimatorMoveCircle();
	p->Reset(center, radSpeed, bAnticlockwise, bSelfRotate, fRad);
	return p;
}
QcAnimatorMove* QcAnimatorCreator::MakeLineAnimator2(const QcPoint& start, const QcPoint& end, f32 speed)
{
	QcAnimatorMoveLine* pMoveLine = new QcAnimatorMoveLine();
	pMoveLine->Reset2(start, end, speed);
	return pMoveLine;
}
QcAnimatorMove* QcAnimatorCreator::MakeLineAnimator(const QcPoint& start, const QcPoint& end, f32 timeForWay)
{
	QcAnimatorMoveLine* pMoveLine = new QcAnimatorMoveLine();
	pMoveLine->Reset(start, end, timeForWay);
	return pMoveLine;
}

QcAnimatorMove* QcAnimatorCreator::MakeBoundBoxAnimator(const QcRect& box, f32 fSpeed)
{
	QcAnimatorMoveBoundBox* pBox =  new QcAnimatorMoveBoundBox();
	pBox->Reset(box, fSpeed);
	return pBox;
}

QiAnimator* QcAnimatorCreator::MakeRotationAnimator(f32 radSpeed, f32 fRad)
{
	return new QcAnimatorRotation(radSpeed, fRad);
}
QiAnimator* QcAnimatorCreator::MakeScaleAnimator(f32 fMin, f32 fEnd, f32 speed, bool bPingpong, bool bLoop)
{
	return new QcAnimatorScale(fMin, fEnd, speed, bPingpong, bLoop);
}

QiAnimator* QcAnimatorCreator::MakeScaleAnimator2(f32 fMin, f32 fEnd, f32 timeForWay, bool bPingpong, bool bLoop)
{
	f32 fSpeed = (fEnd - fMin)/timeForWay;
	return new QcAnimatorScale(fMin, fEnd, fSpeed, bPingpong, bLoop);
}

QiAnimator* QcAnimatorCreator::MakeAlphaAnimator(f32 fMin, f32 fEnd, f32 speed, bool bPingpong, bool bLoop)
{
	return new QcAnimatorAlpha(fMin, fEnd, speed, bPingpong, bLoop);
}

QiAnimator* QcAnimatorCreator::MakeAlphaAnimator2(f32 fMin, f32 fEnd, f32 timeForWay, bool bPingpong, bool bLoop)
{
	f32 fSpeed = (fEnd - fMin)/timeForWay;
	return new QcAnimatorAlpha(fMin, fEnd, fSpeed, bPingpong, bLoop);
}

QiAnimator* QcAnimatorCreator::MakeTextureAnimator(const QcTexturePtrList& textures, f32 timePerFrame, bool loop)
{
	QcAnimatorTexture* pTextureAnimator = new QcAnimatorTexture();
	pTextureAnimator->Reset(textures, timePerFrame, loop);
	return pTextureAnimator;
}

QiAnimator* QcAnimatorCreator::MakeTextureAnimator(const QsStateParameter* pStateParameter)
{
	QcAnimatorTexture* pTextureAnimator = new QcAnimatorTexture();
	pTextureAnimator->Reset(*pStateParameter);
	return pTextureAnimator;
}

QiAnimator* QcAnimatorCreator::MakeFlashAnimator(f32 fShowTime, f32 fHideTime)
{
	QcAnimatorFlash* pFlashAnmator = new QcAnimatorFlash();
	pFlashAnmator->Reset(fShowTime, fHideTime);
	return pFlashAnmator;
}

QiAnimator* QcAnimatorCreator::MakeTrackAnimator(QcSceneNode* pTargetNode, f32 speed, bool bSelfRotate)
{
	QcAnimatorMoveTrack* pTrack = new QcAnimatorMoveTrack();
	pTrack->Reset(pTargetNode, speed, bSelfRotate);
	return pTrack;
}

QiAnimator* QcAnimatorCreator::MakeStretchAnimator(const QcPoint& fixPos, QeMatrixMask StretchType)
{
	QcAnimatorStretch* pStretch = new QcAnimatorStretch(fixPos, StretchType);
	return pStretch;
}

QiAnimator* QcAnimatorCreator::MakeDitherAnimator(f32 fShakeTime, f32 fShakeScope, const QcPoint& orgPt)
{
	QcAnimatorDither* pShake = new QcAnimatorDither();
	pShake->Reset(fShakeTime, fShakeScope, orgPt);
	return pShake;
}	

QiAnimatorDeque* QcAnimatorCreator::MakeAnimatorDeque()
{
	return new QiAnimatorDeque();
}

