#include "pch.hpp"
#include "Scene/QcFnKeyHelper.hpp"
#include "QcPlayer.hpp"
#include "Animator\QcAnimatorFlash.hpp"
#include "FileStorageSystem\QcPlayerInfo.hpp"


QcFnKeyHelper::QcFnKeyHelper() : 
m_EnableKeyList(eFnKeyNone)
,m_iUnfinishedtFnKeyCount(0)
,m_bIsKeyDown(false)
,m_bIsAllFnKeyReBegin(false)
{
	QeKey LeftRightLeftArray[3] = {ePlayerKeyLeft,ePlayerKeyRight,ePlayerKeyLeft};
	FnKeyContent LeftRightLeftList(LeftRightLeftArray, LeftRightLeftArray+sizeof(LeftRightLeftArray)/sizeof(QeKey));
	m_vFnKeyContentList.push_back(LeftRightLeftList);

	QeKey LeftRightUpDownArray[4] = {ePlayerKeyLeft,ePlayerKeyRight,ePlayerKeyUp, ePlayerKeyDown};
	FnKeyContent LeftRightUpDowntList(LeftRightUpDownArray, LeftRightUpDownArray+sizeof(LeftRightUpDownArray)/sizeof(QeKey));
	m_vFnKeyContentList.push_back(LeftRightUpDowntList);

}

void QcFnKeyHelper::Init( FnKeyList EnableKeyList )
{
	m_EnableKeyList = EnableKeyList;
	FnKeyState temp = {0,false, false};
	m_vFnKeyStateList.assign(All, temp);
	FnKeyList::iterator iter = m_EnableKeyList.begin();
	for (;iter != m_EnableKeyList.end(); ++iter)
	{
		int i = *iter;
		FnKeyStateList::reference ref = m_vFnKeyStateList.at(i);
		ref.bIsEnable = true;
	}
}

void QcFnKeyHelper::OnTimer( u32 nTimer, void* pContext )
{
	for (int i = 1; i < (int)m_vFnKeyStateList.size(); ++i)
	{
		if (nTimer == u32(i)) //��ϼ���ʱû���;
		{
			KillTimer(i);
			FnKeyStateList::reference ref = m_vFnKeyStateList.at(i);
			ref.bIsFinish = true;
		}
	}
}

QcFnKeyHelper::FnKey QcFnKeyHelper::OnKeyDown( QeKey key, const QsKeyInfo& info )
{
	FnKey result = eFnKeyNone;
	if (!m_bIsKeyDown)
	{
		m_LastKeyDown = key;
		m_bIsKeyDown = true;
		result = SearchFnKeyLIst(key, info);

		if (m_iUnfinishedtFnKeyCount == 0)
		{
			m_bIsAllFnKeyReBegin = true;         //������ϼ�����
		}

		if(m_bIsAllFnKeyReBegin) //����������ϼ� �ٴ�������ǰ��������ϼ�
		{
			m_bIsAllFnKeyReBegin = false;
			for (int i = 1; i <(int)m_vFnKeyStateList.size(); ++i)
			{
				FnKeyStateList::reference ref = m_vFnKeyStateList.at(i);
				ref.bIsFinish = false;
				ref.iCount = 0;
			}
		}
	}
	return result;
}

QcFnKeyHelper::FnKey QcFnKeyHelper::OnKeyUp( QeKey key, const QsKeyInfo& info )
{
	if(m_LastKeyDown == key)
		m_bIsKeyDown = false;
	return eFnKeyNone;
}

QcFnKeyHelper::FnKey QcFnKeyHelper::SearchFnKeyLIst( QeKey key, const QsKeyInfo& info )
{
	FnKey result = eFnKeyNone;
	m_iUnfinishedtFnKeyCount = 0;
	for (int i = 1; i < (int)m_vFnKeyStateList.size(); ++i)
	{
		FnKeyStateList::reference ref = m_vFnKeyStateList.at(i);
		FnKeyContent temp = m_vFnKeyContentList.at(i-1);						//�����±��0��ʼ
		if (ref.bIsEnable)  //��ϼ��Ƿ���;
		{
			if(ref.iCount < ((int)temp.size()))
			{
				if ((key == temp.at(ref.iCount)) && !(ref.bIsFinish))		//������ϼ�����;
				{
					if(ref.iCount == 0)
						SetTimer(i, 1.0f, true);
					++ref.iCount;
					QmOutputString("iCount%d\n",ref.iCount);
					if(ref.iCount == (int)temp.size())		//��ϼ����
					{
						KillTimer(i);
						result =  FnKey(i);
						ref.bIsFinish = true;
					}
					if(ref.iCount < (int)temp.size())  //���з��ϵ�ǰ�������в��һ�û��ɵ���ϼ�
						++m_iUnfinishedtFnKeyCount;
				}
				else	if ((key != temp.at(ref.iCount)) && !(ref.bIsFinish))																	//��ǰ��ϼ�����;
				{
					if(ref.iCount != 0)
						KillTimer(i);
					ref.bIsFinish = true;
				}
			}
		}
	}
	return result;
}

//m_iUnfinishedtFnKeyCount = 0;
//for (int i = 1; i < m_vFnKeyStateList.size(); ++i)
//{
//	FnKeyStateList::reference ref = m_vFnKeyStateList.at(i);
//	int count = ref.iCount;
//	FnKeyContent temp = m_vFnKeyContentList.at(i-1);						//�����±��0��ʼ
//	if (ref.bIsEnable)  //��ϼ��Ƿ���;
//	{
//		if(ref.iCount < ((int)temp.size()))
//		{
//			if ((key == temp.at(ref.iCount)) && !(ref.bIsFinish))		//������ϼ�����;
//			{
//				if(ref.iCount == 0)
//					SetTimer(i, 1.0f, true);
//				++ref.iCount;
//				QmOutputString("iCount%d\n",ref.iCount);
//				if(ref.iCount == temp.size())		//��ϼ����
//				{
//					KillTimer(i);
//					result =  FnKey(i);
//					ref.bIsFinish = true;
//				}
//				if(ref.iCount < temp.size())  //���з��ϵ�ǰ�������в��һ�û��ɵ���ϼ�
//					++m_iUnfinishedtFnKeyCount;
//			}
//			else														//��ǰ��ϼ�����;
//			{
//				if(ref.iCount != 0)
//					KillTimer(i);
//				ref.bIsFinish = true;
//			}
//		}
//	}
//}