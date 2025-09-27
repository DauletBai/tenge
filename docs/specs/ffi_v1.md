# Tenge FFI v1 (C ABI)

## Goals
- Zero-cost interop with C functions and data layouts.
- Predictable memory and ownership semantics.
- No runtime reflection or hidden allocation.

## ABI
- C calling convention for all foreign functions.
- Primitive mappings: `i8/i16/i32/i64`, `u8/u16/u32/u64`, `f32/f64`, `bool`->`_Bool`.
- Structs: plain C layout, no padding reordering.
- Slices: `{ ptr: *T, len: u64 }` passed by value.
- Strings: `{ ptr: *u8, len: u64 }` UTF-8, not null-terminated.

## Pointers
- `*T` is non-owning borrow; lifetime managed by the caller.
- `*mut T` allows callee mutation; caller guarantees validity.

## Errors
- Foreign functions return `i32` (0 = OK) or a separate status enum; rich errors by out-params.

## Memory
- Default allocator is provided by Tenge runtime.
- Foreign code may accept a user-provided arena.
- No implicit copies across FFI boundaries.

## Linkage
- `extern "C"` symbol names are preserved.
- Static or dynamic linking is allowed.

## Safety Notes
- No panics/unwinds across FFI. Abort or convert to error codes.
- All FFI functions are `unsafe` to call by default at the surface API.