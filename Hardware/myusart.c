#include "stm32f10x.h"

//串口3
#define USART3_GPIO_PORT      GPIOB
#define USART3_GPIO_CLK       RCC_APB2Periph_GPIOB
#define USART3_TX_GPIO_PIN    GPIO_Pin_10
#define USART3_RX_GPIO_PIN    GPIO_Pin_11
extern uint32_t uiBuad;
void My_USART1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStrue;
	USART_InitTypeDef USART_InitStrue;
	NVIC_InitTypeDef NVIC_InitStrue;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//GPIO端口使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//串口端口使能

	GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStrue.GPIO_Pin=GPIO_Pin_9;
	GPIO_InitStrue.GPIO_Speed=GPIO_Speed_10MHz;
	GPIO_Init(GPIOA,&GPIO_InitStrue);

	GPIO_InitStrue.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStrue.GPIO_Pin=GPIO_Pin_10;
	GPIO_InitStrue.GPIO_Speed=GPIO_Speed_10MHz;
	GPIO_Init(GPIOA,&GPIO_InitStrue);

	USART_InitStrue.USART_BaudRate=uiBuad;
	USART_InitStrue.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStrue.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStrue.USART_Parity=USART_Parity_No;
	USART_InitStrue.USART_StopBits=USART_StopBits_1;
	USART_InitStrue.USART_WordLength=USART_WordLength_8b;

	USART_Init(USART1,&USART_InitStrue);

	USART_Cmd(USART1,ENABLE);//使能串口1

	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//开启接收中断

	NVIC_InitStrue.NVIC_IRQChannel=USART1_IRQn;
	NVIC_InitStrue.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStrue.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStrue.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitStrue);
}
void My_USART3_Init(void)
{
	GPIO_InitTypeDef GPIO_Init_Structure;                            //定义GPIO结构体
    USART_InitTypeDef USART_Init_Structure;                          //定义串口结构体
  	NVIC_InitTypeDef  NVIC_Init_Structure;														//定义中断结构体
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
    RCC_APB2PeriphClockCmd(USART3_GPIO_CLK,  ENABLE);                 //开启GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,  ENABLE);            //开启APB2总线复用时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,  ENABLE);          //开启USART1时钟
    
    //配置PB10 TX
    GPIO_Init_Structure.GPIO_Mode  = GPIO_Mode_AF_PP;                //复用推挽
    GPIO_Init_Structure.GPIO_Pin   = USART3_TX_GPIO_PIN;
    GPIO_Init_Structure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(USART3_GPIO_PORT, &GPIO_Init_Structure);
    
    //配置PB11 RX
    GPIO_Init_Structure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;                //复用推挽
    GPIO_Init_Structure.GPIO_Pin   = USART3_RX_GPIO_PIN;
    GPIO_Init( USART3_GPIO_PORT, &GPIO_Init_Structure);
	
    USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);	
    USART_Init_Structure.USART_BaudRate = 460800;                                          //波特率设置为115200
    USART_Init_Structure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;       //硬件流控制为无
    USART_Init_Structure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;                       //模式设为收和发
    USART_Init_Structure.USART_Parity = USART_Parity_No;                                   //无校验位
    USART_Init_Structure.USART_StopBits = USART_StopBits_1;                                //一位停止位
    USART_Init_Structure.USART_WordLength = USART_WordLength_8b;                           //字长为8位   
    USART_Init(USART3, &USART_Init_Structure);   
    USART_Cmd(USART3, ENABLE);
		
		//中断结构体配置
	NVIC_Init_Structure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_Init_Structure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init_Structure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_Init_Structure.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&NVIC_Init_Structure);

}
