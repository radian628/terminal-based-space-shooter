const std = @import("std");
const os = std.os.linux;

pub fn get_all_of_stdin(a: std.mem.Allocator) !std.ArrayList(u8) {
    const stdin = std.io.getStdIn().reader();

    var pfd: [1]os.pollfd = undefined;
    pfd[0].events = os.POLL.IN;
    pfd[0].fd = 0;

    var list = std.ArrayList(u8).init(a);

    while (os.poll(&pfd, 1, 0) != 0) {
        var buf: [1]u8 = undefined;
        _ = try stdin.read(&buf);
        try list.append(buf[0]);
    }

    return list;
}
