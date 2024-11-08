/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdlib.h>
#include "bitmap_matrix.h"

#include "lwip/apps/httpd_opts.h"
#include "lwip/def.h"
#include "lwip/apps/fs.h"
#include "fsdata.h"
#include <string.h>

#include "utilities/fsl_debug_console.h"

//#include "footage.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define ARRAY_CONCAT(TYPE, A, An, B, Bn) \
(TYPE *)array_concat((const void *)(A), (An), (const void *)(B), (Bn), sizeof(TYPE));

#define FOOTAGE_NAME "/Resources/Image.svg"

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/
//

char *file_footage = NULL;

static char header_footage[] = "/Resources/Image.svg\0HTTP/1.0 200 OK\r\nServer: lwIP/pre-0.6 (http://www.sics.se/~adam/lwip/)\r\nContent-type: image/jpeg\r\n\r\n";
//tatic unsigned char* raw_data = (unsigned char*) bitmap_matrix;



void *array_concat(const void *a, size_t an,
               const void *b, size_t bn, size_t s);


#if HTTPD_USE_CUSTOM_FSDATA
#include "fsdata_custom.c"
#else /* HTTPD_USE_CUSTOM_FSDATA */
#include "fsdata.c"
#endif /* HTTPD_USE_CUSTOM_FSDATA */

/*-----------------------------------------------------------------------------------*/

#if LWIP_HTTPD_CUSTOM_FILES
int fs_open_custom(struct fs_file *file, const char *name);
void fs_close_custom(struct fs_file *file);
#if LWIP_HTTPD_FS_ASYNC_READ
u8_t fs_canread_custom(struct fs_file *file);
u8_t fs_wait_read_custom(struct fs_file *file, fs_wait_cb callback_fn, void *callback_arg);
int fs_read_async_custom(struct fs_file *file, char *buffer, int count, fs_wait_cb callback_fn, void *callback_arg);
#else /* LWIP_HTTPD_FS_ASYNC_READ */
int fs_read_custom(struct fs_file *file, char *buffer, int count);
#endif /* LWIP_HTTPD_FS_ASYNC_READ */
#endif /* LWIP_HTTPD_CUSTOM_FILES */

/*-----------------------------------------------------------------------------------*/
err_t
fs_open(struct fs_file *file, const char *name)
{
  const struct fsdata_file *f;

  if ((file == NULL) || (name == NULL)) {
     return ERR_ARG;
  }

#if LWIP_HTTPD_CUSTOM_FILES
  if (fs_open_custom(file, name)) {
    file->is_custom_file = 1;
    return ERR_OK;
  }
  file->is_custom_file = 0;
#endif /* LWIP_HTTPD_CUSTOM_FILES */


	for (f = FS_ROOT; f != NULL; f = f->next) {
		if (!strcmp(name, (const char *)f->name))
		{
			/* CUSTOM */
				if(!strcmp(FOOTAGE_NAME, (const char *)f->name) )
				{
						int len_header = sizeof(header_footage)-1;
						int len_data = jpg_size;
						int len_name = sizeof(FOOTAGE_NAME);

						//Header por separado
						if(file_footage != NULL){
							free(file_footage);
							//PRINTF("\r\nFREE - fs.c - %x\r\n",file_footage);
						}
						file_footage = ARRAY_CONCAT(char, header_footage, len_header, jpg_file, len_data);
						file->data = file_footage + len_name;// data_Resources_BackArrow_svg + 25,
						file->len = (len_header + len_data)- len_name;//sizeof(data_Resources_BackArrow_svg) - 25
						file->index = (len_header + len_data) - len_name;//sizeof(data_Resources_BackArrow_svg) - 25

//						//Header y data todo junto
//						file->data = jpg_file + len_name;
//						file->len = len_data- len_name;//sizeof(data_Resources_BackArrow_svg) - 25
//						file->index = len_data - len_name;//sizeof(data_Resources_BackArrow_svg) - 25


				}

			/* END OF CUSTOM*/
				else
				{
					file->data = (const char*)f->data;
					file->len = f->len;
					file->index = f->len;
				}

				file->pextension = NULL;
				file->flags = f->flags;




#if HTTPD_PRECALCULATED_CHECKSUM
      file->chksum_count = f->chksum_count;
      file->chksum = f->chksum;
#endif /* HTTPD_PRECALCULATED_CHECKSUM */
#if LWIP_HTTPD_FILE_STATE
      file->state = fs_state_init(file, name);
#endif /* #if LWIP_HTTPD_FILE_STATE */
      return ERR_OK;
    }
  }
  /* file not found */
  return ERR_VAL;
}

