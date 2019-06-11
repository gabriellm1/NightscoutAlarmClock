#include "asf.h"
#include "main.h"
#include <string.h>
#include "bsp/include/nm_bsp.h"
#include "driver/include/m2m_wifi.h"
#include "socket/include/socket.h"
#include "simple.h"

#include "icones/tfont.h"
#include "icones/digital521.h"
#include "icones/arrow.h"
#include "icones/bloodG.h"
#include "icones/bloodY.h"
#include "icones/bloodR.h"
#include "icones/arrow45down.h"
#include "icones/arrow90down.h"
#include "icones/arrow45up.h"
#include "icones/arrow90up.h"
#include "icones/Connection.h"
#include "icones/NoConnection.h"

const uint32_t LINE1_Y = 10;
const uint32_t LINE2_Y = 120;
const uint32_t LINE3_Y = 230;

const uint32_t BLOOD_ICON_X = 5;
const uint32_t TREND_ICON_X = 175;
const uint32_t ICON_WIDHT = 50;
const uint32_t ICON_HEIGHT = 51;
const uint32_t PRED_X = 230;
const uint32_t CLOCK_X = 5;
const uint32_t DATE_X = 200;
const uint32_t STAT_X = 390;
const uint32_t CONNECTION_WIDHT = 55;
const uint32_t CONNECTION_HEIGHT = 48;


//#define DEBUG 1

#define STRING_EOL    "\r\n"
#define STRING_HEADER "-- WINC1500 weather client example --"STRING_EOL	\
	"-- "BOARD_NAME " --"STRING_EOL	\
	"-- Compiled: "__DATE__ " "__TIME__ " --"STRING_EOL
  

  

/** IP address of host. */
uint32_t gu32HostIp = 0;

/** TCP client socket handlers. */
static SOCKET tcp_client_socket = -1;

/** Receive buffer definition. */
static uint8_t gau8ReceivedBuffer[MAIN_WIFI_M2M_BUFFER_SIZE] = {0};

/** Wi-Fi status variable. */
static bool gbConnectedWifi = false;

/** Get host IP status variable. */
/** Wi-Fi connection state */
static uint8_t wifi_connected;


/** Instance of HTTP client module. */
static bool gbHostIpByName = false;

/** TCP Connection status variable. */
static bool gbTcpConnection = false;

/** Server host name. */
static char server_host_name[] = MAIN_SERVER_NAME;


#define TASK_WIFI_STACK_SIZE            (3*4096/sizeof(portSTACK_TYPE))
#define TASK_WIFI_STACK_PRIORITY        (tskIDLE_PRIORITY)

#define TASK_JSON_STACK_SIZE            (3*4096/sizeof(portSTACK_TYPE))
#define TASK_JSON_STACK_PRIORITY        (tskIDLE_PRIORITY)

#define TASK_LCD_STACK_SIZE            (4096/sizeof(portSTACK_TYPE))
#define TASK_LCD_STACK_PRIORITY        (tskIDLE_PRIORITY)

#define TASK_ALARME_STACK_SIZE         (4096/sizeof(portSTACK_TYPE))
#define TASK_ALARME_STACK_PRIORITY        (tskIDLE_PRIORITY)

struct ili9488_opt_t g_ili9488_display_opt;

#define YEAR        2019
#define MONTH		5
#define DAY         13
#define WEEK        12
#define HOUR        23
#define MINUTE      59
#define SECOND      24
uint32_t year, week ,month ,day, hour, minu, seg;


extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);

QueueHandle_t xQueueRaw;
QueueHandle_t xQueueDone;
QueueHandle_t xQueueCheck;
SemaphoreHandle_t xSnooze;

// defines do Buzzer
#define BUZZ_PIO           PIOD
#define BUZZ_PIO_ID        ID_PIOD
#define BUZZ_PIO_IDX       30u
#define BUZZ_PIO_IDX_MASK  (1u << BUZZ_PIO_IDX)

