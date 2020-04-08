#pragma once
#include <vector>


typedef vector<QeKey> FnKeyContent;
typedef vector<FnKeyContent> FnKeyContentList;

typedef struct 
{
	int	iCount;
	bool	 bIsEnable;	
	bool bIsFinish;
}FnKeyState;

typedef vector<FnKeyState> FnKeyStateList;

class QcFnKeyHelper : public QcObjectEx
{
public:

	enum FnKey{
		eFnKeyNone = 0,
		LeftRightLeft,
		LeftRightUpDown,
		All,
	};

	typedef vector<FnKey> FnKeyList;

	QcFnKeyHelper();
	void Init(FnKeyList EnableKeyList);
	virtual void OnTimer(u32 nTimer, void* pContext);
	FnKey OnKeyDown(QeKey key, const QsKeyInfo& info);
	FnKey OnKeyUp(QeKey key, const QsKeyInfo& info);
	FnKey SearchFnKeyLIst(QeKey key, const QsKeyInfo& info);
private:
	FnKeyList m_EnableKeyList;
	int				m_iUnfinishedtFnKeyCount;   //每次输入正确还没完成的组合键
	FnKeyStateList m_vFnKeyStateList;
	QeKey		m_LastKeyDown;
	bool				m_bIsKeyDown;
	bool				m_bIsAllFnKeyReBegin;
	FnKeyContentList			m_vFnKeyContentList;
};