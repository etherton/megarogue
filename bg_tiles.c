#include "md_api.h"
const uint32_t bg_tiles_24_24[] = {
0x11111111,0x12333333,0x12233333,0x12223333,0x12222333,0x12222233,0x12222233,0x12222233,
0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,
0x12222233,0x12222233,0x12222444,0x12224444,0x12244444,0x12444444,0x14444444,0x11111111,
0x11111111,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,
0x33333333,0x33333333,0x44444444,0x44444444,0x44444444,0x44444444,0x44444444,0x11111111,
0x11111111,0x33333351,0x33333521,0x33335221,0x33352221,0x33522221,0x33222221,0x33222221,
0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,
0x33222221,0x33222221,0x44422221,0x44442221,0x44444221,0x44444421,0x44444441,0x11111111,
};
const uint16_t bg_palette_24_24[16] = {0x111,0x222,0x666,0xaaa,0x444,0xccc,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,};

const uint32_t bg_tiles_24_48[] = {
0x11111111,0x12333333,0x12233333,0x12223333,0x12222333,0x12222233,0x12222233,0x12222233,
0x11222233,0x12442233,0x12224423,0x12222232,0x12222233,0x12222233,0x12222233,0x12222233,
0x12222233,0x12222233,0x12222444,0x12224444,0x12244444,0x12444444,0x14444444,0x11111111,
0x11111111,0x33333332,0x33333323,0x33333323,0x33333323,0x33333323,0x33333323,0x33333323,
0x33332252,0x33323333,0x33323333,0x23233333,0x52333333,0x33233333,0x33323333,0x33323333,
0x33332333,0x33332333,0x44441444,0x44441444,0x44444144,0x44444144,0x44444144,0x11111111,
0x11111111,0x33333351,0x33333521,0x33335221,0x33352221,0x33522221,0x33222221,0x33222221,
0x33222221,0x22222221,0x33122221,0x33211111,0x33222111,0x33222211,0x33222221,0x33222221,
0x33222221,0x33222221,0x44422221,0x44442221,0x44444221,0x44444421,0x44444441,0x11111111,
};
const uint16_t bg_palette_24_48[16] = {0x111,0x222,0x666,0xaaa,0x444,0xccc,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,};

const uint32_t bg_tiles_24_72[] = {
0x11111111,0x12333332,0x12233334,0x12223335,0x12222333,0x12222222,0x12222233,0x12222233,
0x11222233,0x12442223,0x12224443,0x12222234,0x12222233,0x12222233,0x12222234,0x14224443,
0x12442233,0x14222233,0x12222444,0x12224444,0x12244444,0x12444444,0x14444444,0x11111111,
0x11111111,0x33333332,0x33333323,0x43333343,0x43333342,0x24333343,0x34333343,0x33433343,
0x33434454,0x33543333,0x33343333,0x43433333,0x54333333,0x43433333,0x33343344,0x33343433,
0x33334333,0x33334333,0x44441444,0x44441444,0x44444144,0x44444144,0x44444144,0x11111111,
0x11111111,0x33333351,0x33333521,0x33335221,0x23352221,0x32522221,0x35222221,0x33222221,
0x33222221,0x44222221,0x33122221,0x33211111,0x33242111,0x44422211,0x33222221,0x33222221,
0x33222221,0x33222221,0x44422221,0x44442221,0x44444221,0x44444421,0x44444441,0x11111111,
};
const uint16_t bg_palette_24_72[16] = {0x111,0x222,0x666,0xaaa,0x444,0xccc,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,};

const uint32_t bg_tiles_24_96[] = {
0x11111111,0x11222222,0x12222222,0x12222222,0x12222222,0x12222222,0x12222222,0x12222222,
0x12222222,0x12222222,0x12222222,0x12222222,0x12222222,0x12222222,0x12222222,0x12222222,
0x12222222,0x12222222,0x12222222,0x12222222,0x12222222,0x12222222,0x11222222,0x11111111,
0x11111111,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,
0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,
0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x11111111,
0x11111111,0x22222211,0x22222221,0x22222221,0x22222221,0x22222221,0x22222221,0x22222221,
0x22222221,0x22222221,0x22222221,0x22222221,0x22222221,0x22222221,0x22222221,0x22222221,
0x22222221,0x22222221,0x22222221,0x22222221,0x22222221,0x22222221,0x22222211,0x11111111,
};
const uint16_t bg_palette_24_96[16] = {0x111,0x222,0x444,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,};

