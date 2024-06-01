const std = @import("std");
const Multitable = @import("../data-structures/multitable.zig").Multitable;
const PolymorphicSOA = @import("../data-structures/polymorphic-soa.zig").PolymorphicSOA;
const IDList = @import("../data-structures//id-list.zig").IDList;

const Game = struct {
    current_time: f64,
    drawables: IDList(Drawable),
    timer: IDList(Timer),
    enemy_special: PolymorphicSOA(struct {
        four_directions: struct {
            fire_timer: u64,
        },
        down: struct {
            is_moving_left: bool,
            fire_timer: u64,
        },
        follow: struct {
            fire_timer: u64,
        },
    }),
    enemy_general: IDList(EnemyGeneral),
    enemies: Multitable(struct {
        enemy_data: u64,
        drawable: u64,
        hitbox: u64,
    }),
    player: Player,
    player_projectiles: IDList(Projectile),
    enemy_projectiles: IDList(Projectile),
};

const Timer = struct {
    interval: f64,
    runs_out_at: f64,

    // if the timer runs out
    pub fn check(timer: *Timer, game: *Game) bool {
        if (timer.runs_out_at > game.current_time) {
            timer.runs_out_at = game.current_time + timer.interval;
            return true;
        }
        return false;
    }
};

const Drawable = struct {
    str: [*:0]u8,
    x: isize,
    y: isize,
};

const Player = struct {
    hitpoints: u64,

    x: f64,
    y: f64,
};

const Health = struct {
    max: u64,
    current: u64,
};

const EnemyGeneral = struct {
    // position
    x: f64,
    y: f64,

    // hitbox
    left: u64,
    right: u64,
    top: u64,
    bottom: u64,

    // HP
    health: Health,
};

const Projectile = struct {
    // position and velocity
    x: f64,
    y: f64,
    dx: f64,
    dy: f64,

    damage: u64,
};
