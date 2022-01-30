#pragma once
#include "myopenvr.h"


class CClientDriver : public IVRWatchdogProvider
{
public:
	CClientDriver();
	~CClientDriver();
	virtual EVRInitError Init(vr::IVRDriverContext *pDriverContext);
	virtual void Cleanup();
private:
	std::thread *m_pWatchdogThread;
};

