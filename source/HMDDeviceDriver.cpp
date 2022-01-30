#include "HMDDeviceDriver.h"
#include "nvapi.h"
#include "KF_Quaternion.h"

#define VENDOR_ID_HW		0xF622
#define PRODUCT_ID_HW		0x0002
#define HMD_PLUGIN 
#define NVAPI

//===============»ªÎª¿ì½Ý¼ü==================

void MessageLoop()
{
	MSG message;
	while (GetMessage(&message, NULL, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}


HHOOK hKeyboardHook;

LRESULT CALLBACK KeyboardEvent(int nCode, WPARAM wParam, LPARAM lParam)
{
	DWORD CTRL_key = 0;
	DWORD ALT_key = 0;
	if ((nCode == HC_ACTION) && ((wParam == WM_SYSKEYDOWN) || (wParam == WM_KEYDOWN)))
	{
		KBDLLHOOKSTRUCT hooked_key = *((KBDLLHOOKSTRUCT*)lParam);
		int key = hooked_key.vkCode;
		//SHIFT_key = GetAsyncKeyState(VK_SHIFT);
		CTRL_key = GetAsyncKeyState(VK_CONTROL);
		ALT_key = GetAsyncKeyState(VK_MENU);
		if (key >= 'A' && key <= 'Z')
		{
			if (GetAsyncKeyState(VK_SHIFT) >= 0) key += 32;
			if (ALT_key != 0 && CTRL_key != 0 && key == 'q')
			{
				PostQuitMessage(0);
			}

			if (ALT_key != 0 && key == 'x')
			{
				resetPose();
			}
			if (ALT_key != 0 && key == 'a')
			{
				leftRudder();
			}
			if (ALT_key != 0 && key == 'd')
			{
				rightRudder();
			}
			ALT_key = 0;
		}
	}
	return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

DWORD WINAPI driver_HotKey(LPVOID lpParm)
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	if (!hInstance) hInstance = LoadLibrary((LPCTSTR)lpParm);
	if (!hInstance) return 1;
	hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyboardEvent, hInstance, NULL);
	MessageLoop();
	UnhookWindowsHookEx(hKeyboardHook);
	if (hInstance)
		FreeLibrary(hInstance);
	return 0;
}
//================================

CHMDDeviceDriver::CHMDDeviceDriver()
{
	m_BNoloConnected = false;
	m_BRecentering = false;
	m_IsTurnAround = false;
	m_sSerialNumber = "1234";
	m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
	m_ulPropertyContainer = vr::k_ulInvalidPropertyContainer;
	m_flIPD = vr::VRSettings()->GetFloat(k_pch_SteamVR_Section, k_pch_SteamVR_IPD_Float);
	InitializeCriticalSection(&m_NoloCS);
	m_huaDisplayComponent = createHmdDriver();
#ifdef NVAPI
	ActiveateHuaWeiHmd();
#endif
}

CHMDDeviceDriver::~CHMDDeviceDriver()
{
	DeleteCriticalSection(&m_NoloCS);
#ifdef NVAPI
	if (m_bDriectMode)
	{
		NvAPI_DISP_DisableDirectMode(VENDOR_ID_HW, 0);
	}
#endif
}

void CHMDDeviceDriver::ActiveateHuaWeiHmd()
{
	auto status = NVAPI_ERROR;
	int nDisableCnt = 0;
	while (status != NVAPI_OK || nDisableCnt > 3)
	{
		status = NvAPI_DISP_DisableDirectMode(VENDOR_ID_HW, 0);
		nDisableCnt++;
		Sleep(500);
	}
	status = NvAPI_DISP_EnableDirectMode(VENDOR_ID_HW, 0);
	if (status == NVAPI_OK)
	{
		m_bDriectMode = true;
	}
	else
	{
		m_bDriectMode = false;
	}
	// Get the display handles and display identifiers
	// NVIDIA GPUs currently support up to a maximum of four simultaneous outputs
	NvU32 numDisplays = 4;
	NV_DIRECT_MODE_DISPLAY_HANDLE displays[4] = { 0 };
	status = NvAPI_DISP_EnumerateDirectModeDisplays(VENDOR_ID_HW, &numDisplays, displays, NV_ENUM_DIRECTMODE_DISPLAY_ENABLED);
	if (status != NVAPI_OK)
	{
		std::fprintf(stderr, "Failed to query for displayIds with error=0x%x\n", status);
	}
}

EVRInitError CHMDDeviceDriver::Activate(vr::TrackedDeviceIndex_t unObjectId)
{
	DriverLog("===============Activate HuaWeiNOLO=========\n");
	m_unObjectId = unObjectId;
	InitPose();
	m_ulPropertyContainer = vr::VRProperties()->TrackedDeviceToPropertyContainer(m_unObjectId);
	vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_ModelNumber_String, "ViveMV");
	vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_ManufacturerName_String,"HTC");
	vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_RenderModelName_String, "HuaWei1234");
	vr::VRProperties()->SetFloatProperty(m_ulPropertyContainer, Prop_UserIpdMeters_Float, m_flIPD);
	vr::VRProperties()->SetFloatProperty(m_ulPropertyContainer, Prop_UserHeadToEyeDepthMeters_Float, 0.0f);
	vr::VRProperties()->SetFloatProperty(m_ulPropertyContainer, Prop_DisplayFrequency_Float, 0.0f);
	vr::VRProperties()->SetFloatProperty(m_ulPropertyContainer, Prop_SecondsFromVsyncToPhotons_Float, 0.0f);

	// return a constant that's not 0 (invalid) or 1 (reserved for Oculus)
	vr::VRProperties()->SetUint64Property(m_ulPropertyContainer, Prop_CurrentUniverseId_Uint64, 2);

	// avoid "not fullscreen" warnings from vrmonitor
	vr::VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_IsOnDesktop_Bool, false);

	//direct mode
	vr::VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_HasDriverDirectModeComponent_Bool, false);
	vr::VRProperties()->SetInt32Property(m_ulPropertyContainer, Prop_EdidVendorID_Int32, VENDOR_ID_HW);
	vr::VRProperties()->SetInt32Property(m_ulPropertyContainer, Prop_EdidProductID_Int32, PRODUCT_ID_HW);

	bool bSetupIconUsingExternalResourceFile = false;
	if (!bSetupIconUsingExternalResourceFile)
	{
		// Setup properties directly in code.
		// Path values are of the form {drivername}\icons\some_icon_filename.png
		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceOff_String, "{huaweinolo}/icons/headset_status_off.png");
		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceSearching_String, "{huaweinolo}/icons/headset_status_searching.gif");
		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceSearchingAlert_String, "{huaweinolo}/icons/headset_status_searching_alert.gif");
		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceReady_String, "{huaweinolo}/icons/headset_status_ready.png");
		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceReadyAlert_String, "{huaweinolo}/icons/headset_status_ready_alert.png");
		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceNotReady_String, "{huaweinolo}/icons/headset_status_error.png");
		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceStandby_String, "{huaweinolo}/icons/headset_status_standby.png");
		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceAlertLow_String, "");
	}

	if (NULL == m_ThreadHandleKeyboard)
	{
		DWORD dwThread;
		m_ThreadHandleKeyboard = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)driver_HotKey, NULL, NULL, &dwThread);
	}
	return VRInitError_None;
}