// defines da fita (BLUE)
#define BLUE_PIO           PIOA
#define BLUE_PIO_ID        ID_PIOA
#define BLUE_PIO_IDX       6u
#define BLUE_PIO_IDX_MASK  (1u << BLUE_PIO_IDX)

// defines da fita (GREEN)
#define GREEN_PIO           PIOC
#define GREEN_PIO_ID        ID_PIOC
#define GREEN_PIO_IDX       19u
#define GREEN_PIO_IDX_MASK  (1u << GREEN_PIO_IDX)

//defines da fita (RED)
#define RED_PIO           PIOA
#define RED_PIO_ID        ID_PIOA
#define RED_PIO_IDX       6u
#define RED_PIO_IDX_MASK  (1u << RED_PIO_IDX)

//defines do botao snooze
#define SNOOZE_PIO           PIOA
#define SNOOZE_PIO_ID        ID_PIOA
#define SNOOZE_PIO_IDX       24u
#define SNOOZE_PIO_IDX_MASK  (1u << SNOOZE_PIO_IDX)

static void snooze_callback(void){
  xSemaphoreGiveFromISR(xSnooze, NULL);
}

/**
 * \brief Called if stack overflow during execution
 */
extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
		signed char *pcTaskName)
{
	printf("stack overflow %x %s\r\n", pxTask, (portCHAR *)pcTaskName);
	/* If the parameters have been corrupted then inspect pxCurrentTCB to
	 * identify which task has overflowed its stack.
	 */
	for (;;) {
	}
}

/**
 * \brief This function is called by FreeRTOS idle task
 */
extern void vApplicationIdleHook(void)
{
  pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
}

/**
 * \brief This function is called by FreeRTOS each tick
 */
extern void vApplicationTickHook(void)
{
}

extern void vApplicationMallocFailedHook(void)
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

	/* Force an assert. */
	configASSERT( ( volatile void * ) NULL );
}


/**
 * \brief Configure UART console.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate =		CONF_UART_BAUDRATE,
		.charlength =	CONF_UART_CHAR_LENGTH,
		.paritytype =	CONF_UART_PARITY,
		.stopbits =		CONF_UART_STOP_BITS,
	};

	/* Configure UART console. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

static void configure_lcd(void){
  /* Initialize display parameter */
  g_ili9488_display_opt.ul_width = ILI9488_LCD_WIDTH;
  g_ili9488_display_opt.ul_height = ILI9488_LCD_HEIGHT;
  g_ili9488_display_opt.foreground_color = COLOR_CONVERT(COLOR_WHITE);
  g_ili9488_display_opt.background_color = COLOR_CONVERT(COLOR_WHITE);

	pmc_enable_periph_clk(BORAD_ILI9488_ID_USART);


  /* Initialize LCD */
  ili9488_init(&g_ili9488_display_opt);
}
void draw_screen(void) {
  ili9488_set_foreground_color(COLOR_CONVERT(COLOR_WHITE));
  ili9488_draw_filled_rectangle(0, 0, ILI9488_LCD_WIDTH-1, ILI9488_LCD_HEIGHT-1);

}
void font_draw_text(tFont *font, const char *text, int x, int y, int spacing) {
  char *p = text;
  while(*p != NULL) {
    char letter = *p;
    int letter_offset = letter - font->start_char;
    if(letter <= font->end_char) {
      tChar *current_char = font->chars + letter_offset;
      ili9488_draw_pixmap(x, y, current_char->image->width, current_char->image->height, current_char->image->data);
      x += current_char->image->width + spacing;
    }
    p++;
  }
}

/*
 * Check whether "cp" is a valid ascii representation
 * of an Internet address and convert to a binary address.
 * Returns 1 if the address is valid, 0 if not.
 * This replaces inet_addr, the return value from which
 * cannot distinguish between failure and a local broadcast address.
 */
 /* http://www.cs.cmu.edu/afs/cs/academic/class/15213-f00/unpv12e/libfree/inet_aton.c */
