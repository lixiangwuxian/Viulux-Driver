#include "NoloDeviceManager.h"

#include "ServerDriver.h"

NoloDeviceManager::NoloDeviceManager(CServerDriver *server)
{
	m_Server = server;
}


NoloDeviceManager::~NoloDeviceManager() 
{

}

void NoloDeviceManager::InitNoloDevice()
{
	//StartNoloServer();
	SetEventListener(this);
	OpenNoloZeroMQ();
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
		HMD TempHmdData = TempData.hmdData;
		if (leftController.Buttons & EControlerButtonType::eSystemBtn)
		{
			m_Server->RecenterHmd(TempHmdData, leftController);
		}
		else if (rightController.Buttons & EControlerButtonType::eSystemBtn)
		{
			m_Server->RecenterHmd(TempHmdData, rightController);
		}
	}
	if (KeyType == EControlerButtonType::eMenuBtn)
	{
		m_Server->TurnArroundHmd();
	}

}

void NoloDeviceManager::OnNewData(const NOLOData & _noloData)
{
	//NQuaternion tempQuat = _noloData.hmdData.HMDRotation;
	m_Server->UpdatePose(_noloData);
}

