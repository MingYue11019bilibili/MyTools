// Hash/HashMap/ToolHashMap.h

#pragma once

#ifndef MMYMYS_TOOLS_HASH_MAP_H
#define MMYMYS_TOOLS_HASH_MAP_H

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "../../toolsfamily.h"

#define TOOL_HASH_MAP_HASH_EMPTY     ((int64_t)INT64_C(0x8000000000000000))
#define TOOL_HASH_MAP_HASH_TOMBSTONE ((int64_t)INT64_C(0x8000000000000001))
#define TOOL_HASH_MAP_NO_LIVE_IDX    ((uint32_t)UINT32_C(0xffffffff))

#define TOOL_EXPAND_HM(Parameter) Parameter
#define TOOL_GET_NAME_3_HM(_1, _2, _3, ...) _3
#define TOOL_hashMapInit_1(map) toolHashMapInitImpl(map, 8)
#define TOOL_hashMapInit_2(map, initialCap) toolHashMapInitImpl(map, initialCap)
#define toolHashMapInit(...) TOOL_EXPAND_HM(TOOL_GET_NAME_3_HM(__VA_ARGS__, TOOL_hashMapInit_2, TOOL_hashMapInit_1)(__VA_ARGS__))

typedef struct {
	int64_t  key;
	int64_t  value;
	uint32_t liveIdx;
} TOOL_HashSlot;

typedef struct {
	TOOL_HashSlot* slots;
	uint32_t*  liveIndices;
	size_t     cap;
	size_t     nUsed;
	size_t     nLive;
} TOOL_HashMap;

TOOLFUNC__ uint64_t toolHashMapHashKey(int64_t k) {
	uint64_t h = (uint64_t)k;
	h ^= ~h + (h << 31);
	h ^= h >> 33;h *= 0xff51afd7ed558ccdULL;
	h ^= h >> 35;h *= 0xc4ceb9fe1a85ec53ULL;
	h ^= h >> 37;h *= 0xbf58476d1ce4e5b9ULL;
	return h;
}

TOOLFUNC__ void toolHashMapFree(TOOL_HashMap* map) {
	TOOL_ASSERT(map, "map 不为 NULL");
	// 利用 free 传入 NULL 是安全的，所以不需要判断指针是否为 NULL
	free(map->slots);map->slots = NULL;
	free(map->liveIndices);map->liveIndices = NULL;
	map->cap = 0;
	map->nUsed = 0;
	map->nLive = 0;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 6386)
