#ifndef _SERVE_H_
#define _SERVE_H_

_Bool OneNet_DevLink(void);
void OneNet_Subscribe(const char *topics[], unsigned char topic_cnt);
void OneNet_Publish(const char *topic, const char *msg);
void OneNet_RevPro(unsigned char *cmd);
void OneNet_Ping(void);

#endif
