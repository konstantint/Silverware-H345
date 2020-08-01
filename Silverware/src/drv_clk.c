#include "project.h"
#include "binary.h"
#include "config.h"

// the clock is already set, we turn pll off and set a new multiplier
void setclock(void)
{

}


void clk_init(void)
{

		SYS_UnlockReg();

		// Configure the PLL
		CLK_EnablePLL(CLK_PLLCTL_PLLSRC_HIRC, SYS_CLOCK_FREQ_HZ);
		// Set the HCLK divider to 1
		CLK->CLKDIV = (CLK->CLKDIV & ~CLK_CLKDIV_HCLKDIV_Msk) | CLK_CLKDIV_HCLK(1);
		// Select PLL as HCLK source
		CLK->CLKSEL0 = (CLK->CLKSEL0 & ~CLK_CLKSEL0_HCLKSEL_Msk) | CLK_CLKSEL0_HCLKSEL_PLL;
		SystemCoreClockUpdate();

		SYS_LockReg();
}

