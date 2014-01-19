#include "operation.h"

void (*operation)(void) = 0;

_Button Button = {0, 0, BN_NONE, BN_NONE, BN_NONE};

void operation_scan(void)
{
	Button.Pre = Button.Cur;
	//Button.Cur = ~P0;
	if(Button.Cur != BN_NONE && Button.Cur == Button.Pre)
	{
		Button.SlowDown++;
		if(Button.KeepTime < 255)
			Button.KeepTime++;
	}
	else
	{
		Button.KeepTime = 0;
		Button.SlowDown = 0;
	}
}

void operation_init(void)
{
	operation = demo_operation;
}

void demo_operation(void)
{
	operation_scan();
	if(Button.Cur == (BN_BTN7 | BN_BTN8)) //���ȼ���ߵ�ȫ����ϼ�
	{
		if(Button.KeepTime == 2*8)
		{
			//BN_BTN7+BN_BTN8����2���ִ��
		}
		return;
	}
	Button.Tmp = Button.Cur;
	//if(��������״̬����ĳЩ����)
	//{
		Button.Tmp &= ~(BN_BTN5 | BN_BTN6 | BN_BTN7 | BN_BTN8); //������BN_BTN5��BN_BTN6��BN_BTN7��BN_BTN8��
	//}
	switch(Button.Tmp)
	{
		//�ް���
		case BN_NONE:
			if(Button.Pre == BN_BTN1)
			{
				//BN_BTN1�ɿ�
			}
			break;
		//BN_BTN1
		case BN_BTN1:
			if(Button.Pre == BN_NONE)
			{
				//BN_BTN1����
			}
			else if(Button.Pre == BN_BTN1 && Button.KeepTime == 1*8)
			{
				//BN_BTN1������1��
			}
			break;
		//BN_BTN2
		case BN_BTN2:
			if(Button.Pre == BN_NONE)
			{
				//BN_BTN2����
			}
			else if(Button.Pre == BN_BTN1 && Button.SlowDown >= 0.5*8)
			{
				Button.SlowDown = 0;
				//BN_BTN2����ʱÿ0.5��ִ��һ��
			}
			break;
	}
}

