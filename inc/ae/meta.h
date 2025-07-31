#ifndef AE_META_H
#define AE_META_H

#define ae_concat2(A, B) A##B
#define ae_concat(A, B)  ae_concat2(A, B)

/* Generate unique name for internal macro identifiers. */
#define ae_unique(NAME) ae_concat(NAME, __LINE__)

#ifdef __GNUC__
#define ae_static_assert(COND, MSG)                                           \
    __attribute__((unused)) typedef char ae_unique(                           \
        ae_static_assert)[(COND) ? 1 : -1]
#else
#define ae_static_assert(COND, MSG)
#endif

/* Assign pointer to pointer without type cast. */
#define ae_generic_assign(A, B)                                               \
    do {                                                                      \
        void *ae_unique(m) = B;                                               \
        memcpy(&A, &ae_unique(m), sizeof(void *));                            \
    } while (0)

#define ae_arr(T) T *

typedef ae_arr(unsigned char) ae_bytes_t;

#define ae_arr_len(A) ((size_t *)(A))[-2]
#define ae_arr_cap(A) ((size_t *)(A))[-1]

#define ae_arr_new(A)                                                         \
    ae_generic_assign((A), (size_t *)(calloc(2, sizeof(size_t))) + 2);

#define ae_arr_new_n(A, N)                                                    \
    do {                                                                      \
        ae_generic_assign(                                                    \
            (A),                                                              \
            (size_t *)malloc(2 * sizeof(size_t) + (N) * sizeof((A)[0])) + 2); \
        ae_arr_len(A) = ae_arr_cap(A) = (N);                                  \
    } while (0);

#define ae_arr_new_n_zero(A, N)                                               \
    do {                                                                      \
        ae_generic_assign(                                                    \
            (A),                                                              \
            (size_t *)calloc(1, 2 * sizeof(size_t) + (N) * sizeof((A)[0])) +  \
                2);                                                           \
        ae_arr_len(A) = ae_arr_cap(A) = (N);                                  \
    } while (0);

#define ae_arr_new_reserve(A, N)                                              \
    do {                                                                      \
        ae_generic_assign(                                                    \
            (A),                                                              \
            (size_t *)malloc(2 * sizeof(size_t) + (N) * sizeof((A)[0])) + 2); \
        ae_arr_len(A) = 0;                                                    \
        ae_arr_cap(A) = (N);                                                  \
    } while (0);

#define ae_arr_new_reserve_zero(A, N)                                         \
    do {                                                                      \
        ae_generic_assign(                                                    \
            (A),                                                              \
            (size_t *)calloc(1, 2 * sizeof(size_t) + (N) * sizeof((A)[0])) +  \
                2);                                                           \
        ae_arr_len(A) = 0;                                                    \
        ae_arr_cap(A) = (N);                                                  \
    } while (0);

