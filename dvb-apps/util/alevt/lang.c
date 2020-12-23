#include <string.h>
#include <ctype.h>
#include "misc.h"
#include "vt.h"
#include "lang.h"

int latin1 = -1;


static u8 lang_char[256];
static u8 lang_chars[1+8+8][16] =
{
    { 0, 0x23,0x24,0x40,0x5b,0x5c,0x5d,0x5e,0x5f,0x60,0x7b,0x7c,0x7d,0x7e },

    // for latin-1 font
    // English (100%)
    { 0,  '�', '$', '@', '�', '�', '�', '�', '#', '�', '�', '�', '�', '�' },
    // German (100%)
    { 0,  '#', '$', '�', '�', '�', '�', '^', '_', '�', '�', '�', '�', '�' },
    // Swedish/Finnish/Hungarian (100%)
    { 0,  '#', '�', '�', '�', '�', '�', '�', '_', '�', '�', '�', '�', '�' },
    // Italian (100%)
    { 0,  '�', '$', '�', '�', '�', '�', '�', '#', '�', '�', '�', '�', '�' },
    // French (100%)
    { 0,  '�', '�', '�', '�', '�', '�', '�', '#', '�', '�', '�', '�', '�' },
    // Portuguese/Spanish (100%)
    { 0,  '�', '$', '�', '�', '�', '�', '�', '�', '�', '�', '�', '�', '�' },
    // Czech/Slovak (60%)
    { 0,  '#', 'u', 'c', 't', 'z', '�', '�', 'r', '�', '�', 'e', '�', 's' },
    // reserved (English mapping)
    { 0,  '�', '$', '@', '�', '�', '�', '�', '#', '�', '�', '�', '�', '�' },

    // for latin-2 font
    // Polish (100%)
    { 0,  '#', '�', '�', '�', '�', '�', '�', '�', '�', '�', '�', '�', '�' },
    // German (100%)
    { 0,  '#', '$', '�', '�', '�', '�', '^', '_', '�', '�', '�', '�', '�' },
    // Estonian (100%)
    { 0,  '#', '�', '�', '�', '�', '�', '�', '�', '�', '�', '�', '�', '�' },
    // Lettish/Lithuanian (90%)
    { 0,  '#', '$', '�', '�', '�', '�', '�', '�', '�', '�', 'u', '�', 'i' },
    // French (90%)
    { 0,  '�', 'i', 'a', '�', '�', 'u', '�', '#', 'e', '�', '�', 'u', '�' },
    // Serbian/Croation/Slovenian (100%)
    { 0,  '#', '�', '�', '�', '�', '�', '�', '�', '�', '�', '�', '�', '�' },
    // Czech/Slovak (100%)
    { 0,  '#', '�', '�', '�', '�', '�', '�', '�', '�', '�', '�', '�', '�' },
    // Rumanian (95%)
    { 0,  '#', '�', '�', '�', '�', '�', '�', 'i', '�', '�', '�', '�', '�' },
};

/* Yankable latin charset :-)
     !"#$%&'()*+,-./0123456789:;<=>?
    @ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_
    `abcdefghijklmnopqrstuvwxyz{|}~
     �������������������������������
    ��������������������������������
    ��������������������������������
*/


static struct mark { u8 *g0, *latin1, *latin2; } marks[16] =
{
    /* none */		{ "#",
    			  "�",
			  "$"					},
    /* grave - ` */	{ " aeiouAEIOU",
    			  "`����������",
			  "`aeiouAEIOU"				},
    /* acute - ' */	{ " aceilnorsuyzACEILNORSUYZ",
			  "'�c��ln�rs��z�C��LN�RS��Z",
			  "'���������������������ݬ"		},
    /* cirumflex - ^ */	{ " aeiouAEIOU",
    			  "^����������",
			  "^�e��u�E��U"				},
    /* tilde - ~ */	{ " anoANO",
    			  "~������",
			  "~anoANO"				},
    /* ??? - � */	{ "",
    			  "",
			  ""					},
    /* breve - u */	{ "aA",
    			  "aA",
			  "��"					},
    /* abovedot - � */	{ "zZ",
    			  "zZ",
			  "��"					},
    /* diaeresis � */	{ "aeiouAEIOU",
    			  "����������",
			  "��i����I��"				},
    /* ??? - . */	{ "",
    			  "",
			  ""					},
    /* ringabove - � */	{ " auAU",
    			  "��u�U",
			  "�a�A�"				},
    /* cedilla - � */	{ "cstCST",
    			  "�st�ST",
			  "��Ǫ�"				},
    /* ??? - _ */	{ " ",
    			  "_",
			  "_"					},
    /* dbl acute - " */	{ " ouOU",
    			  "\"ouOU",
			  "\"����"				},
    /* ogonek - \, */	{ "aeAE",
    			  "aeAE",
			  "���"				},
    /* caron - v */	{ "cdelnrstzCDELNRSTZ",
			  "cdelnrstzCDELNRSTZ",
			  "����������̥�ة��"			},
};