/*-----------------------------------------------------------------------------------*/
void
fs_close(struct fs_file *file)
{
#if LWIP_HTTPD_CUSTOM_FILES
  if (file->is_custom_file) {
    fs_close_custom(file);
  }
#endif /* LWIP_HTTPD_CUSTOM_FILES */
#if LWIP_HTTPD_FILE_STATE
  fs_state_free(file, file->state);
#endif /* #if LWIP_HTTPD_FILE_STATE */
  LWIP_UNUSED_ARG(file);
}
/*-----------------------------------------------------------------------------------*/
#if LWIP_HTTPD_DYNAMIC_FILE_READ
#if LWIP_HTTPD_FS_ASYNC_READ
int
fs_read_async(struct fs_file *file, char *buffer, int count, fs_wait_cb callback_fn, void *callback_arg)
#else /* LWIP_HTTPD_FS_ASYNC_READ */
int
fs_read(struct fs_file *file, char *buffer, int count)
#endif /* LWIP_HTTPD_FS_ASYNC_READ */
{
  int read;
  if(file->index == file->len) {
    return FS_READ_EOF;
  }
#if LWIP_HTTPD_FS_ASYNC_READ
  LWIP_UNUSED_ARG(callback_fn);
  LWIP_UNUSED_ARG(callback_arg);
#endif /* LWIP_HTTPD_FS_ASYNC_READ */
#if LWIP_HTTPD_CUSTOM_FILES
  if (file->is_custom_file) {
#if LWIP_HTTPD_FS_ASYNC_READ
    return fs_read_async_custom(file, buffer, count, callback_fn, callback_arg);
#else /* LWIP_HTTPD_FS_ASYNC_READ */
    return fs_read_custom(file, buffer, count);
#endif /* LWIP_HTTPD_FS_ASYNC_READ */
  }
#endif /* LWIP_HTTPD_CUSTOM_FILES */

  read = file->len - file->index;
  if(read > count) {
    read = count;
  }

  MEMCPY(buffer, (file->data + file->index), read);
  file->index += read;

  return(read);
}
#endif /* LWIP_HTTPD_DYNAMIC_FILE_READ */
/*-----------------------------------------------------------------------------------*/
#if LWIP_HTTPD_FS_ASYNC_READ
int
fs_is_file_ready(struct fs_file *file, fs_wait_cb callback_fn, void *callback_arg)
{
  if (file != NULL) {
#if LWIP_HTTPD_FS_ASYNC_READ
#if LWIP_HTTPD_CUSTOM_FILES
    if (!fs_canread_custom(file)) {
      if (fs_wait_read_custom(file, callback_fn, callback_arg)) {
        return 0;
      }
    }
#else /* LWIP_HTTPD_CUSTOM_FILES */
    LWIP_UNUSED_ARG(callback_fn);
    LWIP_UNUSED_ARG(callback_arg);
#endif /* LWIP_HTTPD_CUSTOM_FILES */
#endif /* LWIP_HTTPD_FS_ASYNC_READ */
  }
  return 1;
}
#endif /* LWIP_HTTPD_FS_ASYNC_READ */
/*-----------------------------------------------------------------------------------*/
int
fs_bytes_left(struct fs_file *file)
{
  return file->len - file->index;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void *array_concat(const void *a, size_t an,
               const void *b, size_t bn, size_t s)
{

	unsigned int i;
    char *p = malloc(s * (an + bn));
    //PRINTF("\r\nMALLOC - fs.c - %x - %i\r\n",p,s * (an + bn));
    for(i = 0; i < an; i++){p[i] = ((char*)a)[i];}
    //memcpy(p, a, an*s);

    for(i = 0; i < bn; i++){p[i+an] = ((char*)b)[i];}
    //memcpy(p + an*s, b, bn*s);

    return p;
}
