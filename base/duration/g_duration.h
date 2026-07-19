#ifndef GLINT_BASE_DURATION_DURATION_H
#define GLINT_BASE_DURATION_DURATION_H

#include "../core/g_types.h"

// ── Duration type ─────────────────────────────────────────────────────────
// Internal representation is nanoseconds. All creation/conversion uses u64
// to avoid floating-point surprises in the core type.

typedef struct duration
{
    u64 nanos;
} duration;

// ── Shorthand macros ──────────────────────────────────────────────────────
#define g_nanoseconds(n)  duration_from_ns((u64)(n))
#define g_microseconds(n) duration_from_us((u64)(n))
#define g_milliseconds(n) duration_from_ms((u64)(n))
#define g_seconds(n)      duration_from_seconds((f64)(n))
#define g_minutes(n)      duration_from_minutes((f64)(n))
#define g_hours(n)        duration_from_hours((f64)(n))

// ── Creation ──────────────────────────────────────────────────────────────
internal duration duration_from_ns(u64 ns);
internal duration duration_from_us(u64 us);
internal duration duration_from_ms(u64 ms);
internal duration duration_from_seconds(f64 s);
internal duration duration_from_minutes(f64 m);
internal duration duration_from_hours(f64 h);

// ── Conversion ────────────────────────────────────────────────────────────
internal f64  duration_seconds(duration d);
internal f64  duration_milliseconds(duration d);
internal f64  duration_microseconds(duration d);
internal u64  duration_nanoseconds(duration d);

// ── Arithmetic ────────────────────────────────────────────────────────────
internal duration duration_add(duration a, duration b);
internal duration duration_sub(duration a, duration b);  // clamps to zero
internal duration duration_scale(duration d, f64 factor);

// ── Comparison ────────────────────────────────────────────────────────────
internal b8 duration_eq(duration a, duration b);
internal b8 duration_ne(duration a, duration b);
internal b8 duration_gt(duration a, duration b);
internal b8 duration_lt(duration a, duration b);

// ── Extended breakdown ────────────────────────────────────────────────────
// Breaks a duration into logical components for display or inspection.

typedef struct duration_extended
{
    u64 hours;
    u64 minutes;
    u64 seconds;
    u64 milliseconds;
    u64 microseconds;
    u64 nanoseconds;
} duration_extended;

// Decompose a duration into its component parts
internal duration_extended duration_break(duration d);

// Reconstruct a duration from component parts
internal duration duration_from_extended(duration_extended de);

#endif // GLINT_BASE_DURATION_DURATION_H