#define AE_HEX_0   "\x00"
#define AE_HEX_1   "\x01"
#define AE_HEX_2   "\x02"
#define AE_HEX_3   "\x03"
#define AE_HEX_4   "\x04"
#define AE_HEX_5   "\x05"
#define AE_HEX_6   "\x06"
#define AE_HEX_7   "\x07"
#define AE_HEX_8   "\x08"
#define AE_HEX_9   "\x09"
#define AE_HEX_10  "\x0a"
#define AE_HEX_11  "\x0b"
#define AE_HEX_12  "\x0c"
#define AE_HEX_13  "\x0d"
#define AE_HEX_14  "\x0e"
#define AE_HEX_15  "\x0f"
#define AE_HEX_16  "\x10"
#define AE_HEX_17  "\x11"
#define AE_HEX_18  "\x12"
#define AE_HEX_19  "\x13"
#define AE_HEX_20  "\x14"
#define AE_HEX_21  "\x15"
#define AE_HEX_22  "\x16"
#define AE_HEX_23  "\x17"
#define AE_HEX_24  "\x18"
#define AE_HEX_25  "\x19"
#define AE_HEX_26  "\x1a"
#define AE_HEX_27  "\x1b"
#define AE_HEX_28  "\x1c"
#define AE_HEX_29  "\x1d"
#define AE_HEX_30  "\x1e"
#define AE_HEX_31  "\x1f"
#define AE_HEX_32  "\x20"
#define AE_HEX_33  "\x21"
#define AE_HEX_34  "\x22"
#define AE_HEX_35  "\x23"
#define AE_HEX_36  "\x24"
#define AE_HEX_37  "\x25"
#define AE_HEX_38  "\x26"
#define AE_HEX_39  "\x27"
#define AE_HEX_40  "\x28"
#define AE_HEX_41  "\x29"
#define AE_HEX_42  "\x2a"
#define AE_HEX_43  "\x2b"
#define AE_HEX_44  "\x2c"
#define AE_HEX_45  "\x2d"
#define AE_HEX_46  "\x2e"
#define AE_HEX_47  "\x2f"
#define AE_HEX_48  "\x30"
#define AE_HEX_49  "\x31"
#define AE_HEX_50  "\x32"
#define AE_HEX_51  "\x33"
#define AE_HEX_52  "\x34"
#define AE_HEX_53  "\x35"
#define AE_HEX_54  "\x36"
#define AE_HEX_55  "\x37"
#define AE_HEX_56  "\x38"
#define AE_HEX_57  "\x39"
#define AE_HEX_58  "\x3a"
#define AE_HEX_59  "\x3b"
#define AE_HEX_60  "\x3c"
#define AE_HEX_61  "\x3d"
#define AE_HEX_62  "\x3e"
#define AE_HEX_63  "\x3f"
#define AE_HEX_64  "\x40"
#define AE_HEX_65  "\x41"
#define AE_HEX_66  "\x42"
#define AE_HEX_67  "\x43"
#define AE_HEX_68  "\x44"
#define AE_HEX_69  "\x45"
#define AE_HEX_70  "\x46"
#define AE_HEX_71  "\x47"
#define AE_HEX_72  "\x48"
#define AE_HEX_73  "\x49"
#define AE_HEX_74  "\x4a"
#define AE_HEX_75  "\x4b"
#define AE_HEX_76  "\x4c"
#define AE_HEX_77  "\x4d"
#define AE_HEX_78  "\x4e"
#define AE_HEX_79  "\x4f"
#define AE_HEX_80  "\x50"
#define AE_HEX_81  "\x51"
#define AE_HEX_82  "\x52"
#define AE_HEX_83  "\x53"
#define AE_HEX_84  "\x54"
#define AE_HEX_85  "\x55"
#define AE_HEX_86  "\x56"
#define AE_HEX_87  "\x57"
#define AE_HEX_88  "\x58"
#define AE_HEX_89  "\x59"
#define AE_HEX_90  "\x5a"
#define AE_HEX_91  "\x5b"
#define AE_HEX_92  "\x5c"
#define AE_HEX_93  "\x5d"
#define AE_HEX_94  "\x5e"
#define AE_HEX_95  "\x5f"
#define AE_HEX_96  "\x60"
#define AE_HEX_97  "\x61"
#define AE_HEX_98  "\x62"
#define AE_HEX_99  "\x63"
#define AE_HEX_100 "\x64"
#define AE_HEX_101 "\x65"
#define AE_HEX_102 "\x66"
#define AE_HEX_103 "\x67"
#define AE_HEX_104 "\x68"
#define AE_HEX_105 "\x69"
#define AE_HEX_106 "\x6a"
#define AE_HEX_107 "\x6b"
#define AE_HEX_108 "\x6c"
#define AE_HEX_109 "\x6d"
#define AE_HEX_110 "\x6e"
#define AE_HEX_111 "\x6f"
#define AE_HEX_112 "\x70"
#define AE_HEX_113 "\x71"
#define AE_HEX_114 "\x72"
#define AE_HEX_115 "\x73"
#define AE_HEX_116 "\x74"
#define AE_HEX_117 "\x75"
#define AE_HEX_118 "\x76"
#define AE_HEX_119 "\x77"
#define AE_HEX_120 "\x78"
#define AE_HEX_121 "\x79"
#define AE_HEX_122 "\x7a"
#define AE_HEX_123 "\x7b"
#define AE_HEX_124 "\x7c"
#define AE_HEX_125 "\x7d"
#define AE_HEX_126 "\x7e"
#define AE_HEX_127 "\x7f"
#define AE_HEX_128 "\x80"
#define AE_HEX_129 "\x81"
#define AE_HEX_130 "\x82"
#define AE_HEX_131 "\x83"
#define AE_HEX_132 "\x84"
#define AE_HEX_133 "\x85"
#define AE_HEX_134 "\x86"
#define AE_HEX_135 "\x87"
#define AE_HEX_136 "\x88"
#define AE_HEX_137 "\x89"
#define AE_HEX_138 "\x8a"
#define AE_HEX_139 "\x8b"
#define AE_HEX_140 "\x8c"
#define AE_HEX_141 "\x8d"
#define AE_HEX_142 "\x8e"
#define AE_HEX_143 "\x8f"
#define AE_HEX_144 "\x90"
#define AE_HEX_145 "\x91"
#define AE_HEX_146 "\x92"
#define AE_HEX_147 "\x93"
#define AE_HEX_148 "\x94"
#define AE_HEX_149 "\x95"
#define AE_HEX_150 "\x96"
#define AE_HEX_151 "\x97"
#define AE_HEX_152 "\x98"
#define AE_HEX_153 "\x99"
#define AE_HEX_154 "\x9a"
#define AE_HEX_155 "\x9b"
#define AE_HEX_156 "\x9c"
#define AE_HEX_157 "\x9d"
#define AE_HEX_158 "\x9e"
#define AE_HEX_159 "\x9f"
#define AE_HEX_160 "\xa0"
#define AE_HEX_161 "\xa1"
#define AE_HEX_162 "\xa2"
#define AE_HEX_163 "\xa3"
#define AE_HEX_164 "\xa4"
#define AE_HEX_165 "\xa5"
#define AE_HEX_166 "\xa6"
#define AE_HEX_167 "\xa7"
#define AE_HEX_168 "\xa8"
#define AE_HEX_169 "\xa9"
#define AE_HEX_170 "\xaa"
#define AE_HEX_171 "\xab"
#define AE_HEX_172 "\xac"
#define AE_HEX_173 "\xad"
#define AE_HEX_174 "\xae"
#define AE_HEX_175 "\xaf"
#define AE_HEX_176 "\xb0"
#define AE_HEX_177 "\xb1"
#define AE_HEX_178 "\xb2"
#define AE_HEX_179 "\xb3"
#define AE_HEX_180 "\xb4"
#define AE_HEX_181 "\xb5"
#define AE_HEX_182 "\xb6"
#define AE_HEX_183 "\xb7"
#define AE_HEX_184 "\xb8"
#define AE_HEX_185 "\xb9"
#define AE_HEX_186 "\xba"
#define AE_HEX_187 "\xbb"
#define AE_HEX_188 "\xbc"
#define AE_HEX_189 "\xbd"
#define AE_HEX_190 "\xbe"
#define AE_HEX_191 "\xbf"
#define AE_HEX_192 "\xc0"
#define AE_HEX_193 "\xc1"
#define AE_HEX_194 "\xc2"
#define AE_HEX_195 "\xc3"
#define AE_HEX_196 "\xc4"
#define AE_HEX_197 "\xc5"
#define AE_HEX_198 "\xc6"
#define AE_HEX_199 "\xc7"
#define AE_HEX_200 "\xc8"
#define AE_HEX_201 "\xc9"
#define AE_HEX_202 "\xca"
#define AE_HEX_203 "\xcb"
#define AE_HEX_204 "\xcc"
#define AE_HEX_205 "\xcd"
#define AE_HEX_206 "\xce"
#define AE_HEX_207 "\xcf"
#define AE_HEX_208 "\xd0"
#define AE_HEX_209 "\xd1"
#define AE_HEX_210 "\xd2"
#define AE_HEX_211 "\xd3"
#define AE_HEX_212 "\xd4"
#define AE_HEX_213 "\xd5"
#define AE_HEX_214 "\xd6"
#define AE_HEX_215 "\xd7"
#define AE_HEX_216 "\xd8"
#define AE_HEX_217 "\xd9"
#define AE_HEX_218 "\xda"
#define AE_HEX_219 "\xdb"
#define AE_HEX_220 "\xdc"
#define AE_HEX_221 "\xdd"
#define AE_HEX_222 "\xde"
#define AE_HEX_223 "\xdf"
#define AE_HEX_224 "\xe0"
#define AE_HEX_225 "\xe1"
#define AE_HEX_226 "\xe2"
#define AE_HEX_227 "\xe3"
#define AE_HEX_228 "\xe4"
#define AE_HEX_229 "\xe5"
#define AE_HEX_230 "\xe6"
#define AE_HEX_231 "\xe7"
#define AE_HEX_232 "\xe8"
#define AE_HEX_233 "\xe9"
#define AE_HEX_234 "\xea"
#define AE_HEX_235 "\xeb"
#define AE_HEX_236 "\xec"
#define AE_HEX_237 "\xed"
#define AE_HEX_238 "\xee"
#define AE_HEX_239 "\xef"
#define AE_HEX_240 "\xf0"
#define AE_HEX_241 "\xf1"
#define AE_HEX_242 "\xf2"
#define AE_HEX_243 "\xf3"
#define AE_HEX_244 "\xf4"
#define AE_HEX_245 "\xf5"
#define AE_HEX_246 "\xf6"
#define AE_HEX_247 "\xf7"
#define AE_HEX_248 "\xf8"
#define AE_HEX_249 "\xf9"
#define AE_HEX_250 "\xfa"
#define AE_HEX_251 "\xfb"
#define AE_HEX_252 "\xfc"
#define AE_HEX_253 "\xfd"
#define AE_HEX_254 "\xfe"
#define AE_HEX_255 "\xff"

