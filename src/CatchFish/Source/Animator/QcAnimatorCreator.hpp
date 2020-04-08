#pragma once

#include "Animator\QiAnimator.hpp"
#include "Animator\QcAnimatorMove.hpp"
class QiAnimator;
class QcAnimatorMove;
class QcResPathDesc;
class QcAnimatorMoveOffset;
class QiAnimatorDeque;
class QcAnimatorCreator : public QcObject
{
public:
	QcAnimatorMove* MakeCircleAnimator(const QcPoint& center, f32 radSpeed, bool bAnticlockwise  = true, bool bSelfRotate = true, f32 fRad = 0.f);
	QcAnimatorMove* MakeCycloneAnimator(const QcPoint& center, f32 radSpeed, f32 fRadiuSpeed, f32 fRad = 0.f, f32 fTime = 0.f, f32 fMinRadiu = 10.f, f32 fMaxRadiu = 0.f, bool bAnticlockwise  = true, bool bSelfRotate = true, bool bOutScreenDied = false );
	QcAnimatorMove* MakeLineAnimator2(const QcPoint& start, const QcPoint& end, f32 speed);
	QcAnimatorMove* MakeLineAnimator(const QcPoint& start, const QcPoint& end, f32 timeForWay);
	QcAnimatorMove* MakeBoundBoxAnimator(const QcRect& box, f32 fSpeed);

	QiAnimator* MakeRotationAnimator(f32 radSpeed, f32 fRad = 0.f);
	QiAnimator* MakeScaleAnimator(f32 fMin, f32 fEnd, f32 speed, bool bPingpong = false, bool bLoop = false);
	QiAnimator* MakeScaleAnimator2(f32 fMin, f32 fEnd, f32 timeForWay, bool bPingpong = false, bool bLoop = false);
	QiAnimator* MakeAlphaAnimator(f32 fMin, f32 fEnd, f32 speed, bool bPingpong = false, bool bLoop = false);
	QiAnimator* MakeAlphaAnimator2(f32 fBegin, f32 fEnd, f32 timeForWay, bool bPingpong = false, bool bLoop = false);
	QiAnimator* MakeTextureAnimator(const QcTexturePtrList& textures, f32 timePerFrame, bool loop = true);
	QiAnimator* MakeTextureAnimator(const QsStateParameter* pStateParameter);
	QiAnimator* MakeFlashAnimator(f32 fShowTime, f32 fHideTime);
	QiAnimator* MakeTrackAnimator(QcSceneNode* pTargetNode, f32 speed, bool bSelfRotate = true);
	QiAnimator* MakeStretchAnimator(const QcPoint& fixPos, QeMatrixMask StretchType = eMaskXYZ);
	QiAnimator* MakeDitherAnimator(f32 fShakeTime, f32 fShakeScope, const QcPoint& orgPt);
	QiAnimatorDeque* MakeAnimatorDeque();
};