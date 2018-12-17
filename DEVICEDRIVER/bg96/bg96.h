#ifndef __BG96_H
#define __BG96_H

#include "sys.h"
#include "ringbuf.h"


#define BG96_PWRKEY_HIGH	GPIO_SetBits(GPIOC,GPIO_Pin_1)
#define BG96_PWRKEY_LOW		GPIO_ResetBits(GPIOC,GPIO_Pin_1)

#define BG96_RST_HIGH		GPIO_SetBits(GPIOC,GPIO_Pin_2)
#define BG96_RST_LOW		GPIO_ResetBits(GPIOC,GPIO_Pin_2)

#define BG96_PWREN_HIGH		GPIO_SetBits(GPIOB,GPIO_Pin_12)
#define BG96_PWREN_LOW		GPIO_ResetBits(GPIOB,GPIO_Pin_12)

#define READ_BG96_STATUS  	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)
#define READ_BG96_NET  		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)


#define BG96_PRINTF_RX_BUF

#define CMD_DATA_BUFFER_SIZE 256
#define NET_DATA_BUFFER_SIZE 1500

#define TIMEOUT_1S 1000
#define TIMEOUT_2S 2000
#define TIMEOUT_5S 5000
#define TIMEOUT_6S 6000
#define TIMEOUT_7S 7000
#define TIMEOUT_10S 10000
#define TIMEOUT_11S 10000
#define TIMEOUT_12S 10000
#define TIMEOUT_13S 10000
#define TIMEOUT_14S 10000
#define TIMEOUT_15S 15000
#define TIMEOUT_20S 20000
#define TIMEOUT_25S 25000
#define TIMEOUT_30S 30000
#define TIMEOUT_35S 35000
#define TIMEOUT_40S 40000
#define TIMEOUT_45S 45000
#define TIMEOUT_50S 50000
#define TIMEOUT_55S 55000
#define TIMEOUT_60S 60000
#define TIMEOUT_65S 65000
#define TIMEOUT_70S 70000
#define TIMEOUT_75S 75000
#define TIMEOUT_80S 80000
#define TIMEOUT_85S 85000
#define TIMEOUT_90S 90000
#define TIMEOUT_95S 95000
#define TIMEOUT_100S 100000
#define TIMEOUT_105S 105000
#define TIMEOUT_110S 110000
#define TIMEOUT_115S 115000
#define TIMEOUT_120S 120000
#define TIMEOUT_125S 125000
#define TIMEOUT_130S 130000
#define TIMEOUT_135S 135000
#define TIMEOUT_140S 140000
#define TIMEOUT_145S 145000
#define TIMEOUT_150S 150000
#define TIMEOUT_155S 155000
#define TIMEOUT_160S 160000
#define TIMEOUT_165S 165000
#define TIMEOUT_170S 170000
#define TIMEOUT_175S 175000
#define TIMEOUT_180S 180000

#define IMEI_LEN	15

#define NET_NULL 0x00000000

#define ESP_PRINTF_RX_BUF

//接收数据的状态
typedef enum
{
    WAITING = 0,
    RECEIVING = 1,
    RECEIVED  = 2,
    TIMEOUT = 3
} CMD_STATE_E;

//GPRS当前的模式
typedef enum
{
    NET_MODE = 0,
    CMD_MODE = 1,
    TRANSPARENT_MODE = 2
} GPRS_MODE_E;

typedef enum
{
	NEED_PLUS = 0,
	NEED_Q,
	NEED_I,
	NEED_U,
	NEED_R,
	NEED_C,
	NEED_MAO,
	NEED_KONG,
	NEED_YIN,
	NEED_r,
	NEED_e,
	NEED_c,
	NEED_v,
	NEED_DOU,
	NEED_0D,
	NEED_0A,

    NEED_ID_DATA,
    NEED_LEN_DATA,
    NEED_COLON ,
    NEED_USER_DATA ,
    STATE_ERROR ,

} NET_DATA_STATE_E;

//BG96的连接状态 只有在就绪状态才可以连接
typedef enum
{
    GET_FAILED 			= 255,	//获取连接状态失败
    INITIAL 			= 0,	//连接尚未建立
    OPENING 			= 1,	//正在连接
	CONNECTED			= 2,	//连接已经建立
	LISTENING 			= 3,	//服务器正在监听
	CLOSING 			= 4,	//连接正在关闭
	NEED_NEW_CONNECT	= 5,	//需要建立新的连接
} BG96_STATE_E;

//BG96的连接状态 只有在就绪状态才可以连接
typedef enum
{
    UNKNOW_ERROR 	= 255,	//获取连接状态失败
    GET_READY 		= 0,	//就绪状态
    NEED_CLOSE 		= 1,	//需要关闭移动场景
    NEED_WAIT 		= 2,	//需要等待
	ON_SERVER 		= 4,	//连接建立成功
} CONNECT_STATE_E;