#if !defined(__SIZEOF_SIZE_T__) || __SIZEOF_SIZE_T__ == 8
#define AE_ZEROES "\x00\x00\x00\x00\x00\x00"
#elif __SIZEOF_SIZE_T__ == 4
#define AE_ZEROES "\x00\x00"
#else
#define AE_ZEROES
#endif

#if !defined(__BYTE_ORDER__) || __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define ae_literal(LO, HI)                                                    \
    ae_concat(AE_HEX_, LO) ae_concat(AE_HEX_, HI) AE_ZEROES
#else
#define ae_literal(LO, HI)                                                    \
    ae_concat(AE_HEX_, HI) ae_concat(AE_HEX_, LO) AE_ZEROES
#endif

/* Embed string literal and metadata in array without using heap memory. */
#define ae_arr_from_string_literal(A, L, LO, HI)                              \
    do {                                                                      \
        ae_static_assert(sizeof(L) - 1 == (((HI) << 8) | (LO)),               \
                         "Invalid literal length passed.");                   \
        ae_generic_assign((A), (ae_literal(LO, HI) ae_literal(LO, HI) L) +    \
                                   2 * sizeof(size_t));                       \
    } while (0)

#define ae_arr_free(A) free(((size_t *)A) - 2);

#define ae_arr_insert_n(A, I, N, B)                                           \
    do {                                                                      \
        assert((I) <= ae_arr_len(A));                                         \
                                                                              \
        while (ae_arr_len(A) + (N) > ae_arr_cap(A)) {                         \
            ae_arr_cap(A) = ae_arr_cap(A) == 0 ? (N) : ae_arr_cap(A) * 2;     \
        }                                                                     \
                                                                              \
        ae_generic_assign(                                                    \
            (A), (size_t *)(realloc((size_t *)(A) - 2,                        \
                                    2 * sizeof(size_t) +                      \
                                        ae_arr_cap(A) * sizeof((A)[0]))) +    \
                     2);                                                      \
                                                                              \
        if ((I) != ae_arr_len(A)) {                                           \
            memmove((A) + (I) + (N), (A) + (I),                               \
                    (ae_arr_len(A) - (I)) * sizeof((A)[0]));                  \
        }                                                                     \
                                                                              \
        memcpy((A) + (I), (B), (N) * sizeof((A)[0]));                         \
                                                                              \
        ae_arr_len(A) += (N);                                                 \
    } while (0)

