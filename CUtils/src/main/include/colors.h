/**
 * @file
 *
 * The modules allows to generate prints with colors
 *
 * @author koldar
 * @date Apr 6, 2018
 */

#ifndef COLORS_H_
#define COLORS_H_

#include <P99/p99.h>

/**
 * @addtogroup colors
 * @{
 * @brief all the available colors the module offers
 *
 * Note that this module is specically thought for linux
 */

///reset foreground and black background
#define CU_COLOR_NONE "\e[0m"

///white foreground and black background
#define CU_COLOR_WHITE "\e[1;37m"

///black foreground and black background
#define CU_COLOR_BLACK "\e[0;30m"

///blue foreground and black background
#define CU_COLOR_BLUE "\e[0;34m"

///light blue foreground and black background
#define CU_COLOR_LIGHT_BLUE "\e[1;34m"

///gren foreground and black background
#define CU_COLOR_GREEN "\e[0;32m"

///light green foreground and black background
#define CU_COLOR_LIGHT_GREEN "\e[1;32m"

///cyan foreground and black background
#define CU_COLOR_CYAN "\e[0;36m"

///light cyan foreground and black background
#define CU_COLOR_LIGHT_CYAN "\e[1;36m"

///red foreground and black background
#define CU_COLOR_RED "\e[0;31m"

///red foreground and black background
#define CU_COLOR_LIGHT_RED "\e[1;31m"

///purple foreground and black background
#define CU_COLOR_PURPLE "\e[0;35m"

///light purple foreground and black background
#define CU_COLOR_LIGHT_PURPLE "\e[1;35m"

///brown foreground and black background
#define CU_COLOR_BROWN "\e[0;33m"

///yellow foreground and black background
#define CU_COLOR_YELLOW "\e[1;33m"

///gray foreground and black background
#define CU_COLOR_GRAY "\e[0;30m"

///light gray foreground and black background
#define CU_COLOR_LIGHT_GRAY "\e[0;37m"

///reset foreground and black background
#define CU_RESET_COLOR CU_COLOR_NONE

///@}

/**
 * shows the string with a certain color
 *
 * @private
 *
 * @param[in] color the color to use
 * @param[in] str the string to color
 */
#define _CU_WITH_COLOR(color, str) color str CU_RESET_COLOR

/**
 * white foreground and black background
 *
 * @param[in] str the string to color
 */
#define CU_WHITE(str)			_CU_WITH_COLOR(CU_COLOR_WHITE, str)

/**
 * black foreground and black background
 *
 * @param[in] str the string to color
 */
#define CU_BLACK(str)			_CU_WITH_COLOR(CU_COLOR_BLACK, str)

/**blue foreground and black background
 *
 * @param[in] str the string to color
 */
#define CU_BLUE(str)			_CU_WITH_COLOR(CU_COLOR_BLUE, str)

/**
 * light blue foreground and black background
 *
 * @param[in] str the string to color
 */
#define CU_LIGHT_BLUE(str)		_CU_WITH_COLOR(CU_COLOR_LIGHT_BLUE, str)

/**
 * green foreground and black background
 *
 * @param[in] str the string to color
 */
#define CU_GREEN(str)			_CU_WITH_COLOR(CU_COLOR_GREEN, str)

/**
 * light green foreground and black background
 *
 * @param[in] str the string to color
 */
#define CU_LIGHT_GREEN(str)		_CU_WITH_COLOR(CU_COLOR_LIGHT_GREEN, str)

/**
 * cyan foreground and black background
 *
 * @param[in] str the string to color
 */
#define CU_CYAN(str)			_CU_WITH_COLOR(CU_COLOR_CYAN, str)

/**light cyan foreground and black background
 *
 * @param[in] str the string to color
 */
#define CU_LIGHT_CYAN(str)		_CU_WITH_COLOR(CU_COLOR_LIGHT_CYAN, str)

/**
 * red foreground and black background
 *
 * @param[in] str the string to color
 */
#define CU_RED(str)				_CU_WITH_COLOR(CU_COLOR_RED, str)

/**
 * light red foreground and black background
 *
 * @param[in] str the string to color
 */
#define CU_LIGHT_RED(str)		_CU_WITH_COLOR(CU_COLOR_LIGHT_RED, str)

