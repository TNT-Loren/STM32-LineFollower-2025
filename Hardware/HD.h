#ifndef  __HD_H__
#define  __HD_H__

void HD_Init(void);
extern int Keep_Ok, Yaw_ok;

extern int  HD[7];
extern char  HD_State;
extern float HD_Out, HD_Out1, HD_Out2;

void HD_Read(void);
#endif

