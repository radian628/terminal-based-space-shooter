const std = @import("std");
const Type = std.builtin.Type;

const KeyType = u64;

pub fn PolymorphicSOAImpl(comptime Shapes: type) type {
    const shape_info = @typeInfo(Shapes).Struct;

    comptime var fields: []const Type.StructField = &.{};

    // id for adding items
    fields = fields ++ [_]Type.StructField{
        .{
            .name = "current_id",
            .type = KeyType,
            .is_comptime = false,
            .alignment = @alignOf(KeyType),
            .default_value = null,
        },
    };

    // add individual hash tables for items
    inline for (shape_info.fields) |field| {
        fields = fields ++ [_]Type.StructField{
            .{
                .name = "map_" ++ field.name,
                .type = std.AutoArrayHashMap(
                    KeyType,
                    field.type,
                ),
                .default_value = null,
                .alignment = @alignOf(std.AutoArrayHashMap(KeyType, field.type)),
                .is_comptime = false,
            },
        };
    }

    const polymorphic_soa_info = Type{
        .Struct = .{
            .layout = Type.ContainerLayout.auto,
            .is_tuple = false,
            .fields = fields,
            .decls = &.{},
        },
    };

    return @Type(polymorphic_soa_info);
}

pub fn PolymorphicSOA(comptime Shapes: type) type {
    return struct {
        impl: PolymorphicSOAImpl(Shapes) = undefined,
        allocator: std.mem.Allocator,

        const shapes_info = @typeInfo(Shapes).Struct;
        const Self = @This();

        pub fn init(a: std.mem.Allocator) Self {
            var soa: PolymorphicSOA(Shapes) = undefined;
            soa.allocator = a;
            soa.impl.current_id = 0;

            inline for (shapes_info.fields) |field| {
                @field(soa.impl, "map_" ++ field.name) //
                = std.AutoArrayHashMap(KeyType, field.type).init(soa.allocator);
            }

            return soa;
        }

        pub fn deinit(soa: *Self) void {
            inline for (shapes_info.fields) |field| {
                @field(soa.impl, "map_" ++ field.name).deinit();
            }
        }

        pub fn add(soa: *Self, comptime col: []const u8, value: ColType(col)) !KeyType {
            var map = &@field(soa.impl, "map_" ++ col);
            try map.put(soa.impl.current_id, value);
            const prev_id = soa.impl.current_id;
            soa.impl.current_id += 1;
            return prev_id;
        }

        pub fn get(soa: *Self, comptime col: []const u8, key: KeyType) ?*ColType(col) {
            var map = @field(soa.impl, "map_" ++ col);
            return map.getPtr(key);
        }

        pub fn remove(soa: *Self, id: KeyType) bool {
            inline for (shapes_info.fields) |field| {
                const delete_result = @field(soa.impl, "map_" ++ field.name) //
                    .swapRemove(id);

                if (delete_result) return true;
            }

            return false;
        }

        fn ColType(comptime col: []const u8) type {
            inline for (shapes_info.fields) |field| {
                if (std.mem.eql(u8, field.name, col)) {
                    return field.type;
                }
            }
            @compileError("Column'" ++ col ++ "'does not exist.");
        }

        pub fn get_array(soa: *Self, comptime col: []const u8) std.AutoArrayHashMap(usize, ColType(col)) {
            return @field(soa.impl, col);
        }
    };
}

test "Simple PolymorphicSOA example." {
    var soa = PolymorphicSOA(comptime struct { a: u64, b: []const u8 }).init(
        std.testing.allocator,
    );
    defer soa.deinit();

    const id1 = try soa.add("a", 69);
    const id2 = try soa.add("b", "test");
    const id3 = try soa.add("a", 32);
    const id4 = try soa.add("b", "foo bar");

    try std.testing.expect(soa.get("a", id1).?.* == 69);
    try std.testing.expect(soa.get("a", id3).?.* == 32);
    try std.testing.expect(std.mem.eql(
        u8,
        soa.get("b", id2).?.*,
        "test",
    ));
    try std.testing.expect(std.mem.eql(
        u8,
        soa.get("b", id4).?.*,
        "foo bar",
    ));

    try std.testing.expect(soa.remove(id1));
    try std.testing.expect(soa.remove(id2));
    try std.testing.expect(soa.remove(id3));
    try std.testing.expect(soa.remove(id4));
}
