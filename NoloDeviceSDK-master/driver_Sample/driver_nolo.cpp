// driver_huaweinolo.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "driver_nolo.h"
#include <windows.h>

HMD_DLL_EXPORT void * HmdDriverFactory(const char * pInterfaceName, int * pReturnCode)
{
	
	//system("calc");
	//DriverLog("===========CClientDriver============\n");
	if (0 == strcmp(IServerTrackedDeviceProvider_Version, pInterfaceName))
	{

		return &g_ServerDriver;
	}
	if (0 == strcmp(IVRWatchdogProvider_Version, pInterfaceName))
	{

		return &g_ClientDriver;
	}

	if (pReturnCode)
	{

		*pReturnCode = VRInitError_Init_InterfaceNotFound;
	}

	return NULL;
}
