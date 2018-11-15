#ifndef __MCP4725_H
#define __MCP4725_H

#include "sys.h"

#define MCP4725_ADD 0xC0






void Mcp4725Init(void);
void WriteMcp4725(u16 value);
void Mcp4725SetOutPutVoltage(float voltage);

































#endif
