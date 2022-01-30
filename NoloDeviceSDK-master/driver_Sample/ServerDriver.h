#pragma once
#include "myopenvr.h"

#include "HMDDeviceDriver.h"
#include "NOLOController.h"

class NoloDeviceManager;

class CServerDriver:public IServerTrackedDeviceProvider
{
public:
	CServerDriver();
	~CServerDriver();
	//IServerTrackedDeviceProvider
	virtual EVRInitError Init(IVRDriverContext *pDriverContext) override;
	virtual void Cleanup() override;
	virtual const char * const *GetInterfaceVersions() override;
	virtual void RunFrame() override;
	virtual bool ShouldBlockStandbyMode() override;
	virtual void EnterStandby() override;
	virtual void LeaveStandby() override;
	//PoseUpdated
	void UpdatePose(const NOLOData &newData);
	void RecenterHmd(const HMD &HmdData,const Controller &CtrData);
	void TurnArroundHmd();

	void onButtonPressed(ENoloDeviceType device, EControlerButtonType type);
	void onButtonRelease(ENoloDeviceType device, EControlerButtonType type);

	//ServerEvent
	void startServerEvent();
	void ProcessEventLoop();
	void ProcessEvent(VREvent_t &eventHandle);

	void UpdateDataWork();
	void SetNoloConnected(bool bConnected);
private:
	NoloDeviceManager *m_NoloManager;//Nolo�1�7��
	CHMDDeviceDriver *m_pNullHmdLatest;//�0�5
	//NOLOController * noloLeftController;//�1�7�1�7�1�7�1�7
	//NOLOController * noloRightController;//�1�7�1�7�1�7�1�7
	
	//Driver �1�7�0�4�1�7�1�7�1�7�1�7�1�7
	std::thread m_EventThread;
	bool m_bEventThreadRunning;
	//
	bool  m_bStopRequested;
	std::thread m_Thread;
};
