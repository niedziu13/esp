#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "driver/uart.h"
#include "ip_addr.h"
#include "espconn.h"
#include "user_interface.h"
#include "user_config.h"


static const int pin = 1;
static volatile os_timer_t some_timer;
esp_udp my_udp;
esp_tcp my_tcp;
struct espconn espconn;
uint8_t status=0;

void tcp_conntected(void *arg)
{
    uart0_sendStr("TCP conneted \n\r");
}

void tcp_disconntected(void *arg)
{
    uart0_sendStr("TCP disconneted \n\r");
}

void sent_callback(void *arg)
{
    uart0_sendStr("Send cb \n\r");
}

void recv_callback (void *arg, char *pdata, unsigned short len)
{
    uart0_sendStr("recv cb \n\r Len: ");
    char tab[] = {len+48, '\n', '\r', '\0'};
    uart0_sendStr(tab);
    uart0_sendStr("Data: \n\r");
    uart0_sendStr(pdata);
    uint8_t tab3[10] = {49, 50, 51, 51, 52, 53, 56, 77, 91, 61};
    char tab1[] = {'s', 't', espconn_sent(&espconn, tab3, 10) + 48, '\n', '\r', '\0'};
    uart0_sendStr(tab1);
}

void wifi_callback( System_Event_t *evt ){
  uart0_sendStr("CB wifi \n\r");

  switch ( evt->event )
  {
      case EVENT_STAMODE_CONNECTED:
      {
          uart0_sendStr("Wifi cb stammode connected \n\r");
    	  break;
      }
      case EVENT_STAMODE_DISCONNECTED:
      {
          uart0_sendStr("Wifi cb stammode disconnected \n\r");
          break;
      }
      case EVENT_STAMODE_AUTHMODE_CHANGE:
      {
          uart0_sendStr("Wifi cb stammode authmode change \n\r");
          break;
      }
      case EVENT_STAMODE_DHCP_TIMEOUT:
      {
          uart0_sendStr("Wifi cb stammode DHCP timeout \n\r");
          break;
      }
      case EVENT_SOFTAPMODE_STACONNECTED:
      {
          uart0_sendStr("Wifi cb stammode softapmode statconnected \n\r");
          break;
      }
      case EVENT_SOFTAPMODE_PROBEREQRECVED:
      {
          uart0_sendStr("Wifi cb stammode EVENT_SOFTAPMODE_PROBEREQRECVED \n\r");
          break;
      }
      case EVENT_MAX:
      {
          uart0_sendStr("Wifi cb stammode EVENT_MAX \n\r");
          break;
      }
      case EVENT_STAMODE_GOT_IP:
      {
          uart0_sendStr("Wifi cb stammode got IP \n\r");
 /*         espconn.type = ESPCONN_UDP;
          espconn.state = ESPCONN_NONE;
          espconn.proto.udp = &my_udp;
          espconn.proto.udp->local_port = espconn_port();
          espconn.proto.udp->remote_port = 1234;
          espconn.proto.udp->remote_ip[0] = 192;
          espconn.proto.udp->remote_ip[1] = 168;
          espconn.proto.udp->remote_ip[2] = 1;
          espconn.proto.udp->remote_ip[3] = 5;
          char tab4[] = {'c', 'u', espconn_create(&espconn), '\n', '\r', '\0'}; */
/*
          espconn.type = ESPCONN_TCP;
          espconn.state = ESPCONN_NONE;
          espconn.proto.tcp = &my_tcp;
          espconn.proto.tcp->local_port = espconn_port();
          espconn.proto.tcp->remote_port = 1234;
          espconn.proto.tcp->remote_ip[0] = 192;
          espconn.proto.tcp->remote_ip[1] = 168;
          espconn.proto.tcp->remote_ip[2] = 1;
          espconn.proto.tcp->remote_ip[3] = 5;
          char tab4[] = {'c', 't', espconn_connect(&espconn), '\n', '\r', '\0'};
          uart0_sendStr(tab4);
          espconn_regist_connectcb(&espconn, tcp_conntected);
          espconn_regist_disconcb(&espconn, tcp_disconntected);*/

          espconn.type = ESPCONN_TCP;
          espconn.state = ESPCONN_NONE;
          espconn.proto.tcp = &my_tcp;
          espconn.proto.tcp->local_port = 1234;

          char tab4[] = {'a', 't', espconn_accept(&espconn) + 48, '\n', '\r', '\0'};
          uart0_sendStr(tab4);
          espconn_regist_connectcb(&espconn, tcp_conntected);
          espconn_regist_disconcb(&espconn, tcp_disconntected);
          espconn_regist_sentcb(&espconn, sent_callback);
          espconn_regist_recvcb(&espconn, recv_callback);
      }
      default:
      {
          uart0_sendStr("Wifi cb default \n\r");
          break;
      }
  }
}