static u8 g2map_latin1[] =
   /*0123456789abcdef*/
    " ���$�#��'\"�    "
    "����׵���'\"�����"
    " `�^~   �.��_\"  "
    "_���            "
    " �ЪH ILL� ��TNn"
    "K�d�hiill� ��tn\x7f";


static u8 g2map_latin2[] =
   /*0123456789abcdef*/
    " icL$Y#��'\"<    "
    "�   �u  �'\">    "
    " `�^~ ���.��_���"
    "- RC            "
    "  �aH iL�O opTNn"
    "K �dhiil�o �ptn\x7f";


void lang_init(void)
{
    int i;

    memset(lang_char, 0, sizeof(lang_char));
    for (i = 1; i <= 13; i++)
	lang_char[lang_chars[0][i]] = i;
}


void conv2latin(u8 *p, int n, int lang)
{
    int c, gfx = 0, lat=0;

  if ((latin1 == KOI8) && lang==12) { /* russian */
    while (n--) {
      c=*p;

      if(c==0x1b) lat = !lat; /* ESC switches languages inside page */

       if ( is_koi(c)) {
         if (not gfx || (c & 0xa0) != 0x20) {
            if(!lat) conv2koi8(p);
         }
       }
	else if ((c & 0xe8) == 0)
	    gfx = c & 0x10;
	p++;
   }
 }
else if ((latin1 == GREEK) && lang==15) { /* Hellas */
    while (n--) {
      c=*p;

      if(c==0x1b) lat = !lat; /* ESC switches languages inside page */

       if ( is_greek(c)) {
         if (not gfx || (c & 0xa0) != 0x20) {
            if(!lat) conv2greek(p);
         }
       }
	else if ((c & 0xe8) == 0)
	    gfx = c & 0x10;
	p++;
   }
 }

 else {
    while (n--)
    {
	if (lang_char[c = *p])
	{
	    if (not gfx || (c & 0xa0) != 0x20) 
		*p = lang_chars[lang + 1][lang_char[c]];
	}
	else if ((c & 0xe8) == 0)
	    gfx = c & 0x10;
	p++;
    }
  }
}


/* check for Greek chars - needs locale iso8859-7 set */
int is_greek(int c)
{
  if( isalpha(c | 0x80)) return 1;
  return 0;
}


/* check for russian chars - needs locale KOI8-R set */
int is_koi(int c)
{
  if( isalpha(c | 0x80)) return 1;
  if( c=='&' ) return 1;
  return 0;
}


/* teletext to koi8-r conversion */
void conv2koi8(u8 *p)
{
    u8 c;
    static u8 l2koi[]={
	0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9,
	0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 0xF0, 0xF1, 0xF2, 0xF3,
	0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xFF, 0xFA, 0xFB, 0xFC, 0xFD,
	0xFE, 0xF9, 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
	0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1,
	0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB,
	0xDC, 0xDD, 0xDE, 0xDF
    };

      c= *p;
      if ( (c >= 0x40) && (c <= 0x7f)) *p=l2koi[(c & 0x7f) - 0x40];
      if (c=='&') *p='�';
}


