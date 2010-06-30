/* 
 *	Copyright (C) 2003-2006 Gabest
 *	http://www.gabest.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *   
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *   
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include <stdio.h>
#include <malloc.h>
#include "libpng.h"
#include "..\..\libpng\png.h"

static void read_data_fn(png_structp png_ptr, png_bytep data, png_size_t length)
{
	struct png_t* png = (struct png_t*)png_get_progressive_ptr(png_ptr);
	if(png->pos + length > png->size) png_error(png_ptr, "Read Error");
	memcpy(data, &png->data[png->pos], length);
	png->pos += length;
}
unsigned char* DecompressPNG(struct png_t* png, int* w, int* h)
{
    png_structp png_ptr;
	png_infop info_ptr;
	png_infop end_info;

	unsigned char* pic;
	unsigned char* row;
	unsigned int x, y, c;

	if(png_sig_cmp(png->data, 0, 8) != 0) 
		return NULL;

	png->pos = 8;

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		// (png_voidp)user_error_ptr, user_error_fn, user_warning_fn);
    if(!png_ptr) 
		return NULL;

    png_set_read_fn(png_ptr, (png_voidp)png, read_data_fn);

    info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        return NULL;
    }

    end_info = png_create_info_struct(png_ptr);
    if(!end_info)
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        return NULL;
    }

    if(setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        return NULL;
    }

	png_set_sig_bytes(png_ptr, 8);

	png_read_png(
		png_ptr, info_ptr, 
		PNG_TRANSFORM_STRIP_16 | 
		PNG_TRANSFORM_STRIP_ALPHA | 
		PNG_TRANSFORM_PACKING |
		PNG_TRANSFORM_EXPAND |
		PNG_TRANSFORM_BGR, 
		NULL);

	if(png_get_channels(png_ptr, info_ptr) != 3) 
	{
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		return NULL;
	}

	pic = calloc(info_ptr->width * info_ptr->height, 4);

	*w = info_ptr->width;
	*h = info_ptr->height;

	for(y = 0; y < info_ptr->height; y++)
	{
		row = &pic[y * info_ptr->width * 4];

		for(x = 0; x < info_ptr->width*3; row += 4)
		{
			for(c = 0; c < 3; c++)
			{
				row[c] = info_ptr->row_pointers[y][x++];
			}
		}
	}

    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

	return pic;
}
