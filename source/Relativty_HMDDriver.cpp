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
	float qres[4];
	ServerDriver::Log("Load OpenHMD Successful!!\n");

	qconj =  NOLOVR::NQuaternion(0, 0, 0, 1);
	qconj = qconj* NOLOVR::NQuaternion(0, 0, 1, 0);
	qconj = qconj * NOLOVR::NQuaternion(0, sin(PI / 8), 0, cos(PI/8));
	
	this->retrieve_quaternion_isOn = true;
	this->retrieve_quaternion_thread_worker = std::thread(&Relativty::HMDDriver::retrieve_device_quaternion_packet_threaded, this);

	return vr::VRInitError_None;
}

void Relativty::HMDDriver::Deactivate() {
	this->retrieve_quaternion_isOn = false;
	this->retrieve_quaternion_thread_worker.join();

	RelativtyDevice::Deactivate();
	//this->update_pose_thread_worker.join();

	ServerDriver::Log("Thread0: all threads exit correctly \n");
}


void Relativty::HMDDriver::calibrate_quaternion() {
	/*if ((0x01 & GetAsyncKeyState(0x52)) != 0) {
		qconj.w.store(quat.w);
		qconj.x.store(-1 * quat.x);
		qconj.y.store(-1 * quat.y);
		qconj.z.store(-1 * quat.z);
	}*/
	float qres[4];
	/*
	qres[0] = qconj.w * quat.w - qconj.x * quat.x - qconj.y * quat.y - qconj.z * quat.z;
	qres[1] = qconj.w * quat.x + qconj.x * quat.w + qconj.y * quat.z - qconj.z * quat.y;
	qres[2] = qconj.w * quat.y - qconj.x * quat.z + qconj.y * quat.w + qconj.z * quat.x;
	qres[3] = qconj.w * quat.z + qconj.x * quat.y - qconj.y * quat.x + qconj.z * quat.w;
	*/
	this->quat.w = qres[0];
	this->quat.x = qres[1];
	this->quat.y = qres[2];
	this->quat.z = qres[3];
}

void Relativty::HMDDriver::retrieve_device_quaternion_packet_threaded() {//这个函数用来接收四元数
	ServerDriver::Log("Opened thread 1 successful!\n");
	bool useNoloRot = false;
	HMD HMDData;
	float qres[4]={};
	while (this->retrieve_quaternion_isOn) {
		if (!useNoloRot) {
			ohmd_ctx_update(ctx);
			ohmd_device_getf(HMDRot, OHMD_ROTATION_QUAT, qres);
			this->quat.w = -1 * qres[0];
			this->quat.x = qres[3];
			this->quat.y = -1 * qres[2];
			this->quat.z = qres[1];
		}
		else {
			HMDData = NOLOVR::GetHMDData();
			this->quat = HMDData.HMDRotation;
			quat.x = -quat.x;
			quat.y = -quat.y;
			quat = quat * NQuaternion(1, 0, 0, 0);
		}

		quat = quat * qconj;


		m_Pose.qRotation.x = this->quat.x;
		m_Pose.qRotation.y = this->quat.y;
		m_Pose.qRotation.z = this->quat.z;
		m_Pose.qRotation.w = this->quat.w;
		
		nolo_HMD_data = GetNoloData();
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
	ServerDriver::Log("Thread1: successfully stopped\n");
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
		m_hmdTurnBackPos.x = -m_hmdTurnBackPos.x;

		DriverLog("==SetNoloHmdData==%f,%f,%f=\n", tempHmd.HMDPosition.x, tempHmd.HMDPosition.y, tempHmd.HMDPosition.z);
	}
	m_IsTurnAround = !m_IsTurnAround;
	ServerDriver::Log("Turned Around!\n");
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
	m_sBindPath = "{Relativty}/input/vive_hmd_profile.json";

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
