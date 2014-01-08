/*

  ucg_dev_ic_ili9325.c
  
  Specific code for the ili9325 controller (OLED displays)

  Universal uC Color Graphics Library
  
  Copyright (c) 2014, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  

*/

#include "ucg.h"



const uint8_t ucg_ili9325_set_pos_dir0_seq[] = 
{
  UCG_CS(0),					/* enable chip */
  UCG_C10(0x020),	UCG_VARX(0,0x00, 0), UCG_VARX(0,0x0ff, 0),					/* set x position */
  UCG_C10(0x021),	UCG_VARY(0,0x01, 0), UCG_VARY(0,0x0ff, 0),		/* set y position */
  UCG_C10(0x050),	UCG_VARX(0,0x00, 0), UCG_VARX(0,0x0ff, 0),					/* set x position */
  UCG_C10(0x052),	UCG_VARY(0,0x01, 0), UCG_VARY(0,0x0ff, 0),		/* set y position */
  UCG_C10(0x051),	UCG_VARX(0,0x00, 0), UCG_VARX(0,0x0ff, 0),					/* set x position */
  UCG_C10(0x053),	UCG_VARY(0,0x01, 0), UCG_VARY(0,0x0ff, 0),		/* set y position */
  UCG_C10(0x022),							/* write to RAM */
  UCG_DATA(),								/* change to data mode */
  UCG_END()
};

const uint8_t ucg_ili9325_set_pos_dir1_seq[] = 
{
  UCG_END()
};

ucg_int_t ucg_handle_ili9325_l90fx(ucg_t *ucg)
{
  uint8_t c[3];
  if ( ucg_clip_l90fx(ucg) != 0 )
  {
    switch(ucg->arg.dir)
    {
      case 0: 
	ucg_com_SendCmdSeq(ucg, ucg_ili9325_set_pos_dir0_seq);	
	break;
      case 1: 
	ucg_com_SendCmdSeq(ucg, ucg_ili9325_set_pos_dir1_seq);	
	break;
      case 2: 
	ucg->arg.pixel.pos.x -= ucg->arg.len;
	ucg->arg.pixel.pos.x++;
	ucg_com_SendCmdSeq(ucg, ucg_ili9325_set_pos_dir0_seq);	
	break;
      case 3: 
      default: 
	ucg->arg.pixel.pos.y -= ucg->arg.len;
	ucg->arg.pixel.pos.y++;
	ucg_com_SendCmdSeq(ucg, ucg_ili9325_set_pos_dir1_seq);	
	break;
    }
    c[0] = ucg->arg.pixel.rgb.color[0]>>2;
    c[1] = ucg->arg.pixel.rgb.color[1]>>2;
    c[2] = ucg->arg.pixel.rgb.color[2]>>2;
    ucg_com_SendRepeat3Bytes(ucg, ucg->arg.len, c);
    ucg_com_SetCSLineStatus(ucg, 1);		/* disable chip */
    return 1;
  }
  return 0;
}

/*
  L2TC (Glyph Output)
  Because of this for vertical lines the x min and max values in
  "ucg_ili9325_set_pos_for_y_seq" are identical to avoid changes of the x position
  
*/

const uint8_t ucg_ili9325_set_pos_for_x_seq[] = 
{
  UCG_CS(0),					/* enable chip */
  UCG_C10(0x015),	UCG_VARX(0,0x0ff, 0), UCG_D1(0x07f),		/* set x position */
  UCG_C10(0x075),	UCG_VARY(0,0x0ff, 0), UCG_VARY(0,0x0ff, 0),		/* set y position */
  UCG_END()
};

const uint8_t ucg_ili9325_set_pos_for_y_seq[] = 
{
  UCG_CS(0),					/* enable chip */
  UCG_C10(0x015),	UCG_VARX(0,0x0ff, 0), UCG_VARX(0,0x0ff, 0),		/* set x position */
  UCG_C10(0x075),	UCG_VARY(0,0x0ff, 0), UCG_D1(0x07f),		/* set y position */
  UCG_END()
};

const uint8_t ucg_ili9325_set_x_pos_seq[] = 
{
  UCG_C10(0x015),	UCG_VARX(0,0x0ff, 0), UCG_D1(0x07f),		/* set x position */
  UCG_C10(0x05c),							/* write to RAM */
  UCG_DATA(),								/* change to data mode */
  UCG_END()
};

const uint8_t ucg_ili9325_set_y_pos_seq[] = 
{
  UCG_C10(0x075),	UCG_VARY(0,0x0ff, 0), UCG_D1(0x07f),		/* set y position */
  UCG_C10(0x05c),							/* write to RAM */
  UCG_DATA(),								/* change to data mode */
  UCG_END()
};


