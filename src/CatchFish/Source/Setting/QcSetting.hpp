#pragma once

enum
{
	eDlgInit,
	eDlgCancel,
	eDlgOK,
};

typedef boost::function<void()> QcFunCb0;

class QcResDialogDesc;
class QcResDialogItemDesc;
typedef QcSharedPtr<QcResDialogItemDesc> QcResDialogItemDescPtr;
typedef QcSharedPointerList<QcResDialogItemDesc> QcResDialogItemDescList;
typedef QcResDialogItemDescList::iterator QcResDialogItemDescListIter;
typedef QcResDialogItemDescList::const_iterator QcResDialogItemDescConstIter;

class QcDialog;
typedef QcSharedPtr<QcDialog> QcDialogPtr;
typedef QcSharedPointerList<QcDialog> QcDialogPtrList;
typedef QcDialogPtrList::iterator QcDialogPtrListIter;
typedef QcDialogPtrList::const_iterator QcDialogPtrListConstIter;

class QcWidget;
typedef QcSharedPtr<QcWidget> QcWidgetPtr;
typedef QcSharedPointerList<QcWidget> QcWidgetPtrList;
typedef QcWidgetPtrList::iterator QcWidgetPtrListIter;
typedef QcWidgetPtrList::const_iterator QcWidgetPtrListConstIter;

typedef boost::function<void(int iFlag)> QcFunDialogCb;
typedef boost::function<void()> QcFunBtnCb;
typedef boost::function<void(s32&, bool bUpdate)> QcFunEnumCb;
typedef QcFunEnumCb QcFunDigitalCb;
typedef boost::function<void(BYTE*,s32& nBit,bool bUpdate)> QcFunInputCb;
typedef boost::function<s32(BYTE*,s32 nBit)> QcFunInputCheckCb;

#define QmDlgCb(classFun, pObj) QcWidgetCallBacker(QcFunDialogCb(boost::bind(classFun, pObj,_1)))
#define QmBtnCb(classFun, pObj) QcWidgetCallBacker(QcFunBtnCb(boost::bind(classFun, pObj)))
#define QmEnumCb(classFun, pObj) QcWidgetCallBacker(QcFunEnumCb(boost::bind(classFun, pObj, _1, _2)))
#define QmDigitalCb(classFun, pObj) QcWidgetCallBacker(QcFunDigitalCb(boost::bind(classFun, pObj, _1, _2)))
#define QmDigitalInputCb(classFun, pObj) QcWidgetCallBacker(QcFunInputCb(boost::bind(classFun, pObj, _1, _2, _3)))
#define QmInputCheckCb(classFun, pObj) QcWidgetCallBacker(QcFunInputCheckCb(boost::bind(classFun, pObj, _1, _2)))

#define QmRegisterDlgCb(name, classFun, pObj) Register(name, QmDlgCb(classFun, pObj))
#define QmRegisterBtnCb(name, classFun, pObj) Register(name, QmBtnCb(classFun, pObj))
#define QmRegisterEnumCb(name, classFun, pObj) Register(name, QmEnumCb(classFun, pObj))
#define QmRegisterDigitalCb(name, classFun, pObj) Register(name, QmDigitalCb(classFun, pObj))
#define QmRegisterDigitalInputCb(name, classFun, pObj) Register(name, QmDigitalInputCb(classFun, pObj))
#define QmRegisterInputCheckCb(name, classFun, pObj) Register(name, QmInputCheckCb(classFun, pObj))

#define QmRegisterDlgCbEx(name, classFun, pObj) QmSceneMgr->QcSettingMgr()->QmRegisterDlgCb(name, classFun, pObj)
#define QmRegisterBtnCbEx(name, classFun, pObj) QmSceneMgr->QcSettingMgr()->QmRegisterBtnCb(name, classFun, pObj)
#define QmRegisterEnumCbEx(name, classFun, pObj) QmSceneMgr->QcSettingMgr()->QmRegisterEnumCb(name, classFun, pObj)
#define QmRegisterDigitalCbEx(name, classFun, pObj) QmSceneMgr->QcSettingMgr()->QmRegisterDigitalCb(name, classFun, pObj)
#define QmRegisterDigitalInputCbEx(name, classFun, pObj) QmSceneMgr->QcSettingMgr()->QmRegisterDigitalInputCb(name, classFun, pObj)
#define QmRegisterInputCheckCbEx(name, classFun, pObj) QmSceneMgr->QcSettingMgr()->QmRegisterInputCheckCb(name, classFun, pObj)

//Dialog
QmDefineObjType(SettingMain);
QmDefineObjType(ParameterSetting);
QmDefineObjType(DecodeSetting);
QmDefineObjType(CheckAccount);
QmDefineObjType(Account);
QmDefineObjType(AccountMenu);
QmDefineObjType(SystemSetting);
QmDefineObjType(AccountWarning);
QmDefineObjType(CheckCodeDlg);
QmDefineObjType(DecodeInputDlg);
QmDefineObjType(Reboot);
QmDefineObjType(PasswordDlg);
QmDefineObjType(SysLineNumEditor);
QmDefineObjType(SysMatchineIDEditor);
QmDefineObjType(SysExDifficultCheck);
QmDefineObjType(SysExDifficult);

#include "DaMa.h"
#include "Setting\QcWidgetCallBacker.hpp"
#include "Setting\QcResDialogItemDesc.hpp"
#include "Setting\QcResDialogDesc.hpp"
#include "Setting\QcWidget.hpp"
#include "Setting\QcDialog.hpp"
#include "Setting\QcSettingMgr.hpp"



