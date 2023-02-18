const std = @import("std");
const Builder = std.build.Builder;

pub fn build(b: *Builder) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{ .preferred_optimize_mode = .ReleaseSafe });

    const s4 = b.dependency("s4", .{
        .target = target,
    });

    const libgitz = b.dependency("libgitz", .{
        .target = target,
    });

    const flog = b.addExecutable(.{ .name = "flog", .target = target, .optimize = optimize });
    flog.install();
    flog.linkLibC();
    flog.linkLibrary(s4.artifact("s4"));
    flog.linkLibrary(libgitz.artifact("gitz"));
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