const uint32_t bg_tiles_24_120[] = {
0x11111111,0x11222222,0x12222222,0x12222222,0x12222222,0x12222222,0x12222222,0x12222222,
0x12222222,0x12222222,0x12222222,0x12222222,0x12222222,0x12222222,0x12222222,0x12222222,
0x12222222,0x12222222,0x12222222,0x12222222,0x12222222,0x12222222,0x11222222,0x11111111,
0x11111111,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,
0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,
0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x11111111,
0x11111111,0x22222211,0x22222221,0x22222221,0x22222221,0x22222221,0x22222221,0x22222221,
0x22222221,0x22222221,0x22222221,0x22222221,0x22222221,0x22222221,0x22222221,0x22222221,
0x22222221,0x22222221,0x22222221,0x22222221,0x22222221,0x22222221,0x22222211,0x11111111,
};
const uint16_t bg_palette_24_120[16] = {0x111,0x222,0x444,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,};

const uint32_t bg_tiles_24_144[] = {
0x11111111,0x11222222,0x12222222,0x12222222,0x12222222,0x12222222,0x12222222,0x12222222,
0x12222222,0x12222222,0x12222222,0x12222222,0x12222222,0x11222111,0x12111222,0x11222222,
0x12222222,0x12222222,0x12222222,0x12222222,0x12222222,0x12222222,0x11222222,0x11111111,
0x11111111,0x22222222,0x22222222,0x22222222,0x22222221,0x22222221,0x22222221,0x22222221,
0x22222221,0x22222212,0x22221111,0x22212222,0x22122222,0x11222222,0x21222222,0x22122222,
0x22122222,0x22122222,0x22212222,0x22112222,0x21212222,0x12221222,0x12221222,0x11111111,
0x11111111,0x12222211,0x12222221,0x12222221,0x22222221,0x22222221,0x22222221,0x22222221,
0x22222221,0x22222221,0x11112221,0x22221121,0x22222211,0x22222221,0x22222221,0x22222221,
0x22222221,0x22222221,0x22222221,0x22222221,0x22222221,0x22222221,0x22222211,0x11111111,
};
const uint16_t bg_palette_24_144[16] = {0x111,0x222,0x444,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,};

const uint32_t bg_tiles_24_168[] = {
0x11111111,0x12222112,0x12222112,0x12222112,0x12222112,0x11111111,0x11111111,0x12222112,
0x12222112,0x12222112,0x12222112,0x11111111,0x11111111,0x12222112,0x12222112,0x12222112,
0x12222112,0x11111111,0x11111111,0x12222112,0x12222112,0x12222112,0x12222112,0x11111111,
0x11111111,0x22211222,0x22211222,0x22211222,0x22211222,0x11111111,0x11111111,0x22211222,
0x22211222,0x22211222,0x22211222,0x11111111,0x11111111,0x22211222,0x22211222,0x22211222,
0x22211222,0x11111111,0x11111111,0x22211222,0x22211222,0x22211222,0x22211222,0x11111111,
0x11111111,0x21122221,0x21122221,0x21122221,0x21122221,0x11111111,0x11111111,0x21122221,
0x21122221,0x21122221,0x21122221,0x11111111,0x11111111,0x21122221,0x21122221,0x21122221,
0x21122221,0x11111111,0x11111111,0x21122221,0x21122221,0x21122221,0x21122221,0x11111111,
};
const uint16_t bg_palette_24_168[16] = {0x111,0x222,0x444,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,};

