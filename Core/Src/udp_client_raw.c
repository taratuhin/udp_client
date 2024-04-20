
/**
 *   \file     udp_client_raw.c
 *   \version  0.02
 *   \date     2024.04.13
 */

#include  <stdio.h>
#include  <string.h>
#include  "lwip/pbuf.h"
#include  "lwip/udp.h"
#include  "udp_client_raw.h"
#include  "gpio.h"


static  struct  udp_pcb  * upcb = NULL;
static  int  counter = 0;


static  void  udp_receive_callback( void  * arg, struct  udp_pcb  * upcb, struct  pbuf  * p, const  ip_addr_t  * addr, u16_t  port );


/**
 *   \brief   Настройка соединения
 *   \param   Нет
 *   \return  Код ошибки
 */
err_t  udp_client_connect( void )
{
    err_t  err = ERR_OK;
    ip_addr_t  server_ip_addr;


    upcb = udp_new();

    if ( upcb != NULL )
    {
        IP_ADDR4( &server_ip_addr, 192, 168, 0, 1 );
        err = udp_connect( upcb, &server_ip_addr, 3333 );

        if ( ERR_OK == err )
        {
            udp_recv( upcb, udp_receive_callback, NULL );
        }
    }

    return  err;
}


/**
 *   \brief   Отправка пакета
 *   \param   Нет
 *   \return  Код ошибки
 */
err_t  udp_client_send( void )
{
    struct  pbuf  * tx_buf = NULL;
    err_t  err = ERR_OK;
    char  data[100];
    int  len = sprintf( data, "Hello, World %d!", counter );


    len++;
	if ( upcb != NULL )
	{
        tx_buf = pbuf_alloc( PBUF_TRANSPORT, len, PBUF_RAM );

        if ( tx_buf != NULL )
        {
        	err = pbuf_take( tx_buf, data, len );

        	if ( ERR_OK == err )
        	{
        	    err = udp_send( upcb, tx_buf );
        	}

            pbuf_free( tx_buf );
        }
	}

	return  err;
}


/**
 *   \brief   Получение пакета
 *   \param  *arg - предоставленный пользователем аргумент (udp_pcb.recv_arg)
 *   \param  *upcb - идентификатор соединения
 *   \param  *p - буфер содержащий пакет
 *   \param  *addr - IP-адрес абонента от которого был получен пакет
 *   \param  *port - порт абонента от которого был получен пакет
 *   \return  Нет
 */
static  void  udp_receive_callback( void  * arg, struct  udp_pcb  * upcb, struct  pbuf  * p, const  ip_addr_t  * addr, u16_t  port )
{
	char  buffer[100];


	strncpy( buffer, ( char  * ) p->payload, p->len );

	counter++;

	pbuf_free( p );

	HAL_GPIO_TogglePin( GPIOB, GREEN_LED_Pin );

	if ( HAL_GPIO_ReadPin( GPIOB, RED_LED_Pin ) == GPIO_PIN_SET )
	{
		HAL_GPIO_WritePin( GPIOB, RED_LED_Pin, GPIO_PIN_RESET );
	}
}
