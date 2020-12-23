#ifndef LIBRETRO_CORE_OPTIONS_INTL_H__
#define LIBRETRO_CORE_OPTIONS_INTL_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* https://support.microsoft.com/en-us/kb/980263 */
#pragma execution_character_set("utf-8")
#pragma warning(disable:4566)
#endif

#include <libretro.h>

/*
 ********************************
 * VERSION: 1.3
 ********************************
 *
 * - 1.3: Move translations to libretro_core_options_intl.h
 *        - libretro_core_options_intl.h includes BOM and utf-8
 *          fix for MSVC 2010-2013
 *        - Added HAVE_NO_LANGEXTRA flag to disable translations
 *          on platforms/compilers without BOM support
 * - 1.2: Use core options v1 interface when
 *        RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION is >= 1
 *        (previously required RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION == 1)
 * - 1.1: Support generation of core options v0 retro_core_option_value
 *        arrays containing options with a single value
 * - 1.0: First commit
*/

#ifdef __cplusplus
extern "C" {
#endif

/*
 ********************************
 * Core Option Definitions
 ********************************
*/

/* RETRO_LANGUAGE_JAPANESE */

/* RETRO_LANGUAGE_FRENCH */

/* RETRO_LANGUAGE_SPANISH */

/* RETRO_LANGUAGE_GERMAN */

/* RETRO_LANGUAGE_ITALIAN */

/* RETRO_LANGUAGE_DUTCH */

/* RETRO_LANGUAGE_PORTUGUESE_BRAZIL */

/* RETRO_LANGUAGE_PORTUGUESE_PORTUGAL */

/* RETRO_LANGUAGE_RUSSIAN */

/* RETRO_LANGUAGE_KOREAN */

/* RETRO_LANGUAGE_CHINESE_TRADITIONAL */

/* RETRO_LANGUAGE_CHINESE_SIMPLIFIED */

/* RETRO_LANGUAGE_ESPERANTO */

/* RETRO_LANGUAGE_POLISH */

/* RETRO_LANGUAGE_VIETNAMESE */

/* RETRO_LANGUAGE_ARABIC */

/* RETRO_LANGUAGE_GREEK */

/* RETRO_LANGUAGE_TURKISH */

struct retro_core_option_definition option_defs_tr[] = {
   {
      "quicknes_up_down_allowed",
      "Karşı Yönlere İzin Ver",
      "Bunu etkinleştirmek aynı anda hem sola hem de sağa (veya bazı oyunlarda yukarı ve aşağı) yönlere basma / hızlı değiştirme / tutma olanağı sağlar. Bu, bazı oyunlarda harekete dayalı hataların oluşmasına neden olabilir. Bu core seçeneğinin devre dışı bırakılması en iyisidir.",
      {
         { NULL, NULL },
      },
      NULL,
   },
   {
      "quicknes_aspect_ratio_par",
      "En Boy Oranı",
      "QuickNES Core'un sağlanan en boy oranını yapılandırın.",
      {
         { "PAR", NULL },
         { "4:3", NULL },
         { NULL, NULL },
      },
      NULL,
   },
#ifndef PSP
   {
      "quicknes_use_overscan_h",
      "Yatay ekran taşmasını göster",
      "Standart bir televizyon ekranının kenarına çerçeve tarafından gizlenmiş potansiyel olarak rastgele rastlanan video çıkışını kesmek (yatay olarak) için bunu devre dışı olarak ayarlayın.",
      {
         { NULL, NULL },
      },
      NULL,
   },
   {
      "quicknes_use_overscan_v",
      "Yatay ekran taşmasını göster",
      "Standart bir televizyon ekranının kenarına çerçeve tarafından gizlenmiş potansiyel olarak rastgele rastlanan video çıkışını kesmek (dikey olarak) için bunu devre dışı olarak ayarlayın.",
      {
         { NULL, NULL },
      },
      NULL,
   },
#endif
   {
      "quicknes_no_sprite_limit",
      "Sprite Sınırı Yok",
      "Scanline başına 8 donanım sınırını kaldırır. Bu, sprite titremesini azaltır ancak bazı efektler için bunu kullandığında bazı oyunların hata yapmasına neden olabilir.",
      {
         { NULL, NULL },
      },
      NULL,
   },
   {
      "quicknes_audio_nonlinear",
      "Ses Modu",
      "Ses modunu yapılandırın. Stereo kaydırma, derinlik yöntemi eklemek için kaydırma yöntemi ve bazı yankı efektleri kullanarak stereoyu simüle eder.",
      {
         { "nonlinear",       NULL },
         { "linear",          NULL },
         { "stereo spanning", NULL },
         { NULL, NULL },
      },
      NULL,
   },
   {
      "quicknes_audio_eq",
      "Ses ekolayzer ön ayarı",
      "Sesi eşitlemeye bir ön ayar uygular",
      {
         { "default", "Varsayılan" },
         { "famicom", "Famicom" },
         { "tv",      "TV" },
         { "flat",    "Flat" },
         { "crisp",   "Crisp" },
         { "tinny",   "Tinny" },
         { NULL, NULL },
      },
      NULL,
   },
   {
      "quicknes_palette",
      "Renk paleti",
      "NTS tarafından NTSC video sinyali çıkışının kodunu çözerken hangi renk paletinin kullanılacağını belirtir.",
      {
         { "default",              "Varsayılan" },
         { "asqrealc",             NULL },
         { "nintendo-vc",          NULL },
         { "rgb",                  NULL },
         { "yuv-v3",               NULL },
         { "unsaturated-final",    NULL },
         { "sony-cxa2025as-us",    NULL },
         { "pal",                  NULL },
         { "bmf-final2",           NULL },
         { "bmf-final3",           NULL },
         { "smooth-fbx",           NULL },
         { "composite-direct-fbx", NULL },
         { "pvm-style-d93-fbx",    NULL },
         { "ntsc-hardware-fbx",    NULL },
         { "nes-classic-fbx-fs",   NULL },
         { "nescap",               NULL },
         { "wavebeam",             NULL },
         { NULL, NULL },
      },
      NULL,
   },
   {
      "quicknes_turbo_enable",
      "Turbo'yu Etkinleştir",
      "Turbo A ve Turbo B düğmelerinin kullanılmasını sağlar.",
      {
         { "none",     "Hiçbiri" },
         { "player 1", "1. Oyuncu" },
         { "player 2", "2. Oyuncu" },
         { "both",     "ikisi içinde" },
         { NULL, NULL },
      },
      NULL,
   },
   {
      "quicknes_turbo_pulse_width",
      "Turbo darbe genişliği (çerçevelerde)",
      "Turbo A ve Turbo B düğmeleri basılı tutulduğunda 'darbelerin' girişinin hem genişliğini hem de aralığını (çerçevelerde) belirtir. Örneğin, varsayılan '3' ayarı bir (60 / (3 + 3)) = 10 Hz turbo frekansına (saniyede 10 basma) karşılık gelir.",
      {
         { NULL, NULL },
      },
      NULL,
   },
   { NULL, NULL, NULL, {{0}}, NULL },
};

#ifdef __cplusplus
}
#endif

#endif
