#include "Pch.hpp"
#include "QcSceneCircleRegularMaker.hpp"
#include "QcResSceneRegularDesc.hpp"
#include "QcRegularMgr.hpp"
#include "QcResFishRegularDesc.hpp"
#include "Res\QcResImageDesc.hpp"
#include "Res\QcResFishDesc.hpp"
#include "Res\QcResPathDesc.hpp"
#include "Animator\QcAnimatorTexture.hpp"
#include "Scene\Fish\QcFish.hpp"
#include "Animator\QcAmimatorCyclone.hpp"
#include "Animator\QiAnimatorDeque.hpp"
#include "Math\QcVector2d.hpp"
#include "Scene\Fish\QcSameKindBombFish.hpp"


ImplRuntimeClass(QcSceneCircleRegularMaker);

void QcSceneCircleRegularMaker::Animate( double dCurrentTime,f32 fElapsedTime, QcList<QcFish*>& fishList )
{
		if (m_fLivingTime > .0f && m_nFishCount > 20)
		{
			m_fLivingTime -= fElapsedTime;
		}
		else
		{
			m_bFinished = true;
		}
}

void QcSceneCircleRegularMaker::BeginSceneRegular( QcRegularMgr* pRegularMgr, QcList<QcFish*>& fishList, QcResSceneRegularDesc* pSceneRegularDesc )
{
	m_fLivingTime = pSceneRegularDesc->GetLivingTime();
	if (pSceneRegularDesc->GetObjType() == QmObjName(SceneRegular10))
	{
		static const QcObjType gsFishPool[] = 
		{
			QmObjName(fish01),
			QmObjName(fish02),
			QmObjName(fish03),
			QmObjName(fish04),
			QmObjName(fish05),
			QmObjName(fish06),
			QmObjName(fish07),
			QmObjName(fish08),
			QmObjName(fish09),
			QmObjName(fish10),
			QmObjName(fish11),
			QmObjName(fish12),
			QmObjName(fish13),
			QmObjName(fish14),
		}; 
		f32 fRadiuWidth = QcVector(QmGlobalData.gScreenRect.LeftBottom(), QmGlobalData.gScreenRect.RightTop()).getLength();
		f32 fSpeed = 100.f;
		int iFishStartNum = 42;
		f32 fStartTime = 0.f;
		for (int i = 0; i <QmCountOf(gsFishPool); i++)
		{
			iFishStartNum -= 2;
			if (i >10)
				fStartTime += 5.0f;
			else
				fStartTime += 3.0f;

			f32 fGapRad = TWO_PI/iFishStartNum;
			f32 fCs = cos(fGapRad);
			f32 fSn = sin(fGapRad);
			f32 fFishRad = 0.f;
			QcVector vec(1.0f, 0.f);

			const QcResFishDesc *pFishDesc = static_cast<const QcResFishDesc*>(QmResMgr->GetImageDesc(gsFishPool[i]));
			f32 fFishRadiu = fRadiuWidth + pFishDesc->GetWidth() * 0.5f + 20.f;

			int iSameKindFishNum =  QcSameKindBombFish::CanGenSameKindFish(pFishDesc) ? RandomInt(0,iFishStartNum-1) : -1;
			for (int j = 0; j < iFishStartNum; j++)
			{
				vec.rotateBy(fCs, fSn);
				fFishRad += fGapRad;
				QcFish *pFish ;
				if (j == iSameKindFishNum)
				{
					pFish = QcSameKindBombFish::MakeSameKindBombFish(pFishDesc);
				}
				else
					pFish = pFishDesc->MakeInstance()->As<QcFish*>();
				pFish->FindAnimator<QcAnimatorTexture>()->SetLoop(true);
				pFish->SetRotation(fFishRad);
				pFish->SetStartTime(fStartTime);

				QiAnimator* pMoveAni = QmAnimatorCreator->MakeLineAnimator2(QcPoint(0.f, 0.f), vec * fFishRadiu, fSpeed);
				pFish->AddAnimator(pMoveAni);
				fishList.PushBack(pFish);
			}
		}
		m_fLivingTime = fStartTime + (fRadiuWidth * 0.5f)/fSpeed;
	}
	else if (pSceneRegularDesc->GetObjType() == QmObjName(SceneRegular17))
	{
		static const QcObjType gsFishPool[] = 
		{
			QmObjName(fish01),
			QmObjName(fish02),
			QmObjName(fish03),
			QmObjName(fish04),
			QmObjName(fish05),
			QmObjName(fish06),
		}; 
		f32 fGapRad = TWO_PI/QmCountOf(gsFishPool);
		f32 fStartRad = 0.f;
		for (int j=0; j<QmCountOf(gsFishPool); ++j)
		{
			fStartRad += fGapRad;
			const QcResFishDesc *pFishDesc = (const QcResFishDesc*)QmResMgr->GetImageDesc(gsFishPool[j]);
			f32 fMaxRadiu = (QmGlobalData.gScreenRect.getWidth() + pFishDesc->GetWidth());

			int iSameKindFishNum = QcSameKindBombFish::CanGenSameKindFish(pFishDesc) ? RandomInt(0,69) : -1;
			for (int i = 0; i< 70; i++)
			{
				QcFish *pFish ;
				if (i == iSameKindFishNum)
				{
					pFish = QcSameKindBombFish::MakeSameKindBombFish(pFishDesc);
				}
				else
					pFish = pFishDesc->MakeInstance()->As<QcFish*>();
				QcAnimatorCyclone* pCyclone = static_cast<QcAnimatorCyclone*>(QmAnimatorCreator->MakeCycloneAnimator(QcPoint(0.f, 0.f), HALF_PI * 0.08f, 70.f, 0.f, fMaxRadiu/20.f, 10.f, fMaxRadiu,true,true,true));
				QcAnimatorMove* pMoveAnimator = QmAnimatorCreator->MakeCircleAnimator(pFish->GetPosition(), HALF_PI * 0.3f);
				pFish->AddAnimator(pCyclone);
				pFish->AddAnimator(pMoveAnimator);

				QcPoint startPos(3.f, 0.f);
				f32 fRad = fStartRad + (i * 0.5f) *  (HALF_PI * 0.3f);
				QcMath::surroundRad(fRad);
				startPos.rotateByRad(fRad);
				pFish->SetPosition(startPos);
				pFish->SetRotation(fRad);
				pFish->SetStartTime(i * 0.6f);
				pFish->SetFinishFlag(pCyclone);

				fishList.PushBack(pFish);
			}
		}
		m_fLivingTime = 50.0f;
	}
	else if (pSceneRegularDesc->GetObjType() == QmObjName(SceneRegular18))
	{
		const QcResImageDesc* pMainFishDesc = QmResMgr->GetImageDesc(QmObjName(fish14));
		struct QsSubFishInfor
		{
			QcObjType fishType;
			int nCount;
			f32 fRadiu;
		};
		static QsSubFishInfor garSubFish[] = 
		{
			{QmObjName(fish01), 10, 110.f},//15, 110.f
			{QmObjName(fish02), 18, 180.f},//21, 160.f
			{QmObjName(fish01), 29, 250.f},//30, 210.f
		};
		
		f32 fGapWidth = QmGlobalData.gScreenRect.getWidth() * 0.29f;
		f32 fOutCircleRadiu = 250.f + 70.f;
		for (int  m = 0; m < 3; ++m)
		{
			f32 fDelayTime = m * 8.f;
			QcPoint fFishStartPos(-fGapWidth, QmGlobalData.gScreenRect.top() + fOutCircleRadiu);
			f32 fFishRotation = -HALF_PI;
			f32 fMoveSpeed = 150.f;
			for (int n = 0; n< 3; n++)
			{
				QcFish *pMainFish = pMainFishDesc->MakeInstance()->As<QcFish*>();
				pMainFish->SetPosition(fFishStartPos);
				pMainFish->SetRotation(fFishRotation);
				pMainFish->SetStartTime(fDelayTime);

				QcPoint vFirstStateEndPos(fFishStartPos.X, 0.f);
				QcPoint vSecondStateEndPos(fFishStartPos.X, -fFishStartPos.Y);

				QcAnimatorMove* pMainMoveAnimator1 = QmAnimatorCreator->MakeLineAnimator2(fFishStartPos, vFirstStateEndPos,  fMoveSpeed);
				QcAnimatorMove* pMainMoveAnimator2 = QmAnimatorCreator->MakeLineAnimator2(vFirstStateEndPos, vSecondStateEndPos,  fMoveSpeed);
				QiAnimatorDeque* pMainAnimatorQeque = QmAnimatorCreator->MakeAnimatorDeque();
				pMainAnimatorQeque->AddAnimator(pMainMoveAnimator1);
				pMainAnimatorQeque->AddAnimator(pMainMoveAnimator2);
				pMainFish->AddAnimator(pMainAnimatorQeque);
				pMainFish->SetFinishFlag(pMainAnimatorQeque);
				fishList.PushBack(pMainFish);

				for (int i=0; i<QmCountOf(garSubFish); ++i)
				{
					const QcResFishDesc* pSubFishDesc = (const QcResFishDesc*)QmResMgr->GetImageDesc(garSubFish[i].fishType);
					f32 fGapRad = TWO_PI/garSubFish[i].nCount;
					f32 fCs = cos(fGapRad);
					f32 fSn = sin(fGapRad);

					QcVector vec(1.0f, 0.f);
					f32 fDestRad = 0.f;
					QcPoint vPos;
					f32 fRotationTime = 0.5f;
					int iSameFishIndex = QcSameKindBombFish::CanGenSameKindFish(pSubFishDesc) ? RandomInt(0, garSubFish[i].nCount) : -1;
					for (int iFishCount = 0; iFishCount < garSubFish[i].nCount; ++iFishCount)
					{
						vPos = fFishStartPos + vec * garSubFish[i].fRadiu;
						QcFish *pFish ;
						if (iFishCount == iSameFishIndex)
						{
							pFish = QcSameKindBombFish::MakeSameKindBombFish(pSubFishDesc);
						}
						else
							pFish = pSubFishDesc->MakeInstance()->As<QcFish*>();

						pFish->SetPosition(vPos);
						pFish->SetRotation(fFishRotation);
						pFish->SetStartTime(fDelayTime);
						fishList.PushBack(pFish);

						f32 fRotationRad = fDestRad - fFishRotation;
						QcMath::surroundRad(fRotationRad);
 						if (fRotationRad > PI)
 							fRotationRad = fRotationRad - TWO_PI;
						QiAnimator* pRotationAnimator = QmAnimatorCreator->MakeRotationAnimator(fRotationRad/fRotationTime, abs(fRotationRad));

 						vPos = vFirstStateEndPos + vec * garSubFish[i].fRadiu;
 						QcPoint endPos;
 						QmAssert2(QcResPathDesc::MakeScreenIntersection(vPos, vec, endPos));
						endPos += vec * (pSubFishDesc->GetWidth() + 20.f);
 						QcAnimatorMove* pSubMoveAnimator2 = QmAnimatorCreator->MakeLineAnimator2(vPos, endPos,  fMoveSpeed);

						QiAnimatorDeque* pSubAnimatorQeque = QmAnimatorCreator->MakeAnimatorDeque();
						pSubAnimatorQeque->AddAnimator(pMainMoveAnimator1);
						pSubAnimatorQeque->AddAnimator(pRotationAnimator);
						pSubAnimatorQeque->AddAnimator(pSubMoveAnimator2);
						pFish->AddAnimator(pSubAnimatorQeque);
						pFish->SetFinishFlag(pSubAnimatorQeque);

						vec.rotateBy(fCs, fSn);
						fDestRad += fGapRad;
					}
				}
				fFishRotation = -fFishRotation;
				fFishStartPos.X += fGapWidth;
				fFishStartPos.Y = -fFishStartPos.Y;
			}
		}
		m_fLivingTime = 68.0f;
	}
	m_nFishCount = fishList.size();
}



