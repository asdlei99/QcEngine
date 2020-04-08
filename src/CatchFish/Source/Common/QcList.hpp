#pragma once

#include <vector>
#include "Common\QcReferenceCounted.hpp"

template<class T>
class QcList : public std::vector<T>
{
public:
	struct Tracker
	{
		friend class QcList<T>;
	private:
		QmDisableCopyAssignment(Tracker);
	public:
		Tracker(QcList<T>& l) : m_list(l), m_arRemoveIdx(NULL), m_sBuf(NULL)
		{
			if (m_list.m_pTracker == NULL)
				m_list.m_pTracker = this;
		}
		~Tracker()
		{
			if (m_list.m_pTracker == this)
			{
				m_list.m_pTracker = NULL;
				if (m_arRemoveIdx)
				{
					std::vector<int>& removeList = *m_arRemoveIdx;
					size_t removeSize = removeList.size();
					if (m_list.size() == removeSize)
					{
						m_list.clear();
					}
					else if (removeSize == 1)
					{
						m_list.EraseAt(removeList[0]);
					}
					else
					{
						//从后面删起
						std::sort(removeList.begin(), removeList.end(), std::greater<int>());
						//int nSize = m_arRemoveIdx.size();
						size_t iIndex = 0;
						size_t i=1;
						for (i=1; i<removeSize; ++i)
						{
							if (removeList[i] != removeList[iIndex] - int(i - iIndex))
							{
								iterator iter = begin() + removeList[i-1];
								m_list.erase(iter, iter + (i - iIndex) );
								iIndex = i;
							}
						}
						if (iIndex != i)
						{	
							iterator iter = begin() + removeList[i-1];
							m_list.erase(iter, iter + (i - iIndex) );
						}
					}
					delete m_arRemoveIdx;
				}
				if (m_sBuf)
				{
					m_list.insert(m_list.end(), m_sBuf->begin(), m_sBuf->end());
					delete m_sBuf;
				}
			}
		}
		vector<T>::iterator begin()
		{
			return m_list.begin();
		}
		vector<T>::const_iterator begin() const
		{
			return m_list.begin();
		}
		vector<T>::iterator end()
		{
			return m_list.end();
		}
		vector<T>::const_iterator end() const
		{
			return m_list.end();
		}
	private:
		void push_back(const T& source)
		{
			if (m_sBuf == NULL)
				m_sBuf = new std::vector<T>();
			m_sBuf->push_back(source);
		}
		void eraseAt(int iPos)
		{
			if (m_arRemoveIdx == NULL)
				m_arRemoveIdx = new std::vector<int>();
			m_arRemoveIdx->push_back(iPos);
		}
		void clear()
		{
			QmSafeDelete(m_sBuf);
			if (m_arRemoveIdx == NULL)
				m_arRemoveIdx = new std::vector<int>();
			else
				m_arRemoveIdx->clear();

			int nSize = m_list.size();
			m_arRemoveIdx->reserve(nSize);
			for (int i=nSize-1; i>=0; --i)
			{
				m_arRemoveIdx->push_back(i);
			}
		}
	private:
		QcList<T>& m_list;
		std::vector<int>* m_arRemoveIdx;
		std::vector<T>* m_sBuf;
	};
	QcList():m_pTracker(NULL){}

	template <class P>
	int Find(const P& source) const
	{
		int nSize = size();
		for (int i=0; i<nSize; ++i)
		{
			if ((*this)[i] == source)
				return i;
		}
		return -1;
	}
	template <class P>
	int PushBack(const P& source)
	{
		if (m_pTracker)
		{
			m_pTracker->push_back(T(source));
			return -1;
		}
		else
		{
			push_back(T(source));
		}
		return size() - 1;
	}
	template <class P>
	void Erase(const P& source)
	{
		int iPos = Find(source);
        if (iPos < 0)
            return;

		if (m_pTracker)
		{
			m_pTracker->eraseAt(iPos);
		}
		else
		{
			erase(begin() + iPos);
		}
	}
	void EraseAt(int index)
	{
        if (index < 0)
            return;

		if (m_pTracker)
		{
			m_pTracker->eraseAt(index);
		}
		else
		{
			erase(begin() + index);
		}
	}
	void Clear()
	{
		if (size())
		{
			if (m_pTracker)
			{
				m_pTracker->clear();
			}
			else
			{
				clear();
			}
		}
	}
protected:
	Tracker* m_pTracker;
};

template<class T>
class QcPointerListBase : public QcList<T>
{
public:
	QcPointerListBase() : m_nEmptyCount(0) {}

	void RemoveEmptyItem()
	{
		if (m_pTracker == NULL && m_nEmptyCount > 5 && (m_nEmptyCount/(f32)size()) > 0.5f)
		{
			iterator iter(std::remove(begin(), end(), (const T)(NULL)));
			if (iter != end())
				erase(iter, end());
			m_nEmptyCount = 0;
		}
	}

	template <class P>
	void Erase(const P& source)
	{
		if (source)
		{
			int iPos = Find(source);
			EraseAt(iPos);
		}
	}
	void EraseAt(int index)
	{
		if (index >= 0 && index < (int)size() && (*this)[index] != NULL)
		{
			(*this)[index] = NULL;
			++m_nEmptyCount;
		}
	}
	template <class P>
	int PushBack(const P& source)
	{
		RemoveEmptyItem();
		return QcList::PushBack(source);
	}
	template <class P>
	int Insert(const P& source)
	{
		if (m_nEmptyCount > 0)
		{
			int iPos = Find((const T)(NULL));
			if (iPos != -1)
			{
				(*this)[iPos] = source;
				--m_nEmptyCount;
				return iPos;
			}
			else
			{
				QmAssert(false);
			}
		}
		return QcList::PushBack(source);
	}
	void Clear()
	{
		if (m_pTracker == NULL)
		{
			clear();
			m_nEmptyCount = 0;
		}
		else
		{
			iterator iter = begin();
			iterator endIter = end();
			for (; iter != endIter; ++iter)
			{
				*iter = NULL;
			}
			m_nEmptyCount = size();
		}
	}
protected:
	u32 m_nEmptyCount;
};

template<class T>
class QcPointerList : public QcPointerListBase<T*>
{
};


template<class T>
class QcSharedPointerList : public QcPointerListBase<QcSharedPtr<T>>
{
};