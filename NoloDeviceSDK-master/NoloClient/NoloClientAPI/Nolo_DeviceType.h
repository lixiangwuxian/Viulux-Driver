#pragma once
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <cwchar>
#include "Nolo_Math.h"

namespace NOLOVR
{

enum ENoloDeviceType
{
	eHmd = 0,
	eLeftController,
	eRightController,
	eBaseStation
};

enum EControlerButtonType {
	ePadBtn     = 0x01,
	eTriggerBtn = 0x02,
	eMenuBtn    = 0x04,
	eSystemBtn  = 0x08,
	eGripBtn    = 0x10,
	ePadTouch   = 0x20
};

enum ERotationType {
	eNoloRotation = 0x01,
	eHmdRotation
};
#pragma pack(push,1)

struct Controller
{   // 4+12+16+4+4+8+4+4 = 56
	int VersionID;
	NVector3 Position;
	NQuaternion Rotation;
	UINT Buttons;
	int Touched;
	NVector2 TouchAxis;
	int Battery;
	int State;
};

struct HMD
{ // 4+12+12+4+16+4 = 52
	int HMDVersionID;
	NVector3 HMDPosition;
	NVector3 HMDInitPostion;//
	UINT HMDTwoPointDriftAngle;//������
	NQuaternion HMDRotation;
	int HMDState;
};

struct BaseStation
{// 8
	int BaseStationVersionID;
	int BaseStationPower;
};

struct NoloSensorData
{//12*6 = 72
	NVector3 vecLVelocity;
	NVector3 vecLAngularVelocity;
	NVector3 vecRVelocity;
	NVector3 vecRAngularVelocity;
	NVector3 vecHVelocity;
	NVector3 vecHAngularVelocity;
};

struct NOLOData
{
	Controller leftData;  
	Controller rightData;
	HMD hmdData;
	BaseStation bsData;
	byte expandData[64];
	NoloSensorData NoloSensorData;
	UCHAR leftPackNumber;
	UCHAR rightPackNumber;
	NVector3 FixedEyePosition;
};

#pragma pack(pop)

//CStyle FunPotinters
typedef void(__cdecl *pfnKeyEvent)(ENoloDeviceType DevType, UCHAR Keys);
typedef void(__cdecl *pfnKeyUpDownEvent)(ENoloDeviceType DevType, EControlerButtonType type);
typedef void(__cdecl *pfnVoidCallBack)();
typedef void(__cdecl *pfnDataCallBack)(const NOLOData &noloData);
typedef void(__cdecl *pfnVoidIntCallBack)(int Versions);

enum EClientCallBackTypes
{
	eOnZMQConnected = 0,  
	eOnZMQDisConnected,       
	eOnButtonDoubleClicked, 
	eOnKeyPressEvent,
	eOnKeyReleaseEvent,
	eOnNewData,        // pfnDataCallBack
	eOnNoloDevVersion, // pfnVoidIntCallBack
	eCallBackCount
};


//��Ҫ���������������κλص��������Լ������ʱ�Ĺ���
class INOLOZMQEvent
{
public:
	//�Ѿ������˽�������
	virtual void OnZMQConnected() = 0;
	//�Ѿ������˶Ͽ�����
	virtual void OnZMQDisConnected() = 0;
	//DevType���ڵ�ĳ��������˫���� 
	//ͨ���ж� ��Keys & EControlerButtonType::XX�����жϸð�ť�Ƿ�˫����
	virtual void OnKeyDoubleClicked(ENoloDeviceType DevType, UCHAR Keys) {};
	//���ڰ�ť������
	virtual void OnButtonPressed(ENoloDeviceType device, EControlerButtonType type) {};
	//���ڰ�ť��̧��
	virtual void OnButtonRelease(ENoloDeviceType device, EControlerButtonType type) {};
	//������
	virtual void OnNewData(const NOLOData &_noloData) {};
	//�ж�Nolo�豸�Ƿ���Ҫ����
	virtual void OnNoloDevNeedUpdate(int Versions) {};

};

}