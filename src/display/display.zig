const std = @import("std");
const os = std.os.linux;

pub fn getWinsize() os.winsize {
    var ws: os.winsize = undefined;
    _ = os.ioctl(0, os.T.IOCGWINSZ, @intFromPtr(&ws));
    return ws;
}
