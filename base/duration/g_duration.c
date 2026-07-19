#include "g_duration.h"

#include "../core/g_types.h"

#define NANOS_PER_US  ((u64)1000)
#define NANOS_PER_MS  ((u64)1000000)
#define NANOS_PER_S   ((u64)1000000000)
#define NANOS_PER_MIN ((u64)60000000000)
#define NANOS_PER_H   ((u64)3600000000000)

// ── Creation ──────────────────────────────────────────────────────────────

internal duration duration_from_ns(u64 ns)
{
    return (duration){ .nanos = ns };
}

internal duration duration_from_us(u64 us)
{
    return (duration){ .nanos = us * NANOS_PER_US };
}

internal duration duration_from_ms(u64 ms)
{
    return (duration){ .nanos = ms * NANOS_PER_MS };
}

internal duration duration_from_seconds(f64 s)
{
    return (duration){ .nanos = (u64)(s * (f64)NANOS_PER_S) };
}

internal duration duration_from_minutes(f64 m)
{
    return (duration){ .nanos = (u64)(m * (f64)NANOS_PER_MIN) };
}

internal duration duration_from_hours(f64 h)
{
    return (duration){ .nanos = (u64)(h * (f64)NANOS_PER_H) };
}

// ── Conversion ────────────────────────────────────────────────────────────

internal f64 duration_seconds(duration d)
{
    return (f64)d.nanos / (f64)NANOS_PER_S;
}

internal f64 duration_milliseconds(duration d)
{
    return (f64)d.nanos / (f64)NANOS_PER_MS;
}

internal f64 duration_microseconds(duration d)
{
    return (f64)d.nanos / (f64)NANOS_PER_US;
}

internal u64 duration_nanoseconds(duration d)
{
    return d.nanos;
}

// ── Arithmetic ────────────────────────────────────────────────────────────

internal duration duration_add(duration a, duration b)
{
    return (duration){ .nanos = a.nanos + b.nanos };
}

internal duration duration_sub(duration a, duration b)
{
    return (duration){ .nanos = a.nanos > b.nanos ? a.nanos - b.nanos : 0 };
}

internal duration duration_scale(duration d, f64 factor)
{
    return (duration){ .nanos = (u64)((f64)d.nanos * factor) };
}

// ── Comparison ────────────────────────────────────────────────────────────

internal b8 duration_eq(duration a, duration b)
{
    return a.nanos == b.nanos;
}

internal b8 duration_ne(duration a, duration b)
{
    return a.nanos != b.nanos;
}

internal b8 duration_gt(duration a, duration b)
{
    return a.nanos > b.nanos;
}

internal b8 duration_lt(duration a, duration b)
{
    return a.nanos < b.nanos;
}

// ── Extended breakdown ────────────────────────────────────────────────────

internal duration_extended duration_break(duration d)
{
    duration_extended de = {0};
    u64 ns = d.nanos;

    de.hours        = ns / NANOS_PER_H;
    ns             %= NANOS_PER_H;
    de.minutes      = ns / NANOS_PER_MIN;
    ns             %= NANOS_PER_MIN;
    de.seconds      = ns / NANOS_PER_S;
    ns             %= NANOS_PER_S;
    de.milliseconds = ns / NANOS_PER_MS;
    ns             %= NANOS_PER_MS;
    de.microseconds = ns / NANOS_PER_US;
    de.nanoseconds  = ns % NANOS_PER_US;

    return de;
}

internal duration duration_from_extended(duration_extended de)
{
    return (duration){
        .nanos = de.hours        * NANOS_PER_H
               + de.minutes      * NANOS_PER_MIN
               + de.seconds      * NANOS_PER_S
               + de.milliseconds * NANOS_PER_MS
               + de.microseconds * NANOS_PER_US
               + de.nanoseconds,
    };
}
