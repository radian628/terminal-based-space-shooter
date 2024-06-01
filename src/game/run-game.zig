const std = @import("std");

const game_format = @import("./game-format.zig");

const Game = game_format.Game;

fn updateProjectiles(array: *std.AutoArrayHashMap(game_format.Projectile)) void {
    for (array) |proj| {
        proj.x += proj.dx;
        proj.y += proj.dy;
    }
}

// fn runGameStep(game: *Game) void {
//     // updateProjectiles(game.player_projectiles.getArray());
//     // updateProjectiles(game.enemy_projectiles.getArray());

// }
