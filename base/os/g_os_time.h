#ifndef GLINT_BASE_OS_OS_TIME_H
#define GLINT_BASE_OS_OS_TIME_H

#include "../../base/core/g_types.h"

typedef struct os_time
{
	u64 seconds;
	u64 nanoseconds; // 0–999,999,999
} os_time;

// A day of the week. Sunday == 0 to match POSIX `struct tm::tm_wday`.
typedef enum os_weekday
{
	os_weekday_sunday	= 0,
	os_weekday_monday	= 1,
	os_weekday_tuesday	= 2,
	os_weekday_wednesday = 3,
	os_weekday_thursday	= 4,
	os_weekday_friday	= 5,
	os_weekday_saturday	= 6,
	os_weekday_COUNT	= 7,
} os_weekday;

// Broken-down calendar date extracted from an `os_time` instant. This is the
// instant-shaped companion to `os_time` — use it when you need year/month/day
// or weekday. `os_time` itself intentionally stays a thin epoch pair because it
// doubles as a duration (os_time_sub returns one); a duration has no calendar.
typedef struct os_date
{
	u16			year;				// e.g. 2026
	u8			month;				// 1..12
	u8			day;				// 1..31
	u8			hour;				// 0..23
	u8			minute;				// 0..59
	u8			second;				// 0..59 (leap seconds are not represented)
	u32			nanosecond;			// 0..999_999_999
	os_weekday	weekday;			// Sunday..Saturday
	u16			day_of_year;		// 1..366
	b8			is_dst;				// daylight saving in effect?
	i32			utc_offset_seconds; // local tz offset from UTC, east positive
} os_date;

// Current wall-clock time (real-time clock)
internal os_time os_time_now(void);

// High-resolution monotonic time for measuring intervals.
// Not related to wall-clock time — use os_time_sub/as_ms/as_us for deltas.
internal os_time os_time_monotonic(void);

// a - b, with nanosecond borrow (returns zero if a < b)
internal os_time os_time_sub(os_time a, os_time b);

// Convert to scalar — useful for deltas from os_time_sub
internal u64 os_time_as_ms(os_time t);
internal u64 os_time_as_us(os_time t);

// Break an instant into local-time calendar fields.
internal os_date os_time_to_date(os_time t);

// Break an instant into UTC calendar fields. utc_offset_seconds is always 0.
internal os_date os_time_to_date_utc(os_time t);

#endif // GLINT_BASE_OS_OS_TIME_H
