#pragma once

#include <windows.h>
#include <math.h>
#include <thread>
#include <openvr_driver.h>
#include "NoloClientLib.h"
#include "driver_log.h"

using namespace vr;

inline HmdQuaternion_t HmdQuaternion_Init(double w, double x, double y, double z)
{
	HmdQuaternion_t quat;
	quat.w = w;
	quat.x = x;
	quat.y = y;
	quat.z = z;
	return quat;
}
//华为接口函数
#pragma comment(lib,"..\\x64\\release\\Hmd.lib")
#pragma comment(lib,"..\\x64\\release\\nvapi64.lib")
__declspec(dllimport) void setPCMode();
__declspec(dllimport) IVRDisplayComponent *  createHmdDriver();
__declspec(dllimport) void   releaseHmdDriver(IVRDisplayComponent * display);
__declspec(dllimport) DriverPose_t getHWVRPose();
__declspec(dllimport) void resetPose();
__declspec(dllimport) void leftRudder();
__declspec(dllimport) void rightRudder();
__declspec(dllimport) bool enableHmdImu();
__declspec(dllimport) void disableHmdImu();