/* teletext to iso8859-7 conversion */
void conv2greek(u8 *p)
{
    u8 c;
   static u8 l2greek[]={
/* 1 @ 0x40->�*/0xc0,
/* 2 A 0x41->�*/0xc1,
/* 3 B 0x42->�*/0xc2,
/* 4 C 0x43->�*/0xc3,
/* 5 D 0x44->�*/0xc4,
/* 6 E 0x45->�*/0xc5,
/* 7 F 0x46->�*/0xc6,
/* 8 G 0x47->�*/0xc7,
/* 9 H 0x48->�*/0xc8,
/*10 I 0x49->�*/0xc9,
/*11 J 0x4a->�*/0xca,
/*12 K 0x4b->�*/0xcb,
/*13 L 0x4c->�*/0xcc,
/*14 M 0x4d->�*/0xcd,
/*15 N 0x4e->�*/0xce,
/*16 O 0x4f->�*/0xcf,
/*17 P 0x50->�*/0xd0,
/*18 Q 0x51->�*/0xd1,
/*19 R 0x52->?*/0x52,
/*20 S 0x53->�*/0xd3,
/*21 T 0x54->�*/0xd4,
/*22 U 0x55->�*/0xd5,
/*23 V 0x56->�*/0xd6,
/*24 W 0x57->�*/0xd7,
/*25 X 0x58->�*/0xd8,
/*26 Y 0x59->�*/0xd9,
/*27 Z 0x5a->?*/0x5a,
/*28 [ 0x5b->?*/0x5b,
/*!29 \ 0x5c->�*/0xdc,
/*!30 ] 0x5d->�*/0xdd,
/*!31 ^ 0x5e->�*/0xde,
/*!32 _ 0x5f->�*/0xdf,
/*33 ` 0x60->?*/0x60,
/*!34 a 0x61->�*/0xe1,
/*!35 b 0x62->�*/0xe2,
/*!36 c 0x63->�*/0xe3,
/*!37 d 0x64->�*/0xe4,
/*!38 e 0x65->�*/0xe5,
/*!39 f 0x66->�*/0xe6,
/*!40 g 0x67->�*/0xe7,
/*!41 h 0x68->�*/0xe8,
/*!42 i 0x69->�*/0xe9,
/*!43 j 0x6a->�*/0xea,
/*!44 k 0x6b->�*/0xeb,
/*!45 l 0x6c->�*/0xec,
/*!46 m 0x6d->�*/0xed,
/*!47 n 0x6e->�*/0xee,
/*!48 o 0x6f->�*/0xef,
/*!49 p 0x70->�*/0xf0,
/*!50 q 0x71->�*/0xf1,
/*!51 r 0x72->�*/0xf2,
/*!52 s 0x73->�*/0xf3,
/*!53 t 0x74->�*/0xf4,
/*!54 u 0x75->�*/0xf5,
/*!55 v 0x76->�*/0xf6,
/*!56 w 0x77->�*/0xf7,
/*!57 x 0x78->�*/0xf8,
/*!58 y 0x79->�*/0xf9,
/*59 z 0x7a->�(�� ����)*/0xc0,
/*60 { 0x7b->?*/0x7b,
/*!61 | 0x7c->�*/0xfc,
/*!62 } 0x7d->�*/0xfd,
/*!63 ~ 0x7e->�*/0xfe,
/*64   0x7f->?*/0x7f
  };
      c= *p;
      if ( (c >= 0x40) && (c <= 0x7f)) *p=l2greek[(c & 0x7f) - 0x40];
}


void init_enhance(struct enhance *eh)
{
    eh->next_des = 0;
}


void add_enhance(struct enhance *eh, int dcode, u32 *t)
{

    if (dcode == eh->next_des)
    {
	memcpy(eh->trip + dcode * 13, t, 13 * sizeof(*t));
	eh->next_des++;
    }
    else
	eh->next_des = -1;
}


void enhance(struct enhance *eh, struct vt_page *vtp)
{
    int row = 0;
    u32 *p, *e;

    if (eh->next_des < 1)
	return;

    for (p = eh->trip, e = p + eh->next_des * 13; p < e; p++)
	if (*p % 2048 != 2047)
	{
	    int adr = *p % 64;
	    int mode = *p / 64 % 32;
	    int data = *p / 2048 % 128;

	    if (adr < 40)
	    {
		// col functions
		switch (mode)
		{
		    case 15: // char from G2 set
			if (adr < W && row < H)
			    if (latin1==LATIN1)
				vtp->data[row][adr] = g2map_latin1[data-32];
			    else if (latin1==LATIN2)
				vtp->data[row][adr] = g2map_latin2[data-32];
			break;
		    case 16 ... 31: // char from G0 set with diacritical mark
			if (adr < W && row < H)
			{
			    struct mark *mark = marks + (mode - 16);
			    u8 *x;

			    if (x = strchr(mark->g0, data))
				if (latin1==LATIN1)
				    data = mark->latin1[x - mark->g0];
				else if (latin1==LATIN2)
				    data = mark->latin2[x - mark->g0];
			    vtp->data[row][adr] = data;
			}
			break;
		}
	    }
	    else
	    {
		// row functions
		if ((adr -= 40) == 0)
		    adr = 24;
		
		switch (mode)
		{
		    case 1: // full row color
			row = adr;
			break;
		    case 4: // set active position
			row = adr;
			break;
		    case 7: // address row 0 (+ full row color)
			if (adr == 23)
			    row = 0;
			break;
		}
	    }
	}
}
