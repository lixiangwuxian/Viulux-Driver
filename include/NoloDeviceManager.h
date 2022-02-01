#pragma once

#include "NoloClientLib.h"
#pragma comment(lib,"NoloClientLib.lib")
using namespace NOLOVR;


class ServerDriver;
class NoloDeviceManager:public INOLOZMQEvent
{
public:
	NoloDeviceManager(ServerDriver *server);
	~NoloDeviceManager();
	bool InitNoloDevice();

protected:	
	// Í¨¹ý INOLOZQMEvent ¼Ì³Ð
	virtual void OnZMQConnected() override;
	virtual void OnZMQDisConnected() override;
	virtual void OnKeyDoubleClicked(ENoloDeviceType DevType, UCHAR Keys)override;
	virtual void OnNewData(const NOLOData & _noloData) override;
	virtual void OnButtonPressed(ENoloDeviceType device, EControlerButtonType type) override;
	virtual void OnButtonRelease(ENoloDeviceType device, EControlerButtonType type) override;
private:
	ServerDriver *m_Server;
};