#define ae_arr_insert(A, I, E) ae_arr_insert_n(A, I, 1, &(E))

#define ae_arr_append_n(A, N, B) ae_arr_insert_n(A, ae_arr_len(A), N, B)

#define ae_arr_append(A, E) ae_arr_append_n(A, 1, &(E))

#define ae_arr_remove_n(A, I, N)                                              \
    do {                                                                      \
        assert((I) < ae_arr_len(A));                                          \
        assert((I) + (N) <= ae_arr_len(A));                                   \
                                                                              \
        if ((I) + (N) < ae_arr_len(A)) {                                      \
            memmove((A) + (I), (A) + (I) + (N),                               \
                    (ae_arr_len(A) - (I) - (N)) * sizeof((A)[0]));            \
        }                                                                     \
                                                                              \
        ae_arr_len(A) -= (N);                                                 \
    } while (0)

#define ae_arr_remove(A, I) ae_arr_remove_n(A, I, 1)

#define ae_arr_remove_ptr(A, P) ae_arr_remove(A, (size_t)((P) - (A)))

#define AE_MAP_CAP 16

typedef enum ae_map_entry_state_e {
    AE_MAP_ENTRY_STATE_EMPTY,
    AE_MAP_ENTRY_STATE_USED,
    AE_MAP_ENTRY_STATE_REMOVED
} ae_map_entry_state_t;

