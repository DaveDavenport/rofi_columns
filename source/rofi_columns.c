/**
 *
 * MIT/X11 License
 * Copyright (c) 2016 Qball Cow <qball@gmpclient.org>
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
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <glib.h>
#include <glib/gstdio.h>

/**
 * Stores a field.
 * Contains strings (need to be free-ed).
 * Contains the lengths (in characters, not bytes) of the string.
 */
typedef struct Field
{
    char   *value;
    size_t length;
} Field;

/**
 * Stores a column.
 * Contains max_lengths (in characters, not bytes) of the fields.
 * number of rows in this column
 * Array of fields (n_rows long)
 */
typedef struct Column
{
    size_t max_length;
    size_t n_rows;
    Field  *fields;
}Column;

/**
 * Stores the data.
 * contains number of columns and rows.
 * array of columns.
 */
typedef struct Data
{
    size_t n_columns;
    size_t n_rows;

    Column **columns;
} Data;

Data internal_store = {
    .n_columns =    0,
    .n_rows    =    0,
    .columns   = NULL,
};

/**
 * Get column, creates it if it does not exists.
 */
static Column * get_column ( size_t index )
{
    if ( internal_store.n_columns <= index ) {
        internal_store.n_columns = index + 1;
        internal_store.columns   = g_realloc ( internal_store.columns,
                                               ( internal_store.n_columns ) * sizeof ( Column* ) );
        internal_store.columns[index] = g_malloc0 ( sizeof ( Column ) );
    }
    return internal_store.columns[index];
}

/**
 * Free internal data storage.
 */
static void free_internal ( void )
{
    for ( size_t c = 0; c < internal_store.n_columns; c++ ) {
        Column *col = get_column ( c );
        for ( size_t r = 0; r < col->n_rows; r++ ) {
            g_free ( col->fields[r].value );
        }
        g_free ( col->fields );
        g_free ( col );
    }
    g_free ( internal_store.columns );
}

/**
 * Set column in last row.
 * Takes ownership of field and set field pointer to NULL.
 */
static void set_row ( Column *column, char **field )
{
    size_t nl = internal_store.n_rows + 1;
    column->fields = g_realloc ( column->fields, nl * sizeof ( Field ) );
    if ( internal_store.n_rows > column->n_rows ) {
        for ( size_t s = column->n_rows; s < internal_store.n_rows; s++ ) {
            column->fields[s].value  = NULL;
            column->fields[s].length = 0;
        }
    }
    column->fields[internal_store.n_rows].value = *field;
    size_t strl = g_utf8_strlen ( *field, -1 );
    column->max_length                           = ( strl > column->max_length ) ? strl : column->max_length;
    column->fields[internal_store.n_rows].length = strl;
    *field                                       = NULL;
    // Internal counter;
    column->n_rows = nl;
}
/**
 * Stores an array of fields in current row.
 */
static void store_result ( char **split )
{
    for ( size_t column = 0; split && split[column]; column++ ) {
        Column *c = get_column ( column );
        set_row ( c, &( split[column] ) );
    }
}

static void read_input ( FILE *io, GRegex *splitter_regex, gboolean advanced )
{
    char    *data     = NULL;
    size_t  data_size = 0;
    ssize_t l         = 0;

    while ( ( l = getline ( &data, &data_size, io ) ) > 0 ) {
        if ( data[l - 1] == '\n' ) {
            data[l - 1] = '\0';
        }
        if ( advanced ) {
            size_t     column = 0;
            GMatchInfo *info;
            g_regex_match_full ( splitter_regex, data, l, 0, 0, &info, NULL );
            while ( g_match_info_matches ( info ) ) {
                int mc = g_match_info_get_match_count ( info );
                if ( mc == 1 ) {
                    gchar  *word = g_match_info_fetch ( info, 0 );
                    Column *c    = get_column ( column );
                    set_row ( c, &( word ) );
                    column++;
                }
                else {
                    for ( int i = 1; i < mc; i++ ) {
                        gchar  *word = g_match_info_fetch ( info, i );
                        Column *c    = get_column ( i - 1 );
                        set_row ( c, &( word ) );
                    }
                }
                g_match_info_next ( info, NULL );
            }
            g_match_info_free ( info );
        }
        else {
            char **split = g_regex_split_full ( splitter_regex, data, l, 0, 0, 0, NULL );
            store_result ( split );
            g_strfreev ( split );
        }
        internal_store.n_rows++;
    }

    if ( data_size > 0 ) {
        free ( data );
        data = NULL;
    }
}

