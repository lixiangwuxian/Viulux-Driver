#include "ServerDriver.h"
#include "CommonUtil.h"
#include "NoloDeviceManager.h"
#include "Nolo_DeviceType.h"

using namespace std;

CServerDriver::CServerDriver()
{
	m_NoloManager = new NoloDeviceManager(this);
	m_pNullHmdLatest = nullptr;
	m_bEventThreadRunning = false;
	m_bStopRequested = false;
}

CServerDriver::~CServerDriver()
{
}

EVRInitError CServerDriver::Init(IVRDriverContext * pDriverContext)
{
	VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);
	InitDriverLog(vr::VRDriverLog());
	DriverLog("HuaWei Nolo 1.0.13 CServerDriver Init ==========\n");
	m_pNullHmdLatest = new CHMDDeviceDriver();
	//noloLeftController = new NOLOController("1314", TrackedControllerRole_LeftHand);
	//noloRightController = new NOLOController("1315", TrackedControllerRole_RightHand);
	
	vr::VRServerDriverHost()->TrackedDeviceAdded("1234", vr::TrackedDeviceClass_HMD, m_pNullHmdLatest);
	//bool bAddController = false;
	//vr::VRServerDriverHost()->TrackedDeviceAdded(noloLeftController->GetSerialNumber().c_str(),  vr::TrackedDeviceClass_Controller, noloLeftController);
	//vr::VRServerDriverHost()->TrackedDeviceAdded(noloRightController->GetSerialNumber().c_str(), vr::TrackedDeviceClass_Controller, noloRightController);
	//事件处理线程
	//startServerEvent();
	m_Thread = std::thread(&CServerDriver::UpdateDataWork, this);
	m_Thread.detach();

	m_NoloManager->InitNoloDevice();

	return VRInitError_None;
}

void CServerDriver::Cleanup()
{
	CleanupDriverLog();
	SafeDelte(m_pNullHmdLatest);
}

const char * const * CServerDriver::GetInterfaceVersions()
{
	return vr::k_InterfaceVersions;
}

void CServerDriver::RunFrame()
{
}

bool CServerDriver::ShouldBlockStandbyMode()
{
	return false;
}

void CServerDriver::EnterStandby()
{
}

void CServerDriver::LeaveStandby()
{
}

void CServerDriver::UpdatePose(const NOLOData &newData)
{
	if (m_pNullHmdLatest)
	{
		m_pNullHmdLatest->SetNoloHmdData(newData.hmdData);
	}
	//if (noloLeftController)
	//{
	//	noloLeftController->UpdatePose(newData.LeftHand);
	//}
	//if (noloRightController)
	//{
	//	noloRightController->UpdatePose(newData.RightHand);
	//}
}

void CServerDriver::RecenterHmd(const HMD & HmdData, const Controller & CtrData)
{
	m_pNullHmdLatest->RecenterHmd(HmdData, CtrData);
}

void CServerDriver::TurnArroundHmd()
{
	m_pNullHmdLatest->TurnAroundHmd();
}

void CServerDriver::onButtonPressed(ENoloDeviceType device, EControlerButtonType type)
{
	//if (device == ENoloDeviceType::eLeftController)
	//{
	//	noloLeftController->onButtonPressed(type);
	//}
	//else if (device == ENoloDeviceType::eRightController)
	//{
	//	noloRightController->onButtonPressed(type);
	//}

}

void CServerDriver::onButtonRelease(ENoloDeviceType device, EControlerButtonType type)
{
	//if (device == ENoloDeviceType::eLeftController)
	//{
	//	noloLeftController->onButtonReleased(type);
	//}
	//else if (device == ENoloDeviceType::eRightController)
	//{
	//	noloRightController->onButtonReleased(type);
	//}
}


void CServerDriver::startServerEvent()
{
	if (!m_bEventThreadRunning)
	{
		m_bEventThreadRunning = true;
		m_EventThread = thread(&CServerDriver::ProcessEventLoop, this);
		m_EventThread.detach();
	}
}

void CServerDriver::ProcessEventLoop()
{
	VREvent_t pEventHandle;
	bool bHasEvent = false;
	while (m_bEventThreadRunning)
	{
		bHasEvent = vr::VRServerDriverHost()->PollNextEvent(&pEventHandle,sizeof(VREvent_t));
		//getEvent
		if (bHasEvent)
		{
			ProcessEvent(pEventHandle);
		}
		else
		{
			Sleep(3);
		}
		memset(&pEventHandle, 0, sizeof(VREvent_t));
	}
}

int ConvertAmplitude(float fAmplitude)
{
	//NOLO Controler Valid Amplitude: 52 - 100
	float newAmplitude = (fAmplitude * 100.0f) + 52.0f;
	int iAmp = newAmplitude;
	if (iAmp > 100)
	{
		iAmp = 100;
	}
	return iAmp;
}


void CServerDriver::ProcessEvent(VREvent_t &eventHandle)
{
	//DriverLog("#=========Type=%d== InDex=%d=========\n", eventHandle.eventType, eventHandle.trackedDeviceIndex);
	if (eventHandle.eventType == VREvent_Input_HapticVibration)
	{
		VREvent_HapticVibration_t data = eventHandle.data.hapticVibration;
		int iAmplitude = ConvertAmplitude(data.fAmplitude);
		//if (noloLeftController->GetPropertyContainer() == data.containerHandle)
		//{
		//	m_NoloManager->Context()->setTriggerHapticPulse(ENoloDeviceType::eLeftController, iAmplitude);
		//}
		//else if (noloRightController->GetPropertyContainer() == data.containerHandle)
		//{
		//	m_NoloManager->Context()->setTriggerHapticPulse(ENoloDeviceType::eRightController, iAmplitude);
		//}
	}
}

void CServerDriver::UpdateDataWork()
{
	auto retryInterval = std::chrono::milliseconds(16);
	auto pollDeadline = std::chrono::steady_clock::now();

	while (!m_bStopRequested)
	{
		if (m_pNullHmdLatest != nullptr)
		{
			m_pNullHmdLatest->RunFrame();
		}
		//this_thread::sleep_until(pollDeadline + retryInterval);
	}
}

void CServerDriver::SetNoloConnected(bool bConnected)
{
	if (m_pNullHmdLatest != nullptr)
	{
		m_pNullHmdLatest->SetNoloConnected(bConnected);
	}
}