#define ae_map(K, V)                                                          \
    struct {                                                                  \
        K *keys;                                                              \
        K *old_keys;                                                          \
        V *vals;                                                              \
        V *old_vals;                                                          \
        ae_map_entry_state_t *states;                                         \
                                                                              \
        size_t len;                                                           \
        size_t cap;                                                           \
                                                                              \
        uint64_t (*hash)(K);                                                  \
        bool (*eq)(K, K);                                                     \
    }

#define ae_map_foreach(M, I)                                                  \
    for (size_t I = 0; I < (M).cap; I++)                                      \
        if ((M).states[I] == AE_MAP_ENTRY_STATE_USED)

#define ae_map_new_reserve(M, N, HASH, EQ)                                    \
    do {                                                                      \
        ae_generic_assign((M).keys, calloc((N), sizeof((M).keys[0])));        \
        ae_generic_assign((M).vals, calloc((N), sizeof((M).vals[0])));        \
        ae_generic_assign((M).states,                                         \
                          calloc((N), sizeof(ae_map_entry_state_t)));         \
                                                                              \
        (M).len = 0;                                                          \
        (M).cap = (N);                                                        \
                                                                              \
        (M).hash = (HASH);                                                    \
        (M).eq   = (EQ);                                                      \
    } while (0)

#define ae_map_new(M, HASH, EQ) ae_map_new_reserve(M, AE_MAP_CAP, HASH, EQ)

#define ae_map_free(M)                                                        \
    do {                                                                      \
        free((M).keys);                                                       \
        free((M).vals);                                                       \
        free((M).states);                                                     \
    } while (0)

#define _ae_map_get(M, K, V)                                                  \
    do {                                                                      \
        (V)                 = NULL;                                           \
        size_t ae_unique(i) = (M).hash(K) % (M).cap;                          \
                                                                              \
        while ((M).states[ae_unique(i)] != AE_MAP_ENTRY_STATE_EMPTY) {        \
            if ((M).states[ae_unique(i)] == AE_MAP_ENTRY_STATE_USED &&        \
                (M).eq(K, (M).keys[ae_unique(i)])) {                          \
                (V) = &(M).vals[ae_unique(i)];                                \
                break;                                                        \
            }                                                                 \
            if (++ae_unique(i) == (M).cap) {                                  \
                ae_unique(i) = 0;                                             \
            }                                                                 \
        }                                                                     \
    } while (0)

#define ae_map_get(M, K, V)                                                   \
    do {                                                                      \
        _ae_map_get(M, K, V);                                                 \
        assert((V) && "Entry not found in map.");                             \
    } while (0)

#define ae_map_contains(M, K, BOOL)                                           \
    do {                                                                      \
        void *ae_unique(value);                                               \
        _ae_map_get(M, K, ae_unique(value));                                  \
        (BOOL) = ae_unique(value) != NULL;                                    \
    } while (0)

