/*
 * SelfTestConfig.h
 *
 *  Created on: 07.04.2021
 *      Author: kraemere
 */

#ifndef SELFTEST_CONFIG_H_
#define SELFTEST_CONFIG_H_


//*******************************************
//* Set the selftest actions active         *
#define SELFTEST_S_ENABLE       1   // The selftests that run once at startup

#if ! SELFTEST_S_ENABLE             // Cyclic selftest may only run if startup test is activated !
    #define SELFTEST_C_ENABLE   0   // The selftests that run cyclically in main loop
#else
    #define SELFTEST_C_ENABLE   1
#endif

//*******************************************
//* Defines for inclusion of startup tests  *

#define EXEC_STARTUP_CPUREG             1u          // Execute CPU_Reg test
#define EXEC_STARTUP_CPUPC              1u          // Execute CPU_PC test
#define EXEC_STARTUP_TIMEBASE           1u          // Execute Interrupt test
#define EXEC_STARTUP_RAM                0u          // Execute Ram test
#define EXEC_STARTUP_STACK              1u          // Execute Stack test
#define EXEC_STARTUP_FLASH              0u          // Execute Flash test
#define EXEC_STARTUP_IO                 0u          // Execute IO test
#define EXEC_STARTUP_ADC                0u          // Execute ADC test
#define EXEC_STARTUP_UART               0u          // Do NOT execute UART test

//*******************************************
//* Defines for inclusion of cyclic tests   *

#define EXEC_CYCLIC_CPUREG              1u          // Execute CPU_Reg test
#define EXEC_CYCLIC_CPUPC               1u          // Execute CPU_PC test
#define EXEC_CYCLIC_TIMEBASE            1u          // Execute Interrupt test
#define EXEC_CYCLIC_RAM                 0u          // Execute SRAM March tests
#define EXEC_CYCLIC_STACK               1u          // Execute Stack March tests
#define EXEC_CYCLIC_STACKOVF            0u          // Execute Stack Overflow test
#define EXEC_CYCLIC_FLASH               0u          // Execute Flash test
#define EXEC_CYCLIC_IO                  0u          // Do NOT execute IO test on LV
#define EXEC_CYCLIC_ADC                 0u          // Do NOT execute ADC test on LV
#define EXEC_CYCLIC_UART                0u          // Do NOT execute UART test on LV

#if (EXEC_STARTUP_CPUREG == 1 || EXEC_CYCLIC_CPUREG == 1 || EXEC_STARTUP_CPUPC == 1 || EXEC_CYCLIC_CPUPC == 1)
    #define HAL_SELFTEST_CPU    
#endif

#if (EXEC_STARTUP_TIMEBASE == 1 || EXEC_CYCLIC_TIMEBASE == 1)
    #define HAL_SELFTEST_TIMEBASE
#endif

#if (EXEC_STARTUP_RAM == 1 || EXEC_CYCLIC_RAM == 1)
    #define HAL_SELFTEST_RAM   
#endif

#if (EXEC_STARTUP_STACK == 1 || EXEC_CYCLIC_STACK == 1 || EXEC_CYCLIC_STACKOVF)
    #define HAL_SELFTEST_STACK 
#endif

#if (EXEC_STARTUP_FLASH == 1 || EXEC_CYCLIC_FLASH == 1)
    #define HAL_SELFTEST_FLASH    
#endif

#if (EXEC_STARTUP_IO == 1 || EXEC_CYCLIC_IO == 1)
    #define HAL_SELFTEST_IO    
#endif

#if (EXEC_STARTUP_ADC == 1 || EXEC_CYCLIC_ADC == 1)
    #define HAL_SELFTEST_ADC
#endif

#if (EXEC_STARTUP_UART == 1 || EXEC_CYCLIC_UART == 1)
    #define HAL_SELFTEST_UART
#endif

#endif /* SELFTEST_CONFIG_H_ */
