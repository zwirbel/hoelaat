/** \file
 * Word clock, with swiss german time.
 *
 * Four lines:
 *
 * minutes (or blank)
 * over/voor/half (or blank)
 * HOUR (in bold)
 * an morgä /  am nami
 *
 * \todo: When any line changes, it is pushed to the side
 *
 * een uur
 * een over een
 * kwart over een
 * tien voor half twee
 * half twee
 * vijf over half twee
 * kwart voor twee
 * 
 * And 's ochtends, 's middags, 's avonds, 's nachts
 * vier uur
 * half vier 
 */
#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define UUID { 0x5D, 0xBE, 0xBB, 0x58, 0x3C, 0xF4, 0x4C, 0xAF, 0xB4, 0xAC, 0x02, 0x44, 0xA2, 0x64, 0x40, 0x37 },
PBL_APP_INFO(
	UUID,
	"wiä schpaat isch es?",
	"hudson, züritüütsch",
	3, 0, // Version
	INVALID_RESOURCE, // RESOURCE_ID_IMAGE_MENU_ICON,
	APP_INFO_WATCH_FACE
);



static Window window;
static GFont font_thin;
static GFont font_thick;
static GFont font_thin_s;

typedef struct
{
	TextLayer layer;
	PropertyAnimation anim;
	const char * text;
	const char * old_text;
} word_t;

static word_t min_word;
static word_t rel_word;
static word_t hour_word;
static word_t ampm_word;
static word_t kpi1_word;
static word_t kpi2_word;
static word_t kpi3_word;


static const char *nums[] = {
	"",
	"eis",
	"zwöi",
	"drü",
	"vier",
	"föif",
	"sächs",
	"siibe",
	"acht",
	"nüün",
	"zä",
	"ölf",
	"zwölf",
	"drizäh",
	"vierzä",
	"füfzäh",
        "sächszäh",
        "siibezäh",
        "achtzäh",
        "nüünzäh",
        "zwänzg",
	"einäzwanzg",
};

static const char *
min_string(
	int i
)
{
	return nums[i];
}


static const char *
hour_string(
	int h
)
{
	// only if it is actually midnight, not "before midnight"
	if (h == 0)
		return "mittär-\nnacht";

	if (h == 12)
		return "mittag";

	if (h < 12)
		return nums[h];
	else
		return nums[h - 12];
}


