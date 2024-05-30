const std = @import("std");
const fs = std.fs;
const os = std.os.linux;
const debug = std.debug;

const display = @import("./display/display.zig");

pub fn main() !void {
    std.debug.print("\n");
    const ws = display.getWinsize();
    std.debug.print("winsize {d} {d}\n", .{ ws.ws_col, ws.ws_row });

    // get current terminal
    var tty: fs.File = try fs.cwd().openFile("/dev/tty", .{ .mode = .read_write });
    defer tty.close();

    // get termios object
    var old_termios: os.termios = undefined;
    _ = os.tcgetattr(tty.handle, &old_termios);
    var modified_termios = old_termios;

    // disable echo and icanon
    modified_termios.lflag.ECHO = false;
    modified_termios.lflag.ICANON = false;
    _ = os.tcsetattr(tty.handle, .FLUSH, &modified_termios);

    // repeatedly get chars from input
    while (true) {
        // get input
        var buffer: [1]u8 = undefined;
        _ = try tty.read(&buffer);

        // exit if input is 'x'
        if (buffer[0] == 'x') {
            break;
        }

        // acknowledge the input; print it to console
        try tty.writer().print("got input: {s}\n", .{buffer});
    }

    // re-enable echo and icanon
    _ = os.tcsetattr(tty.handle, .FLUSH, &old_termios);
}
