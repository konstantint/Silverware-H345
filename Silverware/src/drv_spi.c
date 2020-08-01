

#include "project.h"
#include "drv_spi.h"
#include "binary.h"
#include "config.h"

#ifdef SOFTSPI_4WIRE

void spi_init(void)
{    
		SYS->P0_MFP |= SYS_MFP_P01_GPIO; // SPI CS
		SYS->P0_MFP |= SYS_MFP_P07_SPI0_CLK|SYS_MFP_P05_SPI0_MOSI|SYS_MFP_P06_SPI0_MISO;
		SYS->P5_MFP |= SYS_MFP_P52_GPIO;

		GPIO_SetMode(P0, BIT1, GPIO_MODE_OUTPUT);// SPI CS
		GPIO_SetMode(P5, BIT2, GPIO_MODE_INPUT); // SPI IRQ

		CLK_EnableModuleClock(SPI0_MODULE);
		CLK_SetModuleClock(SPI0_MODULE,CLK_CLKSEL1_SPISEL_HCLK,1);

		SPI_Open(SPI0, SPI_MASTER, SPI_MODE_0, 8, 2000000);
		SPI_DisableAutoSS(SPI0);
		SPI_WRITE_TX(SPI0, 0);
}


void spi_cson( )
{
		P01 = 0;
}

void spi_csoff( )
{
		P01 = 1;
}


void spi_sendbyte ( int data)
{
	    SPI_WRITE_TX(SPI0, data);
	    SPI_TRIGGER(SPI0);
	    while(SPI_IS_BUSY(SPI0));
	    return;
}


 int spi_sendrecvbyte( int data)
{ 	   
	    SPI_WRITE_TX(SPI0, data);
	    SPI_TRIGGER(SPI0);
	    while(SPI_IS_BUSY(SPI0));
	    return SPI0->RX;
}


 int spi_sendzerorecvbyte( )
{
	return spi_sendrecvbyte(0);
}


#endif









