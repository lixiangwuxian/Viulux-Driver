#include "NOLOController.h"
#include <driverlog.h>

#define PI 3.1415926535897932384626433832

using namespace vr;

NOLOController::NOLOController(std::string std, ETrackedControllerRole type)
{
	IsTurnAround = false;
	m_ulPropertyContainer = vr::k_ulInvalidPropertyContainer;
	m_sSerialNumber = std;
	m_sModelNumber = "{relativty}/rendermodels/nolo_controller";
	m_Type = type;
}

NOLOController::~NOLOController()
{
}

EVRInitError NOLOController::Activate(uint32_t unObjectId)
{
	DriverLog("NOLOController really Activate %s Whid ID: %d\n", m_sSerialNumber, unObjectId);
	m_unObjectId = unObjectId;
	initPos();
	m_ulPropertyContainer = vr::VRProperties()->TrackedDeviceToPropertyContainer(m_unObjectId);
	vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_SerialNumber_String, m_sSerialNumber.c_str());
	vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_ModelNumber_String, "ViveMV");
	vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_RenderModelName_String, m_sModelNumber.c_str());
	vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_ManufacturerName_String, "HTC");
	vr::VRProperties()->SetInt32Property(m_ulPropertyContainer, Prop_Axis0Type_Int32, k_eControllerAxis_TrackPad);
	vr::VRProperties()->SetInt32Property(m_ulPropertyContainer, Prop_Axis1Type_Int32, k_eControllerAxis_Trigger);
	vr::VRProperties()->SetStringProperty(m_ulPropertyContainer,Prop_InputProfilePath_String, "{relativty}/input/vive_controller_profile.json");
	
	//ÊÖ±úÀàÐÍ
	vr::VRProperties()->SetInt32Property(m_ulPropertyContainer, Prop_ControllerRoleHint_Int32, m_Type);
	ETrackedPropertyError erro;
	int DevClass = vr::VRProperties()->GetInt32Property(m_ulPropertyContainer, Prop_ControllerRoleHint_Int32, &erro);
	DriverLog("NOLOController Activate RoleHint: %d  # Erro: %d\n", DevClass, erro);


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
	vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/system/click", &m_system);
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
	m_Pose.vecDriverFromHeadTranslation[1] = 0.000f;
	m_Pose.vecDriverFromHeadTranslation[2] = 0.000f;
	//m_Pose.vecDriverFromHeadTranslation[1] = 0.007f;
	//m_Pose.vecDriverFromHeadTranslation[2] = -0.073f;

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
	DriverLog("NoloKeyPressed!");
	SendButtonUpdate(type,true);
}

void NOLOController::onButtonReleased(EControlerButtonType type)
{
	SendButtonUpdate(type, false);
}

