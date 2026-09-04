# Memory Compacter

A lightweight C++ utility for **compacting objects into a contiguous memory region**.  
This function is useful when working with custom allocators, arenas, or low‑level memory layouts where objects must be tightly packed and relocated safely.

---

## Overview

`Compact` takes a list of object pointers and a raw byte region, then:

- Validates all inputs  
- Sorts the object pointers  
- Moves each object into the provided memory region  
- Destroys the original objects  
- Updates the pointers to their new locations  
- Returns how many objects were successfully compacted

It is a deterministic, move‑based compaction routine intended for performance‑sensitive or memory‑constrained environments.

---

## Function Signature

```cpp
template <typename T>
CompactionDetails Compact(std::vector<T*>& objects, ByteView region);
```

Where:

```cpp
struct CompactionDetails {
    int count{};   // number of objects moved
};

using ByteView = std::span<std::byte>;
```

---

## Behavior

### 1. Input Validation
The function throws `std::logic_error` if:

- Any pointer in `objects` is `nullptr`
- The memory region is empty
- The region is too small to hold all objects

### 2. Sorting
Objects are sorted before compaction to ensure a stable, predictable order.

### 3. Compaction Process
For each object:

- It is move‑constructed into the next available location in `region`
- The original object is destroyed
- The pointer in `objects` is updated
- The region span is advanced by `sizeof(T)`
- The compaction stops early if the region runs out of space

### 4. Result
The returned `CompactionDetails` reports how many objects were successfully relocated.

---

## Constraints & Notes

- The caller is responsible for ensuring the memory region is valid and properly aligned for `T`.
- The compaction is **destructive** — original objects are destroyed.
- The function assumes `T` is move‑constructible.
- The region must be large enough for `objects.size() * sizeof(T)` bytes.
- Ordering after compaction is based on sorted pointer values.

---

## Example Usage

```cpp
std::vector<MyType*> objs = { a, b, c };
std::array<std::byte, 1024> buffer;

ByteView region(buffer.data(), buffer.size());

auto result = Compact(objs, region);

std::cout << "Moved objects: " << result.count << "\n";
```

---

## Purpose

This utility is designed for systems programming, custom allocators, game engines, embedded systems, and any environment where **manual memory layout control** is required.

---
