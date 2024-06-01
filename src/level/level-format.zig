const LevelCell = packed struct(u8) {
    wall: bool,
    breakable: bool,
    kill: bool,
};

const Level = struct {
    data: [*]LevelCell,
    width: usize,
    height: usize,
};
