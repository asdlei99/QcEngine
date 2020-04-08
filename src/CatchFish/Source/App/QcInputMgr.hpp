#pragma once

struct QsKeyInfo
{
	f32 fKeyDownTime;
	u32 nKeyDownCount;
	f32 fGapTime;
	mutable f32 fTimeCount;
	mutable bool bOnlyOneDown;
	mutable bool bIngoreUp;
};

bool IsKeyDown(QeKey key, int ID);

class QcInputMgr : public QcObject
{
public:
	QcInputMgr(QcScene* pScene);

	void Poll();
protected:
	void Poll(__int32 key[eKeyTotal], f32 fElapsedTime);
protected:
	QcScene* m_pScene;
};