void CHMDDeviceDriver::Deactivate()
{
	DriverLog("===============Deactivate=========\n");
	auto status = NvAPI_DISP_DisableDirectMode(VENDOR_ID_HW, 0);
	m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
	releaseHmdDriver(m_huaDisplayComponent);
	m_huaDisplayComponent = NULL;
}

void CHMDDeviceDriver::EnterStandby()
{
}

void * CHMDDeviceDriver::GetComponent(const char * pchComponentNameAndVersion)
{
	DriverLog(__FUNCTION__" %s", pchComponentNameAndVersion);
	if (!_stricmp(pchComponentNameAndVersion, ITrackedDeviceServerDriver_Version)) //always return
	{
		return (ITrackedDeviceServerDriver*)this;
	}
	if (!_stricmp(pchComponentNameAndVersion, vr::IVRDisplayComponent_Version))
	{
		return m_huaDisplayComponent;
	}
	return NULL;
}


void CHMDDeviceDriver::DebugRequest(const char * pchRequest, char * pchResponseBuffer, uint32_t unResponseBufferSize)
{
	if (unResponseBufferSize >= 1)
		pchResponseBuffer[0] = 0;
}

DriverPose_t CHMDDeviceDriver::GetPose()
{
	return m_DriverPose_t;
}

void CHMDDeviceDriver::InitPose()
{
	m_DriverPose_t.poseTimeOffset = -0.016f;
	m_DriverPose_t.result = TrackingResult_Running_OK;

	m_DriverPose_t.poseIsValid = false;
	m_DriverPose_t.willDriftInYaw = true;
	m_DriverPose_t.shouldApplyHeadModel = false;
	m_DriverPose_t.deviceIsConnected = true;

	m_DriverPose_t.qWorldFromDriverRotation = HmdQuaternion_Init(1, 0, 0, 0);
	m_DriverPose_t.qDriverFromHeadRotation = HmdQuaternion_Init(1, 0, 0, 0);

	// NOLO VR driver
	m_DriverPose_t.vecWorldFromDriverTranslation[0] = 0;
	m_DriverPose_t.vecWorldFromDriverTranslation[1] = 0;
	m_DriverPose_t.vecWorldFromDriverTranslation[2] = 0;

	m_DriverPose_t.vecDriverFromHeadTranslation[0] = 0.000f;
	m_DriverPose_t.vecDriverFromHeadTranslation[1] = 0.000f;
	m_DriverPose_t.vecDriverFromHeadTranslation[2] = 1.000f;
}

void CHMDDeviceDriver::SetNoloHmdData(const HMD & hmd)
{
	EnterCriticalSection(&m_NoloCS);
	memcpy(&m_NoloHmdData, &hmd, sizeof(HMD));
	LeaveCriticalSection(&m_NoloCS);
}

