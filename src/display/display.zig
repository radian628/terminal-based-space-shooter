const std = @import("std");
const os = std.os.linux;

pub fn getWinsize() os.winsize {
    var ws: os.winsize = undefined;
    _ = os.ioctl(0, os.T.IOCGWINSZ, @intFromPtr(&ws));
    return ws;
}

pub fn clear() void {
    std.debug.print("\x1b[2J\x1b[1;1H", .{});
}

pub fn printTooSmall(allocator: std.mem.Allocator, buffer: []u8) !void {
    const ws = getWinsize();
    defer allocator.free(buffer);

    // print top and bottom
    for (0..ws.ws_col) |x| {
        buffer[x] = '#';
        buffer[x + ws.ws_col * (ws.ws_row - 1)] = '#';
    }

    // print sides
    for (1..ws.ws_row - 1) |y| {
        buffer[y * ws.ws_col] = '#';
        buffer[(y + 1) * ws.ws_col - 1] = '#';
        for (1..ws.ws_col - 1) |x| {
            buffer[y * ws.ws_col + x] = ' ';
        }
    }

    // create message
    const msg = try std.fmt.allocPrint(allocator, "{d} x {d} Too Small!", .{ ws.ws_col, ws.ws_row });
    defer allocator.free(msg);

    // add message to buffer
    for (msg, 0..) |char, i| {
        buffer[
            // center vertically
            ws.ws_row * (ws.ws_col / 2)
            // center horizontally
            + ws.ws_col / 2
            // offset to center text
            - msg.len / 2 + i
        ] = char;
    }

    std.debug.print("{s}", .{buffer});
}