static void output_simple ( FILE *io, gboolean escape  )
{
    for ( size_t row = 0; row < internal_store.n_rows; row++ ) {
        for ( size_t column = 0; column < internal_store.n_columns; column++ ) {
            Column *c     = get_column ( column );
            char   *d     = ( row < c->n_rows && c->fields[row].value ) ? c->fields[row].value : "";
            size_t length = ( row < c->n_rows ) ? c->fields[row].length : 0;
            if ( column ) {
                putc ( ' ', io );
            }
            // Copy result
            if ( escape ) {
                char *m = g_markup_escape_text ( d, -1 );
                fputs ( m, io );
                g_free ( m );
            }
            else {
                fputs ( d, io );
            }
            for ( size_t sp = c->max_length - length; sp > 0; sp-- ) {
                putc ( ' ', io );
            }
        }
        if ( ( row + 1 ) < internal_store.n_rows ) {
            putchar ( '\n' );
        }
    }
}
/**
 * Advanced formatting.
 * {1} First column
 * {2} Second.
 * {1:0}  Max padding.
 * {1:-1} no padding
 * {1:10} max 10 width
 */
struct FormatArg
{
    size_t   row;
    gboolean escape;
};
static gboolean helper_eval_cb ( const GMatchInfo *info, GString *str, gpointer data )
{
    struct FormatArg *arg = ( (struct FormatArg *) data );
    gchar            *match;
    // Get the match
    match = g_match_info_fetch ( info, 0 );
    if ( match != NULL ) {
        char    *iter  = &match[1];
        ssize_t type   = 0;
        size_t  column = g_ascii_strtoll ( iter, &iter, 10 );
        if ( *iter == ':' ) {
            iter++;
            type = g_ascii_strtoll ( iter, &iter, 10 );
        }
        // TODO check column
        if ( ( column ) > internal_store.n_columns  ) {
            g_free ( match );
            return FALSE;
        }
        Column *c     = get_column ( column - 1 );
        char   *d     = ( arg->row < c->n_rows && c->fields[arg->row].value ) ? c->fields[arg->row].value : "";
        size_t length = ( arg->row < c->n_rows ) ? c->fields[arg->row].length : 0;
        if ( type == 0 ) {
            // Padding.
            if ( arg->escape ) {
                d = g_markup_escape_text ( d, -1 );
            }
            g_string_append ( str, d );
            for ( size_t sp = c->max_length - length; sp > 0; sp-- ) {
                g_string_append_c ( str, ' ' );
            }
            if ( arg->escape ) {
                g_free ( d );
            }
        }
        else if ( type == -1 ) {
            // Print withtout anything done to it.
            if ( arg->escape ) {
                d = g_markup_escape_text ( d, -1 );
            }
            g_string_append ( str, d );
            if ( arg->escape ) {
                g_free ( d );
            }
        }
        else if ( type > 0 ) {
            size_t rlength = type;
            if ( ( type > 0 && ( length < rlength ) ) ) {
                if ( arg->escape ) {
                    d = g_markup_escape_text ( d, -1 );
                }
                g_string_append ( str, d );
                for ( size_t sp = type - length; sp > 0; sp-- ) {
                    g_string_append_c ( str, ' ' );
                }
                if ( arg->escape ) {
                    g_free ( d );
                }
            }
            else {
                char *e = g_utf8_offset_to_pointer ( d, rlength );
                if ( arg->escape ) {
                    d = g_markup_escape_text ( d, e - d );
                    g_string_append_len ( str, d, -1 );
                    g_free ( d );
                }
                else {
                    g_string_append_len ( str, d, e - d );
                }
            }
        }
        g_free ( match );
    }
    return FALSE;
}
static void output_advanced (  FILE *io, gboolean escape, const char *format )
{
    GRegex *r = g_regex_new ( "{[-\\w]+(:-?[0-9]+)?}", 0, 0, NULL );
    for ( size_t row = 0; row < internal_store.n_rows; row++ ) {
        struct FormatArg arg  = { .row = row, .escape = escape, };
        char             *res = g_regex_replace_eval ( r, format, -1, 0, 0, helper_eval_cb, &arg, NULL );
        fputs ( res, io );
        g_free ( res );
    }
    g_regex_unref ( r );
}