void CHMDDeviceDriver::GetHoloHmdData(HMD & hmdDataOut)
{
	EnterCriticalSection(&m_NoloCS);
	memcpy(&hmdDataOut, &m_NoloHmdData, sizeof(HMD));
	LeaveCriticalSection(&m_NoloCS);
}

void CHMDDeviceDriver::RecenterHmd(const HMD & HmdData, const Controller & CtrData)
{
	m_HmdRecenterData = HmdData;
	m_RecenterContoller = CtrData;
	m_BRecentering = true;
}


void CHMDDeviceDriver::TurnAroundHmd()
{
	if (m_IsTurnAround == false)
	{
		HMD tempHmd;
		GetHoloHmdData(tempHmd);
		m_hmdTurnBackPos = tempHmd.HMDPosition;

		DriverLog("==SetNoloHmdData==%f,%f,%f=\n", tempHmd.HMDPosition.x, tempHmd.HMDPosition.y, tempHmd.HMDPosition.z);
	}
	m_IsTurnAround = !m_IsTurnAround;
	
}

void CHMDDeviceDriver::RunFrame()
{
	if (m_unObjectId != vr::k_unTrackedDeviceIndexInvalid)
	{
		vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_unObjectId, GetDriverPose(), sizeof(DriverPose_t));
	}
}

DriverPose_t & CHMDDeviceDriver::GetDriverPose()
{
	static float s_offsetYaw = 0.0f;
	static bool g_bUnpluggedFlag = false;
	QUAT s_HeadOffsetQuat = { 1,0,0,0 };

	HMD tempHmd;
	GetHoloHmdData(tempHmd);
	NVector3 HeadPosition = tempHmd.HMDPosition;
	DriverPose_t Huawei = getHWVRPose();
	NOLOData TempData = GetNoloData();//yiming
	QUAT HeadRot = { Huawei.qRotation.w, Huawei.qRotation.x,- Huawei.qRotation.z, Huawei.qRotation.y };
	if (m_BRecentering)
	{
		DriverLog("==m_BRecentering===\n");
		VECT Head_Euler;
		
		NVector3 HandPosStd = m_RecenterContoller.Position;
		NVector3 HeadPosStd = m_HmdRecenterData.HMDPosition; 
		float real_head_yaw = 0.0f;
		if ((TempData.leftData.Position - TempData.rightData.Position).length() < 0.2)
		{
			HandPosStd = (TempData.leftData.Position + TempData.rightData.Position) / 2.0f;
		}

		if ((HandPosStd.x - HeadPosStd.x) > 1e-5f)
		{
			real_head_yaw = atan((HandPosStd.z - HeadPosStd.z) / (HandPosStd.x - HeadPosStd.x)) - PI / 2;
		}
		else if ((HandPosStd.x - HeadPosStd.x) < -1e-5f)
		{
			real_head_yaw = PI / 2 + atan((HandPosStd.z - HeadPosStd.z) / (HandPosStd.x - HeadPosStd.x));
		}
		Q2Attitude(&Head_Euler, HeadRot);
		s_offsetYaw = real_head_yaw - Head_Euler.k;
		if (s_offsetYaw < 0) {
			s_offsetYaw += 2 * PI;
		}
		m_BRecentering = false;
	}
	QUAT head_with_rotation;

	
	if (m_IsTurnAround)
	{
		s_HeadOffsetQuat = { (float)cos((s_offsetYaw + PI) / 2.0),0,0,(float)sin((s_offsetYaw + PI) / 2.0) };
		HeadPosition.x = 2 * m_hmdTurnBackPos.x - HeadPosition.x;
		HeadPosition.z =(2 * m_hmdTurnBackPos.z - HeadPosition.z);
	}
	else
	{
		s_HeadOffsetQuat = { (float)cos(s_offsetYaw / 2.0),0,0,(float)sin(s_offsetYaw / 2.0) };
	}

	QMultiply(&head_with_rotation, s_HeadOffsetQuat, HeadRot);
	Huawei.qRotation = { head_with_rotation.q0,head_with_rotation.q1, head_with_rotation.q3,-head_with_rotation.q2 };

	if (Huawei.poseIsValid == false)
	{
		g_bUnpluggedFlag = true;
	}
	if ((g_bUnpluggedFlag == true) && (Huawei.poseIsValid == true))
	{
		setPCMode();
		g_bUnpluggedFlag = false;
	}

	if (!m_BNoloConnected)
	{
		HeadPosition.y = 1.65f;
	}

	Huawei.vecPosition[0] = HeadPosition.x;
	Huawei.vecPosition[1] = HeadPosition.y;
	Huawei.vecPosition[2] = -HeadPosition.z;
	Huawei.poseTimeOffset = -0.0f;
	Huawei.shouldApplyHeadModel = false;
	Huawei.deviceIsConnected = true;
	Huawei.result = TrackingResult_Running_OK;
	Huawei.poseIsValid = true;
	m_DriverPose_t = Huawei;
	return m_DriverPose_t;
}

void CHMDDeviceDriver::SetNoloConnected(bool bConnected)
{
	m_BNoloConnected = bConnected;
	DriverLog("==SetNoloConnected===%d\n", bConnected);
}