static void
nederlands_format(
	int hour,
	int min
)
{
	if (min == 0)
	{
		min_word.text = "";
		rel_word.text = "";
	} else
        if (min == 15)
	{
		// just the kwart
		min_word.text = "";
		rel_word.text = "viertäl ab";
	} else
	if (min < 21)
	{
		// over the hour
		min_word.text = min_string(min);
		rel_word.text = "ab";
	} else
	if (min == 21)
	{
		// over the hour
		min_word.text = min_string(min);
		rel_word.text = "ab";
	} else
	if (min < 30)
	{
		// over the kwart
		min_word.text = min_string(30 - min);
		rel_word.text = "vor halbi";
		hour++;
	} else
	if (min == 30)
	{
		// just the half
		min_word.text = "";
		rel_word.text = "halbi";
		hour++;
	} else
	if (min < 41)
	{
		// over the half
		min_word.text = min_string(min - 30);
		rel_word.text = "über halbi";
		hour++;
	} else
	if (min == 45)
	{
		// just the kwart
		min_word.text = "";
		rel_word.text = "viertäl vor";
		hour++;
	} else
	if (min < 60)
	{
		// over the kwart
		min_word.text = min_string(60 - min);
		rel_word.text = "vor";
		hour++;
	}

	hour_word.text = hour_string(hour);

	// at midnight and noon do not display an am/pm notation
	// but don't say "before midnight"
	//if (hour == 0 || hour == 12)
	// {ampm_word.text = "";	} else
	if (min == 1) {ampm_word.text = "12900 User"; kpi1_word.text = "3020 IT";	kpi2_word.text = "1034 IT Int";	kpi3_word.text = "1986 IT Ext";}
	else if (min == 2) {ampm_word.text = "60 WAN connections"; kpi1_word.text = "88.75 Kbps/User";}
	else if (min == 3) {ampm_word.text = "14220 WPs"; kpi1_word.text = "2988 Desktops";	kpi2_word.text = "11232 Laptops";}
	else if (min == 4) {ampm_word.text = "3155 Servers";kpi1_word.text = "1230 Physical Server";	kpi2_word.text = "1925 Virtual Servers";}
	else if (min == 5) {ampm_word.text = "3200 TB";kpi1_word.text = "32% data growth";	kpi2_word.text = "1051 TB last 12mths";}
	else if (min == 6) {ampm_word.text = "6015 Mobildev";  kpi1_word.text = "5051 SIMs";	kpi2_word.text = "2765 iOS Dev ByoD";	kpi3_word.text = "3250 Blackberrys";}
	else if (min == 7) {ampm_word.text = "3370 Appl";  kpi1_word.text = "959 Biz / 900 LN"; kpi2_word.text = "1254 EndUsing"; kpi3_word.text = "257 InfraApp";}
	else if (min == 8) {ampm_word.text = "12900 User"; kpi1_word.text = "3020 IT";	kpi2_word.text = "1034 IT Int";	kpi3_word.text = "1986 IT Ext";}
	else if (min == 9) {ampm_word.text = "60 WAN connections"; kpi1_word.text = "88.75 Kbps/User";}
	else if (min == 10) {ampm_word.text = "14220 WPs"; kpi1_word.text = "2988 Desktops";	kpi2_word.text = "11232 Laptops";}
	else if (min == 11) {ampm_word.text = "3155 Servers";kpi1_word.text = "1230 Physical Server";	kpi2_word.text = "1925 Virtual Servers";}
	else if (min == 12) {ampm_word.text = "3200 TB";kpi1_word.text = "32% data growth";	kpi2_word.text = "1051 TB last 12mths";}
	else if (min == 13) {ampm_word.text = "6015 Mobildev";  kpi1_word.text = "5051 SIMs";	kpi2_word.text = "2765 iOS Dev ByoD";	kpi3_word.text = "3250 Blackberrys";}
	else if (min == 14) {ampm_word.text = "3370 Appl";  kpi1_word.text = "959 Biz / 900 LN"; kpi2_word.text = "1254 EndUsing"; kpi3_word.text = "257 InfraApp";}
	else if (min == 15) {ampm_word.text = "12900 User"; kpi1_word.text = "3020 IT";	kpi2_word.text = "1034 IT Int";	kpi3_word.text = "1986 IT Ext";}
	else if (min == 16) {ampm_word.text = "60 WAN connections"; kpi1_word.text = "88.75 Kbps/User";}
	else if (min == 17) {ampm_word.text = "14220 WPs"; kpi1_word.text = "2988 Desktops";	kpi2_word.text = "11232 Laptops";}
	else if (min == 18) {ampm_word.text = "3155 Servers";kpi1_word.text = "1230 Physical Server";	kpi2_word.text = "1925 Virtual Servers";}
	else if (min == 19) {ampm_word.text = "3200 TB";kpi1_word.text = "32% data growth";	kpi2_word.text = "1051 TB last 12mths";}
	else if (min == 20) {ampm_word.text = "6015 Mobildev";  kpi1_word.text = "5051 SIMs";	kpi2_word.text = "2765 iOS Dev ByoD";	kpi3_word.text = "3250 Blackberrys";}
	else if (min == 21) {ampm_word.text = "3370 Appl";  kpi1_word.text = "959 Biz / 900 LN"; kpi2_word.text = "1254 EndUsing"; kpi3_word.text = "257 InfraApp";}
	else if (min == 22) {ampm_word.text = "12900 User"; kpi1_word.text = "3020 IT";	kpi2_word.text = "1034 IT Int";	kpi3_word.text = "1986 IT Ext";}
	else if (min == 23) {ampm_word.text = "60 WAN connections"; kpi1_word.text = "88.75 Kbps/User";}
	else if (min == 24) {ampm_word.text = "14220 WPs"; kpi1_word.text = "2988 Desktops";	kpi2_word.text = "11232 Laptops";}
	else if (min == 25) {ampm_word.text = "3155 Servers";kpi1_word.text = "1230 Physical Server";	kpi2_word.text = "1925 Virtual Servers";}
	else if (min == 26) {ampm_word.text = "3200 TB";kpi1_word.text = "32% data growth";	kpi2_word.text = "1051 TB last 12mths";}
	else if (min == 27) {ampm_word.text = "6015 Mobildev";  kpi1_word.text = "5051 SIMs";	kpi2_word.text = "2765 iOS Dev ByoD";	kpi3_word.text = "3250 Blackberrys";}
	else if (min == 28) {ampm_word.text = "3370 Appl";  kpi1_word.text = "959 Biz / 900 LN"; kpi2_word.text = "1254 EndUsing"; kpi3_word.text = "257 InfraApp";}
	//biz
	else if (min == 29) {ampm_word.text = "5 OSs";  kpi1_word.text = "AIX / Linux"; kpi2_word.text = "ESX / Windows"; kpi3_word.text = "Solaris";}
	else if (min == 30) {ampm_word.text = "IT Strategy";  kpi1_word.text = "Making better use of Information"; kpi2_word.text = "Building Global Operating Platforms"; kpi3_word.text = "Improving Services and Performance";}
		//it
	else if (min == 31) {ampm_word.text = "12900 User"; kpi1_word.text = "3020 IT";	kpi2_word.text = "1034 IT Int";	kpi3_word.text = "1986 IT Ext";}
	else if (min == 32) {ampm_word.text = "60 WAN connections"; kpi1_word.text = "88.75 Kbps/User";}
	else if (min == 33) {ampm_word.text = "14220 WPs"; kpi1_word.text = "2988 Desktops";	kpi2_word.text = "11232 Laptops";}
	else if (min == 34) {ampm_word.text = "3155 Servers";kpi1_word.text = "1230 Physical Server";	kpi2_word.text = "1925 Virtual Servers";}
	else if (min == 35) {ampm_word.text = "3200 TB";kpi1_word.text = "32% data growth";	kpi2_word.text = "1051 TB last 12mths";}
	else if (min == 36) {ampm_word.text = "6015 Mobildev";  kpi1_word.text = "5051 SIMs";	kpi2_word.text = "2765 iOS Dev ByoD";	kpi3_word.text = "3250 Blackberrys";}
	else if (min == 37) {ampm_word.text = "3370 Appl";  kpi1_word.text = "959 Biz / 900 LN"; kpi2_word.text = "1254 EndUsing"; kpi3_word.text = "257 InfraApp";}
	else if (min == 38) {ampm_word.text = "12900 User"; kpi1_word.text = "3020 IT";	kpi2_word.text = "1034 IT Int";	kpi3_word.text = "1986 IT Ext";}
	else if (min == 39) {ampm_word.text = "60 WAN connections"; kpi1_word.text = "88.75 Kbps/User";}
	else if (min == 40) {ampm_word.text = "14220 WPs"; kpi1_word.text = "2988 Desktops";	kpi2_word.text = "11232 Laptops";}
	else if (min == 41) {ampm_word.text = "3155 Servers";kpi1_word.text = "1230 Physical Server";	kpi2_word.text = "1925 Virtual Servers";}
	else if (min == 42) {ampm_word.text = "3200 TB";kpi1_word.text = "32% data growth";	kpi2_word.text = "1051 TB last 12mths";}
	else if (min == 43) {ampm_word.text = "6015 Mobildev";  kpi1_word.text = "5051 SIMs";	kpi2_word.text = "2765 iOS Dev ByoD";	kpi3_word.text = "3250 Blackberrys";}
	else if (min == 44) {ampm_word.text = "3370 Appl";  kpi1_word.text = "959 Biz / 900 LN"; kpi2_word.text = "1254 EndUsing"; kpi3_word.text = "257 InfraApp";}
	else if (min == 45) {ampm_word.text = "12900 User"; kpi1_word.text = "3020 IT";	kpi2_word.text = "1034 IT Int";	kpi3_word.text = "1986 IT Ext";}
	else if (min == 46) {ampm_word.text = "60 WAN connections"; kpi1_word.text = "88.75 Kbps/User";}
	else if (min == 47) {ampm_word.text = "14220 WPs"; kpi1_word.text = "2988 Desktops";	kpi2_word.text = "11232 Laptops";}
	else if (min == 48) {ampm_word.text = "3155 Servers";kpi1_word.text = "1230 Physical Server";	kpi2_word.text = "1925 Virtual Servers";}
	else if (min == 49) {ampm_word.text = "3200 TB";kpi1_word.text = "32% data growth";	kpi2_word.text = "1051 TB last 12mths";}
	else if (min == 50) {ampm_word.text = "6015 Mobildev";  kpi1_word.text = "5051 SIMs";	kpi2_word.text = "2765 iOS Dev ByoD";	kpi3_word.text = "3250 Blackberrys";}
	else if (min == 51) {ampm_word.text = "3370 Appl";  kpi1_word.text = "959 Biz / 900 LN"; kpi2_word.text = "1254 EndUsing"; kpi3_word.text = "257 InfraApp";}
	else if (min == 52) {ampm_word.text = "12900 User"; kpi1_word.text = "3020 IT";	kpi2_word.text = "1034 IT Int";	kpi3_word.text = "1986 IT Ext";}
	else if (min == 53) {ampm_word.text = "60 WAN connections"; kpi1_word.text = "88.75 Kbps/User";}
	else if (min == 54) {ampm_word.text = "14220 WPs"; kpi1_word.text = "2988 Desktops";	kpi2_word.text = "11232 Laptops";}
	else if (min == 55) {ampm_word.text = "3155 Servers";kpi1_word.text = "1230 Physical Server";	kpi2_word.text = "1925 Virtual Servers";}
	else if (min == 56) {ampm_word.text = "3200 TB";kpi1_word.text = "32% data growth";	kpi2_word.text = "1051 TB last 12mths";}
	else if (min == 57) {ampm_word.text = "6015 Mobildev";  kpi1_word.text = "5051 SIMs";	kpi2_word.text = "2765 iOS Dev ByoD";	kpi3_word.text = "3250 Blackberrys";}
	else if (min == 58) {ampm_word.text = "3370 Appl";  kpi1_word.text = "959 Biz / 900 LN"; kpi2_word.text = "1254 EndUsing"; kpi3_word.text = "257 InfraApp";}
	//if (min <60) {kpi1_word.text = "test kpi1";	kpi2_word.text = "test kpi2";	kpi3_word.text = "test kpi3";}
}


