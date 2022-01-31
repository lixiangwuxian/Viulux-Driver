#include <NoloDeviceManager.h>
#include "Relativty_ServerDriver.hpp"

NoloDeviceManager::NoloDeviceManager(ServerDriver *server)
{
	m_Server = server;
}


NoloDeviceManager::~NoloDeviceManager() 
{

}

bool NoloDeviceManager::InitNoloDevice()
{
	StartNoloServer();
	NOLOVR::SetEventListener(this);
	return NOLOVR::OpenNoloZeroMQ();
}

void NoloDeviceManager::OnZMQConnected()
{
	SetHmdCenter(NVector3(0.0f, 0.09f, 0.07f));


	m_Server->SetNoloConnected(true);
}

void NoloDeviceManager::OnZMQDisConnected()
{
	m_Server->SetNoloConnected(false);
}

void NoloDeviceManager::OnKeyDoubleClicked(EControlerButtonType KeyType)
{
	NOLOData TempData = GetNoloData();
	if (KeyType == EControlerButtonType::eSystemBtn)
	{
		Controller leftController = TempData.leftData;
		Controller rightController = TempData.rightData;
		if (leftController.Buttons & EControlerButtonType::eSystemBtn)
		{
			//m_Server->RecenterHmd(TempHmdData, leftController);
		}
		else if (rightController.Buttons & EControlerButtonType::eSystemBtn)
		{
			//m_Server->RecenterHmd(TempHmdData, rightController);
		}
	}
	if (KeyType == EControlerButtonType::eMenuBtn)
	{
		//m_Server->TurnArroundHmd();
	}

}

void NoloDeviceManager::OnNewData(const NOLOData & _noloData)
{
	//NQuaternion tempQuat = _noloData.hmdData.HMDRotation;
	m_Server->UpdateNoloPose(_noloData);
}

void NoloDeviceManager::OnButtonPressed(ENoloDeviceType device, EControlerButtonType type)
{

	m_Server->UpdateNoloKey(device,type,true);
}

void NoloDeviceManager::OnButtonRelease(ENoloDeviceType device, EControlerButtonType type)
{
	m_Server->UpdateNoloKey(device,type,false);
}

void NoloDeviceManager::OnKeyDoubleClicked(ENoloDeviceType device, UCHAR Keys)
{
}
