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

#pragma once
#include <thread>
#include <atomic>
#include <WinSock2.h>
#include "hidapi/hidapi.h"
#include "openvr_driver.h"
#include "Relativty_components.h"
#include "Relativty_base_device.h"

#include "NoloClientLib.h"
#include "openhmd.h"

namespace Relativty {
	class HMDDriver : public RelativtyDevice<false>
	{
	public:
		HMDDriver(std::string myserial);
		~HMDDriver() = default;

		void frameUpdate();
		inline void setProperties();

		// Inherited from RelativtyDevice, to be overridden
		virtual vr::EVRInitError Activate(uint32_t unObjectId);
		virtual void Deactivate();
		void SetNoloConnected(bool bcnnected);
		void TurnAround();
		void RecenterHMD(const NOLOVR::HMD& HmdData, const NOLOVR::Controller& CtrData);
		void retrieve_device_quaternion_packet_threaded();

	private:

		NOLOVR::NOLOData nolo_HMD_data;

		ohmd_device *HMDRot;
		ohmd_context* ctx;

		NOLOVR::HMD  m_HmdRecenterData;
		NOLOVR::Controller m_RecenterContoller;
		bool m_BRecentering;

		bool m_BNoloConnected;
		bool m_IsTurnAround;
		NOLOVR::NVector3 m_hmdTurnBackPos;

		float SecondsFromVsyncToPhotons;
		float DisplayFrequency;
		float IPD;
		float HeadToEyeDepth;

		vr::DriverPose_t lastPose = {0};

		//std::atomic<float> quat[4];
		NOLOVR::NQuaternion quat;
		NOLOVR::NQuaternion qOffset={0,0,0,1};

		std::atomic<bool> retrieve_quaternion_isOn = false;

		NOLOVR::NQuaternion qconj={0,0,0,1};

		

		//void retrieve_device_quaternion_packet_threaded();

		NOLOVR::NVector3 vector_xyz;
		std::thread retrieve_quaternion_thread_worker;
		//std::atomic<float> vector_xyz[3];
	};
}