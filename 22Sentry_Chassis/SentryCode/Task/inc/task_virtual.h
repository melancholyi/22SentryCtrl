#ifndef __TASK_VIRTUAL_H
#define __TASK_VIRTUAL_H
/**FreeRTOS*START***************/
#include "FreeRTOS.h" //FreeRTOSʹ��
#include "timers.h"
#include "list.h"
#include "queue.h"
#include "task.h"
/**FreeRTOS*END***************/

class VirtualTask
{
	public:
	//���캯��
	VirtualTask();

	void setTaskHandler(TaskHandle_t taskHandler = NULL);

	//����д��ң�����ݸ���ʱ����
	virtual void remoteCtrlUpdate();

	//����
	virtual void deforceCallBack();
	//ȡ������
	virtual void deforceCancelCallBack();
		
	//�����б�
	static VirtualTask **taskList;
	//��������
	static int taskNum;
	
private:
	TaskHandle_t taskHandler;

};

#endif
