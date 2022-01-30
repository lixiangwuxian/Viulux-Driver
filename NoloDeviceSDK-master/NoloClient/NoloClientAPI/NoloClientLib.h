
#pragma once
#ifndef _ZMQRECEIVELIB_H_
#define _ZMQRECEIVELIB_H_

#include "Nolo_DeviceType.h"
//Disable ������UDT��C����
#pragma warning(disable:4190)

namespace NOLOVR
{

extern "C"
{
	/*************************************************
	��������: ����NoloServer
	�����б�:
			  @ StrServerPath �����롿 ָ�� NoloServer.exe ·�������û��ָ��·����
			  Ĭ�ϵ�NoloClientLib.dll ͬ��Ŀ¼��NoloServerĿ¼�²���
	�� �� ֵ: �������Server�ɹ�������true,���򷵻�false
	����˵��: ��
	*************************************************/
	NOLO_API bool __cdecl StartNoloServer(const wchar_t *StrServerPath = L"");

	/*************************************************
	��������: �����¼������ӿ���ָ��,C++ ���Ե���
	�����б�:
			  @ Listener  �����롿 �ӽӿ�INOLOZQMEvent�������� ʵ��ָ��
	�� �� ֵ: void
	����˵��: ��
	*************************************************/
	NOLO_API void __cdecl SetEventListener(INOLOZMQEvent *Listener);

	/*************************************************
	��������: �����Ƿ�ʹ������ģʽ��
	�����б�:
		  @ bCellingMode  �����롿 true Ϊ ����ģʽ�� false Ϊƽ��ģʽ
	�� �� ֵ: void
	����˵��: �˺�������ҪZMQ�������Ӻ󣬵��òŻ���Ч���Ƽ��ڻص�����OnZMQConnected()�е���
			  ͨ������£�������ͻ�ʹ������ģʽ���Ƽ�C++������ʹ�ñ��ӿ�
	*************************************************/
	NOLO_API void __cdecl SetBCellingMode(bool bCellingMode);

	/*************************************************
	��������: ���ûص� ��SetEventListener,����ѡһ,���ɻ���
	�����б�:
			  @ callBackType  �����롿 �ص�����
			  @ pCallBackFun  �����롿 ����ָ��
	�� �� ֵ: void
	����˵��: �Ƽ�C#ʹ�ñ��ӿ�
	*************************************************/
	NOLO_API void __cdecl RegisterCallBack(EClientCallBackTypes callBackType, void *pCallBackFun);

	/*************************************************
	��������: ͷ����ת����ת������
	�����б�:
			  @ hmdCenter �����롿 �� [˫���е�] �� [ͷ����λ��] ��λ������ֵ
	�� �� ֵ: void
	����˵��: ����Ҫʹ��ͷ��λ��ʱ��Ӧ�����ø�ֵ����ͷ��λ��ת����
			  ��������ϵ��x������: �ң� y������: �ϣ�z������ǰ
			  ʾ��ֵ�� (0.00f,0.08f,0.08f)
	*************************************************/
	NOLO_API void __cdecl SetHmdCenter(const NVector3 &hmdCenter);

	/*************************************************
	��������: ��ZMQ����
	�����б�: ��
	�� �� ֵ: �򿪳ɹ����� true,ʧ���򷵻� false
	����˵��: ������ô˺�����SDK�Ż����Nolo���ݣ�ǰ����NoloServer
			  ���� NOLOHOME �Ѿ���
	*************************************************/
	NOLO_API bool __cdecl OpenNoloZeroMQ();

	/*************************************************
	��������: �ر�ZMQ����
	�����б�: ��
	�� �� ֵ: void
	����˵��: ��
	*************************************************/
	NOLO_API void __cdecl CloseNoloZeroMQ();

	/*************************************************
	��������: ��NOLO�ֱ�����������
	�����б�:
			  @ deviceType �����롿 �ֱ����ͣ���ЧֵΪ��eLeftController,eRightController
			  @ intensity  �����롿 ��ǿ�ȣ�������ǿ�� 50 - 100
	�� �� ֵ: ��
	����˵��: һ�ε�����һ֡��ʱ����Լ16ms ��
	*************************************************/
	NOLO_API void __cdecl TriggerHapticPulse(ENoloDeviceType deviceType, int intensity);

	/*************************************************
	��������: ��ȡ���µ�NOLO���ֱ�����
	�����б�: ��
	�� �� ֵ: @ Controller
	����˵��: ��
	*************************************************/
	NOLO_API Controller __cdecl GetLeftControllerData();

	/*************************************************
	��������: ��ȡ���µ�NOLO���ֱ�����
	�����б�: ��
	�� �� ֵ: @ Controller
	����˵��: ��
	*************************************************/
	NOLO_API Controller __cdecl GetRightControllerData();

	/*************************************************
	��������: ��ȡ���µ�NOLOͷ����λ������
	�����б�: ��
	�� �� ֵ: @ HMD
	����˵��: ��
	*************************************************/
	NOLO_API HMD        __cdecl GetHMDData();

	/*************************************************
	��������: ��ȡ���µ�NOLOͷ����λ������
	�����б�: ��
	�� �� ֵ: @ NOLOData
	����˵��: ��
	*************************************************/
	NOLO_API NOLOData   __cdecl GetNoloData();

	/*************************************************
	��������: ��Server�˷�������
	�����б�: @CmdContent ��һ��json�ַ������� "{Type:\"ShowPsvr\",Msg:\"OK\"}"��������ͻ����̶�
				����ַ�����Ŀ 60
	�� �� ֵ: @ NOLOData
	����˵��: ��
	*************************************************/
	NOLO_API void   __cdecl SendUIComand(const char *strCmdContent);

}

}
#endif // !_ZMQRECEIVELIB_H_