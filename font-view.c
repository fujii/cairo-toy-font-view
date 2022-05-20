/*
 * Copyright © 2008 Behdad Esfahbod
 * Copyright © 2009 Chris Wilson
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of
 * Chris Wilson not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission. Chris Wilson makes no representations about the
 * suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * CHRIS WILSON DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL CHRIS WILSON BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Chris Wilson <chris@chris-wilson.co.uk>
 */

#include <stdlib.h>
#include <string.h>
#include <cairo.h>

struct options {
    const char *text;
    const char *family;
    cairo_font_weight_t weight;
    cairo_font_slant_t slant;
    double size;
    int PAD;
    const char *png;
};

static void
draw (cairo_t *cr, struct options *options)
{
    cairo_text_extents_t extents;
    cairo_font_extents_t font_extents;

    cairo_select_font_face (cr,
			    options->family, options->slant, options->weight);

    double y = 0;
    for (int i = 0; i < 18; i++) {
	cairo_save (cr);
	cairo_set_font_size (cr, options->size + i);

	cairo_text_extents (cr, options->text, &extents);
	cairo_translate (cr,
			 options->PAD - extents.x_bearing,
			 options->PAD - extents.y_bearing + y);

	cairo_font_extents (cr, &font_extents);
	cairo_rectangle (cr, 0, -font_extents.ascent,
			 extents.x_advance, font_extents.height);
	cairo_move_to (cr, -options->PAD, 0);
	cairo_line_to (cr, extents.width + options->PAD, 0);
	cairo_set_source_rgba (cr, 1, 0, 0, .7);
	cairo_stroke (cr);

	cairo_rectangle (cr,
			 extents.x_bearing, extents.y_bearing,
			 extents.width, extents.height);
	cairo_set_source_rgba (cr, 0, 1, 0, .7);
	cairo_stroke (cr);

	cairo_move_to (cr, 0, 0);
	cairo_set_source_rgb (cr, 0, 0, 1);
	cairo_show_text (cr, options->text);
	cairo_fill (cr);

	cairo_restore (cr);
	y += font_extents.height + options->PAD;
    }
}

static int
expose_event (struct options *options)
{
    cairo_t *cr;

    cairo_surface_t *image;

    image = cairo_image_surface_create (CAIRO_FORMAT_RGB24,
					200,
					900);
    cr = cairo_create (image);
    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_paint (cr);

    draw (cr, options);

    cairo_destroy (cr);
    cairo_surface_write_to_png (image, options->png);
    cairo_surface_destroy (image);

    return 0;
}

int
main (int argc, char **argv)
{
    struct options options = {
	"ニコ日",
	"MS Gothic",
	CAIRO_FONT_WEIGHT_NORMAL,
	CAIRO_FONT_SLANT_NORMAL,
	10,
	30,
	"font-view.png"
    };

    /* rudimentary argument processing */
    if (argc >= 2) {
	options.family = argv[1];
    }
    if (argc >= 3) {
	if (strcmp (argv[2], "italic") == 0)
	    options.slant = CAIRO_FONT_SLANT_ITALIC;
	else if (strcmp (argv[2], "oblique") == 0)
	    options.slant = CAIRO_FONT_SLANT_OBLIQUE;
	else
	    options.slant = atoi (argv[2]);
    }
    if (argc >= 4) {
	if (strcmp (argv[3], "bold") == 0)
	    options.weight = CAIRO_FONT_WEIGHT_BOLD;
	else
	    options.weight = atoi (argv[3]);
    }
    if (argc >= 5) {
	options.size = atof (argv[4]);
    }
    if (argc >= 6) {
	options.text = argv[5];
    }

    expose_event(&options);

    return 0;
}
