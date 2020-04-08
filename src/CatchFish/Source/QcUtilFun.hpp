#pragma once

inline QeMachineType MachineType(u32 nPlayerNumber)
{
	switch(nPlayerNumber)
	{
	case 10:
		return eMachine_10_1;
		break;
	case 8:
		return eMachine_8_1;
		break;
	case 6:
		return eMachine_6_1;
		break;
	case 5:
		return eMachine_4_1;
		break;
	case 4:
		return eMachine_4_2;
		break;
	case 3:
		return eMachine_3_2;
		break;
	}
	return eMachine_10_1;
}

inline QeAlign QfStringToAlign(const char* psName)
{
	if (QfStrNoCaseCmp(psName, "LeftTop") == 0)
		return eAlignLeftTop;
	else if (QfStrNoCaseCmp(psName, "LeftCenter") == 0)
		return eAlignLeftCenter;
	else if (QfStrNoCaseCmp(psName, "LeftButtom") == 0)
		return eAlignLeftButtom;
	else if (QfStrNoCaseCmp(psName, "CenterTop") == 0)
		return eAlignCenterTop;
	else if (QfStrNoCaseCmp(psName, "Center") == 0)
		return eAlignCenter;
	else if (QfStrNoCaseCmp(psName, "CenterBottom") == 0)
		return eAlignCenterBottom;
	else if (QfStrNoCaseCmp(psName, "RightTop") ==0)
		return eAlignRightTop;
	else if (QfStrNoCaseCmp(psName, "RightCenter") == 0)
		return eAlignRightCenter;
	else if (QfStrNoCaseCmp(psName, "RightBottom") == 0)
		return eAlignRightBottom;
	return eAlignCenter;
}

inline QcPoint& AdjustAlginPos(QeAlign algin, const QcPoint& pos, f32 fWidth, f32 fHeight, QcPoint& out)
{
	fWidth *= 0.5f;
	fHeight *= 0.5f;

	switch(algin)
	{
	case eAlignLeftTop:
		{
			out.X = pos.X + fWidth;
			out.Y = pos.Y - fHeight;
			break;
		}
	case eAlignLeftCenter:
		{
			out.X = pos.X + fWidth;
			out.Y = pos.Y;
			break;
		}
	case eAlignLeftButtom:
		{
			out.X = pos.X + fWidth;
			out.Y = pos.Y + fHeight;
			break;
		}
	case eAlignCenterTop:
		{
			out.X = pos.X;
			out.Y = pos.Y - fHeight;
			break;
		}
	case eAlignCenter:
		{
			out = pos;
			break;
		}
	case eAlignCenterBottom:
		{
			out.X = pos.X;
			out.Y = pos.Y + fHeight;
			break;
		}
	case eAlignRightTop:
		{
			out.X = pos.X - fWidth;
			out.Y = pos.Y - fHeight;
			break;
		}
	case eAlignRightCenter:
		{
			out.X = pos.X - fWidth;
			out.Y = pos.Y;
			break;
		}
	case eAlignRightBottom:
		{
			out.X = pos.X - fWidth;
			out.Y = pos.Y + fHeight;
			break;
		}
	}
	return out;
}

inline QcPoint& ReverseAlginPos(QeAlign algin, const QcPoint& pos, f32 fWidth, f32 fHeight, QcPoint& out)
{
	QcPoint offsetPt = AdjustAlginPos(algin, pos, fWidth, fHeight, out) - pos;
	out += offsetPt * (-2.0f);
	return out;
}

inline QcRect QfMakeScreenRect(f32 fX, f32 fY, f32 fWidth, f32 fHeight)
{
	QcRect rect;
	rect._BottomLeftPt.X = fX;
	rect._BottomLeftPt.Y = fY + fHeight;
	rect._TopRightPt.X = fX + fWidth;
	rect._TopRightPt.Y = fY;
	return rect;
}