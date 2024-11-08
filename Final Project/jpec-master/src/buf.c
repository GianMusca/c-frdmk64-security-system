/**
 * Copyright (c) 2012-2016 Moodstocks SAS
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <buf.h>
#include <conf.h>
#include "my_assert.h"
#include "utilities/fsl_debug_console.h"
//#include <assert.h>

#define JPEC_BUFFER_INIT_SIZ 65536

jpec_buffer_t *jpec_buffer_new(uint8_t* buf) {
  return jpec_buffer_new2(-1,buf);
}

jpec_buffer_t *jpec_buffer_new2(int siz, uint8_t* buf) {
  if (siz < 0) siz = 0;
  jpec_buffer_t *b = malloc(sizeof(*b));
  //PRINTF("\r\nMALLOC - buf.c - %x - %i\r\n",b,sizeof(*b));
  b->stream = siz > 0 ? malloc(siz) : NULL;
  //PRINTF("\r\nMALLOC - buf.c - %x - %i\r\n",b->stream,siz);
  //b->stream = siz > 0 ? buf : NULL;
  b->siz = siz;
  b->len = 0;
  return b;
}

void jpec_buffer_del(jpec_buffer_t *b) {
  my_assert(b?1:0);
  if (b->stream){
	  free(b->stream);
	  //PRINTF("\r\nFREE - buf.c - %x\r\n",b->stream);
  }
  free(b);
  //PRINTF("\r\nFREE - buf.c - %x\r\n",b);
}

void jpec_buffer_write_byte(jpec_buffer_t *b, int val) {
  my_assert(b?1:0);
  if (b->siz == b->len) {
    int nsiz = (b->siz > 0) ? 2 * b->siz : JPEC_BUFFER_INIT_SIZ;
    void* tmp = realloc(b->stream, nsiz);
    //PRINTF("\r\nREALLOC - buf.c - %x -> %x - %i\r\n",b->stream,tmp,nsiz);
    b->stream = (uint8_t *) tmp;
    b->siz = nsiz;
  }
  b->stream[b->len++] = (uint8_t) val;
}

void jpec_buffer_write_2bytes(jpec_buffer_t *b, int val) {
  my_assert(b?1:0);
  jpec_buffer_write_byte(b, (val >> 8) & 0xFF);
  jpec_buffer_write_byte(b, val & 0xFF);  
}
