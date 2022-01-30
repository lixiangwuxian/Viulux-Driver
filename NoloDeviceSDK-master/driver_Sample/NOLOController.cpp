#include "NOLOController.h"

NOLOController::NOLOController(std::string std, ETrackedControllerRole type)
{
	IsTurnAround = false;
	m_ulPropertyContainer = vr::k_ulInvalidPropertyContainer;
	m_sSerialNumber = std;
	m_sModelNumber = "vr_controller_vive_1_5";
	m_Type = type;
}

NOLOController::~NOLOController()
{
}

EVRInitError NOLOController::Activate(uint32_t unObjectId)
{
	DriverLog("NOLOController Activate %s Whid ID: %d\n", m_sSerialNumber, unObjectId);
	m_unObjectId = unObjectId;
	initPos();
	m_ulPropertyContainer = vr::VRProperties()->TrackedDeviceToPropertyContainer(m_unObjectId);
	vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_SerialNumber_String, m_sSerialNumber.c_str());
	vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_ModelNumber_String, "ViveMV");
	vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_RenderModelName_String, m_sModelNumber.c_str());
	vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_ManufacturerName_String, "HTC");
	vr::VRProperties()->SetInt32Property(m_ulPropertyContainer, Prop_Axis0Type_Int32, k_eControllerAxis_TrackPad);
	vr::VRProperties()->SetInt32Property(m_ulPropertyContainer, Prop_Axis1Type_Int32, k_eControllerAxis_Trigger);
	vr::VRProperties()->SetStringProperty(m_ulPropertyContainer,
		Prop_InputProfilePath_String, "{huaweinolo}/input/controller_profile.json");
	
	//ÊÖ±úÀàÐÍ
	vr::VRProperties()->SetInt32Property(m_ulPropertyContainer, Prop_ControllerRoleHint_Int32, m_Type);
	//ETrackedPropertyError erro;
	//int DevClass = vr::VRProperties()->GetInt32Property(m_ulPropertyContainer, Prop_ControllerRoleHint_Int32, &erro);
	//DriverLog("NOLOController Activate RoleHint: %d  # Erro: %d\n", DevClass, erro);


	uint64_t reval= vr::ButtonMaskFromId(vr::k_EButton_ApplicationMenu) |
		vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad) |
		vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Trigger) |
		vr::ButtonMaskFromId(vr::k_EButton_System) |
		vr::ButtonMaskFromId(vr::k_EButton_Grip);
	vr::VRProperties()->SetUint64Property(m_ulPropertyContainer, Prop_SupportedButtons_Uint64, reval);
	InitEventHandler();
	return VRInitError_None;
}

void NOLOController::InitEventHandler()
{
	//in
	vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer,"/input/system/click",&m_system);
	vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/grip/click", &m_grip);
	vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/application_menu/click", &m_application_menu);
	vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/trigger/click", &m_trigger);
	vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/trackpad/click", &m_trackpad);
	vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/trackpad/touch", &m_touch);

	vr::VRDriverInput()->CreateScalarComponent(m_ulPropertyContainer, "/input/trigger/value",
		&m_trigger_value, VRScalarType_Absolute, VRScalarUnits_NormalizedOneSided);

	vr::VRDriverInput()->CreateScalarComponent(m_ulPropertyContainer,"/input/trackpad/x", 
		&m_trackpadx, VRScalarType_Absolute, VRScalarUnits_NormalizedTwoSided);
	vr::VRDriverInput()->CreateScalarComponent(m_ulPropertyContainer, "/input/trackpad/y",
		&m_trackpady, VRScalarType_Absolute, VRScalarUnits_NormalizedTwoSided);
	//out
	vr::VRDriverInput()->CreateHapticComponent(m_ulPropertyContainer, "/output/haptic", &m_out_Haptic);
	
}


void NOLOController::Deactivate()
{
	m_unObjectId = k_unTrackedDeviceIndexInvalid;
}

void NOLOController::EnterStandby()
{
}

void * NOLOController::GetComponent(const char * pchComponentNameAndVersion)
{
	return NULL;
}

void NOLOController::DebugRequest(const char * pchRequest, char * pchResponseBuffer, uint32_t unResponseBufferSize)
{
	if (unResponseBufferSize >= 1)
		pchResponseBuffer[0] = 0;
}

DriverPose_t NOLOController::GetPose()
{
	return m_Pose; 
}

PropertyContainerHandle_t NOLOController::GetPropertyContainer()
{
	return m_ulPropertyContainer;
}

