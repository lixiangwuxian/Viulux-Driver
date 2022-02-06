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

#pragma comment(lib, "Ws2_32.lib")
#pragma comment (lib, "Setupapi.lib")
#pragma comment(lib, "User32.lib")

#define PI 3.1415926535897932384626433832

#include <atomic>
#include <WinSock2.h>
#include <Windows.h>
#include "hidapi/hidapi.h"
#include "openvr_driver.h"

#include "driverlog.h"

#include "openhmd.h"

#include "NoloClientLib.h"

#include "Relativty_HMDDriver.hpp"
#include "Relativty_ServerDriver.hpp"
#include "Relativty_components.h"
#include "Relativty_base_device.h"


#include <string>

typedef union {
	struct {
		float x, y, z, w;
	};
	float arr[4];
} quatf;

inline vr::HmdQuaternion_t HmdQuaternion_Init(double w, double x, double y, double z) {
	vr::HmdQuaternion_t quat;
	quat.w = w;
	quat.x = x;
	quat.y = y;
	quat.z = z;
	return quat;
}

inline void Normalize(float norma[3], float v[3], float max[3], float min[3], int up, int down, float scale[3], float offset[3]) {
	for (int i = 0; i < 4; i++) {
		norma[i] = (((up - down) * ((v[i] - min[i]) / (max[i] - min[i])) + down) / scale[i])+ offset[i];
	}
}

vr::EVRInitError Relativty::HMDDriver::Activate(uint32_t unObjectId) {
	RelativtyDevice::Activate(unObjectId);
	this->setProperties();
	ctx = ohmd_ctx_create();
	int num_devices = ohmd_ctx_probe(ctx);
	HMDRot= ohmd_list_open_device(ctx, 0);
	ServerDriver::Log("Load OpenHMD Successful!!\n");
	this->retrieve_quaternion_isOn = true;
	this->retrieve_quaternion_thread_worker = std::thread(&Relativty::HMDDriver::retrieve_device_quaternion_packet_threaded, this);
	return vr::VRInitError_None;
}

void Relativty::HMDDriver::Deactivate() {
	this->retrieve_quaternion_isOn = false;
	this->retrieve_quaternion_thread_worker.join();

	RelativtyDevice::Deactivate();

	ServerDriver::Log("Thread0: all threads exit correctly \n");
}

void Relativty::HMDDriver::retrieve_device_quaternion_packet_threaded() {//�����������������Ԫ��
	bool useNoloRot = false;
	HMD HMDData;
	float qres[4]={};
	while (this->retrieve_quaternion_isOn) {
		/*ohmd_ctx_update(ctx);
		ohmd_device_getf(HMDRot, OHMD_ROTATION_QUAT, qres);
		this->quat.w = -1 * qres[0];
		this->quat.x = qres[3];
		this->quat.y = -1 * qres[2];
		this->quat.z = qres[1];
		quat = quat * qconj;*/

		nolo_HMD_data = GetNoloData();

		this->quat = nolo_HMD_data.hmdData.HMDRotation;
		this->quat.x = -this->quat.x;
		this->quat.y = -this->quat.y;
		quat = quat * qconj*NQuaternion(0,1,0,0);

		/*if (m_BRecentering) {
			NQuaternion HMDRot(this->quat);
			NVector3 HMDEula = HMDRot.GetEulerAngle();
			NVector3 HMDPos = m_HmdRecenterData.HMDPosition;
			NVector3 CtrPos = m_RecenterContoller.Position;
			if ((nolo_HMD_data.leftData.Position-nolo_HMD_data.rightData.Position).length()<0.2) {
				CtrPos = (nolo_HMD_data.leftData.Position - nolo_HMD_data.rightData.Position) / 2.0;
			}
			float DetaX = HMDPos.x - CtrPos.x;
			float DetaZ = HMDPos.z - CtrPos.z;
			float RotAngle = HMDEula.x + atan(DetaX/DetaZ);
			if (DetaZ > 0) {
				RotAngle += PI;
			}
			if (m_IsTurnAround) {
				RotAngle += PI;
			}
			//NVector3 CtrEula = CtrRot.GetEulerAngle();
			//if()
			DriverLog("HMDEula y:%f,RotAngle y:%f\n", HMDEula.x, RotAngle);
			qOffset =  NQuaternion(0, -sin(RotAngle/2), 0, cos(RotAngle/2));
			qOffset = qOffset * NQuaternion(0, 1, 0, 0);
			m_BRecentering = false;
		}


		quat = quat * qOffset;*/

		m_Pose.qRotation.x = this->quat.x;
		m_Pose.qRotation.y = this->quat.y;
		m_Pose.qRotation.z = this->quat.z;
		m_Pose.qRotation.w = this->quat.w;
		
		this->vector_xyz.x = -nolo_HMD_data.hmdData.HMDPosition.x;
		this->vector_xyz.y = nolo_HMD_data.hmdData.HMDPosition.y;
		this->vector_xyz.z = nolo_HMD_data.hmdData.HMDPosition.z;
		
		if (m_IsTurnAround) {
			vector_xyz.x = 2 * m_hmdTurnBackPos.x - vector_xyz.x;
			vector_xyz.z = 2 * m_hmdTurnBackPos.z - vector_xyz.z;
		}

		

		m_Pose.vecPosition[0] = this->vector_xyz.x;
		m_Pose.vecPosition[1] = this->vector_xyz.y;
		m_Pose.vecPosition[2] = this->vector_xyz.z;


		vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_unObjectId, m_Pose, sizeof(vr::DriverPose_t));

		Sleep(3);
	}
}

