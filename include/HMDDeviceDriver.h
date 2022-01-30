#pragma once
#include <windows.h>
#include <math.h>
#include <thread>
#include <openvr_driver.h>
#include "NoloClientLib.h"
#include "driver_log.h"

using namespace NOLOVR;
using namespace vr;

class CHMDDeviceDriver :public ITrackedDeviceServerDriver
{
public:
	CHMDDeviceDriver();
	~CHMDDeviceDriver();
	//ITrackedDeviceServerDriver
	virtual EVRInitError Activate(vr::TrackedDeviceIndex_t unObjectId);
	virtual void Deactivate();
	virtual void EnterStandby();
	virtual void *GetComponent(const char *pchComponentNameAndVersion);
	virtual void DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize);
	virtual DriverPose_t GetPose();

	void ActiveateHuaWeiHmd();
	//std::string GetSerialNumber();
	//UpdatePos
	void SetNoloHmdData(const HMD &hmd);
	void GetHoloHmdData(HMD &hmdDataOut);
	void RecenterHmd(const HMD &HmdData, const Controller &CtrData);
	void TurnAroundHmd();

	void RunFrame();
	DriverPose_t &GetDriverPose();
	void SetNoloConnected(bool bConnected);
private:
	void InitPose();
private:
	//===========xiaoyang================
	IVRDisplayComponent* m_huaDisplayComponent;
	DriverPose_t m_DriverPose_t;
	//===================================
	vr::TrackedDeviceIndex_t m_unObjectId;
	vr::PropertyContainerHandle_t m_ulPropertyContainer;

	std::string m_sSerialNumber;
	std::string m_sModelNumber;

	float m_flIPD;
	bool m_IsTurnAround;

	//================
	bool m_bDriectMode;
	HMD  m_NoloHmdData;
	CRITICAL_SECTION m_NoloCS;
	//Recenter
	HMD  m_HmdRecenterData;
	Controller m_RecenterContoller;
	bool m_BRecentering;
	//TurnBack 时的头盔位置
	NVector3 m_hmdTurnBackPos;

	bool m_BNoloConnected;

	std::thread m_ThreadAltADKey;

	HANDLE m_ThreadHandleKeyboard = NULL;
};