typedef enum
{
    SIGNLE = 0,
	MULTIPLE = 1
} CIP_MUX_MODE;


#define Bg96 struct BG96
typedef struct BG96 *pBg96;
struct BG96
{
	char        	*rx_cmd_buf;
    unsigned short  rx_cnt;

	void			(*hard_init)(pBg96 *bg96);
	void			(*hard_enable)(pBg96 *bg96);
	void			(*hard_disable)(pBg96 *bg96);
	void 			(*hard_reset)(pBg96 *bg96);
	unsigned char 	(*send_string)(pBg96 *bg96,USART_TypeDef* USARTx,unsigned char *str, unsigned short len);


	int         	(*available)(pBg96 *bg96);
	unsigned short 	(*read)(pBg96 *bg96,unsigned char *buf);
	unsigned char	(*send)(pBg96 *bg96, unsigned char *buffer, unsigned int len);

	unsigned char	(*restart)(pBg96 *bg96);
	CONNECT_STATE_E (*get_connect_state)(pBg96 *bg96);
	unsigned char	(*get_local_ip)(pBg96 *bg96,char *msg);

	unsigned char	(*create_TCP)(pBg96 *bg96,char *addr, char *port);
	unsigned char	(*release_TCP)(pBg96 *bg96);
	unsigned char	(*register_UDP)(pBg96 *bg96,char *addr, char *port);
	unsigned char	(*unregister_UDP)(pBg96 *bg96);

	unsigned char	(*set_AT)(pBg96 *bg96);
	unsigned char	(*set_AT_ATE)(pBg96 *bg96,char cmd);
	unsigned char	(*set_AT_UART)(pBg96 *bg96,unsigned int baud_rate);

	unsigned char	(*get_AT_CPIN)(pBg96 *bg96);
	unsigned char	(*get_AT_CSQ)(pBg96 *bg96);

	unsigned char   (*set_AT_QCFG1)(pBg96 *bg96);
	unsigned char   (*set_AT_QCFG2)(pBg96 *bg96);
	unsigned char   (*set_AT_QCFG3)(pBg96 *bg96);
	unsigned char   (*set_AT_QCFG4)(pBg96 *bg96,unsigned char operators);
	unsigned char   (*set_AT_QCFG5)(pBg96 *bg96);
	unsigned char   (*set_AT_QICSGP)(pBg96 *bg96,unsigned char operators);
	unsigned char   (*set_AT_QIACT)(pBg96 *bg96);
	unsigned char   (*get_AT_QIACT)(pBg96 *bg96,char *list);

	BG96_STATE_E 	(*get_AT_QISTATE)(pBg96 *bg96);
	unsigned char 	(*set_AT_QIOPEN)(pBg96 *bg96,char *type, char *addr, char *port);
	unsigned char 	(*set_AT_QICLOSE)(pBg96 *bg96);
	unsigned char 	(*set_AT_QISEND)(pBg96 *bg96,unsigned char *buffer, unsigned int len);
	unsigned char 	(*get_AT_QISEND)(pBg96 *bg96);
	unsigned char 	(*get_AT_QIDNSGIP)(pBg96 *bg96,const char *domain, unsigned char **ip);
	unsigned char 	(*get_AT_QPING)(pBg96 *bg96,const char *host, char *msg);
	unsigned char   (*get_AT_GSN)(pBg96 *bg96);

	unsigned char 	(*set_AT_QGPS)(pBg96 *bg96);
	unsigned char 	(*set_AT_QGPSLOC)(pBg96 *bg96,char *msg);
	unsigned char 	(*set_AT_QGPSEND)(pBg96 *bg96);

	unsigned char 	(*set_AT_QNTP)(pBg96 *bg96,char *server,unsigned short port,char *msg);

    pRingBuf     	net_buf;
    unsigned short  net_data_rx_cnt;
    unsigned short  net_data_len;
    unsigned char   net_data_id;

//	char        	*rx_cmd_buf;
//    unsigned short  rx_cnt;
    void        	(*clear_rx_cmd_buffer)(pBg96 *bg96);
    void        	(*get_char)(pBg96 *bg96);

	unsigned char   break_out_wait_cmd;

    void        	(*print_rx_buf)(pBg96 *bg96);
    void        	(*print_cmd)(pBg96 *bg96,CMD_STATE_E cmd);
    CMD_STATE_E 	(*wait_cmd1)(pBg96 *bg96,unsigned int wait_time);
    CMD_STATE_E 	(*wait_cmd2)(pBg96 *bg96,const char *spacial_target, unsigned int wait_time);
    unsigned char   (*wait_bg96_mode)(pBg96 *bg96,GPRS_MODE_E mode);