DriverPose_t NOLOController::GetPose(Controller ctrData,bool LeftOrRight)
{
	m_Pose.poseIsValid = true;
	m_Pose.deviceIsConnected = true;
	m_Pose.vecPosition[0] = -ctrData.Position.x;
	m_Pose.vecPosition[1] = ctrData.Position.y;
	m_Pose.vecPosition[2] = ctrData.Position.z;

	NQuaternion offsetRot(0,1,0,0);
	ctrData.Rotation = offsetRot * ctrData.Rotation;

	m_Pose.qRotation.w = -ctrData.Rotation.w;
	m_Pose.qRotation.x = -ctrData.Rotation.x;
	m_Pose.qRotation.y = ctrData.Rotation.y;
	m_Pose.qRotation.z = ctrData.Rotation.z;
	NOLOData tmpNolo = GetNoloData();
	if (LeftOrRight == true) {//left
		m_Pose.vecVelocity[0] = -tmpNolo.NoloSensorData.vecLVelocity.x; //ctrData.TrackPos.Velocity.x;
		m_Pose.vecVelocity[1] = tmpNolo.NoloSensorData.vecLVelocity.y; //ctrData.TrackPos.Velocity.y;
		m_Pose.vecVelocity[2] = -tmpNolo.NoloSensorData.vecLVelocity.z; //ctrData.Trackos.Velocity.z;
		m_Pose.vecAngularVelocity[0] = -tmpNolo.NoloSensorData.vecLAngularVelocity.x;//ctrData.TrackPos.AngularVelocity.x;
		m_Pose.vecAngularVelocity[1] = tmpNolo.NoloSensorData.vecLAngularVelocity.y;//ctrData.TrackPos.AngularVelocity.y;
		m_Pose.vecAngularVelocity[2] = tmpNolo.NoloSensorData.vecLAngularVelocity.z;//ctrData.TrackPos.AngularVelocity.z;
	}
	else {
		m_Pose.vecVelocity[0] = -tmpNolo.NoloSensorData.vecRVelocity.x;//ctrData.TrackPos.Velocity.x;
		m_Pose.vecVelocity[1] = tmpNolo.NoloSensorData.vecRVelocity.y;//ctrData.TrackPos.Velocity.y;
		m_Pose.vecVelocity[2] = -tmpNolo.NoloSensorData.vecRVelocity.z;
		m_Pose.vecAngularVelocity[0] = -tmpNolo.NoloSensorData.vecRAngularVelocity.x;//ctrData.TrackPos.AngularVelocity.x;
		m_Pose.vecAngularVelocity[1] = tmpNolo.NoloSensorData.vecRAngularVelocity.y;//ctrData.TrackPos.AngularVelocity.y;
		m_Pose.vecAngularVelocity[2] = tmpNolo.NoloSensorData.vecRAngularVelocity.z;//ctrData.TrackPos.AngularVelocity.z;
	}
	if (IsTurnAround)
	{
		NQuaternion rot(ctrData.Rotation);
		NQuaternion rotBackQ(0, 1, 0, 0);
		rot = rot * rotBackQ* NQuaternion(0,0,0,1);
		m_Pose.vecPosition[0] = 2 * m_controllerTurnBackPos.x - m_Pose.vecPosition[0];
		m_Pose.vecPosition[2] = 2 * m_controllerTurnBackPos.z - m_Pose.vecPosition[2];
		m_Pose.vecVelocity[0] = -m_Pose.vecVelocity[0];
		m_Pose.vecVelocity[2] = -m_Pose.vecVelocity[2];
		m_Pose.vecAngularVelocity[0] = -m_Pose.vecAngularVelocity[0];
		m_Pose.vecAngularVelocity[2] = -m_Pose.vecAngularVelocity[2];
		m_Pose.qRotation.w = -rot.w;
		m_Pose.qRotation.x = -rot.x;
		m_Pose.qRotation.y = rot.y;
		m_Pose.qRotation.z = rot.z;
	}
	return m_Pose;
}

std::string NOLOController::GetSerialNumber()
{
	return m_sSerialNumber;
}

void NOLOController::UpdatePose(Controller ctrData,bool leftOrRight)
{
	if (m_unObjectId != vr::k_unTrackedDeviceIndexInvalid)
	{
		vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_unObjectId, GetPose(ctrData,leftOrRight), sizeof(DriverPose_t));
		//if (ctrData.Touched)
		//{
			float x = ctrData.TouchAxis.x;
			float y = ctrData.TouchAxis.y;
			vr::VRDriverInput()->UpdateScalarComponent(m_trackpadx, x, -0.005);
			vr::VRDriverInput()->UpdateScalarComponent(m_trackpady, y, -0.005);
		//}
		/**else {
			vr::VRDriverInput()->UpdateScalarComponent(m_trackpadx, 0, -0.005);
			vr::VRDriverInput()->UpdateScalarComponent(m_trackpady, 0, -0.005);
		}**/
	}
}



void NOLOController::SetTurnAround(Controller ctrData,HMD hmdData)
{
	if (IsTurnAround == false)
	{
		m_controllerTurnBackPos = hmdData.HMDPosition;
		m_controllerTurnBackPos.y = ctrData.Position.y;
	}
	IsTurnAround = !IsTurnAround;
}


