/* (hookshot) How much VRAM the game's memory pools may be mirrored in (vk_memory.cpp mirror_budget), by the card: Auto's rule, and
   the cap on what a Quality preset asks. No Vulkan here. tests/test_mirror_budget.py */
#pragma once

#include <algorithm>
#include <cstdint>

namespace mkwvk::mirror_rule {

/// Auto's mirrors: a quarter of the card, at most 3 GB, and an eighth on a card of 4.5 GB or less (512 MB on 4 GB). A pool left out
/// is read by the GPU over PCIe: on 8 GB cards, a 3070 or a 4060, that left the GPU at a few percent and the frame stuttering, and
/// on a 6 GB RTX 3060 Laptop at an eighth (768 MB) or the Low preset's 512 MB, 30-32 GB of bound ranges went over PCIe every 600
/// submits in Hyrule Field, the GPU's frame 27 ms against 19 at a quarter. 6 GB cards had an eighth because, with a quarter, the
/// mirrors and the textures went past what the card has; the image blocks made at the start were then 896 MB of it held empty,
/// and since #2711 they are made in the textures' pool, where the textures take them
inline uint64_t auto_bytes(uint64_t vram) {
    if (!vram) return 3072ull << 20;
    if (vram <= (4ull << 30) + (512ull << 20)) return vram / 8;
    return std::min<uint64_t>(vram / 4, 3072ull << 20);
}

/// The mirrors when a Quality preset set the budgets (by_preset): never more than Auto gives the card, so a preset made for a
/// larger card (Low's 1536 MB on a 4 GB one, preset_caps_memory lets Low's budgets through up to 6.75 GB) keeps a small card at
/// Auto's. A budget chosen on the VRAM budget row itself is the player's on any card, as chosen
inline uint64_t preset_bytes(uint64_t asked, bool by_preset, uint64_t vram) {
    return by_preset && vram ? std::min(asked, auto_bytes(vram)) : asked;
}

}  // namespace mkwvk::mirror_rule
