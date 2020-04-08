#pragma once


/*不能存储复杂类型: 既数据之前不能存在指针引用关系*/
template <class T, size_t NBufSize = 1, typename TAlloc = std::allocator<T> >
class QcArray
{
public:
	typedef T value_type;
	typedef T* iterator;
	typedef const T* const_iterator;
	iterator begin() { return pointer();}
	const_iterator begin() const { return pointer();}
	iterator end(){ return pointer() + size();}
	const_iterator end() const{ return pointer() + size();}

	QcArray()
		: allocated(NBufSize)
		, m_pData((T*)m_bufData)
		, used(0)
	{}
	QcArray(const T* pData, int nCount)
		: allocated(NBufSize)
		, m_pData((T*)m_bufData)
		, used(0)
	{
		reset(pData, nCount);
	}
	QcArray(u32 start_count, bool bMakeInstance = false)
		: allocated(NBufSize)
		, m_pData((T*)m_bufData)
		, used(0)
	{
		if (bMakeInstance)
			resize(start_count);
		else
			reallocate(start_count);
	}
	~QcArray()
	{
		clear(true);
	}
	//copy
	template<size_t OtherBufferSize>
	QcArray(const QcArray<T, OtherBufferSize, TAlloc>& other)
		: allocated(NBufSize)
		, m_pData((T*)m_bufData)
		, used(0)
	{
		*this = other;
	}
	//operator=
	template<size_t OtherBufferSize>
	const QcArray<T, NBufSize, TAlloc>& operator=(const QcArray<T, OtherBufferSize, TAlloc>& other)
	{
		if (this == &other)
			return *this;
		reset(other.pointer(), other.size());
		return *this;
	}
	template<size_t OtherBufferSize>
	bool operator == (const QcArray<T, OtherBufferSize, TAlloc>& other) const
	{
		if (used != other.used)
			return false;

		for (u32 i=0; i<other.used; ++i)
			if (m_pData[i] != other[i])
				return false;
		return true;
	}
	template<size_t OtherBufferSize>
	bool operator != (const QcArray<T, OtherBufferSize, TAlloc>& other) const
	{
		return !(*this==other);
	}

	void reallocate(u32 new_size, bool canShrink=false)
	{
		if (allocated == new_size)
			return;
		if (!canShrink && (new_size < allocated))
			return;

		if (new_size > NBufSize || allocated > NBufSize)
		{
			T* newData = NULL;
			T* oldData = pointer();
			if (new_size > NBufSize)
				newData = allocator.allocate(new_size);
			else
				newData = (T*)m_bufData;

			//copy
			s32 end = min(used, new_size);
			move(newData, oldData, end);

			// destruct old data
			destory(oldData + end, int(used - end));

			if (allocated > NBufSize)
				allocator.deallocate(oldData, allocated);

			m_pData = newData;
		}
		else
		{
			destory(m_pData + new_size, int(used - new_size));
			used = min(new_size, used);
		}
		allocated = max(NBufSize, new_size);
		used = min(new_size, used);
	}
	void resize(u32 new_size)
	{
		reallocate(new_size, false);
		if (!IsPod())
		{
			for (u32 i=used; i<new_size; ++i)
			{
				allocator.construct(&m_pData[i], T());
			}
		}
		used = new_size;
	}
	void clear(bool bFreeMem = false)
	{
		destory(m_pData, used);
		if (bFreeMem && allocated > NBufSize)
		{
			allocator.deallocate(m_pData, allocated);
			m_pData = (T*)m_bufData;
			allocated = NBufSize;
		}
		used = 0;
	}
	void reset(const T* pData, int nData)
	{
		clear(false);
		reallocate(nData, false);
		insert(0, pData, nData);
		used = nData;
	}

