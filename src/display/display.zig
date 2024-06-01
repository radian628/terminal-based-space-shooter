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

// TODO: add more colors
const Color = enum {
    White,
    Red,
    Green,
    Blue,
    Yellow,
};

const DisplayBuffers = struct {
    prev_data: [*]u8,
    next_data: [*]u8,

    prev_color: [*]Color,
    next_color: [*]Color,

    width: usize,
    height: usize,

    allocator: std.mem.Allocator,

    pub fn init(a: std.mem.Allocator) DisplayBuffers {
        var buf: DisplayBuffers = undefined;
        buf.allocator = a;
        buf.width = 0;
        buf.height = 0;
        buf.realloc_buffers();
        return buf;
    }

    pub fn realloc_buffers(buf: *DisplayBuffers) !void {
        const size = buf.width * buf.height;

        buf.allocator.free(buf.prev_data);
        buf.allocator.free(buf.next_data);
        buf.allocator.free(buf.prev_color);
        buf.allocator.free(buf.next_color);

        buf.prev_data = try buf.allocator.alloc(u8, size);
        buf.next_data = try buf.allocator.alloc(u8, size);
        buf.prev_color = try buf.allocator.alloc(Color, size);
        buf.next_color = try buf.allocator.alloc(Color, size);
    }

    pub fn redraw_screen(
        buffers: *DisplayBuffers,
        comptime Context: type,
        comptime redraw: fn (buffers: DisplayBuffers, ctx: Context) void,
        context: Context,
    ) !void {
        const ws = getWinsize();

        if ((ws.ws_col != buffers.width) || ws.ws_row != buffers.height) {
            buffers.width = ws.ws_col;
            buffers.height = ws.ws_row;
            buffers.realloc_buffers();
        }

        redraw(buffers, context);
    }
};
