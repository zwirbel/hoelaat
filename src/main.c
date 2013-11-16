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
		hour++;
	} else
	if (min <= 21)
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
	if (hour == 0 || hour == 12)
	{
		// nothing to do
		ampm_word.text = "";
	} else
	if (hour < 6)
		ampm_word.text = "z'nacht";
	else
	if (hour <= 12)
		ampm_word.text = "am morgä";
	else
	if (hour <= 17)
		ampm_word.text = "am namitag";
	else
	if (hour <= 24)
		ampm_word.text = "am abig";
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

	// Stack top to bottom.  Note that the hour can take up
	// two rows at midnight.
	text_layer(&ampm_word, GRect(4, y + 3*h, 144, h+8), font_thin);
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
