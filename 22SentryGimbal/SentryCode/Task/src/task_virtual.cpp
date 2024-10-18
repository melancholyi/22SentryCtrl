#include "task_virtual.h"
//1�����񡢶��󶼲�����ɾ��
//2��ֻ�����������洴��
//3��������Ҫ���ù��ڸ�����Ĺ��𡢻ָ�������������ع��𡢻ָ���

int VirtualTask::taskNum = 0;
VirtualTask **VirtualTask::taskList = 0;
VirtualTask::VirtualTask():taskHandler(NULL)
{
	if(taskList == 0)
	{
		taskList = (VirtualTask **)malloc(sizeof(VirtualTask *));
	}
	else
	{
		taskList = (VirtualTask **)realloc(taskList, sizeof(VirtualTask *) * taskNum+1);
	}
	
	taskList[taskNum] = this;
	taskNum+=1;
}

void VirtualTask::setTaskHandler(TaskHandle_t taskHandler)
{
	this->taskHandler = taskHandler;
	if (taskHandler == NULL)
	{
		this->taskHandler = xTaskGetCurrentTaskHandle();
	}
}

void VirtualTask::remoteCtrlUpdate()
{

}

void VirtualTask::deforceCallBack()
{
	if (taskHandler != NULL) //����һ���ǰ����
		vTaskSuspend(taskHandler);
}

void VirtualTask::deforceCancelCallBack()
{
	if (taskHandler != NULL) //����һ���ǰ����
		vTaskResume(taskHandler);
}