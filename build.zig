const std = @import("std");

pub fn build(b: *std.build.Builder) void {
    const target = b.standardTargetOptions(.{});
    const mode = b.standardReleaseOptions();

    const s4 = b.addStaticLibrary("s4", null);
    s4.defineCMacro("CONFIG_VERSION", "\"2021-03-27\"");
    s4.setTarget(target);
    s4.setBuildMode(mode);
    s4.linkLibC();
    s4.addCSourceFiles(&.{
        "deps/s4/cutils.c",
        "deps/s4/libregexp.c",
        "deps/s4/libunicode.c",
        "deps/s4/quickjs.c",
    }, &.{
        "-Wall",
        "-W",
        "-Wstrict-prototypes",
        "-Wwrite-strings",
        "-Wno-missing-field-initializers",
    });
    const flog = b.addExecutable("flog", null);
    flog.setTarget(target);
    flog.setBuildMode(mode);
    flog.install();
    flog.linkLibC();
    flog.linkLibrary(s4);
    flog.linkSystemLibrary("git2");
    flog.addIncludePath("deps");
    flog.addCSourceFiles(&.{
        "src/app.c",
        "src/database.c",
        "src/engine.c",
        "src/file.c",
        "src/flog.c",
        "src/git.c",
        "src/module.c",
        "src/module-json.c",
        "src/string.c",
        "src/commands/base.c",
        "src/commands/file.c",
        "src/commands/help.c",
        "src/commands/info.c",
        "src/commands/install.c",
        "src/commands/list.c",
        "src/commands/script.c",
        "src/commands/sync.c",
        "src/commands/update.c",
        "src/commands/with.c",
    }, &.{
        "-std=c11",
        "-pedantic",
        "-Wall",
        "-W",
        "-Wno-missing-field-initializers",
        "-fno-sanitize=undefined",
    });
}