void some_timerfunc(void *arg)
{

  //Do blinky stuff
  //if (GPIO_REG_READ(GPIO_OUT_ADDRESS) & (1 << pin))
  //{
    // set gpio low
	uint8_t status = wifi_station_get_connect_status();
	char tab[] = {'S', 'C', 'S', ':', status + 48, '\n', '\r', '\0'};
	uart0_sendStr(tab);

	if (GPIO_REG_READ(GPIO_OUT_ADDRESS) & BIT2) {
 	    gpio_output_set(BIT2, 0, BIT2, 0);
	}
	else {
 	    gpio_output_set(0, BIT2, BIT2, 0);
	}

	/*	if (status==5)
	{
      uint8_t tab3[10] = {49, 50, 51, 51, 52, 53, 56, 77, 91, 61};
  	  char tab1[] = {'s', 'u', espconn_sent(&espconn, tab3, 10), '\n', '\r', '\0'};
  	  uart0_sendStr(tab1);
	}*/

    char tab1[] = {'s', 't', 'a', espconn.state + 48, '\n', '\r', '\0'};
	uart0_sendStr(tab1);

//	char tab2[] = {'u', 'c', 's', espconn.state + 48, '\n', '\r', '\0'};
//	uart0_sendStr(tab2);

	//}
  //else
  //{
    // set gpio high
  //}
}


void ICACHE_FLASH_ATTR user_init()
{
  // init gpio sussytem
  gpio_init();

  //Set GPIO2 to output mode
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);

  //Set GPIO2 low
  gpio_output_set(0, BIT2, BIT2, 0);




  struct station_config wifi_conf;
  struct ip_info info;

  //  espconn.proto.udp->local_ip
//  info.ip.addr

  //  IP4_ADDR(espconn.udp->remote_ip, 192, 168, 1, 5)

/*  espconn.proto.udp->local_ip[0] = 192;
  espconn.proto.udp->local_ip[1] = 168;
  espconn.proto.udp->local_ip[2] = 1;
  espconn.proto.udp->local_ip[3] = 4;*/

  wifi_set_opmode(0x01);
  uart_init(BIT_RATE_115200, BIT_RATE_115200);

  uart0_sendStr("\n\r Siema \n\r");

  os_memcpy(wifi_conf.ssid, "NETIASPOT-D93A70", 32);
  os_memcpy(wifi_conf.password, "5z6uu24axp7e", 64);
  wifi_conf.bssid_set=0;

  char tab[] = {'s', 'c', wifi_station_set_config(&wifi_conf) + 48, '\n', '\r', '\0'};
  uart0_sendStr(tab);
  wifi_set_event_handler_cb(wifi_callback);
//  char tab2[] = {'i', 'p', wifi_get_ip_info(0, &info)+48, '\n', '\r', '\0'};
//  uart0_sendStr(tab2);
  //uart0_sendStr("ip:" ",mask:");
  /*  char tab3[] = {'i', 'p', 'a', ((uint8_t)(info.ip.addr&0xFF))+48,
		         ((uint8_t)(info.ip.addr>>8&0xFF)),
		         ((uint8_t)(info.ip.addr>>16&0xFF)),
		         ((uint8_t)(info.ip.addr>>24&0xFF)), '\n', '\r', '\0'};
  uart0_sendStr(tab3);*/


  // setup timer (500ms, repeating)
  os_timer_setfn(&some_timer, (os_timer_func_t *)some_timerfunc, NULL);
  os_timer_arm(&some_timer, 10000, 1);
}

