#pragma once

class QcParaModifier
{
	friend class QcDebugHelper;
public:
	class QiItem
	{
	public:
		virtual void Increace() = 0;
		virtual void Decreace() = 0;
		virtual void Print(QcString& output) const = 0;
		QcString m_psName;
	};
	template<class T>
	class QsItem : public QiItem
	{
		QmDisableCopyAssignment(QsItem);
		friend class QcParaModifier;
	public:
		QsItem(T& value, T min, T max, T dGap, const char* psName) : m_value(value)
			, m_min(min)
			, m_dMax(max)
			, m_dGap(dGap)
		{
			m_psName = psName;
		}
	private:
		void Increace()
		{
			if (QcMath::equals(m_value, m_dMax))
				m_value = m_min;
			else
				m_value = QcMath::min_(m_value + m_dGap, m_dMax);
		}
		void Decreace()
		{
			if (QcMath::equals(m_value, m_min))
				m_value = m_dMax;
			else
				m_value = QcMath::max_(m_value - m_dGap, m_min);
		}
		void Print(QcString& output) const
		{
			char sBuf[256];
			if (typeid(T) == typeid(f32))
				sprintf_s(sBuf, "%s:%f\n", m_psName.c_str(), m_value);
			else if (typeid(T) == typeid(f64))
				sprintf_s(sBuf, "%s:%lf\n", m_psName.c_str(), m_value);
			else
				sprintf_s(sBuf, "%s:%d\n", m_psName.c_str(), m_value);
			output += sBuf;
		}
		T m_min;
		T m_dMax;
		T m_dGap;
		T& m_value;
	};
	QcParaModifier()
	{
		m_iCurrentItem = 0;
	}
	~QcParaModifier()
	{
		for(u32 i=0; i<m_arAdjustItem.size(); ++i)
		{
			delete m_arAdjustItem[i];
		}
	}
	template<class T>
	void AddModifyItem(T& value, T min, T max, T gap, const char* psName)
	{
		m_arAdjustItem.push_back(new QsItem<T>(value, min, max, gap, psName));
	}
protected:
#define QmCheckEmpty() if (m_arAdjustItem.size() == 0) return
	void Increace(){ QmCheckEmpty();m_arAdjustItem[m_iCurrentItem]->Increace();}
	void Decreace(){ QmCheckEmpty();m_arAdjustItem[m_iCurrentItem]->Decreace();}
	void Prev(){ QmCheckEmpty();m_iCurrentItem = QcMath::surround(m_iCurrentItem - 1, m_arAdjustItem.size());}
	void Next(){ QmCheckEmpty();m_iCurrentItem = QcMath::surround(m_iCurrentItem + 1, m_arAdjustItem.size());}
	void Print(QcString& text)
	{
		QmCheckEmpty();
		for(u32 i=0; i<m_arAdjustItem.size(); ++i)
		{
			if (m_iCurrentItem == s32(i))
				text += "-----> ";
			m_arAdjustItem[i]->Print(text);
		}
	}
private:
	std::vector<QiItem*> m_arAdjustItem;
	s32 m_iCurrentItem;
};
