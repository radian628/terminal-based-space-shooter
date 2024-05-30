const std = @import("std");
const fs = std.fs;
const os = std.os.linux;
const debug = std.debug;

pub fn main() !void {
    // Prints to stderr (it's a shortcut based on `std.io.getStdErr()`)
    std.debug.print("All your {s} are belong to us.\n", .{"codebase"});

    // stdout is for the actual output of your application, for example if you
    // are implementing gzip, then only the compressed bytes should be sent to
    // stdout, not any debugging messages.
    const stdout_file = std.io.getStdOut().writer();
    var bw = std.io.bufferedWriter(stdout_file);
    const stdout = bw.writer();

    try stdout.print("Run `zig build test` to run the tests.\n", .{});

    try bw.flush(); // don't forget to flush!

    var tty: fs.File = try fs.cwd().openFile("/dev/tty", .{ .mode = .read_write });
    defer tty.close();

    var old_termios: os.termios = undefined;
    _ = os.tcgetattr(tty.handle, &old_termios);
    var modified_termios = old_termios;

    // modified_termios.lflag &= ~@as(os.tc_lflag_t, .os.tc_lflag_t.ECHO | .os.tc_lflag_t.ICANON);
    modified_termios.lflag.ECHO = false;
    modified_termios.lflag.ICANON = false;

    _ = os.tcsetattr(tty.handle, .FLUSH, &modified_termios);

    while (true) {
        var buffer: [1]u8 = undefined;

        _ = try tty.read(&buffer);

        if (buffer[0] == 'x') {
            break;
        }

        try tty.writer().print("got input: {s}\n", .{buffer});
    }

    _ = os.tcsetattr(tty.handle, .FLUSH, &old_termios);
}

test "simple test" {
    var list = std.ArrayList(i32).init(std.testing.allocator);
    defer list.deinit(); // try commenting this out and see if zig detects the memory leak!
    try list.append(42);
    try std.testing.expectEqual(@as(i32, 42), list.pop());
}