static void
update_word(
	word_t * const word
)
{
	text_layer_set_text(&word->layer, word->text);
	if (word->text != word->old_text)
		animation_schedule(&word->anim.animation);
}


/** Called once per minute */
static void
handle_tick(
	AppContextRef ctx,
	PebbleTickEvent * const event
)
{
	(void) ctx;
	const PblTm * const ptm = event->tick_time;

	int hour = ptm->tm_hour;
	int min = ptm->tm_min;

	kpi3_word.old_text = kpi3_word.text;
	kpi2_word.old_text = kpi2_word.text;
	kpi1_word.old_text = kpi1_word.text;
	ampm_word.old_text = ampm_word.text;
	hour_word.old_text = hour_word.text;
	rel_word.old_text = rel_word.text;
	min_word.old_text = min_word.text;

	nederlands_format(hour,  min);

/*
	string_format_time(
		time_buffer,
		sizeof(time_buffer),
		"%H:%M",
		event->tick_time
	);
*/

	update_word(&kpi3_word);
	update_word(&kpi2_word);
	update_word(&kpi1_word);
	update_word(&ampm_word);
	update_word(&hour_word);
	update_word(&rel_word);
	update_word(&min_word);
}


static void
text_layer_setup(
	Window * window,
	TextLayer * layer,
	GRect frame,
	GFont font
)
{
	text_layer_init(layer, frame);
	text_layer_set_text(layer, "");
	text_layer_set_text_color(layer, GColorWhite);
	text_layer_set_background_color(layer, GColorClear);
	text_layer_set_font(layer, font);
        layer_add_child(&window->layer, &layer->layer);
}