int inet_aton(const char *cp, in_addr *ap)
{
  int dots = 0;
  register u_long acc = 0, addr = 0;

  do {
	  register char cc = *cp;

	  switch (cc) {
	    case '0':
	    case '1':
	    case '2':
	    case '3':
	    case '4':
	    case '5':
	    case '6':
	    case '7':
	    case '8':
	    case '9':
	        acc = acc * 10 + (cc - '0');
	        break;

	    case '.':
	        if (++dots > 3) {
		    return 0;
	        }
	        /* Fall through */

	    case '\0':
	        if (acc > 255) {
		    return 0;
	        }
	        addr = addr << 8 | acc;
	        acc = 0;
	        break;

	    default:
	        return 0;
    }
  } while (*cp++) ;

  /* Normalize the address */
  if (dots < 3) {
	  addr <<= 8 * (3 - dots) ;
  }

  /* Store it if requested */
  if (ap) {
	  ap->s_addr = _htonl(addr);
  }

  return 1;
}

/**
 * \brief Callback function of IP address.
 *
 * \param[in] hostName Domain name.
 * \param[in] hostIp Server IP.
 *
 * \return None.
 */
static void resolve_cb(uint8_t *hostName, uint32_t hostIp)
{
	gu32HostIp = hostIp;
	gbHostIpByName = true;
	printf("resolve_cb: %s IP address is %d.%d.%d.%d\r\n\r\n", hostName,
			(int)IPV4_BYTE(hostIp, 0), (int)IPV4_BYTE(hostIp, 1),
			(int)IPV4_BYTE(hostIp, 2), (int)IPV4_BYTE(hostIp, 3));
}

struct message_t {
  char content[MAIN_WIFI_M2M_BUFFER_SIZE];
  };
 
/**
 * \brief Callback function of TCP client socket.
 *
 * \param[in] sock socket handler.
 * \param[in] u8Msg Type of Socket notification
 * \param[in] pvMsg A structure contains notification informations.
 *
 * \return None.
 */
static void socket_cb(SOCKET sock, uint8_t u8Msg, void *pvMsg)
{

	/* Check for socket event on TCP socket. */
	if (sock == tcp_client_socket) {

		switch (u8Msg) {
		case SOCKET_MSG_CONNECT:
		{
      printf("socket_msg_connect\n");
			if (gbTcpConnection) {
				memset(gau8ReceivedBuffer, 0, sizeof(gau8ReceivedBuffer));
				sprintf((char *)gau8ReceivedBuffer, "%s", MAIN_PREFIX_BUFFER);

				tstrSocketConnectMsg *pstrConnect = (tstrSocketConnectMsg *)pvMsg;
				if (pstrConnect && pstrConnect->s8Error >= SOCK_ERR_NO_ERROR) {
          printf("send \n");
					send(tcp_client_socket, gau8ReceivedBuffer, strlen((char *)gau8ReceivedBuffer), 0);

					memset(gau8ReceivedBuffer, 0, MAIN_WIFI_M2M_BUFFER_SIZE);
					recv(tcp_client_socket, &gau8ReceivedBuffer[0], MAIN_WIFI_M2M_BUFFER_SIZE, 0);
				} else {
					printf("socket_cb: connect error!\r\n");
					gbTcpConnection = false;
					close(tcp_client_socket);
					tcp_client_socket = -1;
				}
			}
		}
		break;



		case SOCKET_MSG_RECV:
		{
			char *pcIndxPtr;
			char *pcEndPtr;
      
      
      // oq  e o queue length?
      //xQueueData = xQueueCreate( 10, sizeof( pstrRecv->pu8Buffer ) );

			tstrSocketRecvMsg *pstrRecv = (tstrSocketRecvMsg *)pvMsg;
			if (pstrRecv && pstrRecv->s16BufferSize > 0) {
        /************************************************************************/
        /* DADO PRONTO!!! */
        //printf("---------------------------------\n" );
				//printf(pstrRecv->pu8Buffer);
        struct message_t mensagem;
        strcpy(&mensagem.content, pstrRecv->pu8Buffer);
        xQueueSend(xQueueRaw, &mensagem, 0);
        /************************************************************************/

				memset(gau8ReceivedBuffer, 0, sizeof(gau8ReceivedBuffer));
				sprintf((char *)gau8ReceivedBuffer, "%s", MAIN_PREFIX_BUFFER);
				send(tcp_client_socket, gau8ReceivedBuffer, strlen((char *)gau8ReceivedBuffer), 0);

				memset(gau8ReceivedBuffer, 0, sizeof(gau8ReceivedBuffer));
				recv(tcp_client_socket, &gau8ReceivedBuffer[0], MAIN_WIFI_M2M_BUFFER_SIZE, 0);
			} else {
				printf("socket_cb: recv error!\r\n");
				close(tcp_client_socket);
				tcp_client_socket = -1;
			}
		}
		break;

		default:
			break;
		}
	}
}