const uint32_t bg_tiles_24_192[] = {
0x00000000,0x00000000,0x00000000,0x00000000,0x00000111,0x00000123,0x11111133,0x12333433,
0x13333433,0x13333433,0x13333433,0x13333433,0x13333433,0x13333433,0x13333433,0x13333433,
0x13333433,0x13333433,0x13333433,0x13333433,0x13333444,0x13333444,0x14444444,0x11111111,
0x00000000,0x00000000,0x00011111,0x00012333,0x11113333,0x33343333,0x33343333,0x33343333,
0x33343333,0x33343333,0x33343333,0x33343333,0x33343333,0x33343333,0x33343333,0x33343333,
0x33343333,0x33343333,0x33344444,0x33344444,0x44444444,0x44444444,0x44444444,0x11111111,
0x01111111,0x01233331,0x11333331,0x34333331,0x34333331,0x34333331,0x34333331,0x34333331,
0x34333331,0x34333331,0x34333331,0x34333331,0x34333331,0x34333331,0x34333331,0x34333331,
0x34444441,0x34444441,0x44444441,0x44444441,0x44444441,0x44444441,0x44444441,0x11111111,
};
const uint16_t bg_palette_24_192[16] = {0x111,0x222,0xaaa,0x666,0x444,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,};

const uint32_t bg_tiles_24_216[] = {
0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,
0x12222311,0x12222211,0x12222242,0x12222242,0x12222242,0x12222242,0x12222242,0x12222242,
0x12222242,0x12222242,0x12222242,0x12222242,0x12222242,0x12222242,0x12222242,0x11111111,
0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,
0x11111111,0x11111111,0x22231111,0x22221111,0x22224222,0x22224222,0x22224222,0x22224222,
0x22224222,0x22224222,0x22224222,0x22224222,0x22224222,0x22224222,0x22224222,0x11111111,
0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,
0x11111111,0x11111111,0x11111111,0x11111111,0x23111111,0x22111111,0x22422231,0x22422221,
0x22422221,0x22422221,0x22422221,0x22422221,0x22422221,0x22422221,0x22422221,0x11111111,
};
const uint16_t bg_palette_24_216[16] = {0x111,0x222,0x666,0xaaa,0x444,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,};

const uint32_t bg_tiles_24_240[] = {
0x11111111,0x12333333,0x12233333,0x12223333,0x12222333,0x12222233,0x12222233,0x12222233,
0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222444,0x12224444,
0x12244444,0x12444444,0x14444444,0x11111111,0x14444444,0x14444444,0x14444444,0x11111111,
0x11111111,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x44444444,0x44444444,
0x44444444,0x44444444,0x44444444,0x11111111,0x44444444,0x44444444,0x44444444,0x11111111,
0x11111111,0x33333351,0x33333521,0x33335221,0x33352221,0x33522221,0x33222221,0x33222221,
0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x44422221,0x44442221,
0x44444221,0x44444421,0x44444441,0x11111111,0x44444441,0x44444441,0x44444441,0x11111111,
};
const uint16_t bg_palette_24_240[16] = {0x111,0x222,0x666,0xaaa,0x444,0xccc,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,};

const uint32_t bg_tiles_24_264[] = {
0x11111111,0x12333333,0x12233333,0x12223333,0x12222333,0x12222233,0x12222233,0x12222233,
0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222444,0x12224444,
0x12244444,0x12444444,0x14444444,0x11111111,0x14444444,0x14444444,0x14444444,0x11111111,
0x11111111,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x44444444,0x44444444,
0x44444444,0x44444444,0x44444444,0x11111111,0x44444444,0x44444444,0x44444444,0x11111111,
0x11111111,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x44444444,0x44444444,
0x44444444,0x44444444,0x44444444,0x11111111,0x44444444,0x44444444,0x44444444,0x11111111,
};
const uint16_t bg_palette_24_264[16] = {0x111,0x222,0x666,0xaaa,0x444,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,};

const uint32_t bg_tiles_24_288[] = {
0x11111111,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,
0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x11111111,0x33333333,0x33333333,0x33333333,0x11111111,
0x11111111,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,
0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x11111111,0x33333333,0x33333333,0x33333333,0x11111111,
0x11111111,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,
0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x11111111,0x33333333,0x33333333,0x33333333,0x11111111,
};
const uint16_t bg_palette_24_288[16] = {0x111,0x222,0xaaa,0x444,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,};