void NOLOController::initPos()
{
	m_Pose.result = vr::TrackingResult_Running_OK;
	m_Pose.poseIsValid = true;
	m_Pose.willDriftInYaw = true;
	m_Pose.shouldApplyHeadModel = false;
	m_Pose.deviceIsConnected = true;

	m_Pose.poseTimeOffset = -0.016f;
	m_Pose.qWorldFromDriverRotation.w = 1.0;
	m_Pose.qWorldFromDriverRotation.x = 0.0;
	m_Pose.qWorldFromDriverRotation.y = 0.0;
	m_Pose.qWorldFromDriverRotation.z = 0.0;
	m_Pose.vecWorldFromDriverTranslation[0] = 0.0;
	m_Pose.vecWorldFromDriverTranslation[1] = 0.0;
	m_Pose.vecWorldFromDriverTranslation[2] = 0.0;

	m_Pose.qDriverFromHeadRotation.w = 1.0f;
	m_Pose.qDriverFromHeadRotation.x = 0.0f;
	m_Pose.qDriverFromHeadRotation.y = 0.0f;
	m_Pose.qDriverFromHeadRotation.z = 0.0f;
	m_Pose.vecDriverFromHeadTranslation[0] = 0.000f;
	m_Pose.vecDriverFromHeadTranslation[1] = 0.007f;
	m_Pose.vecDriverFromHeadTranslation[2] = -0.073f;

	m_Pose.vecAcceleration[0] = 0.0;
	m_Pose.vecAcceleration[1] = 0.0;
	m_Pose.vecAcceleration[2] = 0.0;
	m_Pose.vecAngularAcceleration[0] = 0.0;
	m_Pose.vecAngularAcceleration[1] = 0.0;
	m_Pose.vecAngularAcceleration[2] = 0.0;
}


//this is a private method
void NOLOController::SendButtonUpdate(EControlerButtonType type, bool bPress)
{
	const double fTimeOffset = -1.005;
	float TriggerValue = 0.0f;
	switch (type)
	{
	case NOLOVR::ePadBtn:
		vr::VRDriverInput()->UpdateBooleanComponent(m_trackpad, bPress,fTimeOffset);
		break;
	case NOLOVR::eTriggerBtn:
		vr::VRDriverInput()->UpdateBooleanComponent(m_trigger, bPress, fTimeOffset);
		if (bPress)
		{
			TriggerValue = 1.0f;
		}
		vr::VRDriverInput()->UpdateScalarComponent(m_trigger_value, TriggerValue, fTimeOffset);
		break;
	case NOLOVR::eMenuBtn:
		vr::VRDriverInput()->UpdateBooleanComponent(m_application_menu, bPress, fTimeOffset);
		break;
	case NOLOVR::eSystemBtn:
		vr::VRDriverInput()->UpdateBooleanComponent(m_system, bPress, fTimeOffset);
		break;
	case NOLOVR::eGripBtn:
		vr::VRDriverInput()->UpdateBooleanComponent(m_grip, bPress, fTimeOffset);
		break;
	case NOLOVR::ePadTouch:
		vr::VRDriverInput()->UpdateBooleanComponent(m_touch, bPress, fTimeOffset);
		break;
	default:
		break;
	}
}

void NOLOController::onButtonPressed(EControlerButtonType type)
{
	SendButtonUpdate(type,true);
}

void NOLOController::onButtonReleased(EControlerButtonType type)
{
	SendButtonUpdate(type, false);
}

DriverPose_t NOLOController::GetPose(Controller ctrData)
{
	m_Pose.poseIsValid = true;
	m_Pose.deviceIsConnected = true;
	//m_Pose.vecPosition[0] = ctrData.TrackPos.Position.x;
	//m_Pose.vecPosition[1] = ctrData.TrackPos.Position.y;
	//m_Pose.vecPosition[2] = -ctrData.TrackPos.Position.z;

	//m_Pose.qRotation.w = -ctrData.TrackPos.Rotation.w;
	//m_Pose.qRotation.x = ctrData.TrackPos.Rotation.x;
	//m_Pose.qRotation.y = ctrData.TrackPos.Rotation.y;
	//m_Pose.qRotation.z = -ctrData.TrackPos.Rotation.z;

	//m_Pose.vecVelocity[0] = ctrData.TrackPos.Velocity.x;
	//m_Pose.vecVelocity[1] = ctrData.TrackPos.Velocity.y;
	//m_Pose.vecVelocity[2] = ctrData.TrackPos.Velocity.z;
	//m_Pose.vecAngularVelocity[0] = ctrData.TrackPos.AngularVelocity.x;
	//m_Pose.vecAngularVelocity[1] = ctrData.TrackPos.AngularVelocity.y;
	//m_Pose.vecAngularVelocity[2] = ctrData.TrackPos.AngularVelocity.z;
	
	if (IsTurnAround)
	{
		//NQuaternion rot(ctrData.TrackPos.Rotation);
		//NQuaternion rotBackQ(0, 1, 0, 0);
		//rot = rot * rotBackQ;

		//m_Pose.qRotation.w = -rot.w;
		//m_Pose.qRotation.x = rot.x;
		//m_Pose.qRotation.y = rot.y;
		//m_Pose.qRotation.z = -rot.z;
	}
	return m_Pose;
}

std::string NOLOController::GetSerialNumber()
{
	return m_sSerialNumber;
}

void NOLOController::UpdatePose(Controller ctrData)
{
	if (m_unObjectId != vr::k_unTrackedDeviceIndexInvalid)
	{
		vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_unObjectId, GetPose(ctrData), sizeof(DriverPose_t));
		if (ctrData.Touched)
		{
			float x = ctrData.TouchAxis.x;
			float y = ctrData.TouchAxis.y;
			vr::VRDriverInput()->UpdateScalarComponent(m_trackpadx, x, -0.005);
			vr::VRDriverInput()->UpdateScalarComponent(m_trackpady, y, -0.005);
		}
	}
}


void NOLOController::SetTurnAround(bool bTurn)
{
	IsTurnAround = bTurn;
}


