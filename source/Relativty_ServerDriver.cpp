// Copyright (C) 2020  Max Coutte, Gabriel Combe
// Copyright (C) 2020  Relativty.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "openvr_driver.h"

#include "driverlog.h"

#include "Relativty_ServerDriver.hpp"
#include "Relativty_HMDDriver.hpp"
#include "NoloDeviceManager.h"

vr::EVRInitError ServerDriver::Init(vr::IVRDriverContext* DriverContext) {

	vr::EVRInitError eError = vr::InitServerDriverContext(DriverContext);
		if (eError != vr::VRInitError_None) {
			return eError;
	}
	#ifdef DRIVERLOG_H
	InitDriverLog(vr::VRDriverLog());
	DriverLog("Relativty driver version 0.1.1");
	DriverLog("Thread1: HMD quat & pose loop");
	DriverLog("Thread2: update driver pose loop");
	#endif

	this->Log("Relativty Init successful.\n");

	this->HMDDriver = new Relativty::HMDDriver("zero");
	this->m_NoloManager = new NoloDeviceManager(this);
	this->noloLeftController = new NOLOController("1314", TrackedControllerRole_LeftHand);
	this->noloRightController = new NOLOController("1315", TrackedControllerRole_RightHand);
	
	if (m_NoloManager->InitNoloDevice()) {
		this->Log("ZMQ Init successful.\n");
	}

	vr::VRServerDriverHost()->TrackedDeviceAdded(HMDDriver->GetSerialNumber().c_str(), vr::ETrackedDeviceClass::TrackedDeviceClass_HMD, this->HMDDriver);
	vr::VRServerDriverHost()->TrackedDeviceAdded(noloLeftController->GetSerialNumber().c_str(), vr::TrackedDeviceClass_Controller, this->noloLeftController);
	vr::VRServerDriverHost()->TrackedDeviceAdded(noloRightController->GetSerialNumber().c_str(), vr::TrackedDeviceClass_Controller, this->noloRightController);

	// GetSerialNumber() is there for a reason!
	m_bEventThreadRunning = false;
	if (!m_bEventThreadRunning)
	{
		m_bEventThreadRunning = true;
		send_haptic_thread_worker = std::thread::thread(&ServerDriver::Send_haptic_event_thread, this);
		send_haptic_thread_worker.detach();
	}
	return vr::VRInitError_None;
}

void ServerDriver::Cleanup() {
	delete this->HMDDriver;
	delete this->noloLeftController;
	delete this->noloRightController;

	this->HMDDriver = NULL;
	this->noloLeftController = NULL;
	this->noloRightController = NULL;


	#ifdef DRIVERLOG_H
	CleanupDriverLog();
	#endif

	VR_CLEANUP_SERVER_DRIVER_CONTEXT();
}

const char* const* ServerDriver::GetInterfaceVersions() {
	return vr::k_InterfaceVersions;
}

void ServerDriver::RunFrame() {} // if ur not using it don't populate it with garbage!

bool ServerDriver::ShouldBlockStandbyMode() {
	return false;
}

void ServerDriver::EnterStandby() {

}

void ServerDriver::LeaveStandby() {

}

void ServerDriver::Log(std::string log) {
	vr::VRDriverLog()->Log(log.c_str());
}

void ServerDriver::SetNoloConnected(bool bcnnected)
{
	if (HMDDriver != nullptr) {
		HMDDriver->SetNoloConnected(bcnnected);
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

void ServerDriver::UpdateHaptic(VREvent_t& eventHandle)
{
	DriverLog("#=========Type=%d== InDex=%d=========\n", eventHandle.eventType, eventHandle.trackedDeviceIndex);
	if (eventHandle.eventType == VREvent_Input_HapticVibration)
	{
		VREvent_HapticVibration_t data = eventHandle.data.hapticVibration;
		int iAmplitude = ConvertAmplitude(data.fAmplitude);
		if (noloLeftController->GetPropertyContainer() == data.containerHandle)
		{
			NOLOVR::TriggerHapticPulse(NOLOVR::eLeftController, iAmplitude);
		}
		else if (noloRightController->GetPropertyContainer() == data.containerHandle)
		{
			NOLOVR::TriggerHapticPulse(NOLOVR::eRightController, iAmplitude);
		}
	}
}

void ServerDriver::UpdateNoloPose(const NOLOData& newData) {
	if (noloLeftController){
		noloLeftController->UpdatePose(newData.leftData,true);
	}
	if (noloRightController){
		noloRightController->UpdatePose(newData.rightData,false);
	}
}

void ServerDriver::UpdateNoloKey(ENoloDeviceType device, EControlerButtonType type,bool ifPress)
{
	if (device == NOLOVR::eLeftController) {
		noloLeftController->SendButtonUpdate(type, ifPress);
	}
	else if(device == NOLOVR::eRightController){
		noloRightController->SendButtonUpdate(type, ifPress);
	}
}

void ServerDriver::Send_haptic_event_thread()
{
	VREvent_t pEventHandle;
	bool bHasEvent = false;
	while (m_bEventThreadRunning)
	{
		bHasEvent = vr::VRServerDriverHost()->PollNextEvent(&pEventHandle, sizeof(VREvent_t));
		//getEvent
		if (bHasEvent)
		{
			UpdateHaptic(pEventHandle);
		}
		else
		{
			Sleep(3);
		}
		memset(&pEventHandle, 0, sizeof(VREvent_t));
	}
}