const uint32_t bg_tiles_24_312[] = {
0x11111111,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,
0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x11111111,0x33333333,0x33333333,0x33333333,0x11111111,
0x11111111,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,
0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x11111111,0x33333333,0x33333333,0x33333333,0x11111111,
0x11111111,0x22222241,0x22222451,0x22224551,0x22245551,0x22455551,0x22555551,0x22555551,
0x22555551,0x22555551,0x22555551,0x22555551,0x22555551,0x22555551,0x33355551,0x33335551,
0x33333551,0x33333351,0x33333331,0x11111111,0x33333331,0x33333331,0x33333331,0x11111111,
};
const uint16_t bg_palette_24_312[16] = {0x111,0x222,0xaaa,0x444,0xccc,0x666,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,};

const uint32_t bg_tiles_24_336[] = {
0x11111111,0x12333333,0x12233333,0x12223333,0x12222333,0x12222233,0x12222233,0x12222233,
0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,
0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,
0x11111111,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,
0x11111111,0x33333341,0x33333421,0x33334221,0x33342221,0x33422221,0x33222221,0x33222221,
0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,
0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,
};
const uint16_t bg_palette_24_336[16] = {0x111,0x222,0x666,0xaaa,0xccc,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,};

const uint32_t bg_tiles_24_360[] = {
0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,
0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,
0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,
0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,
0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,
0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,
};
const uint16_t bg_palette_24_360[16] = {0x111,0x222,0x666,0xaaa,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,};

const uint32_t bg_tiles_24_384[] = {
0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,
0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222444,0x12224444,
0x12244444,0x12444444,0x14444444,0x11111111,0x14444444,0x14444444,0x14444444,0x11111111,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x44444444,0x44444444,
0x44444444,0x44444444,0x44444444,0x11111111,0x44444444,0x44444444,0x44444444,0x11111111,
0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,
0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x44422221,0x44442221,
0x44444221,0x44444421,0x44444441,0x11111111,0x44444441,0x44444441,0x44444441,0x11111111,
};
const uint16_t bg_palette_24_384[16] = {0x111,0x222,0x666,0xaaa,0x444,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,};

const uint32_t bg_tiles_24_408[] = {
0x11111111,0x12333333,0x12233333,0x12223333,0x12222333,0x12222233,0x12222233,0x12222233,
0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,
0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,
0x11111111,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,
0x11111111,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33444444,0x33244444,
0x33224444,0x33222444,0x33222244,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,
};
const uint16_t bg_palette_24_408[16] = {0x111,0x222,0x666,0xaaa,0x444,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,};

const uint32_t bg_tiles_24_432[] = {
0x11111111,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,
0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x33333322,0x33333422,
0x33334422,0x33344422,0x33444422,0x14444422,0x14444422,0x14444422,0x14444422,0x14444422,
0x11111111,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,
0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,
0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,
0x11111111,0x22222251,0x22222541,0x22225441,0x22254441,0x22544441,0x22444441,0x22444441,
0x22444441,0x22444441,0x22444441,0x22444441,0x22444441,0x22444441,0x22444441,0x22444441,
0x22444441,0x22444441,0x22444441,0x22444441,0x22444441,0x22444441,0x22444441,0x22444441,
};
const uint16_t bg_palette_24_432[16] = {0x111,0x222,0xaaa,0x444,0x666,0xccc,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,};

const uint32_t bg_tiles_24_456[] = {
0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,
0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222444,0x12224444,
0x12244444,0x12444444,0x14444444,0x11111111,0x14444444,0x14444444,0x14444444,0x11111111,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x44444444,0x44444444,
0x44444444,0x44444444,0x44444444,0x11111111,0x44444444,0x44444444,0x44444444,0x11111111,
0x33222221,0x33222223,0x33222233,0x33222333,0x33223333,0x33233333,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x44444444,0x44444444,
0x44444444,0x44444444,0x44444444,0x11111111,0x44444444,0x44444444,0x44444444,0x11111111,
};
const uint16_t bg_palette_24_456[16] = {0x111,0x222,0x666,0xaaa,0x444,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,};

