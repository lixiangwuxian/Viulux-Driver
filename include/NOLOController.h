#pragma once
#ifndef _NOLORACKEDDEVICE_H_
#define _NOLORACKEDDEVICE_H_

#include <windows.h>
#include <math.h>
#include <thread>
#include "openvr_driver.h"
#include "NoloClientLib.h"

using namespace vr;

using namespace NOLOVR;
class NOLOController : public ITrackedDeviceServerDriver
{
public:
	NOLOController(std::string id, ETrackedControllerRole type);
	~NOLOController();

	virtual EVRInitError Activate(uint32_t unObjectId);


	virtual void Deactivate() ;

	/** Handles a request from the system to put this device into standby mode. What that means is defined per-device. */
	virtual void EnterStandby() ;

	/** Requests a component interface of the driver for device-specific functionality. The driver should return NULL
	* if the requested interface or version is not supported. */
	virtual void *GetComponent(const char *pchComponentNameAndVersion);

	/** A VR Client has made this debug request of the driver. The set of valid requests is entirely
	* up to the driver and the client to figure out, as is the format of the response. Responses that
	* exceed the length of the supplied buffer should be truncated and null terminated */
	virtual void DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize) ;

	// ------------------------------------
	// Tracking Methods
	// ------------------------------------
	virtual DriverPose_t GetPose();

	PropertyContainerHandle_t GetPropertyContainer();

	void SendButtonUpdate(EControlerButtonType type, bool bPress);
	void onButtonPressed(EControlerButtonType type);
	void onButtonReleased(EControlerButtonType type);

	std::string GetSerialNumber();
	void UpdatePose(Controller ctrData,bool leftOrRight);
	void SetTurnAround();

private:
	void initPos();
	void InitEventHandler();
	DriverPose_t GetPose(Controller ctrData, bool LeftOrRight);
private:
	VRControllerState_t m_ControllerState;
	vr::DriverPose_t  m_Pose;
	vr::TrackedDeviceIndex_t m_unObjectId;
	vr::PropertyContainerHandle_t m_ulPropertyContainer;

	//ContolerData Data update To Openvr
	vr::VRInputComponentHandle_t  m_system;			  // "/input/system/click"
	vr::VRInputComponentHandle_t  m_grip;		      // "/input/grip/click"
	vr::VRInputComponentHandle_t  m_application_menu; // "/input/application_menu/click"
	vr::VRInputComponentHandle_t  m_trigger;          // "/input/trigger/click"
	vr::VRInputComponentHandle_t  m_trigger_value;    // "/input/trigger/value"
	vr::VRInputComponentHandle_t  m_trackpad;		  // "/input/trackpad/click"
	vr::VRInputComponentHandle_t  m_touch;            // "/input/trackpad/touch"
	vr::VRInputComponentHandle_t  m_trackpadx;		  // "/input//trackpad/x"
	vr::VRInputComponentHandle_t  m_trackpady;		  // "/input//trackpad/y"
	vr::VRInputComponentHandle_t  m_out_Haptic;       // "/output/haptic"

	std::string m_sSerialNumber;
	std::string m_sModelNumber;
	ETrackedControllerRole  m_Type;
	bool IsTurnAround;
};

#endif // !_NOLORACKEDDEVICE_H_