static void set_dev_name_to_mac(uint8_t *name, uint8_t *mac_addr)
{
	/* Name must be in the format WINC1500_00:00 */
	uint16 len;

	len = m2m_strlen(name);
	if (len >= 5) {
		name[len - 1] = MAIN_HEX2ASCII((mac_addr[5] >> 0) & 0x0f);
		name[len - 2] = MAIN_HEX2ASCII((mac_addr[5] >> 4) & 0x0f);
		name[len - 4] = MAIN_HEX2ASCII((mac_addr[4] >> 0) & 0x0f);
		name[len - 5] = MAIN_HEX2ASCII((mac_addr[4] >> 4) & 0x0f);
	}
}

/**
 * \brief Callback to get the Wi-Fi status update.
 *
 * \param[in] u8MsgType Type of Wi-Fi notification.
 * \param[in] pvMsg A pointer to a buffer containing the notification parameters.
 *
 * \return None.
 */
static void wifi_cb(uint8_t u8MsgType, void *pvMsg)
{
	switch (u8MsgType) {
	case M2M_WIFI_RESP_CON_STATE_CHANGED:
	{
		tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged *)pvMsg;
		if (pstrWifiState->u8CurrState == M2M_WIFI_CONNECTED) {
			printf("wifi_cb: M2M_WIFI_CONNECTED\r\n");
			m2m_wifi_request_dhcp_client();
		} else if (pstrWifiState->u8CurrState == M2M_WIFI_DISCONNECTED) {
			printf("wifi_cb: M2M_WIFI_DISCONNECTED\r\n");
			gbConnectedWifi = false;
 			wifi_connected = 0;
		}

		break;
	}

	case M2M_WIFI_REQ_DHCP_CONF:
	{
		uint8_t *pu8IPAddress = (uint8_t *)pvMsg;
		printf("wifi_cb: IP address is %u.%u.%u.%u\r\n",
				pu8IPAddress[0], pu8IPAddress[1], pu8IPAddress[2], pu8IPAddress[3]);
		wifi_connected = M2M_WIFI_CONNECTED;

    /* Obtain the IP Address by network name */
		gethostbyname((uint8_t *)server_host_name);
		break;
	}

	default:
	{
		break;
	}
	}
}

/**
 * \brief This task, when activated, send every ten seconds on debug UART
 * the whole report of free heap and total tasks status
 */

static void task_wifi(void *pvParameters) {
	tstrWifiInitParam param;
	int8_t ret;
	uint8_t mac_addr[6];
	uint8_t u8IsMacAddrValid;
	struct sockaddr_in addr_in;

	/* Initialize the BSP. */
	nm_bsp_init();

	/* Initialize Wi-Fi parameters structure. */
	memset((uint8_t *)&param, 0, sizeof(tstrWifiInitParam));

	/* Initialize Wi-Fi driver with data and status callbacks. */
	param.pfAppWifiCb = wifi_cb;
	ret = m2m_wifi_init(&param);
	if (M2M_SUCCESS != ret) {
		printf("main: m2m_wifi_init call error!(%d)\r\n", ret);
		while (1) {
		}
	}

	/* Initialize socket module. */
	socketInit();

	/* Register socket callback function. */
	registerSocketCallback(socket_cb, resolve_cb);

	/* Connect to router. */
	printf("main: connecting to WiFi AP %s...\r\n", (char *)MAIN_WLAN_SSID);
	m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID), MAIN_WLAN_AUTH, (char *)MAIN_WLAN_PSK, M2M_WIFI_CH_ALL);

