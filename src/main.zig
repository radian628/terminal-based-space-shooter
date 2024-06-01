const std = @import("std");
const fs = std.fs;
const os = std.os.linux;
const debug = std.debug;
const Multitable = @import("./data-structures/multitable.zig").Multitable;
const input = @import("./game/input.zig");

const display = @import("./display/display.zig");

pub fn main() !void {
    display.clear();

    // allocate a buffer the size of the screen
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    const allocator = gpa.allocator();

    const ws = display.getWinsize();
    const displayBuffer = try allocator.alloc(u8, ws.ws_col * ws.ws_row);

    // display a "your screen is too small!" message
    // (to be changed later)
    try display.printTooSmall(allocator, displayBuffer);

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
        const buf = try input.get_all_of_stdin(allocator);
        defer buf.deinit();

        try tty.writer().print("got input {s}\n", .{buf.items});

        std.time.sleep(1000000000 / 60);
    }

    // re-enable echo and icanon
    _ = os.tcsetattr(tty.handle, .FLUSH, &old_termios);
}
