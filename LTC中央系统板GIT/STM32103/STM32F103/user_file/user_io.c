#include "user_io.h"
#include "user_config.h"

struct status status = {0};

void user_io_init(void)
{

}

void get_bcd()
{
	status.id = 0; /* ¸üĞÂid */
	if(GET_ID_1())
	status.id = status.id + 1;
	if(GET_ID_2())
	status.id = status.id + 2;
	if(GET_ID_4())
	status.id = status.id + 4;
	if(GET_ID_8())
	status.id = status.id + 8;
	if(GET_ID_10())
	status.id = status.id + 10;
	if(GET_ID_20())
	status.id = status.id + 20;
	if(GET_ID_40())
	status.id = status.id + 40;
	if(GET_ID_80())
	status.id = status.id + 80;
}	