void Relativty::HMDDriver::SetNoloConnected(bool bcnnected)
{
	m_BNoloConnected = bcnnected;
}

void Relativty::HMDDriver::TurnAround()
{
	if (m_IsTurnAround == false)
	{
		HMD tempHmd;
		tempHmd=NOLOVR::GetHMDData();
		m_hmdTurnBackPos = tempHmd.HMDPosition;

		DriverLog("==SetNoloHmdData==%f,%f,%f=\n", tempHmd.HMDPosition.x, tempHmd.HMDPosition.y, tempHmd.HMDPosition.z);
	}
	m_IsTurnAround = !m_IsTurnAround;
	ServerDriver::Log("Turned Around!\n");
	qconj = qconj * NOLOVR::NQuaternion(0, 1, 0, 0);
}

void Relativty::HMDDriver::RecenterHMD(const HMD& HmdData, const Controller& CtrData)
{
	m_HmdRecenterData = HmdData;
	m_RecenterContoller = CtrData;
	m_BRecentering = true;
}

Relativty::HMDDriver::HMDDriver(std::string myserial):RelativtyDevice(myserial, "akira_") {

	// keys for use with the settings API
	static const char* const Relativty_hmd_section = "Relativty_hmd";

	// openvr api stuff
	m_sRenderModelPath = "{Relativty}/rendermodels/generic_hmd";
	m_sBindPath = "{Relativty}/input/relativty_hmd_profile.json";

	m_spExtDisplayComp = std::make_shared<Relativty::RelativtyExtendedDisplayComponent>();

	// not openvr api stuff
	ServerDriver::Log("Loading Settings\n");
	this->IPD = vr::VRSettings()->GetFloat(Relativty_hmd_section, "IPDmeters");
	this->SecondsFromVsyncToPhotons = vr::VRSettings()->GetFloat(Relativty_hmd_section, "secondsFromVsyncToPhotons");
	this->DisplayFrequency = vr::VRSettings()->GetFloat(Relativty_hmd_section, "displayFrequency");

	// this is a bad idea, this should be set by the tracking loop
	m_Pose.result = vr::TrackingResult_Running_OK;
}

inline void Relativty::HMDDriver::setProperties() {
	vr::VRProperties()->SetFloatProperty(m_ulPropertyContainer, vr::Prop_UserIpdMeters_Float, this->IPD);
	vr::VRProperties()->SetFloatProperty(m_ulPropertyContainer, vr::Prop_UserHeadToEyeDepthMeters_Float, 0.16f);
	vr::VRProperties()->SetFloatProperty(m_ulPropertyContainer, vr::Prop_DisplayFrequency_Float, this->DisplayFrequency);
	vr::VRProperties()->SetFloatProperty(m_ulPropertyContainer, vr::Prop_SecondsFromVsyncToPhotons_Float, this->SecondsFromVsyncToPhotons);

	// avoid "not fullscreen" warnings from vrmonitor
	vr::VRProperties()->SetBoolProperty(m_ulPropertyContainer, vr::Prop_IsOnDesktop_Bool, false);
}