int main ( int argc, char **argv )
{
    GError   *error = NULL;
    // Config options.
    char     *output_format      = NULL;
    char     *splitter_regex_str = "[ ]+";
    char     *input              = "-";
    char     *output             = "-";
    gboolean output_stats        = FALSE;
    gboolean advanced_matching   = FALSE;
    gboolean escape_output       = FALSE;

    /* Parse commandline options */
    GOptionEntry   entries[] =
    {
        { "matching", 'm', 0, G_OPTION_ARG_NONE,   &advanced_matching,  "Use matching, instead of splitting",        NULL       },
        { "splitter", 's', 0, G_OPTION_ARG_STRING, &splitter_regex_str, "Regex used for splitting the input string", "\"[ ]+\"" },
        { "stats",      0, 0, G_OPTION_ARG_NONE,   &output_stats,       "Output statistics of input",                NULL       },
        { "format",   'f', 0, G_OPTION_ARG_STRING, &output_format,      "The output format",                         NULL       },
        { "escape",   'e', 0, G_OPTION_ARG_NONE,   &escape_output,      "Escape output for use in pango markup",     NULL       },
        { "input",    'i', 0, G_OPTION_ARG_STRING, &input,              "Set input file (- for stdin).",             NULL       },
        { "output",   'o', 0, G_OPTION_ARG_STRING, &output,             "Set output file (- for stdout).",           NULL       },
        { NULL,         0, 0,                   0, NULL,                NULL,                                        NULL       }
    };
    GOptionContext *context;
    context = g_option_context_new ( "- UTF-8 Columnate list tool" );
    g_option_context_add_main_entries ( context, entries, NULL );
    if ( !g_option_context_parse ( context, &argc, &argv, &error ) ) {
        fprintf ( stderr, "option parsing failed: %s\n", error->message );
        g_option_context_free ( context );
        return EXIT_FAILURE;
    }
    g_option_context_free ( context );

    /* Compile splitter regex */
    GRegex *splitter_regex = g_regex_new ( splitter_regex_str, 0, 0, &error );
    if ( error != NULL ) {
        fprintf ( stderr, "Failed to parse regex: %s\n", error->message );
        g_error_free ( error );
        return EXIT_FAILURE;
    }

    FILE *input_fp  = stdin;
    FILE *output_fp = stdout;

    if ( input && !( input[0] == '-' && input[1] == '\0' ) ) {
        input_fp = g_fopen ( input, "r" );
        if ( input_fp == NULL ) {
            fprintf ( stderr, "Failed to open input file: '%s'\n", input );
            g_regex_unref ( splitter_regex );
            return EXIT_FAILURE;
        }
    }
    if ( output && !( output[0] == '-' && output[1] == '\0' ) ) {
        output_fp = g_fopen ( output, "r" );
        if ( output_fp == NULL ) {
            fprintf ( stderr, "Failed to open output file: '%s'\n", output );
            g_regex_unref ( splitter_regex );
            if ( input_fp && input_fp != stdin ) {
                fclose ( input_fp );
            }
            return EXIT_FAILURE;
        }
    }

    /* Parse input into internal data structures. */
    read_input ( stdin, splitter_regex, advanced_matching );

    /* Close input */
    if ( input_fp && input_fp != stdin ) {
        fclose ( input_fp );
    }
    /* Cleanup regex parser */
    g_regex_unref ( splitter_regex );

    if ( output_stats ) {
        fprintf ( output_fp, "Got rows: %zu\n", internal_store.n_rows );
        fprintf ( output_fp, "Got columns: %zu\n", internal_store.n_columns );
        for ( size_t i = 0; i < internal_store.n_columns; i++ ) {
            Column *c = get_column ( i );
            fprintf ( output_fp, "Column: %zu Length: %zu\n", i + 1, c->max_length );
        }
    }
    else {
        if ( output_format == NULL ) {
            // Simple mode
            output_simple ( stdout, escape_output );
        }
        else {
            output_advanced ( stdout, escape_output, output_format );
        }
    }

    /** Cleanup */
    if ( output_fp && output_fp != stdout ) {
        fclose ( output_fp );
    }
    free_internal ();
    return EXIT_SUCCESS;
}
