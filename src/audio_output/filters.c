/*****************************************************************************
 * filters.c : audio output filters management
 *****************************************************************************
 * Copyright (C) 2002-2007 VLC authors and VideoLAN
 * $Id$
 *
 * Authors: Christophe Massiot <massiot@via.ecp.fr>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

/*****************************************************************************
 * Preamble
 *****************************************************************************/
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <assert.h>

#include <vlc_common.h>
#include <vlc_dialog.h>
#include <vlc_modules.h>

#include <vlc_aout.h>
#include <vlc_filter.h>
#include <vlc_cpu.h>
#include "aout_internal.h"
#include <libvlc.h>

/*****************************************************************************
 * FindFilter: find an audio filter for a specific transformation
 *****************************************************************************/
static filter_t * FindFilter( vlc_object_t *obj,
                              const audio_sample_format_t *infmt,
                              const audio_sample_format_t *outfmt )
{
    static const char typename[] = "audio filter";
    const char *type = "audio filter", *name = NULL;
    filter_t * p_filter;

    p_filter = vlc_custom_create( obj, sizeof(*p_filter), typename );

    if ( p_filter == NULL ) return NULL;

    p_filter->fmt_in.audio = *infmt;
    p_filter->fmt_in.i_codec = infmt->i_format;
    p_filter->fmt_out.audio = *outfmt;
    p_filter->fmt_out.i_codec = outfmt->i_format;

    if( infmt->i_format == outfmt->i_format
     && infmt->i_physical_channels == outfmt->i_physical_channels
     && infmt->i_original_channels == outfmt->i_original_channels )
    {
        assert( infmt->i_rate != outfmt->i_rate );
        type = "audio resampler";
        name = "$audio-resampler";
    }

    p_filter->p_module = module_need( p_filter, type, name, false );
    if ( p_filter->p_module == NULL )
    {
        vlc_object_release( p_filter );
        return NULL;
    }

    assert( p_filter->pf_audio_filter );
    return p_filter;
}

/**
 * Splits audio format conversion in two simpler conversions
 * @return 0 on successful split, -1 if the input and output formats are too
 * similar to split the conversion.
 */
static int SplitConversion( const audio_sample_format_t *restrict infmt,
                            const audio_sample_format_t *restrict outfmt,
                            audio_sample_format_t *midfmt )
{
    *midfmt = *outfmt;

    /* Lastly: resample (after format conversion and remixing) */
    if( infmt->i_rate != outfmt->i_rate )
        midfmt->i_rate = infmt->i_rate;
    else
    /* Penultimately: remix channels (after format conversion) */
    if( infmt->i_physical_channels != outfmt->i_physical_channels
     || infmt->i_original_channels != outfmt->i_original_channels )
    {
        midfmt->i_physical_channels = infmt->i_physical_channels;
        midfmt->i_original_channels = infmt->i_original_channels;
    }
    else
    /* Second: convert linear to S16N as intermediate format */
    if( AOUT_FMT_LINEAR( infmt ) )
    {
        /* All conversion from linear to S16N must be supported directly. */
        if( outfmt->i_format == VLC_CODEC_S16N )
            return -1;
        midfmt->i_format = VLC_CODEC_S16N;
    }
    else
    /* First: convert non-linear to FI32 as intermediate format */
    {
        if( outfmt->i_format == VLC_CODEC_FI32 )
            return -1;
        midfmt->i_format = VLC_CODEC_FI32;
    }

    assert( !AOUT_FMTS_IDENTICAL( infmt, midfmt ) );
    aout_FormatPrepare( midfmt );
    return 0;
}

#undef aout_FiltersCreatePipeline
/**
 * Allocates audio format conversion filters
 * @param obj parent VLC object for new filters
 * @param filters table of filters [IN/OUT]
 * @param nb_filters pointer to the number of filters in the table [IN/OUT]
 * @param infmt input audio format
 * @param outfmt output audio format
 * @return 0 on success, -1 on failure
 */
int aout_FiltersCreatePipeline( vlc_object_t *obj,
                                filter_t **filters,
                                int *nb_filters,
                                const audio_sample_format_t *restrict infmt,
                                const audio_sample_format_t *restrict outfmt )
{
    audio_sample_format_t curfmt = *outfmt;
    unsigned i = 0, max = *nb_filters - AOUT_MAX_FILTERS;

    filters += *nb_filters;
    aout_FormatsPrint( obj, "filter(s)", infmt, outfmt );

    while( !AOUT_FMTS_IDENTICAL( infmt, &curfmt ) )
    {
        if( i >= max )
        {
            msg_Err( obj, "max (%u) filters reached", AOUT_MAX_FILTERS );
            dialog_Fatal( obj, _("Audio filtering failed"),
                          _("The maximum number of filters (%u) was reached."),
                          AOUT_MAX_FILTERS );
            goto rollback;
        }

        /* Make room and prepend a filter */
        memmove( filters + 1, filters, i * sizeof( *filters ) );

        *filters = FindFilter( obj, infmt, &curfmt );
        if( *filters != NULL )
        {
            i++;
            break; /* done! */
        }

        audio_sample_format_t midfmt;
        /* Split the conversion */
        if( SplitConversion( infmt, &curfmt, &midfmt ) )
        {
            msg_Err( obj, "conversion pipeline failed: %4.4s -> %4.4s",
                     (const char *)&infmt->i_format,
                     (const char *)&outfmt->i_format );
            goto rollback;
        }

        *filters = FindFilter( obj, &midfmt, &curfmt );
        if( *filters == NULL )
        {
            msg_Err( obj, "cannot find filter for simple conversion" );
            goto rollback;
        }
        curfmt = midfmt;
        i++;
    }

    msg_Dbg( obj, "conversion pipeline completed" );
    *nb_filters += i;
    return 0;

rollback:
    aout_FiltersDestroyPipeline( filters, i );
    return -1;
}

/**
 * Destroys a chain of audio filters.
 */
void aout_FiltersDestroyPipeline( filter_t *const *filters, unsigned n )
{
    for( unsigned i = 0; i < n; i++ )
    {
        filter_t *p_filter = filters[i];

        module_unneed( p_filter, p_filter->p_module );
        vlc_object_release( p_filter );
    }
}

/**
 * Filters an audio buffer through a chain of filters.
 */
void aout_FiltersPlay( filter_t *const *pp_filters,
                       unsigned i_nb_filters, block_t ** pp_block )
{
    block_t *p_block = *pp_block;

    /* TODO: use filter chain */
    for( unsigned i = 0; (i < i_nb_filters) && (p_block != NULL); i++ )
    {
        filter_t * p_filter = pp_filters[i];

        /* Please note that p_block->i_nb_samples & i_buffer
         * shall be set by the filter plug-in. */
        p_block = p_filter->pf_audio_filter( p_filter, p_block );
    }
    *pp_block = p_block;
}