const uint32_t bg_tiles_24_480[] = {
0x12222233,0x32222233,0x33222233,0x33322233,0x33332233,0x33333233,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x44444444,0x44444444,
0x44444444,0x44444444,0x44444444,0x11111111,0x44444444,0x44444444,0x44444444,0x11111111,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x44444444,0x44444444,
0x44444444,0x44444444,0x44444444,0x11111111,0x44444444,0x44444444,0x44444444,0x11111111,
0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,
0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x44422221,0x44442221,
0x44444221,0x44444421,0x44444441,0x11111111,0x44444441,0x44444441,0x44444441,0x11111111,
};
const uint16_t bg_palette_24_480[16] = {0x111,0x222,0x666,0xaaa,0x444,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,};

const uint32_t bg_tiles_24_504[] = {
0x12222233,0x32222233,0x33222233,0x33322233,0x33332233,0x33333233,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x44444433,0x44444233,
0x44442233,0x44422233,0x44222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,
0x33222221,0x33222223,0x33222233,0x33222333,0x33223333,0x33233333,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33444444,0x33244444,
0x33224444,0x33222444,0x33222244,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,
};
const uint16_t bg_palette_24_504[16] = {0x111,0x222,0x666,0xaaa,0x444,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,};

const uint32_t bg_tiles_24_528[] = {
0x11111111,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,
0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x33333322,0x33333422,
0x33334422,0x33344422,0x33444422,0x14444422,0x14444422,0x14444422,0x14444422,0x14444422,
0x11111111,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,
0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,
0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,
0x11111111,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,
0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22222222,0x22333333,0x22433333,
0x22443333,0x22444333,0x22444433,0x22444441,0x22444441,0x22444441,0x22444441,0x22444441,
};
const uint16_t bg_palette_24_528[16] = {0x111,0x222,0xaaa,0x444,0x666,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,};

const uint32_t bg_tiles_24_552[] = {
0x12222233,0x32222233,0x33222233,0x33322233,0x33332233,0x33333233,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x44444433,0x44444233,
0x44442233,0x44422233,0x44222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,
0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,
0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,
0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,
};
const uint16_t bg_palette_24_552[16] = {0x111,0x222,0x666,0xaaa,0x444,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,};

const uint32_t bg_tiles_24_576[] = {
0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,
0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,
0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,0x12222233,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,
0x33222221,0x33222223,0x33222233,0x33222333,0x33223333,0x33233333,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33444444,0x33244444,
0x33224444,0x33222444,0x33222244,0x33222221,0x33222221,0x33222221,0x33222221,0x33222221,
};
const uint16_t bg_palette_24_576[16] = {0x111,0x222,0x666,0xaaa,0x444,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,};

const uint32_t bg_tiles_24_600[] = {
0x12222233,0x32222233,0x33222233,0x33322233,0x33332233,0x33333233,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x44444444,0x44444444,
0x44444444,0x44444444,0x44444444,0x11111111,0x44444444,0x44444444,0x44444444,0x11111111,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x44444444,0x44444444,
0x44444444,0x44444444,0x44444444,0x11111111,0x44444444,0x44444444,0x44444444,0x11111111,
0x33222221,0x33222223,0x33222233,0x33222333,0x33223333,0x33233333,0x33333333,0x33333333,
0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x33333333,0x44444444,0x44444444,
0x44444444,0x44444444,0x44444444,0x11111111,0x44444444,0x44444444,0x44444444,0x11111111,
};
const uint16_t bg_palette_24_600[16] = {0x111,0x222,0x666,0xaaa,0x444,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,};

