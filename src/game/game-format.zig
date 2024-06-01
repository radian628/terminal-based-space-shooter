const std = @import("std");

const Game = struct {
    drawables: std.AutoArrayHashMap(u64, Drawable),
    positions: std.AutoArrayHashMap(u64, Position),
    player: Player,
};

const Drawable = struct {
    str: [*:0]u8,
    x: isize,
    y: isize,
};

const Position = struct {
    x: f64,
    y: f64,
};

const Player = struct {
    pos_id: u64,
    hitpoints: u64,
};

const ProjectileShooter = struct {
    interval: f64,
    fire: fn (
        game: Game,
    ) void,
};

const Health = struct { hitpoints: u64, max_hitpoints: u64 };

const EnemyTag = enum { four_directions, down, follow };

const EnemyData = union(EnemyTag) {
    four_directions: struct {},
    down: struct { is_moving_left: bool },
    follow: struct {},
};

const Enemy = struct {
    data: EnemyData,
    hp_id: u64,
    pos_id: u64,
};
