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
	uint32 max_line_length       = span->row == 0 ? 2 : (uint32)floor(log10(span->row)) + 1;
	uint32 max_lines_for_display = ERROR_MESSAGE_LINES_SHOWN - max_line_length - 2;

	char number_buffer[16];
	snprintf(number_buffer, sizeof(number_buffer), "%%%dd| %%.*s\n", max_line_length);

	int32 index = span->row - ERROR_MESSAGE_LINES_SHOWN + 1;
	if (index < 1)
		index = 1;

	uint32 row_start_index = (uint32)index;

	const char* row_start = span->source_file->content;

	TRACE("\n");

	if (span->row != 1)
	{
		while (row_start_index < span->row)
		{
			row_start = span->source_file->content;
			for (uint32 i = 0; i < row_start_index; i++)
			{
				row_start = strchr(row_start, '\n');
				if (row_start == nullptr)
					break;
				row_start++;
			}

			const char* row_end = strchr(row_start, '\n');
			if (row_end == nullptr)
				row_end = span->source_file->content + span->source_file->content_size;

			uint32 row_length = (uint32)(row_end - row_start);

			VTRACE(number_buffer, row_start_index + 1, row_length, row_start);

			row_start_index++;
		}
	}
	else
	{
		const char* row_end = strchr(row_start, '\n');
		uint32 row_length   = (uint32)(row_end - row_start);
		VTRACE(number_buffer, 1, row_length, row_start);
	}

	// Skip the formatted number buffer line number
	for (uint32 i = 0; i < count_digits(row_start_index); i++)
	{
		TRACE(" ");
	}

	// Skip the "| " before the code starts
	TRACE("  ");

	for (uint32 i = 1; i < span->column; i++)
	{
		TRACE(" ");
	}

	for (uint32 l = 0; l < span->length; l++)
	{
		TRACE(ANSI_COLOR_YELLOW "^" ANSI_COLOR_RESET);
	}

	TRACE("\n");

	while (row_start_index <= span->row + max_lines_for_display + 1)
	{
		row_start = strchr(row_start, '\n');
		if (row_start == nullptr)
			break;

		row_start++;
		const char* row_end = strchr(row_start, '\n');
		if (row_end == nullptr)
			row_end = span->source_file->content + span->source_file->content_size;

		uint32 row_length = (uint32)(row_end - row_start);

		VTRACE(number_buffer, row_start_index + 1, row_length, row_start);

		row_start_index++;
	}

	switch (print_type)
	{
	case PRINT_TYPE_ERROR:
		TRACE(ANSI_COLOR_RED "Error in file (%s) in row %d in column %d\n%s\n" ANSI_COLOR_RESET,
		      span->source_file->path, span->row, span->column, message);
		break;
	case PRINT_TYPE_WARN:
		TRACE(ANSI_COLOR_ORANGE "Warning in file (%s) in row %d in column %d\n%s\n" ANSI_COLOR_RESET,
		      span->source_file->path, span->row, span->column, message);
		break;
	case PRINT_TYPE_NOTE:
		TRACE(ANSI_COLOR_BLUE "Note in file (%s) in row %d in column %d\n%s\n" ANSI_COLOR_RESET,
		      span->source_file->path, span->row, span->column, message);
		break;
	default:
		ASSERT(false, "Invalid print type");
	}

	TRACE("\n");
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
