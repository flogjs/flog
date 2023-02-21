const std = @import("std");
const debug = std.debug;

export fn setup_debug_handlers() void {
    debug.maybeEnableSegfaultHandler();
}

export fn dump_stack_trace() void {
    debug.dumpCurrentStackTrace(@returnAddress());
}

pub const _start = {};