// 	addr_in.sin_family = AF_INET;
// 	addr_in.sin_port = _htons(MAIN_SERVER_PORT);
// 	inet_aton(MAIN_SERVER_NAME, &addr_in.sin_addr);
// 	printf("Inet aton : %d", addr_in.sin_addr);

  while(1){

	  m2m_wifi_handle_events(NULL);
	  if (wifi_connected == M2M_WIFI_CONNECTED) {
		  if (gbHostIpByName) {
			   if (tcp_client_socket < 0) {
					 
					  printf("socket init \n");
					  if ((tcp_client_socket = socket(AF_INET, SOCK_STREAM, SOCKET_FLAGS_SSL)) < 0) {
						  printf("main: failed to create TCP client socket error!\r\n");
					 	  continue;
				    }
						
				    printf("socket connecting\n");
				    addr_in.sin_family = AF_INET;
				  	addr_in.sin_port = _htons(MAIN_SERVER_PORT);
				  	addr_in.sin_addr.s_addr = gu32HostIp;
						
						if (connect(tcp_client_socket, (struct sockaddr *)&addr_in, sizeof(struct sockaddr_in)) != SOCK_ERR_NO_ERROR) {
							close(tcp_client_socket);
							tcp_client_socket = -1;
							printf("error\n");
						} else{
							  gbTcpConnection = true;
               
						}

             

			  } // socket
        vTaskDelay(5000);
			} //hostbyname
		} //wifi connected
	} //while
}

static void task_json(void){
  
  xQueueRaw = xQueueCreate( 1, sizeof( struct message_t ) );
  
  struct message_t mensagem;
  
  data_g get_atual;
  
  while(1){

    if (xQueueReceive(xQueueRaw, &(mensagem), 0)) {
      parseiro(mensagem.content,&get_atual); // Parseia as mensagem recebida e adiciona seus dados em um struct
      xQueueSend(xQueueDone, &get_atual, 0);
      xQueueSend(xQueueCheck, &get_atual, 0); // Manda os dados para uma fila
 
    }
    vTaskDelay(100);
  }
}