const uint32_t bg_tiles_24_624[] = {
0x12222233,0x14222233,0x12444233,0x12222444,0x12222223,0x12222233,0x12222323,0x12222233,
0x12222233,0x11442233,0x12224443,0x12222234,0x12222233,0x12222233,0x12222234,0x14224443,
0x12442223,0x14222233,0x12222233,0x12222232,0x12222323,0x12222233,0x12222233,0x12222233,
0x33333333,0x33333332,0x33333332,0x33333323,0x43333342,0x34333343,0x34333343,0x33433343,
0x33434454,0x33543335,0x33343333,0x43433333,0x54333333,0x43433333,0x33343344,0x33343433,
0x33334333,0x33334333,0x33332333,0x33332333,0x33333233,0x33333333,0x33333233,0x33333333,
0x33222221,0x33222221,0x33222221,0x33222221,0x23222221,0x32222221,0x33222241,0x33354421,
0x33222221,0x44222221,0x33122221,0x33211111,0x33242111,0x44422211,0x33522221,0x32322221,
0x33232221,0x33222221,0x33222221,0x33222221,0x33222221,0x33222411,0x33222221,0x33222221,
};
const uint16_t bg_palette_24_624[16] = {0x111,0x222,0x666,0xaaa,0x444,0xccc,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,};

const uint32_t bg_tiles_24_648[] = {
0x11111111,0x22222322,0x22222232,0x22222232,0x22222323,0x22222222,0x22222222,0x22222222,
0x22222222,0x22222222,0x23232222,0x22223344,0x22222252,0x22224222,0x44443444,0x44444444,
0x44444444,0x44444444,0x44444444,0x11111111,0x44444441,0x44444441,0x44444414,0x11111111,
0x11111111,0x22311322,0x22231222,0x22221222,0x22241223,0x35425124,0x22422542,0x22422242,
0x24222224,0x24222222,0x42222224,0x22222224,0x44222442,0x22424254,0x44131442,0x44414444,
0x41141444,0x14444144,0x14444144,0x11111111,0x44444414,0x44444414,0x14444411,0x11111111,
0x11111111,0x24222222,0x42222222,0x32222222,0x23222222,0x22222222,0x22222222,0x22222222,
0x22232332,0x44443222,0x22222322,0x22222222,0x22222222,0x12222222,0x41144444,0x44414444,
0x44414444,0x44441444,0x44441444,0x11111111,0x44444444,0x44444444,0x44444444,0x11111111,
};
const uint16_t bg_palette_24_648[16] = {0x111,0x222,0xaaa,0x666,0x444,0xccc,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,};

const struct directory { const uint32_t *tilePtr; const uint16_t *palPtr; } bg_directory[] = {
	{bg_tiles_24_24, bg_palette_24_24 },
	{bg_tiles_24_48, bg_palette_24_48 },
	{bg_tiles_24_72, bg_palette_24_72 },
	{bg_tiles_24_96, bg_palette_24_96 },
	{bg_tiles_24_120, bg_palette_24_120 },
	{bg_tiles_24_144, bg_palette_24_144 },
	{bg_tiles_24_168, bg_palette_24_168 },
	{bg_tiles_24_192, bg_palette_24_192 },
	{bg_tiles_24_216, bg_palette_24_216 },
	{bg_tiles_24_240, bg_palette_24_240 },
	{bg_tiles_24_264, bg_palette_24_264 },
	{bg_tiles_24_288, bg_palette_24_288 },
	{bg_tiles_24_312, bg_palette_24_312 },
	{bg_tiles_24_336, bg_palette_24_336 },
	{bg_tiles_24_360, bg_palette_24_360 },
	{bg_tiles_24_384, bg_palette_24_384 },
	{bg_tiles_24_408, bg_palette_24_408 },
	{bg_tiles_24_432, bg_palette_24_432 },
	{bg_tiles_24_456, bg_palette_24_456 },
	{bg_tiles_24_480, bg_palette_24_480 },
	{bg_tiles_24_504, bg_palette_24_504 },
	{bg_tiles_24_528, bg_palette_24_528 },
	{bg_tiles_24_552, bg_palette_24_552 },
	{bg_tiles_24_576, bg_palette_24_576 },
	{bg_tiles_24_600, bg_palette_24_600 },
	{bg_tiles_24_624, bg_palette_24_624 },
	{bg_tiles_24_648, bg_palette_24_648 },
};
const uint16_t bg_directory_count = sizeof(bg_directory) / sizeof(bg_directory[0]);
