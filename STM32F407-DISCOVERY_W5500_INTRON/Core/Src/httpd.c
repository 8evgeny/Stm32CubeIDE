#include "httpd.h"
#include "lfs.h"
//-----------------------------------------------
extern UART_HandleTypeDef huart6;
//-----------------------------------------------
extern char str1[60];
extern char tmpbuf[30];
extern uint8_t sect[515];
extern uint8_t ipaddr[4];
extern uint8_t ipgate[4];
extern uint8_t ipmask[4];
extern uint8_t destip[4];
extern uint8_t sdCartOn;
extern char MD5[32];
uint8_t temp[4];
uint8_t passwordOK = 0;
uint8_t loginOK = 0;
extern uint32_t indexLen;
extern uint32_t mainLen;
uint32_t f_size;
extern char *pindex;
extern char *pmain;
extern char *psettingsIP;

char * pfile;
extern void UART_Printf(const char* fmt, ...);
extern lfs_t lfs;
extern lfs_file_t file;
//-----------------------------------------------
http_sock_prop_ptr httpsockprop[2];
tcp_prop_ptr tcpprop;
extern void Printf(const char* fmt, ...);
//extern FATFS fs;
extern FIL fil;
FIL MyFile;

FRESULT result; //результат выполнения
uint32_t bytesread;
volatile uint16_t tcp_size_wnd = 512; //было 2048  - были ошибки передачи случайный байт
//-----------------------------------------------
const char http_header[] = { "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"};
const char jpg_header[] = {"HTTP/1.0 200 OK\r\nServer: nginx\r\nContent-Type: image/jpeg\r\nConnection: close\r\n\r\n"};
const char icon_header[] = { "HTTP/1.1 200 OK\r\nContent-Type: image/x-icon\r\n\r\n"};
const char error_header[] = {"HTTP/1.0 404 File not found\r\nServer: nginx\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n"};
char *header;
//-----------------------------------------------
const uint8_t e404_htm[] = {
0x3c,0x68,0x74,0x6d,0x6c,0x3e,0x0a,0x20,0x20,0x3c,0x68,0x65,0x61,0x64,0x3e,0x0a,
0x20,0x20,0x20,0x20,0x3c,0x74,0x69,0x74,0x6c,0x65,0x3e,0x34,0x30,0x34,0x20,0x4e,
0x6f,0x74,0x20,0x46,0x6f,0x75,0x6e,0x64,0x3c,0x2f,0x74,0x69,0x74,0x6c,0x65,0x3e,
0x0a,0x20,0x20,0x3c,0x2f,0x68,0x65,0x61,0x64,0x3e,0x0a,0x3c,0x62,0x6f,0x64,0x79,
0x3e,0x0a,0x3c,0x68,0x31,0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x74,0x65,0x78,
0x74,0x2d,0x61,0x6c,0x69,0x67,0x6e,0x3a,0x20,0x63,0x65,0x6e,0x74,0x65,0x72,0x3b,
0x22,0x3e,0x34,0x30,0x34,0x20,0x45,0x72,0x72,0x6f,0x72,0x20,0x46,0x69,0x6c,0x65,
0x20,0x4e,0x6f,0x74,0x20,0x46,0x6f,0x75,0x6e,0x64,0x3c,0x2f,0x68,0x31,0x3e,0x0a,
0x3c,0x68,0x32,0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x74,0x65,0x78,0x74,0x2d,
0x61,0x6c,0x69,0x67,0x6e,0x3a,0x20,0x63,0x65,0x6e,0x74,0x65,0x72,0x3b,0x22,0x3e,
0x20,0x54,0x68,0x65,0x20,0x70,0x61,0x67,0x65,0x20,0x79,0x6f,0x75,0x20,0x61,0x72,
0x65,0x20,0x6c,0x6f,0x6f,0x6b,0x69,0x6e,0x67,0x20,0x66,0x6f,0x72,0x20,0x6d,0x69,
0x67,0x68,0x74,0x20,0x68,0x61,0x76,0x65,0x20,0x62,0x65,0x65,0x6e,0x20,0x72,0x65,
0x6d,0x6f,0x76,0x65,0x64,0x2c,0x20,0x3c,0x62,0x72,0x20,0x2f,0x3e,0x68,0x61,0x64,
0x20,0x69,0x74,0x73,0x20,0x6e,0x61,0x6d,0x65,0x20,0x63,0x68,0x61,0x6e,0x67,0x65,
0x64,0x2c,0x20,0x6f,0x72,0x20,0x69,0x73,0x20,0x74,0x65,0x6d,0x70,0x6f,0x72,0x61,
0x72,0x69,0x6c,0x79,0x20,0x75,0x6e,0x61,0x76,0x61,0x69,0x6c,0x61,0x62,0x6c,0x65,
0x2e,0x3c,0x2f,0x68,0x32,0x3e,0x0a,0x3c,0x2f,0x62,0x6f,0x64,0x79,0x3e,0x3c,0x2f,
0x68,0x74,0x6d,0x6c,0x3e};
//-----------------------------------------------
void tcp_send_http_one(void)
{
  uint16_t i=0;
  uint16_t data_len=0;
  uint16_t header_len=0;
  uint16_t end_point;
  uint8_t num_sect=0;
  uint16_t len_sect;
	if ((httpsockprop[tcpprop.cur_sock].http_doc==EXISTING_HTML)||\
		(httpsockprop[tcpprop.cur_sock].http_doc==EXISTING_JPG)||\
		(httpsockprop[tcpprop.cur_sock].http_doc==EXISTING_ICO))
	{
		switch(httpsockprop[tcpprop.cur_sock].http_doc)
		{
			case EXISTING_HTML:
				header = (void*)http_header;
				break;
			case EXISTING_ICO:
				header = (void*)icon_header;
				break;
			case EXISTING_JPG:
				header = (void*)jpg_header;
				break;
		}
		header_len = strlen(header);

        if (sdCartOn == 1)
            data_len = (uint16_t)f_size(&MyFile);
        if (sdCartOn == 0)
            data_len = f_size;

		end_point = GetWritePointer(tcpprop.cur_sock);
		end_point+=header_len+data_len;
		//Заполним данными буфер для отправки пакета
		SetWritePointer(tcpprop.cur_sock, end_point);
		end_point = GetWritePointer(tcpprop.cur_sock);
		memcpy(sect+3,header,header_len);
		w5500_writeSockBuf(tcpprop.cur_sock, end_point, (uint8_t*)sect, header_len);
		end_point+=header_len;
		num_sect = data_len / 512;
		for(i=0;i<=num_sect;i++)
		{
			//не последний сектор
			if(i<(num_sect-1)) len_sect=512;
			else len_sect=data_len;

            if (sdCartOn == 1)
            {
                result=f_lseek(&MyFile, i*512); //Установим курсор чтения в файле
                result=f_read(&MyFile,sect+3, len_sect,(UINT *)&bytesread);
            }
            else
            {
                strncpy((char*)sect+3, pfile + i*512, len_sect);
            }

			w5500_writeSockBuf(tcpprop.cur_sock, end_point, (uint8_t*)sect, len_sect);
			end_point+=len_sect;
			data_len -= len_sect;
		}
	}
	else
	{
		header_len = strlen(error_header);
		data_len = sizeof(e404_htm);
		end_point = GetWritePointer(tcpprop.cur_sock);
		end_point+=header_len+data_len;
		SetWritePointer(tcpprop.cur_sock, end_point);
		end_point = GetWritePointer(tcpprop.cur_sock);
		//Заполним данными буфер для отправки пакета
		memcpy(sect+3,error_header,header_len);
		w5500_writeSockBuf(tcpprop.cur_sock, end_point, (uint8_t*)sect, header_len);
		end_point+=header_len;
		memcpy(sect+3,e404_htm,data_len);
		w5500_writeSockBuf(tcpprop.cur_sock, end_point, (uint8_t*)sect, data_len);
		end_point+=data_len;
	}
	//отправим данные
  RecvSocket(tcpprop.cur_sock);
  SendSocket(tcpprop.cur_sock);
  httpsockprop[tcpprop.cur_sock].data_stat = DATA_COMPLETED;
}
//-----------------------------------------------
void tcp_send_http_first(void)
{
  uint8_t prt;
  uint16_t i=0;
  uint16_t data_len=0;
  uint16_t header_len=0;
  uint16_t end_point;
  uint8_t num_sect=0;
  uint16_t len_sect;
  uint16_t last_part, last_part_size;
	//На EXISTING проверять не будем, так как будем считать, что error404_htm у нас всегда будет умещаться в один буфер
	switch(httpsockprop[tcpprop.cur_sock].http_doc)
	{
		case EXISTING_HTML:
			header = (void*)http_header;
			break;
		case EXISTING_ICO:
			header = (void*)icon_header;
			break;
		case EXISTING_JPG:
			header = (void*)jpg_header;
			break;
	}
	header_len = strlen(header);
	data_len = tcp_size_wnd-header_len;
	end_point = GetWritePointer(tcpprop.cur_sock);
	end_point+=header_len+data_len;
	SetWritePointer(tcpprop.cur_sock, end_point);
	end_point = GetWritePointer(tcpprop.cur_sock);
	//сохраним некоторые параметры, а то почему-то они теряются
	last_part = httpsockprop[tcpprop.cur_sock].cnt_rem_data_part;
	last_part_size = httpsockprop[tcpprop.cur_sock].last_data_part_size;
	prt = httpsockprop[tcpprop.cur_sock].prt_tp;
	//Заполним данными буфер для отправки пакета
    memcpy(sect+3, header, header_len);
	w5500_writeSockBuf(tcpprop.cur_sock, end_point, (uint8_t*)sect, header_len);
	end_point+=header_len;
	num_sect = data_len / 512;
    for(i=0;i <= num_sect; i++)
	{
		//не последний сектор
		if(i<(num_sect-1)) len_sect=512;
		else len_sect=data_len;
        if (sdCartOn == 1)
        {
            result=f_lseek(&MyFile,i*512); //Установим курсор чтения в файле
            result=f_read(&MyFile,sect+3,len_sect,(UINT *)&bytesread);
        }
        else
        {
            strncpy((char*)sect+3, pfile + i*512, len_sect);
        }
		w5500_writeSockBuf(tcpprop.cur_sock, end_point, (uint8_t*)sect, len_sect);
		end_point+=len_sect;
		data_len -= len_sect;
	}
	//вернем параметры
	httpsockprop[tcpprop.cur_sock].cnt_rem_data_part = last_part;
	httpsockprop[tcpprop.cur_sock].last_data_part_size = last_part_size;
	httpsockprop[tcpprop.cur_sock].prt_tp = prt;
	//отправим данные
	RecvSocket(tcpprop.cur_sock);
	SendSocket(tcpprop.cur_sock);
	//будем считать, что одну часть отправили, поэтому количество оставшихся частей декрементируем
	httpsockprop[tcpprop.cur_sock].cnt_rem_data_part--;
	if(httpsockprop[tcpprop.cur_sock].cnt_rem_data_part>1)
	{
		httpsockprop[tcpprop.cur_sock].data_stat=DATA_MIDDLE;
	}
	else
	{
		httpsockprop[tcpprop.cur_sock].data_stat=DATA_LAST;
	}
	//Количество переданных байтов
  httpsockprop[tcpprop.cur_sock].total_count_bytes = tcp_size_wnd - header_len;

//  Printf("tcp_send_http_first");
}
//-----------------------------------------------
void tcp_send_http_middle(void)
{
  uint16_t i=0;
  uint16_t data_len=0;
  uint16_t end_point;
  uint8_t num_sect=0;
  uint8_t prt;
  uint16_t len_sect;
  uint16_t last_part, last_part_size;
  uint32_t count_bytes;
	data_len = tcp_size_wnd;
	end_point = GetWritePointer(tcpprop.cur_sock);
	end_point+=data_len;
	SetWritePointer(tcpprop.cur_sock, end_point);
	end_point = GetWritePointer(tcpprop.cur_sock);
	//сохраним некоторые параметры, а то почему-то они теряются
	last_part = httpsockprop[tcpprop.cur_sock].cnt_rem_data_part;
	last_part_size = httpsockprop[tcpprop.cur_sock].last_data_part_size;
	count_bytes = httpsockprop[tcpprop.cur_sock].total_count_bytes;
	prt = httpsockprop[tcpprop.cur_sock].prt_tp;
	//Заполним данными буфер для отправки пакета
	num_sect = data_len / 512;
	//борьба с неправильным расчётом, когда размер данных делится на размер сектора без остатка
	if(data_len%512==0) num_sect--;
    for(i=0; i <= num_sect; i++)
	{
		//не последний сектор
		if(i<(num_sect-1)) len_sect=512;
		else len_sect=data_len;
        if (sdCartOn == 1)
        {
            result=f_lseek(&MyFile,(DWORD)(i*512) + count_bytes); //Установим курсор чтения в файле
            result=f_read(&MyFile,sect+3,len_sect,(UINT *)&bytesread);
        }
        else
        {
            strncpy((char*)sect+3, pfile + ((DWORD)(i*512) + count_bytes), len_sect);
        }
//HAL_UART_Transmit(&huart6,(uint8_t*)sect+3,len_sect,0x1000);
//HAL_UART_Transmit(&huart6,(uint8_t*)"\r\n** block **\r\n", 15, 0x1000);
		w5500_writeSockBuf(tcpprop.cur_sock, end_point, (uint8_t*)sect, len_sect);
		end_point+=len_sect;
		data_len -= len_sect;
	}
	//вернем параметры
	httpsockprop[tcpprop.cur_sock].cnt_rem_data_part = last_part;
	httpsockprop[tcpprop.cur_sock].last_data_part_size = last_part_size;
	httpsockprop[tcpprop.cur_sock].total_count_bytes = count_bytes;
	httpsockprop[tcpprop.cur_sock].prt_tp = prt;
	RecvSocket(tcpprop.cur_sock);
	SendSocket(tcpprop.cur_sock);
	//будем считать, что одну часть отправили, поэтому количество оставшихся частей декрементируем
	httpsockprop[tcpprop.cur_sock].cnt_rem_data_part--;
	if(httpsockprop[tcpprop.cur_sock].cnt_rem_data_part>1)
	{
		httpsockprop[tcpprop.cur_sock].data_stat=DATA_MIDDLE;
	}
	else
	{
		httpsockprop[tcpprop.cur_sock].data_stat=DATA_LAST;
	}
	//Количество переданных байтов
	httpsockprop[tcpprop.cur_sock].total_count_bytes += (uint32_t) tcp_size_wnd;

//    UART_Printf("tcp_send_http_middle"); delayUS_ASM(10000);
}
//-----------------------------------------------
void tcp_send_http_last(void)
{
  uint16_t i=0;
  uint16_t data_len=0;
  uint16_t end_point;
  uint8_t num_sect=0;
  uint16_t len_sect;
	data_len = httpsockprop[tcpprop.cur_sock].last_data_part_size;
	end_point = GetWritePointer(tcpprop.cur_sock);
	end_point+=data_len;
	SetWritePointer(tcpprop.cur_sock, end_point);
	end_point = GetWritePointer(tcpprop.cur_sock);
	//Заполним данными буфер для отправки пакета
	num_sect = data_len / 512;
	//борьба с неправильным расчётом, когда размер данных делится на размер сектора без остатка
	if(data_len%512==0) num_sect--;
	for(i=0;i<=num_sect;i++)
	{
		//не последний сектор
		if(i<(num_sect-1)) len_sect=512;
		else len_sect=data_len;
        if (sdCartOn == 1)
        {
            result=f_lseek(&MyFile, (DWORD)(i*512) + httpsockprop[tcpprop.cur_sock].total_count_bytes); //Установим курсор чтения в файле
            result=f_read(&MyFile,sect+3, len_sect, (UINT *)&bytesread);
        }
        else //EEPROM
        {
            strncpy((char*)sect+3, pfile + (DWORD)(i*512) + httpsockprop[tcpprop.cur_sock].total_count_bytes, len_sect);
        }
		w5500_writeSockBuf(tcpprop.cur_sock, end_point, (uint8_t*)sect, len_sect);
		end_point+=len_sect;
		data_len -= len_sect;
	}
	//отправим данные
	RecvSocket(tcpprop.cur_sock);
	SendSocket(tcpprop.cur_sock);
	httpsockprop[tcpprop.cur_sock].data_stat = DATA_COMPLETED;

//    UART_Printf("tcp_send_http_last"); delayUS_ASM(10000);
}
//-----------------------------------------------
void http_request(void)
{
  uint16_t point;
  uint8_t RXbyte;
  uint16_t i=0;
  char *ss1;
  int ch1='.';
 
  // ищем первый "/" в HTTP заголовке
  point = GetReadPointer(tcpprop.cur_sock);
  i = 0;

  while (RXbyte != (uint8_t)'/')
  {
    RXbyte = w5500_readSockBufByte(tcpprop.cur_sock,point+i);
    i++;
  }
	point+=i;
	RXbyte = w5500_readSockBufByte(tcpprop.cur_sock,point);
	if(RXbyte==(uint8_t)' ')
    {
        if ((passwordOK == 1)&&(loginOK == 1))
        {
            strcpy(httpsockprop[tcpprop.cur_sock].fname,"main.html");
        }
        else
        {
            strcpy(httpsockprop[tcpprop.cur_sock].fname,"index.html");
        }
		httpsockprop[tcpprop.cur_sock].http_doc = EXISTING_HTML;
	}
	else
	{
		// ищем следующий пробел (" ") в HTTP заголовке, таким образом считывая имя документа из запроса
		i=0;
		while (1)
		{
			tmpbuf[i] = w5500_readSockBufByte(tcpprop.cur_sock, point+i);
			if(tmpbuf[i] == (uint8_t)' ') break;
			i++;
		}
		tmpbuf[i] = 0; //закончим строку
		strcpy(httpsockprop[tcpprop.cur_sock].fname,tmpbuf);

        //В ЭТОМ МЕСТЕ ПАРСИМ ИЗМЕНЕНИЕ ПАРАМЕТРОВ
        HAL_UART_Transmit(&huart6,(uint8_t*)"\r\n",2,0x1000);
        HAL_UART_Transmit(&huart6,(uint8_t*)tmpbuf,strlen(tmpbuf),0x1000);
        HAL_UART_Transmit(&huart6,(uint8_t*)"\r\n",2,0x1000);

        if (tmpbuf[0] == '1')
        {
            char host_IP_1[5];char host_IP_2[5];char host_IP_3[5];char host_IP_4[5];
            char tmp[100];
    //        HAL_UART_Transmit(&huart6,(uint8_t*)"IP_HOST CHANGE\r\n",strlen("IP_HOST CHANGE\r\n"),0x1000);
            i=1;
            uint8_t j = 0;
            char oktet[3];
            while (1) {if(tmpbuf[i] == (uint8_t)'.') break; oktet[j] = tmpbuf[i]; i++; j++; }
            //i указывает на '.'  j - колл скопированных символов
            if (j == 3) {host_IP_1[0] = oktet[0]; host_IP_1[1] = oktet[1]; host_IP_1[2] = oktet[2]; host_IP_1[3] = '\n'; host_IP_1[4] = 0x00;}
            if (j == 2) {host_IP_1[0] = '0'; host_IP_1[1] = oktet[0]; host_IP_1[2] = oktet[1]; host_IP_1[3] = '\n'; host_IP_1[4] = 0x00;}
            if (j == 1) {host_IP_1[0] = '0'; host_IP_1[1] = '0'; host_IP_1[2] = oktet[0]; host_IP_1[3] = '\n'; host_IP_1[4] = 0x00;}
            temp[0] = atoi(host_IP_1);
            i++; j=0;
            while (1) {if(tmpbuf[i] == (uint8_t)'.') break; oktet[j] = tmpbuf[i]; i++; j++; }
            if (j == 3) {host_IP_2[0] = oktet[0]; host_IP_2[1] = oktet[1]; host_IP_2[2] = oktet[2]; host_IP_2[3] = '\n'; host_IP_2[4] = 0x00;}
            if (j == 2) {host_IP_2[0] = '0'; host_IP_2[1] = oktet[0]; host_IP_2[2] = oktet[1]; host_IP_2[3] = '\n'; host_IP_2[4] = 0x00;}
            if (j == 1) {host_IP_2[0] = '0'; host_IP_2[1] = '0'; host_IP_2[2] = oktet[0]; host_IP_2[3] = '\n'; host_IP_2[4] = 0x00;}
            temp[1] = atoi(host_IP_2);
            i++; j=0;
            while (1) {if(tmpbuf[i] == (uint8_t)'.') break; oktet[j] = tmpbuf[i]; i++; j++; }
            if (j == 3) {host_IP_3[0] = oktet[0]; host_IP_3[1] = oktet[1]; host_IP_3[2] = oktet[2]; host_IP_3[3] = '\n'; host_IP_3[4] = 0x00;}
            if (j == 2) {host_IP_3[0] = '0'; host_IP_3[1] = oktet[0]; host_IP_3[2] = oktet[1]; host_IP_3[3] = '\n'; host_IP_3[4] = 0x00;}
            if (j == 1) {host_IP_3[0] = '0'; host_IP_3[1] = '0'; host_IP_3[2] = oktet[0]; host_IP_3[3] = '\n'; host_IP_3[4] = 0x00;}
            temp[2] = atoi(host_IP_3);
            i++; j=0;
            while (1) {if(tmpbuf[i] == (uint8_t)'\0') break; oktet[j] = tmpbuf[i]; i++; j++; }
            if (j == 3) {host_IP_4[0] = oktet[0]; host_IP_4[1] = oktet[1]; host_IP_4[2] = oktet[2]; host_IP_4[3] = '\n'; host_IP_4[4] = 0x00;}
            if (j == 2) {host_IP_4[0] = '0'; host_IP_4[1] = oktet[0]; host_IP_4[2] = oktet[1]; host_IP_4[3] = '\n'; host_IP_4[4] = 0x00;}
            if (j == 1) {host_IP_4[0] = '0'; host_IP_4[1] = '0'; host_IP_4[2] = oktet[0]; host_IP_4[3] = '\n'; host_IP_4[4] = 0x00;}
            temp[3] = atoi(host_IP_4);

            if ((temp[0] == ipaddr[0])&&(temp[1] == ipaddr[1])&&(temp[2] == ipaddr[2])&&(temp[3] == ipaddr[3]))
            {
                 Printf("*****  hostIP not changed  *****\r\n");
            }
            else
            {
                ipaddr[0] = temp[0];
                ipaddr[1] = temp[1];
                ipaddr[2] = temp[2];
                ipaddr[3] = temp[3];
                sprintf(tmp,"new host IP: %d.%d.%d.%d\r\n",ipaddr[0],ipaddr[1],ipaddr[2],ipaddr[3]);
                Printf(tmp);
                if (sdCartOn == 1)
                {
                    FRESULT result = f_open(&fil, "host_IP", FA_OPEN_ALWAYS | FA_WRITE );
                    if (result == 0)
                    {
                        Printf("*****  write new host IP to SD  *****\r\n");
                        f_lseek(&fil, 0);
                        f_puts(host_IP_1, &fil);
                        f_puts(host_IP_2, &fil);
                        f_puts(host_IP_3, &fil);
                        f_puts(host_IP_4, &fil);
                        f_sync(&fil);
                        f_close(&fil);
                    }
                }
                else //EEPROM
                {
// !!!!!!!!!!!!!


                }
            }
        }

        if (tmpbuf[0] == '2')
        {
            char mask_IP_1[5];char mask_IP_2[5];char mask_IP_3[5];char mask_IP_4[5];
            char tmp[100];
    //        HAL_UART_Transmit(&huart6,(uint8_t*)"IP_HOST CHANGE\r\n",strlen("IP_HOST CHANGE\r\n"),0x1000);
            i=1;
            uint8_t j = 0;
            char oktet[3];
            while (1) {if(tmpbuf[i] == (uint8_t)'.') break; oktet[j] = tmpbuf[i]; i++; j++; }
            //i указывает на '.'  j - колл скопированных символов
            if (j == 3) {mask_IP_1[0] = oktet[0]; mask_IP_1[1] = oktet[1]; mask_IP_1[2] = oktet[2]; mask_IP_1[3] = '\n'; mask_IP_1[4] = 0x00;}
            if (j == 2) {mask_IP_1[0] = '0'; mask_IP_1[1] = oktet[0]; mask_IP_1[2] = oktet[1]; mask_IP_1[3] = '\n'; mask_IP_1[4] = 0x00;}
            if (j == 1) {mask_IP_1[0] = '0'; mask_IP_1[1] = '0'; mask_IP_1[2] = oktet[0]; mask_IP_1[3] = '\n'; mask_IP_1[4] = 0x00;}
            temp[0] = atoi(mask_IP_1);
            i++; j=0;
            while (1) {if(tmpbuf[i] == (uint8_t)'.') break; oktet[j] = tmpbuf[i]; i++; j++; }
            if (j == 3) {mask_IP_2[0] = oktet[0]; mask_IP_2[1] = oktet[1]; mask_IP_2[2] = oktet[2]; mask_IP_2[3] = '\n'; mask_IP_2[4] = 0x00;}
            if (j == 2) {mask_IP_2[0] = '0'; mask_IP_2[1] = oktet[0]; mask_IP_2[2] = oktet[1]; mask_IP_2[3] = '\n'; mask_IP_2[4] = 0x00;}
            if (j == 1) {mask_IP_2[0] = '0'; mask_IP_2[1] = '0'; mask_IP_2[2] = oktet[0]; mask_IP_2[3] = '\n'; mask_IP_2[4] = 0x00;}
            temp[1] = atoi(mask_IP_2);
            i++; j=0;
            while (1) {if(tmpbuf[i] == (uint8_t)'.') break; oktet[j] = tmpbuf[i]; i++; j++; }
            if (j == 3) {mask_IP_3[0] = oktet[0]; mask_IP_3[1] = oktet[1]; mask_IP_3[2] = oktet[2]; mask_IP_3[3] = '\n'; mask_IP_3[4] = 0x00;}
            if (j == 2) {mask_IP_3[0] = '0'; mask_IP_3[1] = oktet[0]; mask_IP_3[2] = oktet[1]; mask_IP_3[3] = '\n'; mask_IP_3[4] = 0x00;}
            if (j == 1) {mask_IP_3[0] = '0'; mask_IP_3[1] = '0'; mask_IP_3[2] = oktet[0]; mask_IP_3[3] = '\n'; mask_IP_3[4] = 0x00;}
            temp[2] = atoi(mask_IP_3);
            i++; j=0;
            while (1) {if(tmpbuf[i] == (uint8_t)'\0') break; oktet[j] = tmpbuf[i]; i++; j++; }
            if (j == 3) {mask_IP_4[0] = oktet[0]; mask_IP_4[1] = oktet[1]; mask_IP_4[2] = oktet[2]; mask_IP_4[3] = '\n'; mask_IP_4[4] = 0x00;}
            if (j == 2) {mask_IP_4[0] = '0'; mask_IP_4[1] = oktet[0]; mask_IP_4[2] = oktet[1]; mask_IP_4[3] = '\n'; mask_IP_4[4] = 0x00;}
            if (j == 1) {mask_IP_4[0] = '0'; mask_IP_4[1] = '0'; mask_IP_4[2] = oktet[0]; mask_IP_4[3] = '\n'; mask_IP_4[4] = 0x00;}
            temp[3] = atoi(mask_IP_4);

            if ((temp[0] == ipmask[0])&&(temp[1] == ipmask[1])&&(temp[2] == ipmask[2])&&(temp[3] == ipmask[3]))
            {
                 Printf("*****  maskIP not changed  *****\r\n");
            }
            else
            {
                ipmask[0] = temp[0];
                ipmask[1] = temp[1];
                ipmask[2] = temp[2];
                ipmask[3] = temp[3];

                sprintf(tmp,"new mask IP: %d.%d.%d.%d\r\n",ipmask[0],ipmask[1],ipmask[2],ipmask[3]);
                UART_Printf(tmp);    delayUS_ASM(10000);
                if (sdCartOn == 1)
                {
                    FRESULT result = f_open(&fil, "mask_IP", FA_OPEN_ALWAYS | FA_WRITE );
                    if (result == 0)
                    {
                        UART_Printf("*****  write new mask IP to SD  *****\r\n"); delayUS_ASM(10000);
                        f_lseek(&fil, 0); delayUS_ASM(10000);
                        f_puts(mask_IP_1, &fil); delayUS_ASM(10000);
                        f_puts(mask_IP_2, &fil); delayUS_ASM(10000);
                        f_puts(mask_IP_3, &fil); delayUS_ASM(10000);
                        f_puts(mask_IP_4, &fil); delayUS_ASM(10000);
                        f_sync(&fil); delayUS_ASM(10000);
                        f_close(&fil); delayUS_ASM(10000);
                    }
                }
                else //EEPROM
                {
// !!!!!!!!!!!!!


                }
            }
        }

        if (tmpbuf[0] == '3')
        {
            char gate_IP_1[5];char gate_IP_2[5];char gate_IP_3[5];char gate_IP_4[5];
            char tmp[100];
    //        HAL_UART_Transmit(&huart6,(uint8_t*)"IP_GATE CHANGE\r\n",strlen("IP_GATE CHANGE\r\n"),0x1000);
            i=1;
            uint8_t j = 0;
            char oktet[3];
            while (1) {if(tmpbuf[i] == (uint8_t)'.') break; oktet[j] = tmpbuf[i]; i++; j++; }
            //i указывает на '.'  j - колл скопированных символов
            if (j == 3) {gate_IP_1[0] = oktet[0]; gate_IP_1[1] = oktet[1]; gate_IP_1[2] = oktet[2]; gate_IP_1[3] = '\n'; gate_IP_1[4] = 0x00;}
            if (j == 2) {gate_IP_1[0] = '0'; gate_IP_1[1] = oktet[0]; gate_IP_1[2] = oktet[1]; gate_IP_1[3] = '\n'; gate_IP_1[4] = 0x00;}
            if (j == 1) {gate_IP_1[0] = '0'; gate_IP_1[1] = '0'; gate_IP_1[2] = oktet[0]; gate_IP_1[3] = '\n'; gate_IP_1[4] = 0x00;}
            temp[0] = atoi(gate_IP_1);
            i++; j=0;
            while (1) {if(tmpbuf[i] == (uint8_t)'.') break; oktet[j] = tmpbuf[i]; i++; j++; }
            if (j == 3) {gate_IP_2[0] = oktet[0]; gate_IP_2[1] = oktet[1]; gate_IP_2[2] = oktet[2]; gate_IP_2[3] = '\n'; gate_IP_2[4] = 0x00;}
            if (j == 2) {gate_IP_2[0] = '0'; gate_IP_2[1] = oktet[0]; gate_IP_2[2] = oktet[1]; gate_IP_2[3] = '\n'; gate_IP_2[4] = 0x00;}
            if (j == 1) {gate_IP_2[0] = '0'; gate_IP_2[1] = '0'; gate_IP_2[2] = oktet[0]; gate_IP_2[3] = '\n'; gate_IP_2[4] = 0x00;}
            temp[1] = atoi(gate_IP_2);
            i++; j=0;
            while (1) {if(tmpbuf[i] == (uint8_t)'.') break; oktet[j] = tmpbuf[i]; i++; j++; }
            if (j == 3) {gate_IP_3[0] = oktet[0]; gate_IP_3[1] = oktet[1]; gate_IP_3[2] = oktet[2]; gate_IP_3[3] = '\n'; gate_IP_3[4] = 0x00;}
            if (j == 2) {gate_IP_3[0] = '0'; gate_IP_3[1] = oktet[0]; gate_IP_3[2] = oktet[1]; gate_IP_3[3] = '\n'; gate_IP_3[4] = 0x00;}
            if (j == 1) {gate_IP_3[0] = '0'; gate_IP_3[1] = '0'; gate_IP_3[2] = oktet[0]; gate_IP_3[3] = '\n'; gate_IP_3[4] = 0x00;}
            temp[2] = atoi(gate_IP_3);
            i++; j=0;
            while (1) {if(tmpbuf[i] == (uint8_t)'\0') break; oktet[j] = tmpbuf[i]; i++; j++; }
            if (j == 3) {gate_IP_4[0] = oktet[0]; gate_IP_4[1] = oktet[1]; gate_IP_4[2] = oktet[2]; gate_IP_4[3] = '\n'; gate_IP_4[4] = 0x00;}
            if (j == 2) {gate_IP_4[0] = '0'; gate_IP_4[1] = oktet[0]; gate_IP_4[2] = oktet[1]; gate_IP_4[3] = '\n'; gate_IP_4[4] = 0x00;}
            if (j == 1) {gate_IP_4[0] = '0'; gate_IP_4[1] = '0'; gate_IP_4[2] = oktet[0]; gate_IP_4[3] = '\n'; gate_IP_4[4] = 0x00;}
            temp[3] = atoi(gate_IP_4);

            if ((temp[0] == ipgate[0])&&(temp[1] == ipgate[1])&&(temp[2] == ipgate[2])&&(temp[3] == ipgate[3]))
            {
                 Printf("*****  gateIP not changed  *****\r\n");
            }
            else
            {
                ipgate[0] = temp[0];
                ipgate[1] = temp[1];
                ipgate[2] = temp[2];
                ipgate[3] = temp[3];

                sprintf(tmp,"new gate IP: %d.%d.%d.%d\r\n",ipgate[0],ipgate[1],ipgate[2],ipgate[3]);
                Printf(tmp);
                if (sdCartOn == 1)
                {
                    FRESULT result = f_open(&fil, "gate_IP", FA_OPEN_ALWAYS | FA_WRITE );
                    if (result == 0)
                    {
                        UART_Printf("*****  write new gate IP to SD  *****\r\n");
                        delayUS_ASM(10000);
                        f_lseek(&fil, 0);
                        f_puts(gate_IP_1, &fil);
                        f_puts(gate_IP_2, &fil);
                        f_puts(gate_IP_3, &fil);
                        f_puts(gate_IP_4, &fil);
                        f_sync(&fil);
                        f_close(&fil);
                    }
                }
                else //EEPROM
                {
// !!!!!!!!!!!!!


                }
            }
        }

        if (tmpbuf[0] == '4')
        {
            char dest_IP_1[5];char dest_IP_2[5];char dest_IP_3[5];char dest_IP_4[5];
            char tmp[100];
//            HAL_UART_Transmit(&huart6,(uint8_t*)"IP_DEST CHANGE\r\n",strlen("IP_DEST CHANGE\r\n"),0x1000);
            i=1;
            uint8_t j = 0;
            char oktet[3];
            while (1) {if(tmpbuf[i] == (uint8_t)'.') break; oktet[j] = tmpbuf[i]; i++; j++; }
            //i указывает на '.'  j - колл скопированных символов
            if (j == 3) {dest_IP_1[0] = oktet[0]; dest_IP_1[1] = oktet[1]; dest_IP_1[2] = oktet[2]; dest_IP_1[3] = '\n'; dest_IP_1[4] = 0x00;}
            if (j == 2) {dest_IP_1[0] = '0'; dest_IP_1[1] = oktet[0]; dest_IP_1[2] = oktet[1]; dest_IP_1[3] = '\n'; dest_IP_1[4] = 0x00;}
            if (j == 1) {dest_IP_1[0] = '0'; dest_IP_1[1] = '0'; dest_IP_1[2] = oktet[0]; dest_IP_1[3] = '\n'; dest_IP_1[4] = 0x00;}
            temp[0] = atoi(dest_IP_1);
            i++; j=0;
            while (1) {if(tmpbuf[i] == (uint8_t)'.') break; oktet[j] = tmpbuf[i]; i++; j++; }
            if (j == 3) {dest_IP_2[0] = oktet[0]; dest_IP_2[1] = oktet[1]; dest_IP_2[2] = oktet[2]; dest_IP_2[3] = '\n'; dest_IP_2[4] = 0x00;}
            if (j == 2) {dest_IP_2[0] = '0'; dest_IP_2[1] = oktet[0]; dest_IP_2[2] = oktet[1]; dest_IP_2[3] = '\n'; dest_IP_2[4] = 0x00;}
            if (j == 1) {dest_IP_2[0] = '0'; dest_IP_2[1] = '0'; dest_IP_2[2] = oktet[0]; dest_IP_2[3] = '\n'; dest_IP_2[4] = 0x00;}
            temp[1] = atoi(dest_IP_2);
            i++; j=0;
            while (1) {if(tmpbuf[i] == (uint8_t)'.') break; oktet[j] = tmpbuf[i]; i++; j++; }
            if (j == 3) {dest_IP_3[0] = oktet[0]; dest_IP_3[1] = oktet[1]; dest_IP_3[2] = oktet[2]; dest_IP_3[3] = '\n'; dest_IP_3[4] = 0x00;}
            if (j == 2) {dest_IP_3[0] = '0'; dest_IP_3[1] = oktet[0]; dest_IP_3[2] = oktet[1]; dest_IP_3[3] = '\n'; dest_IP_3[4] = 0x00;}
            if (j == 1) {dest_IP_3[0] = '0'; dest_IP_3[1] = '0'; dest_IP_3[2] = oktet[0]; dest_IP_3[3] = '\n'; dest_IP_3[4] = 0x00;}
            temp[2] = atoi(dest_IP_3);
            i++; j=0;
            while (1) {if(tmpbuf[i] == (uint8_t)'\0') break; oktet[j] = tmpbuf[i]; i++; j++; }
            if (j == 3) {dest_IP_4[0] = oktet[0]; dest_IP_4[1] = oktet[1]; dest_IP_4[2] = oktet[2]; dest_IP_4[3] = '\n'; dest_IP_4[4] = 0x00;}
            if (j == 2) {dest_IP_4[0] = '0'; dest_IP_4[1] = oktet[0]; dest_IP_4[2] = oktet[1]; dest_IP_4[3] = '\n'; dest_IP_4[4] = 0x00;}
            if (j == 1) {dest_IP_4[0] = '0'; dest_IP_4[1] = '0'; dest_IP_4[2] = oktet[0]; dest_IP_4[3] = '\n'; dest_IP_4[4] = 0x00;}
            temp[3] = atoi(dest_IP_4);

            if ((temp[0] == destip[0])&&(temp[1] == destip[1])&&(temp[2] == destip[2])&&(temp[3] == destip[3]))
            {
                 Printf("*****  destIP not changed  *****\r\n");
            }
            else
            {
                destip[0] = temp[0];
                destip[1] = temp[1];
                destip[2] = temp[2];
                destip[3] = temp[3];

                sprintf(tmp,"new dest IP: %d.%d.%d.%d\r\n",destip[0],destip[1],destip[2],destip[3]);
                UART_Printf(tmp);    delayUS_ASM(10000);
                if (sdCartOn == 1)
                {
                    FRESULT result = f_open(&fil, "dest_IP", FA_OPEN_ALWAYS | FA_WRITE );
                    if (result == 0)
                    {
                        UART_Printf("*****  write new dest IP to SD  *****\r\n");
                        delayUS_ASM(10000);
                        f_lseek(&fil, 0);
                        f_puts(dest_IP_1, &fil);
                        f_puts(dest_IP_2, &fil);
                        f_puts(dest_IP_3, &fil);
                        f_puts(dest_IP_4, &fil);
                        f_sync(&fil);
                        f_close(&fil);
                    }
                }
                else //EEPROM
                {
// !!!!!!!!!!!!!



                }
            }
        }

        if (tmpbuf[0] == '5') //Перезагрузка
        {
            Printf("*****  REBOOT  *****\r\n");

            HAL_NVIC_SystemReset();
        }

        if (tmpbuf[0] == '6') //Смена пароля
        {
            Printf("*****  SET NEW PASSWORD  *****\r\n");
            Printf("new pasword hash: ");
            Printf(tmpbuf+1);
            Printf("\r\n");
            if (sdCartOn == 1)
            {
                FRESULT result = f_open(&fil, "md5", FA_OPEN_ALWAYS | FA_WRITE );
                if (result == 0)
                {
                    UART_Printf("*****  write new md5 IP to SD  *****\r\n");
                    delayUS_ASM(10000);
                    f_lseek(&fil, 0);
                    f_puts(tmpbuf+1, &fil);
                    f_sync(&fil);
                    f_close(&fil);
                }
            }
            else //EEPROM
            {

// !!!!!!!!!!!!!


            }
        }

        if (tmpbuf[0] == 'l')
        {
            Printf("*****  LOGIN  *****\r\n");
            char login1[10] = {'l','a','d','m','i','n','\0'}; //первый символ всегда l
            char login2[10] = {'l','u','s','e','r','\0'};
            char login3[10] = {'l','1','2','3','\0'};
            if ((strcmp(tmpbuf,login1) == 0) ||
                (strcmp(tmpbuf,login2) == 0) ||
                (strcmp(tmpbuf,login3) == 0))
            {
                loginOK = 1;
            }
        }

        if (tmpbuf[0] == 'p')
        {
            Printf("*****  PASSWORD  *****\r\n");
            //первый символ всегда p  (qwe12345@)
//            char md5[34] = {'p','5','f','3','f','b','0','1','2','4','f','2','b','f','c','e','b',
//                            '3','1','c','f','5','3','0','5','1','9','4','d','e','1','4','d','\0'};

            if (strncmp(tmpbuf + 1, MD5 , 32) == 0)
            {
                Printf("password OK\r\n");
                passwordOK = 1;
                // MD5 hash:  5f3fb0124f2bfceb31cf5305194de14d
                // SHA1 Hash: d5a5a5ea0c15c37a7fd6d1b1452bdad545051546
            }
            else
            {
                Printf("password not OK\r\n");
            }
        }
	}

    if (sdCartOn == 1)
    {
        f_close(&MyFile);
        result=f_open(&MyFile,httpsockprop[tcpprop.cur_sock].fname,FA_READ); //Попытка открыть файл
        sprintf(str1,"f_open: %d\r\n",result);
        HAL_UART_Transmit(&huart6,(uint8_t*)str1,strlen(str1),0x1000);
        sprintf(str1,"f_size: %lu\r\n",f_size(&MyFile));
        HAL_UART_Transmit(&huart6,(uint8_t*)str1,strlen(str1),0x1000);
    }
    else //eeprom
    {
        if (strncmp(httpsockprop[tcpprop.cur_sock].fname,"index.html", 10) == 0)
        {
            f_size = indexLen;
            printf("index.html request - %d byte\n", f_size);
            pfile = pindex;
        }
        if (strncmp(httpsockprop[tcpprop.cur_sock].fname,"main.html", 9) == 0)
        {
            f_size = mainLen;
            printf("main.html request - %d byte\n", f_size);
            pfile = pmain;
        }
        if (strncmp(httpsockprop[tcpprop.cur_sock].fname,"host_IP", 7) == 0)
        {
            f_size = 20;
            printf("host_IP request - %d byte\n", f_size);
            pfile = psettingsIP;
        }
        if (strncmp(httpsockprop[tcpprop.cur_sock].fname,"dest_IP", 7) == 0)
        {
            f_size = 20;
            printf("dest_IP request - %d byte\n", f_size);
            pfile = psettingsIP + 20;
        }
        if (strncmp(httpsockprop[tcpprop.cur_sock].fname,"mask_IP", 7) == 0)
        {
            f_size = 20;
            printf("mask_IP request - %d byte\n", f_size);
            pfile = psettingsIP + 40;
        }
        if (strncmp(httpsockprop[tcpprop.cur_sock].fname,"gate_IP", 7) == 0)
        {
            f_size = 20;
            printf("gate_IP request - %d byte\n", f_size);
            pfile = psettingsIP + 60;
        }
        if (strncmp(httpsockprop[tcpprop.cur_sock].fname,"md5", 3) == 0)
        {
            f_size = 33;
            printf("md5 request - %d byte\n", f_size);
            pfile = psettingsIP + 80;
        }
    }
    if (result==FR_OK)
    {
        //изучим расширение файла
        ss1 = strchr(httpsockprop[tcpprop.cur_sock].fname,ch1);
        ss1++;
        if (strncmp(ss1,"jpg", 3) == 0)
        {
            httpsockprop[tcpprop.cur_sock].http_doc = EXISTING_JPG;
            //сначала включаем в размер размер заголовка
            httpsockprop[tcpprop.cur_sock].data_size = strlen(jpg_header);
        }
        if (strncmp(ss1,"ico", 3) == 0)
        {
            httpsockprop[tcpprop.cur_sock].http_doc = EXISTING_ICO;
            //сначала включаем в размер размер заголовка
            httpsockprop[tcpprop.cur_sock].data_size = strlen(icon_header);
        }
        else
        {
            httpsockprop[tcpprop.cur_sock].http_doc = EXISTING_HTML;
            //сначала включаем в размер размер заголовка
            httpsockprop[tcpprop.cur_sock].data_size = strlen(http_header);
        }
        //затем размер самого документа
       if (sdCartOn == 1)
       {
           httpsockprop[tcpprop.cur_sock].data_size += f_size(&MyFile);
       }
       else
       {
           httpsockprop[tcpprop.cur_sock].data_size += f_size;
       }

    }
    else
    {
        httpsockprop[tcpprop.cur_sock].http_doc = E404_HTML;
        //сначала включаем в размер размер заголовка
        httpsockprop[tcpprop.cur_sock].data_size = strlen(error_header);
        //затем размер самого документа
        httpsockprop[tcpprop.cur_sock].data_size += sizeof(e404_htm);
    }
    httpsockprop[tcpprop.cur_sock].cnt_rem_data_part = httpsockprop[tcpprop.cur_sock].data_size / tcp_size_wnd + 1;
    httpsockprop[tcpprop.cur_sock].last_data_part_size = httpsockprop[tcpprop.cur_sock].data_size % tcp_size_wnd;
    //борьба с неправильным расчётом, когда общий размер делится на минимальный размер окна без остатка
    if(httpsockprop[tcpprop.cur_sock].last_data_part_size==0)
    {
        httpsockprop[tcpprop.cur_sock].last_data_part_size=tcp_size_wnd;
        httpsockprop[tcpprop.cur_sock].cnt_rem_data_part--;
    }
    httpsockprop[tcpprop.cur_sock].cnt_data_part = httpsockprop[tcpprop.cur_sock].cnt_rem_data_part;
    sprintf(str1,"data size:%lu; cnt data part:%u; last_data_part_size:%u\r\n",
            (unsigned long)httpsockprop[tcpprop.cur_sock].data_size,
            httpsockprop[tcpprop.cur_sock].cnt_rem_data_part,
            httpsockprop[tcpprop.cur_sock].last_data_part_size);

    HAL_UART_Transmit(&huart6,(uint8_t*)str1,strlen(str1),0x1000);
    if (httpsockprop[tcpprop.cur_sock].cnt_rem_data_part==1)
    {
        httpsockprop[tcpprop.cur_sock].data_stat = DATA_ONE;
    }
    else if (httpsockprop[tcpprop.cur_sock].cnt_rem_data_part>1)
    {
        httpsockprop[tcpprop.cur_sock].data_stat = DATA_FIRST;
    }
    if(httpsockprop[tcpprop.cur_sock].data_stat==DATA_ONE)
    {
         tcp_send_http_one();
        DisconnectSocket(tcpprop.cur_sock); //Разъединяемся
        SocketClosedWait(tcpprop.cur_sock);
        delayUS_ASM(100000); //Иначе сокет иногда виснет
        OpenSocket(tcpprop.cur_sock,Mode_TCP);
        delayUS_ASM(100000);
        //Ждём инициализации сокета (статус SOCK_INIT)
        Printf("SocketInitWait\r\n");
        SocketInitWait(tcpprop.cur_sock);
        Printf("SocketInitWait_OK\r\n");
        //Продолжаем слушать сокет
        ListenSocket(tcpprop.cur_sock);
        SocketListenWait(tcpprop.cur_sock);

    }
    else if(httpsockprop[tcpprop.cur_sock].data_stat==DATA_FIRST)
    {
        tcp_send_http_first();
    }
}
//-----------------------------------------------