static void
text_layer(
	word_t * word,
	GRect frame,
	GFont font
)
{
	text_layer_setup(&window, &word->layer, frame, font);

	GRect frame_right = frame;
	frame_right.origin.x = 150;

	property_animation_init_layer_frame(
		&word->anim,
		&word->layer.layer,
		&frame_right,
		&frame
	);

	animation_set_duration(&word->anim.animation, 500);
	animation_set_curve(&word->anim.animation, AnimationCurveEaseIn);
}


static void
handle_init(
	AppContextRef ctx
)
{
	(void) ctx;

	window_init(&window, "Main");
	window_stack_push(&window, true);
	window_set_background_color(&window, GColorBlack);

	resource_init_current_app(&APP_RESOURCES);

	int y = 15;
	int h = 30;

	font_thin = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ARIAL_28));
	font_thick = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ARIAL_BLACK_30));
	font_thin_s = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ARIAL_12));

	// Stack top to bottom.  Note that the hour can take up
	// two rows at midnight.
	text_layer(&kpi3_word, GRect(4, y + 4.5*h, 144, h+8), font_thin_s);
	text_layer(&kpi2_word, GRect(4, y + 4*h, 144, h+8), font_thin_s);
	text_layer(&kpi1_word, GRect(4, y + 3.5*h, 144, h+8), font_thin_s);
	text_layer(&ampm_word, GRect(4, y + 3*h, 144, h+8), font_thin_s);
	text_layer(&hour_word, GRect(4, y + 2*h, 144, 2*h+8), font_thick);
	text_layer(&rel_word, GRect(4, y + 1*h, 144, h+8), font_thin);
	text_layer(&min_word, GRect(4, y + 0*h, 144, h+8), font_thin);

}


static void
handle_deinit(
	AppContextRef ctx
)
{
	(void) ctx;

	fonts_unload_custom_font(font_thin);
	fonts_unload_custom_font(font_thick);
	fonts_unload_custom_font(font_thin_s);
}


void
pbl_main(
	void * const params
)
{
	PebbleAppHandlers handlers = {
		.init_handler	= &handle_init,
		.deinit_handler = &handle_deinit,
		.tick_info	= {
			.tick_handler = &handle_tick,
			.tick_units = MINUTE_UNIT,
		},
	};

	app_event_loop(params, &handlers);
}