void task_lcd(void){
	configure_lcd();
	draw_screen();
	
	xQueueDone = xQueueCreate( 1, sizeof( data_g ) );
	 
	data_g data_received;

	int  isDead ;
  int glu;
	char tend[] = "115";
	
	int diff = 14;
	  
	uint8_t stingLCD[56];

	int time_in_s  = 60; // segundos
	const TickType_t xDelay = time_in_s*1000 / portTICK_PERIOD_MS; // Converte ticks do CORE para ms

	while (true) {  
    
		if (xQueueReceive(xQueueCheck, &(data_received), 0)) { //Recebe os dados da fila
       glu = atoi(data_received.glicose);
		   //printf("\nALARME %s  %s  %s\n",data_received.id, data_received.glicose,data_received.direction);
        if (glu<=140 && glu>=90){
          isDead = 0;
        }
        else if((glu>140 && glu<=180)||(glu<90 && glu>=70)){
          isDead = 1;
        }
        else{
          isDead = 2;
        }
        //printf("isDead %d\n\n",isDead);
			  sprintf(stingLCD, ":%s", data_received.glicose); 
			  font_draw_text(&digital52, stingLCD, ICON_HEIGHT+1, LINE2_Y, 1);
		  
			  sprintf(stingLCD, "%d/%d",data_received.serverTime);
			  font_draw_text(&digital52, stingLCD, DATE_X, LINE1_Y, 1);

			  sprintf(stingLCD, "%d:%d", data_received.serverDate);
			  font_draw_text(&digital52, stingLCD, CLOCK_X, LINE1_Y, 5);
		  
			  if (strcmp(data_received.serverTime,data_received.serverTime) == 0){
				  diff++;
			  }else{
				  diff=0;
			  }

			  if (isDead==0){
				  ili9488_draw_pixmap(BLOOD_ICON_X,LINE2_Y,ICON_WIDHT,ICON_HEIGHT,image_data_bloodG);
			  }
			  else if (isDead==1){
				  ili9488_draw_pixmap(BLOOD_ICON_X,LINE2_Y,ICON_WIDHT,ICON_HEIGHT,image_data_bloodY);
			  }
			  else if (isDead==2){
				  ili9488_draw_pixmap(BLOOD_ICON_X,LINE2_Y,ICON_WIDHT,ICON_HEIGHT,image_data_bloodR);
			  }
			  if (strcmp(data_received.direction,"90up") == 0){
				  ili9488_draw_pixmap(TREND_ICON_X,LINE2_Y,ICON_WIDHT,ICON_HEIGHT,image_data_arrow90up);
			  }
			  else if (strcmp(data_received.direction,"45up") == 0){
				  ili9488_draw_pixmap(TREND_ICON_X,LINE2_Y,ICON_WIDHT,ICON_HEIGHT,image_data_arrow45up);
			  }
			  else if (strcmp(data_received.direction,"Flat") == 0){
				  ili9488_draw_pixmap(TREND_ICON_X,LINE2_Y,ICON_WIDHT,ICON_HEIGHT,image_data_arrow);
			  }
			  else if (strcmp(data_received.direction,"45down") == 0){
				  ili9488_draw_pixmap(TREND_ICON_X,LINE2_Y,ICON_WIDHT,ICON_HEIGHT,image_data_arrow45down);
			  }
			  else if (strcmp(data_received.direction,"90down") == 0){
				  ili9488_draw_pixmap(TREND_ICON_X,LINE2_Y,ICON_WIDHT,ICON_HEIGHT,image_data_arrow90down);
			  }
			  if(diff>=15){
				  ili9488_draw_pixmap(STAT_X,LINE3_Y,CONNECTION_WIDHT,CONNECTION_HEIGHT,image_data_NoConnection);
			  }
			  else if (diff<15){
				  ili9488_draw_pixmap(STAT_X,LINE3_Y,CONNECTION_WIDHT,CONNECTION_HEIGHT,image_data_Connection);
			  }
	
			  ili9488_set_foreground_color(COLOR_CONVERT(COLOR_WHITE));
			  ili9488_draw_filled_rectangle(LINE2_Y+30,0, LINE2_Y+53,220);
			  ili9488_draw_filled_rectangle(LINE2_Y+105,0, LINE2_Y+153,220);
			  /*
		
			  TENDENCIA VAI AQUI
		
			  */
		
			  vTaskDelay(5000);
		}    
	}	 
}
//cagada
static void task_alarme(void){
  xSnooze = xSemaphoreCreateBinary();
  if (xSnooze == NULL){
    printf("falha em criar o semaforo \n");
  }  
  	
  xQueueCheck = xQueueCreate( 1, sizeof( data_g ) );

   
  printf("PORRAAAA");
	data_g data_check;
  
	int glu;
  
  int snooze = 0;
  
	//TickType_t sleep = 100 ;

	while(1){
      if ( xSemaphoreTake(xSnooze, ( TickType_t ) 500) == pdTRUE )
      {
        if(snooze == 0){
          printf("\n\nTESTEEE\n\n");
          snooze = 1;
          pio_clear(BUZZ_PIO, BUZZ_PIO_IDX_MASK);
          }else{
          snooze = 0;
        }
        
      }
      if (xQueueReceive(xQueueCheck, &(data_check), 0)) {
          //printf("\nALARME %s  %s  %s\n",data_check.id, data_check.glicose,data_check.direction);
          glu = atoi(data_check.glicose);
          //glu = 250;
          printf("glu %d\n",glu);

			if (glu<=140 && glu>=90){
				
				printf("\ngreen");
				pio_set(BLUE_PIO,BLUE_PIO_IDX_MASK);
				pio_clear(GREEN_PIO, GREEN_PIO_IDX_MASK);
				pio_set(RED_PIO,RED_PIO_IDX_MASK);
        pio_clear(BUZZ_PIO, BUZZ_PIO_IDX_MASK);

			}else if((glu>140 && glu<=180)||(glu<90 && glu>=70)){
				printf("\nyellow");
				pio_clear(BLUE_PIO,BLUE_PIO_IDX_MASK);
				pio_set(GREEN_PIO, GREEN_PIO_IDX_MASK);
				pio_set(RED_PIO, RED_PIO_IDX_MASK);
        pio_clear(BUZZ_PIO, BUZZ_PIO_IDX_MASK);
			}    
			else{
        if(!snooze){
          pio_set(BUZZ_PIO, BUZZ_PIO_IDX_MASK);
        }
				pio_set(BLUE_PIO,BLUE_PIO_IDX_MASK);
				pio_set(GREEN_PIO, GREEN_PIO_IDX_MASK);
				pio_clear(RED_PIO, RED_PIO_IDX_MASK);
				printf("\nred");
			}        
        }
        vTaskDelay(100);  
  }
  
}