#endif // _MSC_VER
TOOLFUNC__ bool toolHashMapRehash(TOOL_HashMap* map, size_t newCap) {
	TOOL_ASSERT(map, "map 不为 NULL");
	TOOL_HashSlot* oldSlots = map->slots;
	uint32_t* oldLive = map->liveIndices;
	size_t oldNLive = map->nLive;
	if (!(map->slots = (TOOL_HashSlot*)malloc(newCap * sizeof(TOOL_HashSlot)))) {
		map->slots = oldSlots;
		return false;
	}
	if (!(map->liveIndices = (uint32_t*)malloc(newCap * sizeof(uint32_t)))) {
		free(map->slots);
		map->slots = oldSlots;
		map->liveIndices = oldLive;
		return false;
	}
	map->cap = newCap;
	map->nUsed = 0;
	map->nLive = 0;
	for (size_t i = 0;i < newCap;i++) {
		map->slots[i].key = TOOL_HASH_MAP_HASH_EMPTY;
		map->slots[i].value = 0;
		map->slots[i].liveIdx = TOOL_HASH_MAP_NO_LIVE_IDX;
	}
	for (size_t i = 0;i < oldNLive;i++) {
		uint32_t idx = oldLive[i];
		int64_t k = oldSlots[idx].key;
		int64_t v = oldSlots[idx].value;
		size_t mask = map->cap - 1;
		size_t probe = toolHashMapHashKey(k) & mask;
		while (map->slots[probe].key != TOOL_HASH_MAP_HASH_EMPTY) {
			probe = (probe + 1) & mask;
		}
		map->slots[probe].key = k;
		map->slots[probe].value = v;
		map->slots[probe].liveIdx = (uint32_t)map->nLive;
		map->liveIndices[map->nLive] = (uint32_t)probe;
		map->nLive++;
		map->nUsed++;
	}
	free(oldSlots);
	free(oldLive);
	return true;
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

TOOLFUNC__ void toolHashMapInitImpl(TOOL_HashMap* map, size_t initialCap) {
	TOOL_ASSERT(map, "map 不为 NULL");
	if (initialCap < 8) {
		initialCap = 8;
	}
	else if ((initialCap & (initialCap - 1)) != 0) {
		size_t cap = 1;
		while (cap < initialCap) {
			cap <<= 1;
		}
		initialCap = cap;
	}
	if (!(map->slots = (TOOL_HashSlot*)malloc(initialCap * sizeof(TOOL_HashSlot))))return;
	if (!(map->liveIndices = (uint32_t*)malloc(initialCap * sizeof(uint32_t)))) {
		free(map->slots);
		map->slots = NULL;
		return;
	}
	map->cap = initialCap;
	map->nUsed = 0;
	map->nLive = 0;
	for (size_t i = 0;i < initialCap;i++) {
		map->slots[i].key = TOOL_HASH_MAP_HASH_EMPTY;
		map->slots[i].value = 0;
		map->slots[i].liveIdx = TOOL_HASH_MAP_NO_LIVE_IDX;
	}
}

TOOLFUNC__ bool toolHashMapPush(TOOL_HashMap* map, int64_t key, int64_t value) {
	TOOL_ASSERT(map, "map 不为 NULL");
	// 利用短路求值来实现条件判断和 rehash，避免频繁调用 rehash，自带安全锁，如果没有该特性也不会影响正常使用
	if (((map->nUsed + 1) * 4 >= map->cap * 3) && \
		    (!toolHashMapRehash(map, \
			((map->nUsed + 1) * 4 >= map->cap * 3) ? (map->cap * 2) : (map->cap))))
		return false;
	size_t mask = map->cap - 1;
	size_t idx = (size_t)toolHashMapHashKey(key) & mask;
	for (;;) {
		TOOL_HashSlot* s = &map->slots[idx];
		if (s->key == TOOL_HASH_MAP_HASH_EMPTY || s->key == TOOL_HASH_MAP_HASH_TOMBSTONE) {
			map->nUsed += (s->key == TOOL_HASH_MAP_HASH_TOMBSTONE) ? 0 : 1;
			s->key = key;
			s->value = value;
			s->liveIdx = (uint32_t)map->nLive;
			map->liveIndices[map->nLive] = (uint32_t)idx;
			map->nLive++;
			return true;
		}
	    if (s->key == key) {
			s->value = value;
			return true;
		}
		idx = (idx + 1) & mask;
	}
}

TOOLFUNC__ bool toolHashMapSearch(const TOOL_HashMap* map, int64_t key, int64_t* outValue) {
	TOOL_ASSERT(map, "map 不为 NULL");
	if (map->cap == 0)return false;
	size_t mask = map->cap - 1;
	size_t idx = (size_t)toolHashMapHashKey(key) & mask;
	for (size_t probe = 0;probe < map->cap;probe++) {
		const TOOL_HashSlot* s = &map->slots[idx];
		if (s->key == TOOL_HASH_MAP_HASH_EMPTY)return false;
		if (s->key == key) {
			if (outValue)*outValue = s->value;
			return true;
		}
		idx = (idx + 1) & mask;
	}
	return false;
}

TOOLFUNC__ bool toolHashMapRemove(TOOL_HashMap* map, int64_t key) {
	TOOL_ASSERT(map, "map 不为 NULL");
	if (map->cap == 0) return false;
	size_t mask = map->cap - 1;
	size_t idx = (size_t)toolHashMapHashKey(key) & mask;
	for (size_t probe = 0;probe < map->cap;probe++) {
		TOOL_HashSlot*s = &map->slots[idx];
		if (s->key == TOOL_HASH_MAP_HASH_EMPTY) return false;
		if (s->key == key) {
			uint32_t li = s->liveIdx;
			uint32_t lastLive = (uint32_t)(map->nLive - 1);
			if (li != lastLive) {
				uint32_t lastSlotIdx = map->liveIndices[lastLive];
				map->liveIndices[li] = lastSlotIdx;
				map->slots[lastSlotIdx].liveIdx = li;
			}
			map->nLive--;
			s->key = TOOL_HASH_MAP_HASH_TOMBSTONE;
			s->liveIdx = TOOL_HASH_MAP_NO_LIVE_IDX;
			return true;
		}
		idx = (idx + 1) & mask;
	}
	return false;
}

TOOLFUNC__ size_t toolHashMapSize(const TOOL_HashMap* map) {
	return map->nLive;
}

TOOLFUNC__ int64_t toolHashMapGetLiveKey(const TOOL_HashMap* map, size_t i) {
	TOOL_ASSERT(i < map->nLive, "索引越界");
	uint32_t slotIdx = map->liveIndices[i];
	return map->slots[slotIdx].key;
}

TOOLFUNC__ int64_t toolHashMapGetLiveValue(const TOOL_HashMap* map, size_t i) {
	TOOL_ASSERT(i < map->nLive, "索引越界");
	uint32_t slotIdx = map->liveIndices[i];
	return map->slots[slotIdx].value;
}

#endif // MMYMYS_TOOLS_HASH_MAP_H