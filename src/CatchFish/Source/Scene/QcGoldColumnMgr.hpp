#pragma once

class QcGoldColumn;
class QcGoldColumnMgr : public QcObject
{
	typedef QcSharedPtr<QcGoldColumn> QcGoldColumnPtr;
	typedef std::deque<QcSharedPtr<QcGoldColumn>> QcGoldColumnPtrDeque;
public:
	QcGoldColumnMgr();

	void Init(QcPlayer* pPlayer, const QcObjType& goldColumn);
	virtual void Animate(double dCurrentTime,f32 fElapsedTime);
	virtual void Render(QcDeviceRender& render, const QcMatrix* pWorldMatrix = NULL);

	void UpdateWinScore(u32 iScore, u32 gunLevel);
	void MakeColumn(u32 iScore);
protected:
	QcPlayer* GetPlayer() {return m_pPlayer;}
	void MakeMoveAnimator(f32 fTime, f32 fLen);
	bool IsWinMode() const {return m_bWinMode;}
protected:
	bool m_bWinMode;
	QcPoint m_vOffset;
	QcPoint m_vFirstColumnPos;
	QcPlayer* m_pPlayer;
	QcResImageDesc* m_pColumnDesc;
	QcGoldColumnPtrDeque m_goldColumnDeque;
	QcSharedPtr<QiAnimator> m_pAlphaAnimator;
	QcSharedPtr<QcAnimatorMove> m_pMoveAnimator;
	int m_iLastScore;
};