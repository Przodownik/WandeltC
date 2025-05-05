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

void diagnostics_print_at_location(SourceSpan* span, const char* message, DiagnosticPrintType print_type)
{
	// Calculate start and end positions
	uint32 start_row    = span->row;
	uint32 start_column = span->column;
	uint32 start_index  = get_index_from_position(span->source_file, start_row, start_column);
	uint32 end_index    = start_index + span->length;

	uint32 end_row, end_column;
	get_position_from_index(span->source_file, end_index, &end_row, &end_column);

	// Determine display range (2 lines of context before and after)
	uint32 context           = 2;
	uint32 display_start_row = start_row > context ? start_row - context : 1;
	const char* content_end  = span->source_file->content + span->source_file->content_size;
	uint32 display_end_row =
	    end_row + context < span->source_file->total_rows ? end_row + context : span->source_file->total_rows;

	// Prepare line number formatting
	uint32 max_line_length = (uint32)floor(log10(display_end_row)) + 1;
	char number_buffer[16];
	snprintf(number_buffer, sizeof(number_buffer), "%%%dd| %%.*s\n", max_line_length);

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
				row_end = content_end;
			uint32 row_length = (uint32)(row_end - row_start);

			// Print line number and content
			EVTRACE(number_buffer, current_row, row_length, row_start);

			// Print carets if this row is within the span
			if (current_row >= start_row && current_row <= end_row)
			{
				uint32 highlight_start_col, highlight_end_col;

				if (current_row == start_row)
				{
					highlight_start_col = start_column;
					highlight_end_col   = (start_row == end_row) ? end_column : row_length + 1;
				}
				else if (current_row == end_row)
				{
					highlight_start_col = 1;
					highlight_end_col   = end_column;
				}
				else // Intermediate rows
				{
					highlight_start_col = 1;
					highlight_end_col   = row_length + 1;
				}

				// Align carets with code (account for "  | " prefix)
				for (uint32 i = 0; i < max_line_length; i++) ETRACE(" ");
				ETRACE("  "); // "| "
				for (uint32 i = 1; i < highlight_start_col; i++) ETRACE(" ");
				for (uint32 i = highlight_start_col; i < highlight_end_col && i <= row_length + 1; i++)
					ETRACE(ANSI_COLOR_YELLOW "^" ANSI_COLOR_RESET);
				ETRACE("\n");
			}
		}

		// Move to next line
		row_start = strchr(row_start, '\n');

		if (row_start)
			row_start++;
		else
			break;

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
