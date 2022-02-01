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
#ifndef RELATIVTY_SERVERDRIVER_H
#define RELATIVTY_SERVERDRIVER_H

#include "openvr_driver.h"
#include "Relativty_HMDDriver.hpp"
#include <NOLOController.h>

class NoloDeviceManager;

class ServerDriver : public vr::IServerTrackedDeviceProvider
{
public:
	virtual vr::EVRInitError Init(vr::IVRDriverContext* DriverContext) override;
	virtual void Cleanup() override;
	virtual const char* const* GetInterfaceVersions() override;
	virtual void RunFrame() override;
	virtual bool ShouldBlockStandbyMode() override;
	virtual void EnterStandby() override;
	virtual void LeaveStandby() override;

	static void Log(std::string log);

	void SetNoloConnected(bool bcnnected);

	void UpdateNoloPose(const NOLOData& newData);
	void UpdateHaptic(VREvent_t& eventHandle);
	void UpdateNoloKey(ENoloDeviceType device, EControlerButtonType type,bool ifPress);
	void TurnAroundHMD();
	void RecenterHMD(const HMD& HmdData, const Controller& CtrData);
private:
	Relativty::HMDDriver* HMDDriver = nullptr;
	NoloDeviceManager* m_NoloManager;
	NOLOController* noloLeftController;//◊Û ÷
	NOLOController* noloRightController;//”“ ÷

	
	bool m_bEventThreadRunning;
	std::thread send_haptic_thread_worker;
	void Send_haptic_event_thread();
};

#endif // RELATIVTY_SERVERDRIVER_H