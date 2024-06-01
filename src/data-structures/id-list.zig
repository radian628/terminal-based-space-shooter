const PolymorphicSOA = @import("./polymorphic-soa.zig").PolymorphicSOA;
const KeyType = @import("./polymorphic-soa.zig").KeyType;

const std = @import("std");

pub fn IDList(comptime Shape: type) type {
    return struct {
        impl: PolymorphicSOA(struct { shape: Shape }) = undefined,
        const Self = @This();

        pub fn init(a: std.mem.Allocator) Self {
            var idl: IDList(Shape) = undefined;
            idl.impl = PolymorphicSOA(struct { shape: Shape }).init(a);

            return idl;
        }

        pub fn deinit(idl: *Self) void {
            idl.impl.deinit();
        }

        pub fn add(idl: *Self, value: Shape) !KeyType {
            return idl.impl.add("shape", value);
        }

        pub fn get(idl: *Self, key: KeyType) ?*Shape {
            return idl.impl.get("shape", key);
        }

        pub fn remove(idl: *Self, key: KeyType) bool {
            return idl.impl.remove("shape", key);
        }

        pub fn getArray(idl: *Self) *std.AutoArrayHashMap(KeyType, Shape) {
            return idl.impl.getArray("shape");
        }
    };
}