ucg_int_t ucg_handle_ili9325_l90tc(ucg_t *ucg)
{
  if ( ucg_clip_l90tc(ucg) != 0 )
  {
    uint8_t buf[3];
    ucg_int_t dx, dy;
    ucg_int_t i;
    const uint8_t *seq;
    unsigned char pixmap;
    uint8_t bitcnt;
    ucg_com_SetCSLineStatus(ucg, 0);		/* enable chip */
    switch(ucg->arg.dir)
    {
      case 0: 
	dx = 1; dy = 0; 
	seq = ucg_ili9325_set_x_pos_seq;
	ucg_com_SendCmdSeq(ucg, ucg_ili9325_set_pos_for_x_seq);	
	break;
      case 1: 
	dx = 0; dy = 1; 
	seq = ucg_ili9325_set_y_pos_seq;
	ucg_com_SendCmdSeq(ucg, ucg_ili9325_set_pos_for_y_seq);	
	break;
      case 2: 
	dx = -1; dy = 0; 
	seq = ucg_ili9325_set_x_pos_seq;
	ucg_com_SendCmdSeq(ucg, ucg_ili9325_set_pos_for_x_seq);	
	break;
      case 3: 
      default:
	dx = 0; dy = -1; 
	seq = ucg_ili9325_set_y_pos_seq;
	ucg_com_SendCmdSeq(ucg, ucg_ili9325_set_pos_for_y_seq);
	break;
    }
    pixmap = ucg_pgm_read(ucg->arg.bitmap);
    bitcnt = ucg->arg.pixel_skip;
    pixmap <<= bitcnt;
    buf[0] = ucg->arg.pixel.rgb.color[0]>>2;
    buf[1] = ucg->arg.pixel.rgb.color[1]>>2;
    buf[2] = ucg->arg.pixel.rgb.color[2]>>2;
    //ucg_com_SetCSLineStatus(ucg, 0);		/* enable chip */
    
    for( i = 0; i < ucg->arg.len; i++ )
    {
      if ( (pixmap & 128) != 0 )
      {
	ucg_com_SendCmdSeq(ucg, seq);	
	//ucg_com_SendString(ucg, 3, buf);
	ucg_com_SendRepeat3Bytes(ucg, 1, buf);
      }
      pixmap<<=1;
      ucg->arg.pixel.pos.x+=dx;
      ucg->arg.pixel.pos.y+=dy;
      bitcnt++;
      if ( bitcnt >= 8 )
      {
	ucg->arg.bitmap++;
	pixmap = ucg_pgm_read(ucg->arg.bitmap);
	bitcnt = 0;
      }
    }
    ucg_com_SetCSLineStatus(ucg, 1);		/* disable chip */
    return 1;
  }
  return 0;
}


ucg_int_t ucg_handle_ili9325_l90se(ucg_t *ucg)
{
  uint8_t i;
  uint8_t c[3];
  
  /* Setup ccs for l90se. This will be updated by ucg_clip_l90se if required */
  
  for ( i = 0; i < 3; i++ )
  {
    ucg_ccs_init(ucg->arg.ccs_line+i, ucg->arg.rgb[0].color[i], ucg->arg.rgb[1].color[i], ucg->arg.len);
  }
  
  /* check if the line is visible */
  
  if ( ucg_clip_l90se(ucg) != 0 )
  {
    ucg_int_t dx, dy;
    ucg_int_t i, j;
    switch(ucg->arg.dir)
    {
      case 0: dx = 1; dy = 0; break;
      case 1: dx = 0; dy = 1; break;
      case 2: dx = -1; dy = 0; break;
      case 3: dx = 0; dy = -1; break;
      default: dx = 1; dy = 0; break;	/* avoid compiler warning */
    }
    ucg_com_SendCmdSeq(ucg, ucg_ili9325_set_pos_dir0_seq);	
    for( i = 0; i < ucg->arg.len; i++ )
    {
      c[0] = ucg->arg.ccs_line[0].current >> 2;
      c[1] = ucg->arg.ccs_line[1].current >> 2; 
      c[2] = ucg->arg.ccs_line[2].current >> 2;
      ucg_com_SendRepeat3Bytes(ucg, 1, c);
      ucg->arg.pixel.pos.x+=dx;
      ucg->arg.pixel.pos.y+=dy;
      for ( j = 0; j < 3; j++ )
      {
	ucg_ccs_step(ucg->arg.ccs_line+j);
      }
    }
    ucg_com_SetCSLineStatus(ucg, 1);		/* disable chip */
    return 1;
  }
  return 0;
}


ucg_int_t ucg_dev_ic_ili9325(ucg_t *ucg, ucg_int_t msg, void *data)
{
  switch(msg)
  {
    case UCG_MSG_DEV_POWER_UP:
      /* setup com interface and provide information on the clock speed */
      /* of the serial and parallel interface. Values are nanoseconds. */
      return ucg_com_PowerUp(ucg, 40, 100);
    case UCG_MSG_DEV_POWER_DOWN:
      /* not yet implemented */
      return 1;
    case UCG_MSG_GET_DIMENSION:
      ((ucg_wh_t *)data)->w = 240;
      ((ucg_wh_t *)data)->h = 320;
      return 1;
    case UCG_MSG_DRAW_PIXEL:
      if ( ucg_clip_is_pixel_visible(ucg) !=0 )
      {
	uint8_t c[3];
	ucg_com_SendCmdSeq(ucg, ucg_ili9325_set_pos_dir0_seq);	
	c[0] = ucg->arg.pixel.rgb.color[0];
	c[1] = ucg->arg.pixel.rgb.color[1];
	c[2] = ucg->arg.pixel.rgb.color[2];
	ucg_com_SendRepeat3Bytes(ucg, 1, c);
	ucg_com_SetCSLineStatus(ucg, 1);		/* disable chip */
      }
      return 1;
    case UCG_MSG_DRAW_L90FX:
      ucg_handle_l90fx(ucg, ucg_dev_ic_ili9325);
      //ucg_handle_ili9325_l90fx(ucg);
      return 1;
    case UCG_MSG_DRAW_L90TC:
      ucg_handle_l90tc(ucg, ucg_dev_ic_ili9325);
      //ucg_handle_ili9325_l90tc(ucg);
      return 1;
    case UCG_MSG_DRAW_L90SE:
      return ucg->ext_cb(ucg, msg, data);
  }
  return ucg_dev_default_cb(ucg, msg, data);  
}

ucg_int_t ucg_ext_ili9325(ucg_t *ucg, ucg_int_t msg, void *data)
{
  switch(msg)
  {
    case UCG_MSG_DRAW_L90SE:
      ucg_handle_l90se(ucg, ucg_dev_ic_ili9325);
      //ucg_handle_ili9325_l90se(ucg);
      break;
  }
  return 1;
}