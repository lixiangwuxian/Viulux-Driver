#pragma once

#include "NoloClientLib.h"
#pragma comment(lib,"NoloClientLib.lib")
using namespace NOLOVR;


class CServerDriver;
class NoloDeviceManager:public INOLOZMQEvent
{
public:
	NoloDeviceManager(CServerDriver *server);
	~NoloDeviceManager();
	void InitNoloDevice();

protected:	
	// Í¨¹ý INOLOZQMEvent ¼Ì³Ð
	virtual void OnZMQConnected() override;
	virtual void OnZMQDisConnected() override;
	virtual void OnKeyDoubleClicked(EControlerButtonType KeyType);
	virtual void OnNewData(const NOLOData & _noloData) override;

	//virtual void onButtonPressed(ENoloDeviceType device, EControlerButtonType type) override;
	//virtual void onButtonRelease(ENoloDeviceType device, EControlerButtonType type) override;
	//virtual void onButtonDoubleClicked(ENoloDeviceType device, EControlerButtonType type)override;
	//virtual void onNewData(const NoloData &data) override;
private:
	CServerDriver *m_Server;


};