	inline void push_back(const T& element)
	{
		insert(used, element);
	}
	inline void push_front(const T& element)
	{
		insert(0, element);
	}
	inline void insert(const_iterator where, const T& element)
	{
		insert(IterToIdx(where), element);
	}
	inline void insert(int index, const T& element)
	{
		insert(index, &element, 1);
	}
	inline void insert(int index, const T* pSource, int nCount)
	{
		QmAssert(index<= int(used)); // access violation
		if (used + nCount > (int)allocated)
		{
			u32 newAlloc = max(allocated + (allocated >> 1), used + nCount);
			T* pNewData = allocator.allocate(newAlloc);
			move(pNewData, m_pData, index);
			move(pNewData + index + nCount, m_pData + index, used - index);

			if (allocated > NBufSize)
				allocator.deallocate(m_pData, allocated);

			m_pData = pNewData;
			allocated = newAlloc;
		}
		else
		{
			move(m_pData + index + nCount, m_pData + index, used - index);
		}
		if (IsPod() && nCount > 2)
		{
			move(m_pData + index, pSource, nCount);
		}
		else
		{
			for (int i=0; i<nCount; ++i)
			{
				allocator.construct(&m_pData[index + i], pSource[i]);
			}
		}
		used += nCount;
	}
	inline void pop_back()
	{
		if (used > 0)
			erase(used-1);
	}
	inline iterator erase(const_iterator where)
	{
		int index = IterToIdx(where);
		erase(index);
		return begin() + index;
	}
	inline iterator erase(const_iterator where, const_iterator wEnd)
	{
		QmAssert(where >= begin() && where < end());
		QmAssert(wEnd >= begin() && wEnd <= end());
		int idx = IterToIdx(where);
		u32 nElements = std::distance(where, wEnd);
		erase(idx, nElements);
		return begin() + idx;
	}
	inline void erase(u32 index)
	{
		QmAssert(index<used);
		destory(m_pData + index, 1);
		move(m_pData + index, m_pData + index + 1, used-index-1);
		--used;
	}
	inline void erase(u32 index, s32 count)
	{
		if (index>=used || count<1)
			return;
		if (index+count>used)
			count = used-index;

		destory(m_pData + index, count);
		move(m_pData + index, m_pData + index + count, used-index-count);
		used-= count;
	}

	T& operator [](u32 index)
	{
		QmAssert(index<used);
		return m_pData[index];
	}
	const T& operator [](u32 index) const
	{
		QmAssert(index<used);
		return m_pData[index];
	}
	T& getLast()
	{
		QmAssert(used);
		return m_pData[used-1];
	}
	const T& getLast() const
	{
		QmAssert(used);
		return m_pData[used-1];
	}
	T* pointer()
	{
		return m_pData;
	}
	const T* pointer() const
	{
		return m_pData;
	}
	u32 size() const { return used;}
	bool empty() const {return used == 0;}

	s32 find(const T& element) const
	{
		for (u32 i=0; i<used; ++i)
			if (element == m_pData[i])
				return (s32)i;

		return -1;
	}
	s32 reverse_find(const T& element) const
	{
		for (s32 i=used-1; i>=0; --i)
			if (m_pData[i] == element)
				return i;
		return -1;
	}
protected:
	bool IsPod()
	{
		return true;
	}
	int IterToIdx(const_iterator where)
	{
		QmAssert(where >= begin() && where <= end());
		return where - begin();
	}
	inline void move(T* pDest, const T* pSource, int nCount)
	{
		if (nCount > 0)
		{
			memmove(pDest, pSource, sizeof(T) * nCount);
		}
	}
#ifdef QmBoostOn
	inline void destory(T* pWhere, int nCount)
	{
		destory(pWhere, nCount, boost::has_trivial_destructor<T>());
	}
	inline void destory(T* pWhere, int nCount, const boost::false_type&)
	{
		for (int i=0; i<nCount; ++i)
		{
			//allocator.destroy(&pWhere[i]);
			(*pWhere).~T();
			++pWhere;
		}
	}
	inline void destory(T* pWhere, int nCount, const boost::true_type&){}
#else
	inline void destory(T* pWhere, int nCount)
	{
		//if (!has_trivial_destructor<T>::value)
		//if (_HAS_TRIVIAL_CONSTRUCTOR(T))
		{
			for (int i=0; i<nCount; ++i)
			{
				//allocator.destroy(&pWhere[i]);
				(*pWhere).~T();
				++pWhere;
			}
		}
	}
#endif
private:
	char m_bufData[sizeof(T) * NBufSize];
	T* m_pData;
	u32 allocated;
	u32 used;
	TAlloc allocator;
};