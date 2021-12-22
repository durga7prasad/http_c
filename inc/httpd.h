
#ifndef _HTTPD_H_
#define _HTTPD_H_

#include <stdio.h>

#define MAX_BUFF_LEN 	(8*1024)

extern int gsport;
extern int gsfd;
extern int gcfd;
extern unsigned char grx_data[MAX_BUFF_LEN];

#define HTTPD_BACKLOG	10

int httpd_init(void);

int httpd_server(void);

#endif	/* _HTTPD_H_ */
