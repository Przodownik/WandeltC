#include "diagnostics.h"

#include <math.h>

#define ERROR_MESSAGE_LINES_SHOWN 5

uint32 count_digits(uint32 number)
{
	uint32 count = 0;
	while (number != 0)
	{
		number /= 10;
		count++;
	}
	return count;
}

// Helper function to find the start of a row
static const char* get_row_start(const File* file, uint32 row)
{
	const char* p      = file->content;
	uint32 current_row = 1;
	while (current_row < row && p < file->content + file->content_size)
	{
		if (*p == '\n')
		{
			current_row++;
		}
		p++;
	}
	return p;
}

// Helper function to convert display column to character column
static uint32 get_char_column_from_display_column(const char* line_start, uint32 display_col, uint32 tab_width)
{
	uint32 char_col            = 1;
	uint32 current_display_col = 1;
	const char* p              = line_start;
	while (*p != '\n' && *p != '\0')
	{
		if (current_display_col >= display_col)
		{
			return char_col;
		}
		if (*p == '\t')
		{
			current_display_col += tab_width;
		}
		else
		{
			current_display_col += 1;
		}
		p++;
		char_col++;
	}
	// If display_col exceeds the line, extrapolate additional columns
	while (current_display_col < display_col)
	{
		current_display_col += 1;
		char_col++;
	}
	return char_col;
}

void diagnostics_print_at_location(SourceSpan* span, const char* message, DiagnosticPrintType print_type)
{
	uint32 tab_width = TAB_SIZE; // Assume TAB_SIZE is defined, e.g., 4

	// Get the start of the row to calculate character column from display column
	const char* start_row_start = get_row_start(span->source_file, span->row);
	uint32 start_char_column    = get_char_column_from_display_column(start_row_start, span->column, tab_width);
	uint32 start_index          = get_index_from_position(span->source_file, span->row, start_char_column);
	uint32 end_index            = start_index + span->length;

	uint32 end_row, end_char_column;
	get_position_from_index(span->source_file, end_index, &end_row, &end_char_column);

	// Determine display range (2 lines of context before and after)
	uint32 context           = 2;
	uint32 display_start_row = span->row > context ? span->row - context : 1;
	const char* content_end  = span->source_file->content + span->source_file->content_size;
	uint32 display_end_row   = end_row + context;

	// Prepare line number formatting
	uint32 max_line_length = (uint32)floor(log10(display_end_row)) + 1;
	char number_buffer[16];
	snprintf(number_buffer, sizeof(number_buffer), "%%%dd| ", max_line_length);

	ETRACE("\n");

	// Print each line in the display range
	const char* row_start = span->source_file->content;
	uint32 current_row    = 1;

	while (current_row <= display_end_row && row_start < content_end)
	{
		if (current_row >= display_start_row)
		{
			// Find line end
			const char* row_end = strchr(row_start, '\n');
			if (!row_end)
			{
				row_end = content_end;
			}
			uint32 row_length = (uint32)(row_end - row_start);

			// Print line number
			ETRACE(number_buffer, current_row);

			// Print line content with tabs expanded
			const char* p = row_start;
			while (p < row_end)
			{
				if (*p == '\t')
				{
					for (uint32 i = 0; i < tab_width; i++)
					{
						ETRACE(" ");
					}
				}
				else
				{
					ETRACE("%c", *p);
				}
				p++;
			}
			ETRACE("\n");

			// Print carets if this row is within the span
			if (current_row >= span->row && current_row <= end_row)
			{
				uint32 highlight_start_char_col, highlight_end_char_col;

				// Determine character columns to highlight
				if (current_row == span->row)
				{
					highlight_start_char_col = start_char_column;
				}
				else
				{
					highlight_start_char_col = 1;
				}

				if (current_row == end_row)
				{
					highlight_end_char_col = end_char_column;
				}
				else
				{
					highlight_end_char_col = row_length + 1;
				}

				// Convert to display columns
				uint32 display_start_col = get_display_column(row_start, highlight_start_char_col);
				uint32 display_end_col   = get_display_column(row_start, highlight_end_char_col);

				// Align carets with code (account for prefix)
				for (uint32 i = 0; i < max_line_length; i++)
				{
					ETRACE(" ");
				}
				ETRACE("  "); // "| "
				for (uint32 i = 1; i < display_start_col; i++)
				{
					ETRACE(" ");
				}
				for (uint32 i = display_start_col; i < display_end_col; i++)
				{
					ETRACE(ANSI_COLOR_YELLOW "^" ANSI_COLOR_RESET);
				}
				ETRACE("\n");
			}
		}

		// Move to next line
		row_start = strchr(row_start, '\n');
		if (row_start)
		{
			row_start++;
		}
		else
		{
			break;
		}
		current_row++;
	}

	// Print diagnostic message
	switch (print_type)
	{
	case PRINT_TYPE_ERROR:
		ETRACE(ANSI_COLOR_RED "Error in file (%s) in row %d in column %d\n%s\n" ANSI_COLOR_RESET,
		       span->source_file->path, span->row, span->column, message);
		break;
	case PRINT_TYPE_WARN:
		ETRACE(ANSI_COLOR_ORANGE "Warning in file (%s) in row %d in column %d\n%s\n" ANSI_COLOR_RESET,
		       span->source_file->path, span->row, span->column, message);
		break;
	case PRINT_TYPE_NOTE:
		ETRACE(ANSI_COLOR_BLUE "Note in file (%s) in row %d in column %d\n%s\n" ANSI_COLOR_RESET,
		       span->source_file->path, span->row, span->column, message);
		break;
	default:
		ASSERT(false, "Invalid print type");
	}
}

void diagnostics_vnote_along_span(SourceSpan* location, const char* message, va_list args)
{
	diagnostics_print_at_location(location, cstring_vformat(message, args), PRINT_TYPE_NOTE);
}

void diagnostics_vwarning_along_span(SourceSpan* location, const char* message, va_list args)
{
	diagnostics_print_at_location(location, cstring_vformat(message, args), PRINT_TYPE_WARN);
}

void diagnostics_verror_along_span(SourceSpan* location, const char* message, va_list args)
{
	diagnostics_print_at_location(location, cstring_vformat(message, args), PRINT_TYPE_ERROR);
}