	unsigned char   init_ok;
    unsigned int    last_time;
    CMD_STATE_E 	cmd_state;
    GPRS_MODE_E 	bg96_mode;
	CIP_MUX_MODE	cip_mux_mode;

	USART_TypeDef* 	USARTx;

	char   			*imei;

	void 			(*uart_interrupt_event)(pBg96 *bg96);
    void 			(*net_data_state_process)(pBg96 *bg96,char c);
};


extern pBg96 bg96;

unsigned char 	BG96_InitStep1(pBg96 *bg96);
unsigned char 	BG96_InitStep2(pBg96 *bg96);
void 			BG96_UnInit(pBg96 *bg96);

void			bg96_hard_init(pBg96 *bg96);
void			bg96_hard_enable(pBg96 *bg96);
void			bg96_hard_disable(pBg96 *bg96);
void 			bg96_hard_reset(pBg96 *bg96);
unsigned char 	bg96_send_string(pBg96 *bg96,USART_TypeDef* USARTx,unsigned char *str, unsigned short len);

int         	bg96_available(pBg96 *bg96);
unsigned short 	bg96_read(pBg96 *bg96,unsigned char *buf);
unsigned char	bg96_send(pBg96 *bg96, unsigned char *buffer, unsigned int len);

unsigned char	bg96_restart(pBg96 *bg96);
CONNECT_STATE_E bg96_get_connect_state(pBg96 *bg96);
unsigned char	bg96_get_local_ip(pBg96 *bg96,char *msg);

unsigned char	bg96_create_TCP(pBg96 *bg96,char *addr, char *port);
unsigned char	bg96_release_TCP(pBg96 *bg96);
unsigned char	bg96_register_UDP(pBg96 *bg96,char *addr, char *port);
unsigned char	bg96_unregister_UDP(pBg96 *bg96);

unsigned char	bg96_set_AT(pBg96 *bg96);
unsigned char	bg96_set_AT_ATE(pBg96 *bg96,char cmd);
unsigned char	bg96_set_AT_UART(pBg96 *bg96,unsigned int baud_rate);
unsigned char	bg96_get_AT_CPIN(pBg96 *bg96);
unsigned char	bg96_get_AT_CSQ(pBg96 *bg96);
unsigned char   bg96_set_AT_QCFG1(pBg96 *bg96);
unsigned char   bg96_set_AT_QCFG2(pBg96 *bg96);
unsigned char   bg96_set_AT_QCFG3(pBg96 *bg96);
unsigned char   bg96_set_AT_QCFG4(pBg96 *bg96,unsigned char operators);
unsigned char   bg96_set_AT_QCFG5(pBg96 *bg96);
unsigned char   bg96_set_AT_QICSGP(pBg96 *bg96,unsigned char operators);
unsigned char   bg96_set_AT_QIACT(pBg96 *bg96);
unsigned char   bg96_get_AT_QIACT(pBg96 *bg96,char *list);

BG96_STATE_E 	bg96_get_AT_QISTATE(pBg96 *bg96);
unsigned char 	bg96_set_AT_QIOPEN(pBg96 *bg96,char *type, char *addr, char *port);
unsigned char 	bg96_set_AT_QICLOSE(pBg96 *bg96);
unsigned char 	bg96_set_AT_QISEND(pBg96 *bg96,unsigned char *buffer, unsigned int len);
unsigned char 	bg96_get_AT_QISEND(pBg96 *bg96);
unsigned char 	bg96_get_AT_QIDNSGIP(pBg96 *bg96,const char *domain, unsigned char **ip);
unsigned char 	bg96_get_AT_QPING(pBg96 *bg96,const char *host, char *msg);
unsigned char   bg96_get_AT_GSN(pBg96 *bg96);

unsigned char 	bg96_set_AT_QGPS(pBg96 *bg96);
unsigned char 	bg96_set_AT_QGPSLOC(pBg96 *bg96,char *msg);
unsigned char 	bg96_set_AT_QGPSEND(pBg96 *bg96);

unsigned char 	bg96_set_AT_QNTP(pBg96 *bg96,char *server,unsigned short port,char *msg);


void        	bg96_clear_rx_cmd_buffer(pBg96 *bg96);
void        	bg96_get_char(pBg96 *bg96);

void        	bg96_print_rx_buf(pBg96 *bg96);
void        	bg96_print_cmd(pBg96 *bg96,CMD_STATE_E cmd);
CMD_STATE_E 	bg96_wait_cmd1(pBg96 *bg96,unsigned int wait_time);
CMD_STATE_E 	bg96_wait_cmd2(pBg96 *bg96,const char *spacial_target, unsigned int wait_time);
unsigned char   bg96_wait_bg96_mode(pBg96 *bg96,GPRS_MODE_E mode);

void 			bg96_uart_interrupt_event(pBg96 *bg96);
void 			bg96_net_data_state_process(pBg96 *bg96,char c);

















#endif