void io_init(void){
   // Desativa WatchDog Timer
   WDT->WDT_MR = WDT_MR_WDDIS;

   // Ativa o PIO na qual o BUZZ foi conectado
   // para que possamos controlar o BUZZ.
   pmc_enable_periph_clk(BUZZ_PIO_ID);
   pmc_enable_periph_clk(BLUE_PIO_ID);
   pmc_enable_periph_clk(GREEN_PIO_ID);
   pmc_enable_periph_clk(RED_PIO_ID);
   pmc_enable_periph_clk(SNOOZE_PIO_ID);
   
   pio_configure(SNOOZE_PIO, PIO_INPUT, SNOOZE_PIO_IDX_MASK, PIO_PULLUP);
   pio_set_debounce_filter(SNOOZE_PIO,SNOOZE_PIO_IDX_MASK,20);
   		// Ativa interrup??o
   		pio_enable_interrupt(SNOOZE_PIO, SNOOZE_PIO_IDX_MASK);



   		// Configura NVIC para receber interrupcoes do PIO do botao
   		// com prioridade 1 (quanto mais pr?ximo de 0 maior)
   		NVIC_EnableIRQ(SNOOZE_PIO_ID);
   		NVIC_SetPriority(SNOOZE_PIO_ID, 1); // Prioridade 1

   		pio_handler_set(SNOOZE_PIO,
   		SNOOZE_PIO_ID,
   		SNOOZE_PIO_IDX_MASK,
   		PIO_IT_FALL_EDGE,
   		snooze_callback);
   
   //Inicializa PA19 como sa?da
   pio_set_output(BUZZ_PIO, BUZZ_PIO_IDX_MASK, 0, 0, 0);
   pio_set_output(BLUE_PIO, BLUE_PIO_IDX_MASK, 1, 0, 0);
   pio_set_output(GREEN_PIO, GREEN_PIO_IDX_MASK, 1, 0, 0);
   pio_set_output(RED_PIO, RED_PIO_IDX_MASK, 1, 0, 0);

}

/**
 * \brief Main application function.
 *
 * Initialize system, UART console, network then start weather client.
 *
 * \return Program return value.
 */
int main(void)
{
	/* Initialize the board. */
	sysclk_init();
	board_init();
	io_init();
	

	/* Initialize the UART console. */
	configure_console();
	printf(STRING_HEADER);

	if (xTaskCreate(task_wifi, "Wifi", TASK_WIFI_STACK_SIZE, NULL,
	TASK_WIFI_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create Wifi task\r\n");
	}
	if (xTaskCreate(task_json, "JSON", TASK_JSON_STACK_SIZE, NULL,
	TASK_JSON_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create JSON task\r\n");
	}
	if (xTaskCreate(task_lcd, "LCD", TASK_LCD_STACK_SIZE, NULL,
	TASK_LCD_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create LCD task\r\n");
	}
	if (xTaskCreate(task_alarme, "ALARME", TASK_ALARME_STACK_SIZE, NULL,
	TASK_ALARME_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create ALARME task\r\n");
	}
	vTaskStartScheduler();

	while(1) {};
	return 0;

}