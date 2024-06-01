const std = @import("std");

const Type = std.builtin.Type;

const Allocator = std.mem.Allocator;

pub fn MultitableImpl(comptime Shape: type) type {
    const shape_info = @typeInfo(Shape);

    const shape_info_struct: std.builtin.Type.Struct = switch (shape_info) {
        .Struct => |s| s,
        else => @compileError("Multitables may only created from structs."),
    };

    comptime var fields: []const Type.StructField = &.{};

    inline for (shape_info_struct.fields) |field| {
        var field_copy = field;
        field_copy.name = "map_" ++ field_copy.name;
        field_copy.type = (std.AutoArrayHashMap(
            field_copy.type,
            *std.DoublyLinkedList(Shape).Node,
        ));
        field_copy.default_value = null;
        fields = fields ++ [1]Type.StructField{field_copy};
    }

    fields = fields ++ [_]Type.StructField{
        .{
            .name = "data",
            .type = std.DoublyLinkedList(Shape),
            .is_comptime = false,
            .alignment = @alignOf(Shape),
            .default_value = null,
        },
    };

    const multitable_info = Type{
        .Struct = .{
            .layout = Type.ContainerLayout.auto,
            .is_tuple = false,
            .fields = fields,
            .decls = &.{},
        },
    };

    return @Type(multitable_info);
}

pub fn Multitable(comptime Shape: type) type {
    return struct {
        impl: MultitableImpl(Shape) = undefined,

        allocator: Allocator,

        const Self = @This();

        pub fn init(a: Allocator) Self {
            const shape_info_struct = @typeInfo(Shape).Struct;

            var mt: Multitable(Shape) = undefined;
            mt.allocator = a;
            mt.impl.data = std.DoublyLinkedList(Shape){};

            inline for (shape_info_struct.fields) |field| {
                @field(mt.impl, "map_" ++ field.name) //
                = std.AutoArrayHashMap(
                    field.type,
                    *std.DoublyLinkedList(Shape).Node,
                ).init(mt.allocator);
            }

            return mt;
        }

        pub fn deinit(mt: *Self) void {
            const shape_info_struct = @typeInfo(Shape).Struct;

            inline for (shape_info_struct.fields) |field| {
                @field(mt.impl, "map_" ++ field.name).deinit();
            }
        }

        pub fn get(mt: *Self, comptime col: []const u8, key: anytype) ?*const Shape {
            var map = @field(mt.impl, "map_" ++ col);
            return &(map.get(key) orelse return null).data;
        }

        pub fn remove(mt: *Self, comptime col: []const u8, key: anytype) bool {
            const shape_info_struct = @typeInfo(Shape).Struct;

            var map = @field(mt.impl, "map_" ++ col);
            const node = map.get(key) orelse return false;

            inline for (shape_info_struct.fields) |field| {
                _ = @field(mt.impl, "map_" ++ field.name).swapRemove(
                    @field(node.data, field.name),
                );
            }

            mt.allocator.destroy(node);

            return true;
        }

        pub fn add(mt: *Self, value: Shape) !void {
            const shape_info_struct = @typeInfo(Shape).Struct;

            var node = try mt.allocator.create(std.DoublyLinkedList(Shape).Node);
            node.data = value;

            mt.impl.data.prepend(node);

            inline for (shape_info_struct.fields) |field| {
                try @field(mt.impl, "map_" ++ field.name).put(
                    @field(value, field.name),
                    node,
                );
            }
        }
    };
}

test "Simple multitable example." {
    var mt = Multitable(struct { a: u64, b: u64 }).init(
        std.testing.allocator,
    );
    defer mt.deinit();

    try mt.add(.{ .a = 6, .b = 9 });

    try mt.add(.{ .a = 3, .b = 4 });

    const t = mt.get("a", 3) orelse unreachable;
    try std.testing.expect(t.a == 3);
    try std.testing.expect(t.b == 4);

    const t2 = mt.get("b", 9) orelse unreachable;
    try std.testing.expect(t2.a == 6);
    try std.testing.expect(t2.b == 9);

    _ = mt.remove("a", 6);
    _ = mt.remove("a", 3);
}
