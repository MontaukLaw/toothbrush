#ifndef _BSP_HMC5883_H_
#define _BSP_HMC5883_H_

#define HMC58X3_R_CONFA 0
#define HMC58X3_R_CONFB 1
#define HMC58X3_R_MODE  2

float GetMagX(void);
float GetMagY(void);
float GetMagZ(void);

void InitHMC5883();
void updateHMC5883();

#endif