#define _ae_map_set(M, K, V)                                                  \
    do {                                                                      \
        size_t ae_unique(i)        = (M).hash(K) % (M).cap;                   \
        ssize_t ae_unique(removed) = -1;                                      \
        bool ae_unique(set)        = false;                                   \
                                                                              \
        while ((M).states[ae_unique(i)] != AE_MAP_ENTRY_STATE_EMPTY) {        \
            if ((M).states[ae_unique(i)] == AE_MAP_ENTRY_STATE_USED &&        \
                (M).eq(K, (M).keys[ae_unique(i)])) {                          \
                (M).vals[ae_unique(i)] = V;                                   \
                ae_unique(set)         = true;                                \
                break;                                                        \
            }                                                                 \
            if ((M).states[ae_unique(i)] == AE_MAP_ENTRY_STATE_REMOVED &&     \
                ae_unique(removed) == -1) {                                   \
                ae_unique(removed) = (ssize_t)ae_unique(i);                   \
            }                                                                 \
            if (++ae_unique(i) == (M).cap) {                                  \
                ae_unique(i) = 0;                                             \
            }                                                                 \
        }                                                                     \
        if (!ae_unique(set)) {                                                \
            size_t ae_unique(j)      = (ae_unique(removed) != -1)             \
                                           ? (size_t)ae_unique(removed)       \
                                           : ae_unique(i);                    \
            (M).keys[ae_unique(j)]   = K;                                     \
            (M).vals[ae_unique(j)]   = V;                                     \
            (M).states[ae_unique(j)] = AE_MAP_ENTRY_STATE_USED;               \
            (M).len++;                                                        \
        }                                                                     \
    } while (0)

#define ae_map_set(M, K, V)                                                   \
    do {                                                                      \
        if ((M).len == (M).cap / 2) {                                         \
            (M).old_keys                                = (M).keys;           \
            (M).old_vals                                = (M).vals;           \
            ae_map_entry_state_t *ae_unique(old_states) = (M).states;         \
            size_t ae_unique(old_cap)                   = (M).cap;            \
                                                                              \
            (M).len = 0;                                                      \
            (M).cap *= 2;                                                     \
                                                                              \
            ae_generic_assign((M).keys,                                       \
                              calloc((M).cap, sizeof((M).keys[0])));          \
            ae_generic_assign((M).vals,                                       \
                              calloc((M).cap, sizeof((M).vals[0])));          \
            ae_generic_assign((M).states,                                     \
                              calloc((M).cap, sizeof(ae_map_entry_state_t))); \
                                                                              \
            for (size_t ae_unique(k) = 0; ae_unique(k) < ae_unique(old_cap);  \
                 ae_unique(k)++) {                                            \
                if (ae_unique(old_states)[ae_unique(k)] ==                    \
                    AE_MAP_ENTRY_STATE_USED) {                                \
                    _ae_map_set(M, (M).old_keys[ae_unique(k)],                \
                                (M).old_vals[ae_unique(k)]);                  \
                }                                                             \
            }                                                                 \
                                                                              \
            free((M).old_keys);                                               \
            free((M).old_vals);                                               \
            free(ae_unique(old_states));                                      \
        }                                                                     \
                                                                              \
        _ae_map_set(M, K, V);                                                 \
    } while (0)

#define ae_map_remove(M, K)                                                   \
    do {                                                                      \
        size_t ae_unique(i) = (M).hash(K) % (M).cap;                          \
                                                                              \
        while ((M).states[ae_unique(i)] != AE_MAP_ENTRY_STATE_EMPTY) {        \
            if ((M).states[ae_unique(i)] == AE_MAP_ENTRY_STATE_USED &&        \
                (M).eq(K, (M).keys[ae_unique(i)])) {                          \
                (M).states[ae_unique(i)] = AE_MAP_ENTRY_STATE_REMOVED;        \
                (M).len--;                                                    \
                break;                                                        \
            }                                                                 \
            if (++ae_unique(i) == (M).cap) {                                  \
                ae_unique(i) = 0;                                             \
            }                                                                 \
        }                                                                     \
    } while (0)

#endif