/**
 * purple foreground and black background
 *
 * @param[in] str the string to color
 */
#define CU_PURPLE(str)			_CU_WITH_COLOR(CU_COLOR_PURPLE, str)

/**
 * light purple foreground and black background
 *
 * @param[in] str the string to color
 */
#define CU_LIGHT_PURPLE(str)	_CU_WITH_COLOR(CU_COLOR_LIGHT_PURPLE, str)

/**
 * brown foreground and black background
 *
 * @param[in] str the string to color
 */
#define CU_BROWN(str)			_CU_WITH_COLOR(CU_COLOR_BROWN, str)

/**
 * yellow foreground and black background
 *
 * @param[in] str the string to color
 */
#define CU_YELLOW(str)			_CU_WITH_COLOR(CU_COLOR_YELLOW, str)

/**
 * gray foreground and black background
 *
 * @param[in] str the string to color
 */
#define CU_GRAY(str)			_CU_WITH_COLOR(CU_COLOR_GRAY, str)

/**
 * light gray foreground and black background
 *
 * @param[in] str the string to color
 */
#define CU_LIGHT_GRAY(str)		_CU_WITH_COLOR(CU_COLOR_LIGHT_GRAY, str)

/**
 * snprintf variant where you can specify the color as well
 *
 * @param[in] buffer the buffer where to store the string
 * @param[in] size the size of the buffer
 * @param[in] color the color to use
 * @param[in] template the template string (printf style)
 * @param[in] ... format actual values
 */
#define cuSncprintf(buffer, size, color, template, ...) snprintf(buffer, size, _CU_WITH_COLOR(color, template), ## __VA_ARGS__)

/**
 * sprintf variant where you can specify the color as well
 *
 * @param[in] buffer the buffer where to store the string
 * @param[in] color the color to use
 * @param[in] template the template string (printf style)
 * @param[in] ... format actual values
 */
#define cuScprintf(buffer, color, template, ...) sprintf(buffer, _CU_WITH_COLOR(color, template), ## __VA_ARGS__)

/**
 * fprintf variant where you can specify the color as well
 *
 * @param[in] f the file where to write the string into
 * @param[in] color the color to use
 * @param[in] template the template string (printf style)
 * @param[in] ... format actual values
 */
#define cuFcprintf(f, color, template, ...) fprintf(f, _CU_WITH_COLOR(color, template), ## __VA_ARGS__)

/**
 * printf variant where you can specify the color as well
 *
 * @param[in] color the color to use
 * @param[in] template the template string (printf style)
 * @param[in] ... format actual values
 */
#define cuCprintf(color, template, ...) printf(f, _CU_WITH_COLOR(color, template), ## __VA_ARGS__)

/**
 * vprintf variant where you can specify the color as well
 *
 * @param[in] color the color to use
 * @param[in] format the template string (printf style)
 * @param[in] ap `va_list` variable for `vprintf`
 */
#define cuVcprintf(color, format, ap) vprintf(_CU_WITH_COLOR(color, format), ap)

/**
 * vprintf variant where you can specify the color as well
 *
 * @param[in] f the file where to write the string in
 * @param[in] color the color to use
 * @param[in] format the template string (printf style)
 * @param[in] ap `va_list` variable for `vprintf`
 */
#define cuVfcprintf(f, color, format, ap) vfprintf(_CU_WITH_COLOR(color, f), ap)

/**
 * vsprintf variant where you can specify the color as well
 *
 * @param[in] buffer the buffer where to store the string
 * @param[in] color the color to use
 * @param[in] format the template string (printf style)
 * @param[in] ap `va_list` variable for `vprintf`
 */
#define cuVscprintf(buffer, color, format, ap) vsprintf(buffer, _CU_WITH_COLOR(color, format), ap)

/**
 * vsnprintf variant where you can specify the color as well
 *
 * @param[in] buffer the buffer where to store the string
 * @param[in] size the size of the buffer
 * @param[in] color the color to use
 * @param[in] format the template string (printf style)
 * @param[in] ap `va_list` variable for `vprintf`
 */
#define cuVsncprintf(buffer, size, color, format, ap) vsnprintf(buffer, size, _CU_WITH_COLOR(color, format), ap)

#endif /* COLORS_H_ */
