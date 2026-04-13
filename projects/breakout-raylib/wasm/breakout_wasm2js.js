
function Table(ret) {
  // grow method not included; table is not growable
  ret.set = function(i, func) {
    this[i] = func;
  };
  ret.get = function(i) {
    return this[i];
  };
  return ret;
}

  var bufferView;
  var base64ReverseLookup = new Uint8Array(123/*'z'+1*/);
  for (var i = 25; i >= 0; --i) {
    base64ReverseLookup[48+i] = 52+i; // '0-9'
    base64ReverseLookup[65+i] = i; // 'A-Z'
    base64ReverseLookup[97+i] = 26+i; // 'a-z'
  }
  base64ReverseLookup[43] = 62; // '+'
  base64ReverseLookup[47] = 63; // '/'
  /** @noinline Inlining this function would mean expanding the base64 string 4x times in the source code, which Closure seems to be happy to do. */
  function base64DecodeToExistingUint8Array(uint8Array, offset, b64) {
    var b1, b2, i = 0, j = offset, bLength = b64.length, end = offset + (bLength*3>>2) - (b64[bLength-2] == '=') - (b64[bLength-1] == '=');
    for (; i < bLength; i += 4) {
      b1 = base64ReverseLookup[b64.charCodeAt(i+1)];
      b2 = base64ReverseLookup[b64.charCodeAt(i+2)];
      uint8Array[j++] = base64ReverseLookup[b64.charCodeAt(i)] << 2 | b1 >> 4;
      if (j < end) uint8Array[j++] = b1 << 4 | b2 >> 2;
      if (j < end) uint8Array[j++] = b2 << 6 | base64ReverseLookup[b64.charCodeAt(i+3)];
    }
    return uint8Array;
  }
function initActiveSegments(imports) {
  base64DecodeToExistingUint8Array(bufferView, 1024, "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");
}

  var scratchBuffer = new ArrayBuffer(16);
  var i32ScratchView = new Int32Array(scratchBuffer);
  var f32ScratchView = new Float32Array(scratchBuffer);
  var f64ScratchView = new Float64Array(scratchBuffer);
  
  function wasm2js_scratch_store_i32(index, value) {
    i32ScratchView[index] = value;
  }
      
  function wasm2js_scratch_load_f64() {
    return f64ScratchView[0];
  }
      
  function wasm2js_scratch_store_f32(value) {
    f32ScratchView[2] = value;
  }
      
  function wasm2js_scratch_load_i32(index) {
    return i32ScratchView[index];
  }
      
  function wasm2js_scratch_load_f32() {
    return f32ScratchView[2];
  }
      
function asmFunc(imports) {
 var buffer = new ArrayBuffer(16908288);
 var HEAP8 = new Int8Array(buffer);
 var HEAP16 = new Int16Array(buffer);
 var HEAP32 = new Int32Array(buffer);
 var HEAPU8 = new Uint8Array(buffer);
 var HEAPU16 = new Uint16Array(buffer);
 var HEAPU32 = new Uint32Array(buffer);
 var HEAPF32 = new Float32Array(buffer);
 var HEAPF64 = new Float64Array(buffer);
 var Math_imul = Math.imul;
 var Math_fround = Math.fround;
 var Math_abs = Math.abs;
 var Math_clz32 = Math.clz32;
 var Math_min = Math.min;
 var Math_max = Math.max;
 var Math_floor = Math.floor;
 var Math_ceil = Math.ceil;
 var Math_trunc = Math.trunc;
 var Math_sqrt = Math.sqrt;
 var global$0 = 70384;
 var i64toi32_i32$HIGH_BITS = 0;
 function $0() {
  
 }
 
 function $1($0_1) {
  $0_1 = $0_1 | 0;
  var $3_1 = 0, $2_1 = 0, i64toi32_i32$0 = 0, $4_1 = Math_fround(0), $1_1 = 0, wasm2js_i32$0 = 0, wasm2js_f32$0 = Math_fround(0);
  i64toi32_i32$0 = 1;
  HEAP32[($0_1 + 128 | 0) >> 2] = 0;
  HEAP32[($0_1 + 132 | 0) >> 2] = i64toi32_i32$0;
  i64toi32_i32$0 = 1135214592;
  HEAP32[$0_1 >> 2] = 0;
  HEAP32[($0_1 + 4 | 0) >> 2] = i64toi32_i32$0;
  HEAP8[($0_1 + 948 | 0) >> 0] = 0;
  HEAP32[($0_1 + 944 | 0) >> 2] = 0;
  i64toi32_i32$0 = 3;
  HEAP32[($0_1 + 120 | 0) >> 2] = 1;
  HEAP32[($0_1 + 124 | 0) >> 2] = i64toi32_i32$0;
  HEAP32[($0_1 + 16 | 0) >> 2] = 16;
  i64toi32_i32$0 = 120;
  HEAP32[($0_1 + 8 | 0) >> 2] = 1141473280;
  HEAP32[($0_1 + 12 | 0) >> 2] = i64toi32_i32$0;
  $1_1 = $0_1 + 136 | 0;
  $2_1 = 0;
  label : while (1) {
   $3_1 = $1_1 + Math_imul($2_1, 160) | 0;
   HEAP32[($3_1 + 156 | 0) >> 2] = $2_1;
   HEAP8[($3_1 + 152 | 0) >> 0] = 1;
   HEAP32[($3_1 + 148 | 0) >> 2] = 9;
   HEAP32[($3_1 + 144 | 0) >> 2] = $2_1;
   HEAP32[($3_1 + 140 | 0) >> 2] = $2_1;
   HEAP8[($3_1 + 136 | 0) >> 0] = 1;
   HEAP32[($3_1 + 132 | 0) >> 2] = 8;
   HEAP32[($3_1 + 128 | 0) >> 2] = $2_1;
   HEAP32[($3_1 + 124 | 0) >> 2] = $2_1;
   HEAP8[($3_1 + 120 | 0) >> 0] = 1;
   HEAP32[($3_1 + 116 | 0) >> 2] = 7;
   HEAP32[($3_1 + 112 | 0) >> 2] = $2_1;
   HEAP32[($3_1 + 108 | 0) >> 2] = $2_1;
   HEAP8[($3_1 + 104 | 0) >> 0] = 1;
   HEAP32[($3_1 + 100 | 0) >> 2] = 6;
   HEAP32[($3_1 + 96 | 0) >> 2] = $2_1;
   HEAP32[($3_1 + 92 | 0) >> 2] = $2_1;
   HEAP8[($3_1 + 88 | 0) >> 0] = 1;
   HEAP32[($3_1 + 84 | 0) >> 2] = 5;
   HEAP32[($3_1 + 80 | 0) >> 2] = $2_1;
   HEAP32[($3_1 + 76 | 0) >> 2] = $2_1;
   HEAP8[($3_1 + 72 | 0) >> 0] = 1;
   HEAP32[($3_1 + 68 | 0) >> 2] = 4;
   HEAP32[($3_1 + 64 | 0) >> 2] = $2_1;
   HEAP32[($3_1 + 60 | 0) >> 2] = $2_1;
   HEAP8[($3_1 + 56 | 0) >> 0] = 1;
   HEAP32[($3_1 + 52 | 0) >> 2] = 3;
   HEAP32[($3_1 + 48 | 0) >> 2] = $2_1;
   HEAP32[($3_1 + 44 | 0) >> 2] = $2_1;
   HEAP8[($3_1 + 40 | 0) >> 0] = 1;
   HEAP32[($3_1 + 36 | 0) >> 2] = 2;
   HEAP32[($3_1 + 32 | 0) >> 2] = $2_1;
   HEAP32[($3_1 + 28 | 0) >> 2] = $2_1;
   HEAP8[($3_1 + 24 | 0) >> 0] = 1;
   HEAP32[($3_1 + 20 | 0) >> 2] = 1;
   HEAP32[($3_1 + 16 | 0) >> 2] = $2_1;
   HEAP32[($3_1 + 12 | 0) >> 2] = $2_1;
   HEAP8[($3_1 + 8 | 0) >> 0] = 1;
   HEAP32[($3_1 + 4 | 0) >> 2] = 0;
   HEAP32[$3_1 >> 2] = $2_1;
   $2_1 = $2_1 + 1 | 0;
   if (($2_1 | 0) != (5 | 0)) {
    continue label
   }
   break label;
  };
  i64toi32_i32$0 = 1141309440;
  HEAP32[($0_1 + 20 | 0) >> 2] = 1137180672;
  HEAP32[($0_1 + 24 | 0) >> 2] = i64toi32_i32$0;
  i64toi32_i32$0 = 50;
  HEAP32[($0_1 + 936 | 0) >> 2] = 50;
  HEAP32[($0_1 + 940 | 0) >> 2] = i64toi32_i32$0;
  $3_1 = $46() | 0;
  HEAP8[($0_1 + 948 | 0) >> 0] = 1;
  HEAP8[($0_1 + 36 | 0) >> 0] = 1;
  HEAP8[($0_1 + 116 | 0) >> 0] = 0;
  HEAP8[($0_1 + 96 | 0) >> 0] = 0;
  HEAP8[($0_1 + 76 | 0) >> 0] = 0;
  HEAP8[($0_1 + 56 | 0) >> 0] = 0;
  $4_1 = Math_fround(+Math_fround((($3_1 | 0) % (120 | 0) | 0) + -60 | 0 | 0) * 3.141592653589793 / 180.0);
  (wasm2js_i32$0 = $0_1, wasm2js_f32$0 = Math_fround(Math_fround($40(Math_fround($4_1))) * Math_fround(6.0))), HEAPF32[(wasm2js_i32$0 + 28 | 0) >> 2] = wasm2js_f32$0;
  $4_1 = Math_fround(Math_fround($48(Math_fround($4_1))) * Math_fround(6.0));
  HEAPF32[($0_1 + 32 | 0) >> 2] = $4_1 > Math_fround(0.0) ? Math_fround(-$4_1) : $4_1;
 }
 
 function $2($0_1) {
  $0_1 = $0_1 | 0;
  HEAP8[($0_1 + 948 | 0) >> 0] = 0;
 }
 
 function $3($0_1, $1_1) {
  $0_1 = $0_1 | 0;
  $1_1 = Math_fround($1_1);
  var $2_1 = 0, $7_1 = 0, $3_1 = Math_fround(0), $8_1 = Math_fround(0), $9_1 = Math_fround(0), $10_1 = Math_fround(0), $12_1 = Math_fround(0), $11_1 = Math_fround(0), $18_1 = 0, $13_1 = Math_fround(0), $6_1 = 0, $4_1 = 0, $19_1 = Math_fround(0), $15_1 = Math_fround(0), $20_1 = Math_fround(0), $22_1 = 0, $5_1 = 0, $196 = Math_fround(0), $14_1 = Math_fround(0), $16_1 = Math_fround(0), $17_1 = Math_fround(0), $21_1 = 0, wasm2js_i32$0 = 0, wasm2js_f32$0 = Math_fround(0);
  block1 : {
   block : {
    $2_1 = HEAP32[$0_1 >> 2] | 0;
    if ($2_1 >>> 0 > 5 >>> 0) {
     break block
    }
    if ((1 << $2_1 | 0) & 45 | 0) {
     break block1
    }
    if (($2_1 | 0) != (4 | 0)) {
     break block
    }
    $1_1 = Math_fround(Math_fround(HEAPF32[($0_1 + 952 | 0) >> 2]) - $1_1);
    HEAPF32[($0_1 + 952 | 0) >> 2] = $1_1;
    if (!($1_1 <= Math_fround(0.0))) {
     break block1
    }
    HEAP8[($0_1 + 948 | 0) >> 0] = 1;
    HEAP32[($0_1 + 944 | 0) >> 2] = 1073741824;
    HEAP32[$0_1 >> 2] = 1;
    return;
   }
   block2 : {
    $3_1 = Math_fround(HEAPF32[($0_1 + 944 | 0) >> 2]);
    if (!($3_1 > Math_fround(0.0))) {
     break block2
    }
    $1_1 = Math_fround($3_1 - $1_1);
    HEAPF32[($0_1 + 944 | 0) >> 2] = $1_1;
    if (!($1_1 <= Math_fround(0.0))) {
     break block2
    }
    HEAP8[($0_1 + 948 | 0) >> 0] = 1;
   }
   block3 : {
    if ((HEAPU8[($0_1 + 948 | 0) >> 0] | 0 | 0) != (1 | 0)) {
     break block3
    }
    if ((HEAPU8[($0_1 + 36 | 0) >> 0] | 0 | 0) != (1 | 0)) {
     break block3
    }
    HEAP32[($0_1 + 24 | 0) >> 2] = 1141309440;
    HEAPF32[($0_1 + 20 | 0) >> 2] = Math_fround(Math_fround(HEAPF32[($0_1 + 4 | 0) >> 2]) + Math_fround(60.0));
   }
   block13 : {
    block4 : {
     $4_1 = HEAP32[($0_1 + 120 | 0) >> 2] | 0;
     if (($4_1 | 0) < (1 | 0)) {
      break block4
     }
     $5_1 = $0_1 + 20 | 0;
     $6_1 = 0;
     label2 : while (1) {
      block5 : {
       $7_1 = $5_1 + Math_imul($6_1, 20) | 0;
       if ((HEAPU8[($7_1 + 16 | 0) >> 0] | 0 | 0) != (1 | 0)) {
        break block5
       }
       $8_1 = Math_fround(HEAPF32[($7_1 + 8 | 0) >> 2]);
       $3_1 = Math_fround($8_1 + Math_fround(HEAPF32[$7_1 >> 2]));
       HEAPF32[$7_1 >> 2] = $3_1;
       $9_1 = Math_fround(HEAPF32[($7_1 + 12 | 0) >> 2]);
       $10_1 = Math_fround($9_1 + Math_fround(HEAPF32[($7_1 + 4 | 0) >> 2]));
       HEAPF32[($7_1 + 4 | 0) >> 2] = $10_1;
       block6 : {
        if (!(Math_fround($3_1 + Math_fround(-8.0)) < Math_fround(0.0))) {
         break block6
        }
        HEAP32[$7_1 >> 2] = 1090519040;
        $8_1 = Math_fround(Math_abs($8_1));
        HEAPF32[($7_1 + 8 | 0) >> 2] = $8_1;
        $3_1 = Math_fround(8.0);
       }
       block7 : {
        if (!(Math_fround($3_1 + Math_fround(8.0)) > Math_fround(800.0))) {
         break block7
        }
        HEAP32[$7_1 >> 2] = 1145438208;
        $8_1 = Math_fround(-Math_fround(Math_abs($8_1)));
        HEAPF32[($7_1 + 8 | 0) >> 2] = $8_1;
        $3_1 = Math_fround(792.0);
       }
       block8 : {
        if (!(Math_fround($10_1 + Math_fround(-8.0)) < Math_fround(0.0))) {
         break block8
        }
        HEAP32[($7_1 + 4 | 0) >> 2] = 1090519040;
        $9_1 = Math_fround(Math_abs($9_1));
        HEAPF32[($7_1 + 12 | 0) >> 2] = $9_1;
        $10_1 = Math_fround(8.0);
       }
       $1_1 = Math_fround(HEAPF32[($0_1 + 8 | 0) >> 2]);
       $11_1 = Math_fround($42(Math_fround($1_1), Math_fround(Math_fround($44(Math_fround($10_1), Math_fround(Math_fround($1_1 + Math_fround(HEAP32[($0_1 + 16 | 0) >> 2] | 0 | 0))))))));
       $12_1 = Math_fround(HEAPF32[($0_1 + 4 | 0) >> 2]);
       block9 : {
        $13_1 = Math_fround($3_1 - Math_fround($42(Math_fround($12_1), Math_fround(Math_fround($44(Math_fround($3_1), Math_fround(Math_fround($12_1 + Math_fround(HEAP32[($0_1 + 12 | 0) >> 2] | 0 | 0)))))))));
        $11_1 = Math_fround($10_1 - $11_1);
        if (!(Math_fround(Math_fround($13_1 * $13_1) + Math_fround($11_1 * $11_1)) < Math_fround(64.0))) {
         break block9
        }
        if (!($9_1 > Math_fround(0.0))) {
         break block9
        }
        $10_1 = Math_fround(Math_fround($1_1 + Math_fround(-8.0)) + Math_fround(-1.0));
        HEAPF32[($7_1 + 4 | 0) >> 2] = $10_1;
        $12_1 = Math_fround(+Math_fround(Math_fround(Math_fround(Math_fround($3_1 - $12_1) / Math_fround(120.0)) * Math_fround(120.0)) + Math_fround(150.0)) * 3.141592653589793 / 180.0);
        $1_1 = Math_fround(Math_sqrt(Math_fround(Math_fround($8_1 * $8_1) + Math_fround($9_1 * $9_1))));
        $8_1 = Math_fround(Math_fround($40(Math_fround($12_1))) * $1_1);
        HEAPF32[($7_1 + 8 | 0) >> 2] = $8_1;
        $196 = Math_fround(-$1_1);
        $1_1 = Math_fround(Math_fround($48(Math_fround($12_1))) * $1_1);
        $9_1 = $1_1 > Math_fround(-1.0) ? $196 : $1_1;
        HEAPF32[($7_1 + 12 | 0) >> 2] = $9_1;
       }
       $14_1 = Math_fround($10_1 + Math_fround(8.0));
       $15_1 = Math_fround($10_1 + Math_fround(-8.0));
       $16_1 = Math_fround($3_1 + Math_fround(8.0));
       $17_1 = Math_fround($3_1 + Math_fround(-8.0));
       $18_1 = 0;
       label1 : while (1) {
        $19_1 = Math_fround((Math_imul($18_1, 30) + 80 | 0) >>> 0);
        $20_1 = Math_fround($19_1 + Math_fround(24.0));
        $1_1 = Math_fround($10_1 - Math_fround($42(Math_fround($19_1), Math_fround(Math_fround($44(Math_fround($10_1), Math_fround($20_1)))))));
        $11_1 = Math_fround($1_1 * $1_1);
        $21_1 = $0_1 + Math_imul($18_1, 160) | 0;
        $2_1 = 0;
        block12 : {
         label : while (1) {
          block10 : {
           $22_1 = $21_1 + ($2_1 << 4 | 0) | 0;
           if ((HEAPU8[($22_1 + 144 | 0) >> 0] | 0 | 0) != (1 | 0)) {
            break block10
           }
           $1_1 = Math_fround((Math_imul($2_1, 76) + 30 | 0) >>> 0);
           $13_1 = Math_fround($1_1 + Math_fround(70.0));
           $12_1 = Math_fround($3_1 - Math_fround($42(Math_fround($1_1), Math_fround(Math_fround($44(Math_fround($3_1), Math_fround($13_1)))))));
           if (!(Math_fround(Math_fround($12_1 * $12_1) + $11_1) < Math_fround(64.0))) {
            break block10
           }
           HEAP8[($22_1 + 144 | 0) >> 0] = 0;
           HEAP32[($0_1 + 940 | 0) >> 2] = (HEAP32[($0_1 + 940 | 0) >> 2] | 0) + -1 | 0;
           HEAP32[($0_1 + 128 | 0) >> 2] = (HEAP32[($0_1 + 128 | 0) >> 2] | 0) + Math_imul(5 - $18_1 | 0, 10) | 0;
           block11 : {
            if (!(Math_fround($44(Math_fround(Math_fround($13_1 - $17_1)), Math_fround(Math_fround($16_1 - $1_1)))) < Math_fround($44(Math_fround(Math_fround($20_1 - $15_1)), Math_fround(Math_fround($14_1 - $19_1)))))) {
             break block11
            }
            $8_1 = Math_fround(-$8_1);
            HEAPF32[($7_1 + 8 | 0) >> 2] = $8_1;
            break block12;
           }
           $9_1 = Math_fround(-$9_1);
           HEAPF32[($7_1 + 12 | 0) >> 2] = $9_1;
           break block12;
          }
          $2_1 = $2_1 + 1 | 0;
          if (($2_1 | 0) != (10 | 0)) {
           continue label
          }
          break label;
         };
        }
        $18_1 = $18_1 + 1 | 0;
        if (($18_1 | 0) != (5 | 0)) {
         continue label1
        }
        break label1;
       };
       if (!($15_1 > Math_fround(600.0))) {
        break block5
       }
       HEAP8[($7_1 + 16 | 0) >> 0] = 0;
      }
      $6_1 = $6_1 + 1 | 0;
      if (($6_1 | 0) != ($4_1 | 0)) {
       continue label2
      }
      break label2;
     };
     $2_1 = 0;
     label3 : while (1) {
      if (HEAPU8[(($0_1 + Math_imul($2_1, 20) | 0) + 36 | 0) >> 0] | 0) {
       break block13
      }
      $2_1 = $2_1 + 1 | 0;
      if (($2_1 | 0) != ($4_1 | 0)) {
       continue label3
      }
      break label3;
     };
    }
    $2_1 = HEAP32[($0_1 + 124 | 0) >> 2] | 0;
    HEAP32[($0_1 + 124 | 0) >> 2] = $2_1 + -1 | 0;
    block14 : {
     if (($2_1 | 0) > (1 | 0)) {
      break block14
     }
     HEAP32[$0_1 >> 2] = 3;
     break block13;
    }
    HEAP8[($0_1 + 948 | 0) >> 0] = 0;
    HEAP32[($0_1 + 944 | 0) >> 2] = 1069547520;
    HEAP32[$0_1 >> 2] = 1;
    HEAP32[($0_1 + 24 | 0) >> 2] = 1141309440;
    HEAPF32[($0_1 + 20 | 0) >> 2] = Math_fround(Math_fround(HEAPF32[($0_1 + 4 | 0) >> 2]) + Math_fround(60.0));
    $2_1 = $46() | 0;
    HEAP8[($0_1 + 948 | 0) >> 0] = 1;
    HEAP8[($0_1 + 36 | 0) >> 0] = 1;
    $1_1 = Math_fround(+Math_fround((($2_1 | 0) % (120 | 0) | 0) + -60 | 0 | 0) * 3.141592653589793 / 180.0);
    (wasm2js_i32$0 = $0_1, wasm2js_f32$0 = Math_fround(Math_fround($40(Math_fround($1_1))) * Math_fround(6.0))), HEAPF32[(wasm2js_i32$0 + 28 | 0) >> 2] = wasm2js_f32$0;
    $1_1 = Math_fround(Math_fround($48(Math_fround($1_1))) * Math_fround(6.0));
    HEAPF32[($0_1 + 32 | 0) >> 2] = $1_1 > Math_fround(0.0) ? Math_fround(-$1_1) : $1_1;
   }
   if ((HEAP32[($0_1 + 940 | 0) >> 2] | 0 | 0) > (0 | 0)) {
    break block1
   }
   HEAP32[$0_1 >> 2] = 2;
  }
 }
 
 function $4() {
  $1(3872 | 0);
  HEAP8[(0 + 4820 | 0) >> 0] = 1;
  HEAP32[(0 + 3872 | 0) >> 2] = 1;
  HEAP32[(0 + 4816 | 0) >> 2] = 0;
 }
 
 function $5($0_1) {
  $0_1 = Math_fround($0_1);
  $3(3872 | 0, Math_fround($0_1));
 }
 
 function $6($0_1) {
  $0_1 = Math_fround($0_1);
  var $1_1 = 0;
  $0_1 = Math_fround($0_1 + Math_fround(HEAPF32[(0 + 3876 | 0) >> 2]));
  $0_1 = $0_1 < Math_fround(0.0) ? Math_fround(0.0) : $0_1;
  HEAPF32[(0 + 3876 | 0) >> 2] = $0_1;
  block : {
   $1_1 = HEAP32[(0 + 3884 | 0) >> 2] | 0;
   if (!(Math_fround($0_1 + Math_fround($1_1 | 0)) > Math_fround(800.0))) {
    break block
   }
   HEAPF32[(0 + 3876 | 0) >> 2] = Math_fround(800 - $1_1 | 0 | 0);
  }
 }
 
 function $7() {
  $2(3872 | 0);
 }
 
 function $8() {
  $1(3872 | 0);
  HEAP8[(0 + 4820 | 0) >> 0] = 1;
  HEAP32[(0 + 3872 | 0) >> 2] = 1;
  HEAP32[(0 + 4816 | 0) >> 2] = 0;
 }
 
 function $9() {
  return HEAP32[(0 + 3872 | 0) >> 2] | 0 | 0;
 }
 
 function $10() {
  return HEAP32[(0 + 3996 | 0) >> 2] | 0 | 0;
 }
 
 function $11() {
  return HEAP32[(0 + 4e3 | 0) >> 2] | 0 | 0;
 }
 
 function $12() {
  return HEAP32[(0 + 4004 | 0) >> 2] | 0 | 0;
 }
 
 function $13() {
  return HEAP32[(0 + 4812 | 0) >> 2] | 0 | 0;
 }
 
 function $14() {
  return HEAP32[(0 + 4808 | 0) >> 2] | 0 | 0;
 }
 
 function $15() {
  return HEAPU8[(0 + 4820 | 0) >> 0] | 0 | 0;
 }
 
 function $16() {
  return Math_fround(Math_fround(HEAPF32[(0 + 3876 | 0) >> 2]));
 }
 
 function $17() {
  return Math_fround(Math_fround(HEAPF32[(0 + 3880 | 0) >> 2]));
 }
 
 function $18() {
  return HEAP32[(0 + 3884 | 0) >> 2] | 0 | 0;
 }
 
 function $19() {
  return HEAP32[(0 + 3888 | 0) >> 2] | 0 | 0;
 }
 
 function $20() {
  return Math_fround(Math_fround(HEAPF32[(0 + 3892 | 0) >> 2]));
 }
 
 function $21() {
  return Math_fround(Math_fround(HEAPF32[(0 + 3896 | 0) >> 2]));
 }
 
 function $22() {
  return HEAPU8[(0 + 3908 | 0) >> 0] | 0 | 0;
 }
 
 function $23() {
  return HEAP32[(0 + 3992 | 0) >> 2] | 0 | 0;
 }
 
 function $24($0_1, $1_1) {
  $0_1 = $0_1 | 0;
  $1_1 = $1_1 | 0;
  var $2_1 = 0;
  $2_1 = 0;
  block : {
   if ($0_1 >>> 0 > 4 >>> 0) {
    break block
   }
   if ($1_1 >>> 0 > 9 >>> 0) {
    break block
   }
   $2_1 = HEAPU8[((Math_imul($0_1, 160) + ($1_1 << 4 | 0) | 0) + 4016 | 0) >> 0] | 0;
  }
  return $2_1 | 0;
 }
 
 function $25($0_1, $1_1) {
  $0_1 = $0_1 | 0;
  $1_1 = $1_1 | 0;
  var $2_1 = 0;
  $2_1 = 0;
  block : {
   if ($0_1 >>> 0 > 4 >>> 0) {
    break block
   }
   if ($1_1 >>> 0 > 9 >>> 0) {
    break block
   }
   $2_1 = HEAP32[((Math_imul($0_1, 160) + ($1_1 << 4 | 0) | 0) + 4020 | 0) >> 2] | 0;
  }
  return $2_1 | 0;
 }
 
 function $26($0_1, $1_1) {
  $0_1 = $0_1 | 0;
  $1_1 = $1_1 | 0;
  return Math_fround(Math_fround(Math_imul($1_1, 76) + 30 | 0 | 0));
 }
 
 function $27($0_1, $1_1) {
  $0_1 = $0_1 | 0;
  $1_1 = $1_1 | 0;
  return Math_fround(Math_fround(Math_imul($0_1, 30) + 80 | 0 | 0));
 }
 
 function $28() {
  return 70 | 0;
 }
 
 function $29() {
  return 24 | 0;
 }
 
 function $30() {
  return 800 | 0;
 }
 
 function $31() {
  return 600 | 0;
 }
 
 function $32() {
  return 5 | 0;
 }
 
 function $33() {
  return 10 | 0;
 }
 
 function $34() {
  return Math_fround(Math_fround(8.0));
 }
 
 function $35() {
  return 550 | 0;
 }
 
 function $36($0_1) {
  $0_1 = +$0_1;
  var $1_1 = 0.0;
  $0_1 = $0_1 * $0_1;
  $1_1 = $0_1 * $0_1;
  return Math_fround(Math_fround($0_1 * $1_1 * ($0_1 * 2.439044879627741e-05 + -.001388676377460993) + ($1_1 * .04166662332373906 + ($0_1 * -.499999997251031 + 1.0))));
 }
 
 function $37($0_1) {
  $0_1 = +$0_1;
  var $1_1 = 0.0, $2_1 = 0.0;
  $1_1 = $0_1 * $0_1;
  $2_1 = $0_1 * $1_1;
  return Math_fround(Math_fround($2_1 * ($1_1 * $1_1) * ($1_1 * 2.718311493989822e-06 + -1.9839334836096632e-04) + ($2_1 * ($1_1 * .008333329385889463 + -.16666666641626524) + $0_1)));
 }
 
 function $38($0_1, $1_1, $2_1, $3_1, $4_1) {
  $0_1 = $0_1 | 0;
  $1_1 = $1_1 | 0;
  $2_1 = $2_1 | 0;
  $3_1 = $3_1 | 0;
  $4_1 = $4_1 | 0;
  var $12_1 = 0.0, $11_1 = 0, $6_1 = 0, $5_1 = 0, $14_1 = 0, $7_1 = 0, $9_1 = 0, $20_1 = 0, $13_1 = 0, $18_1 = 0.0, $15_1 = 0, $10_1 = 0, $8_1 = 0, $21_1 = 0.0, $22_1 = 0.0, $17_1 = 0, $19_1 = 0, $16_1 = 0, $187 = 0, $190 = 0;
  $5_1 = global$0 - 560 | 0;
  global$0 = $5_1;
  $6_1 = ($2_1 + -3 | 0 | 0) / (24 | 0) | 0;
  $7_1 = ($6_1 | 0) > (0 | 0) ? $6_1 : 0;
  $8_1 = Math_imul($7_1, -24) + $2_1 | 0;
  block : {
   $9_1 = HEAP32[(($4_1 << 2 | 0) + 1024 | 0) >> 2] | 0;
   $10_1 = $3_1 + -1 | 0;
   if (($9_1 + $10_1 | 0 | 0) < (0 | 0)) {
    break block
   }
   $11_1 = $9_1 + $3_1 | 0;
   $2_1 = $7_1 - $10_1 | 0;
   $6_1 = 0;
   label : while (1) {
    block2 : {
     block1 : {
      if (($2_1 | 0) >= (0 | 0)) {
       break block1
      }
      $12_1 = 0.0;
      break block2;
     }
     $12_1 = +(HEAP32[(($2_1 << 2 | 0) + 1040 | 0) >> 2] | 0 | 0);
    }
    HEAPF64[(($5_1 + 320 | 0) + ($6_1 << 3 | 0) | 0) >> 3] = $12_1;
    $2_1 = $2_1 + 1 | 0;
    $6_1 = $6_1 + 1 | 0;
    if (($6_1 | 0) != ($11_1 | 0)) {
     continue label
    }
    break label;
   };
  }
  $13_1 = $8_1 + -24 | 0;
  $11_1 = 0;
  $14_1 = ($9_1 | 0) > (0 | 0) ? $9_1 : 0;
  $15_1 = ($3_1 | 0) < (1 | 0);
  label2 : while (1) {
   block4 : {
    block3 : {
     if (!$15_1) {
      break block3
     }
     $12_1 = 0.0;
     break block4;
    }
    $6_1 = $11_1 + $10_1 | 0;
    $2_1 = 0;
    $12_1 = 0.0;
    label1 : while (1) {
     $12_1 = +HEAPF64[($0_1 + ($2_1 << 3 | 0) | 0) >> 3] * +HEAPF64[(($5_1 + 320 | 0) + (($6_1 - $2_1 | 0) << 3 | 0) | 0) >> 3] + $12_1;
     $2_1 = $2_1 + 1 | 0;
     if (($2_1 | 0) != ($3_1 | 0)) {
      continue label1
     }
     break label1;
    };
   }
   HEAPF64[($5_1 + ($11_1 << 3 | 0) | 0) >> 3] = $12_1;
   $2_1 = ($11_1 | 0) == ($14_1 | 0);
   $11_1 = $11_1 + 1 | 0;
   if (!$2_1) {
    continue label2
   }
   break label2;
  };
  $16_1 = 47 - $8_1 | 0;
  $17_1 = 48 - $8_1 | 0;
  $15_1 = ($7_1 << 2 | 0) + 1040 | 0;
  $11_1 = $9_1;
  block21 : {
   label10 : while (1) {
    $12_1 = +HEAPF64[($5_1 + ($11_1 << 3 | 0) | 0) >> 3];
    $2_1 = 0;
    $6_1 = $11_1;
    block5 : {
     if (($11_1 | 0) < (1 | 0)) {
      break block5
     }
     label3 : while (1) {
      $18_1 = +(~~($12_1 * 5.9604644775390625e-08) | 0);
      HEAP32[(($5_1 + 480 | 0) + ($2_1 << 2 | 0) | 0) >> 2] = ~~($18_1 * -16777216.0 + $12_1);
      $12_1 = +HEAPF64[(($5_1 + ($6_1 << 3 | 0) | 0) + -8 | 0) >> 3] + $18_1;
      $6_1 = $6_1 + -1 | 0;
      $2_1 = $2_1 + 1 | 0;
      if (($2_1 | 0) != ($11_1 | 0)) {
       continue label3
      }
      break label3;
     };
    }
    $12_1 = +$47(+$12_1, $13_1 | 0);
    $12_1 = $12_1 + +$41(+($12_1 * .125)) * -8.0;
    $7_1 = ~~$12_1;
    $12_1 = $12_1 - +($7_1 | 0);
    block9 : {
     block10 : {
      block8 : {
       block7 : {
        block6 : {
         $19_1 = ($13_1 | 0) < (1 | 0);
         if ($19_1) {
          break block6
         }
         $2_1 = (($5_1 + 480 | 0) + ($11_1 << 2 | 0) | 0) + -4 | 0;
         $187 = $2_1;
         $2_1 = HEAP32[$2_1 >> 2] | 0;
         $190 = $2_1;
         $2_1 = $2_1 >> $17_1 | 0;
         $6_1 = $190 - ($2_1 << $17_1 | 0) | 0;
         HEAP32[$187 >> 2] = $6_1;
         $20_1 = $6_1 >> $16_1 | 0;
         $7_1 = $2_1 + $7_1 | 0;
         break block7;
        }
        if ($13_1) {
         break block8
        }
        $20_1 = (HEAP32[((($5_1 + 480 | 0) + ($11_1 << 2 | 0) | 0) + -4 | 0) >> 2] | 0) >> 23 | 0;
       }
       if (($20_1 | 0) < (1 | 0)) {
        break block9
       }
       break block10;
      }
      $20_1 = 2;
      if ($12_1 >= .5) {
       break block10
      }
      $20_1 = 0;
      break block9;
     }
     $2_1 = 0;
     $14_1 = 0;
     $6_1 = 1;
     block11 : {
      if (($11_1 | 0) < (1 | 0)) {
       break block11
      }
      label4 : while (1) {
       $10_1 = ($5_1 + 480 | 0) + ($2_1 << 2 | 0) | 0;
       $6_1 = HEAP32[$10_1 >> 2] | 0;
       block15 : {
        block14 : {
         block13 : {
          block12 : {
           if (!$14_1) {
            break block12
           }
           $14_1 = 16777215;
           break block13;
          }
          if (!$6_1) {
           break block14
          }
          $14_1 = 16777216;
         }
         HEAP32[$10_1 >> 2] = $14_1 - $6_1 | 0;
         $14_1 = 1;
         $6_1 = 0;
         break block15;
        }
        $14_1 = 0;
        $6_1 = 1;
       }
       $2_1 = $2_1 + 1 | 0;
       if (($2_1 | 0) != ($11_1 | 0)) {
        continue label4
       }
       break label4;
      };
     }
     block16 : {
      if ($19_1) {
       break block16
      }
      $2_1 = 8388607;
      block17 : {
       switch ($13_1 + -1 | 0 | 0) {
       case 1:
        $2_1 = 4194303;
        break;
       case 0:
        break block17;
       default:
        break block16;
       };
      }
      $14_1 = (($5_1 + 480 | 0) + ($11_1 << 2 | 0) | 0) + -4 | 0;
      HEAP32[$14_1 >> 2] = (HEAP32[$14_1 >> 2] | 0) & $2_1 | 0;
     }
     $7_1 = $7_1 + 1 | 0;
     if (($20_1 | 0) != (2 | 0)) {
      break block9
     }
     $12_1 = 1.0 - $12_1;
     $20_1 = 2;
     if ($6_1) {
      break block9
     }
     $12_1 = $12_1 - +$47(+(1.0), $13_1 | 0);
    }
    block19 : {
     if ($12_1 != 0.0) {
      break block19
     }
     $6_1 = 0;
     $2_1 = $11_1;
     block20 : {
      if (($2_1 | 0) <= ($9_1 | 0)) {
       break block20
      }
      label5 : while (1) {
       $2_1 = $2_1 + -1 | 0;
       $6_1 = HEAP32[(($5_1 + 480 | 0) + ($2_1 << 2 | 0) | 0) >> 2] | 0 | $6_1 | 0;
       if (($2_1 | 0) > ($9_1 | 0)) {
        continue label5
       }
       break label5;
      };
      if (!$6_1) {
       break block20
      }
      label6 : while (1) {
       $13_1 = $13_1 + -24 | 0;
       $11_1 = $11_1 + -1 | 0;
       if (!(HEAP32[(($5_1 + 480 | 0) + ($11_1 << 2 | 0) | 0) >> 2] | 0)) {
        continue label6
       }
       break block21;
      };
     }
     $2_1 = 1;
     label7 : while (1) {
      $6_1 = $2_1;
      $2_1 = $2_1 + 1 | 0;
      if (!(HEAP32[(($5_1 + 480 | 0) + (($9_1 - $6_1 | 0) << 2 | 0) | 0) >> 2] | 0)) {
       continue label7
      }
      break label7;
     };
     $14_1 = $6_1 + $11_1 | 0;
     label9 : while (1) {
      $6_1 = $11_1 + $3_1 | 0;
      $11_1 = $11_1 + 1 | 0;
      HEAPF64[(($5_1 + 320 | 0) + ($6_1 << 3 | 0) | 0) >> 3] = +(HEAP32[($15_1 + ($11_1 << 2 | 0) | 0) >> 2] | 0 | 0);
      $2_1 = 0;
      $12_1 = 0.0;
      block22 : {
       if (($3_1 | 0) < (1 | 0)) {
        break block22
       }
       label8 : while (1) {
        $12_1 = +HEAPF64[($0_1 + ($2_1 << 3 | 0) | 0) >> 3] * +HEAPF64[(($5_1 + 320 | 0) + (($6_1 - $2_1 | 0) << 3 | 0) | 0) >> 3] + $12_1;
        $2_1 = $2_1 + 1 | 0;
        if (($2_1 | 0) != ($3_1 | 0)) {
         continue label8
        }
        break label8;
       };
      }
      HEAPF64[($5_1 + ($11_1 << 3 | 0) | 0) >> 3] = $12_1;
      if (($11_1 | 0) < ($14_1 | 0)) {
       continue label9
      }
      break label9;
     };
     $11_1 = $14_1;
     continue label10;
    }
    break label10;
   };
   block24 : {
    block23 : {
     $12_1 = +$47(+$12_1, 24 - $8_1 | 0 | 0);
     if (!($12_1 >= 16777216.0)) {
      break block23
     }
     $2_1 = ~~($12_1 * 5.9604644775390625e-08);
     HEAP32[(($5_1 + 480 | 0) + ($11_1 << 2 | 0) | 0) >> 2] = ~~(+($2_1 | 0) * -16777216.0 + $12_1);
     $11_1 = $11_1 + 1 | 0;
     $13_1 = $8_1;
     break block24;
    }
    $2_1 = ~~$12_1;
   }
   HEAP32[(($5_1 + 480 | 0) + ($11_1 << 2 | 0) | 0) >> 2] = $2_1;
  }
  $12_1 = +$47(+(1.0), $13_1 | 0);
  block25 : {
   if (($11_1 | 0) < (0 | 0)) {
    break block25
   }
   $3_1 = $11_1;
   label11 : while (1) {
    $2_1 = $3_1;
    HEAPF64[($5_1 + ($2_1 << 3 | 0) | 0) >> 3] = $12_1 * +(HEAP32[(($5_1 + 480 | 0) + ($2_1 << 2 | 0) | 0) >> 2] | 0 | 0);
    $3_1 = $2_1 + -1 | 0;
    $12_1 = $12_1 * 5.9604644775390625e-08;
    if ($2_1) {
     continue label11
    }
    break label11;
   };
   $14_1 = $11_1;
   label13 : while (1) {
    block27 : {
     block26 : {
      $15_1 = $11_1 - $14_1 | 0;
      $6_1 = ($9_1 | 0) < ($15_1 | 0) ? $9_1 : $15_1;
      if (($6_1 | 0) >= (0 | 0)) {
       break block26
      }
      $12_1 = 0.0;
      break block27;
     }
     $0_1 = $5_1 + ($14_1 << 3 | 0) | 0;
     $2_1 = 0;
     $12_1 = 0.0;
     label12 : while (1) {
      $3_1 = $2_1 << 3 | 0;
      $12_1 = +HEAPF64[($3_1 + 3808 | 0) >> 3] * +HEAPF64[($0_1 + $3_1 | 0) >> 3] + $12_1;
      $3_1 = ($2_1 | 0) != ($6_1 | 0);
      $2_1 = $2_1 + 1 | 0;
      if ($3_1) {
       continue label12
      }
      break label12;
     };
    }
    HEAPF64[(($5_1 + 160 | 0) + ($15_1 << 3 | 0) | 0) >> 3] = $12_1;
    $2_1 = ($14_1 | 0) > (0 | 0);
    $14_1 = $14_1 + -1 | 0;
    if ($2_1) {
     continue label13
    }
    break label13;
   };
  }
  block31 : {
   block33 : {
    block29 : {
     switch ($4_1 | 0) {
     case 3:
      $21_1 = 0.0;
      block32 : {
       if (($11_1 | 0) <= (0 | 0)) {
        break block32
       }
       $2_1 = $11_1;
       label14 : while (1) {
        $3_1 = ($5_1 + 160 | 0) + ($2_1 << 3 | 0) | 0;
        $6_1 = $3_1 + -8 | 0;
        $12_1 = +HEAPF64[$6_1 >> 3];
        $18_1 = +HEAPF64[$3_1 >> 3];
        $22_1 = $12_1 + $18_1;
        HEAPF64[$6_1 >> 3] = $22_1;
        HEAPF64[$3_1 >> 3] = $18_1 + ($12_1 - $22_1);
        $3_1 = $2_1 >>> 0 > 1 >>> 0;
        $2_1 = $2_1 + -1 | 0;
        if ($3_1) {
         continue label14
        }
        break label14;
       };
       if (($11_1 | 0) == (1 | 0)) {
        break block32
       }
       $2_1 = $11_1;
       label15 : while (1) {
        $3_1 = ($5_1 + 160 | 0) + ($2_1 << 3 | 0) | 0;
        $6_1 = $3_1 + -8 | 0;
        $12_1 = +HEAPF64[$6_1 >> 3];
        $18_1 = +HEAPF64[$3_1 >> 3];
        $22_1 = $12_1 + $18_1;
        HEAPF64[$6_1 >> 3] = $22_1;
        HEAPF64[$3_1 >> 3] = $18_1 + ($12_1 - $22_1);
        $3_1 = $2_1 >>> 0 > 2 >>> 0;
        $2_1 = $2_1 + -1 | 0;
        if ($3_1) {
         continue label15
        }
        break label15;
       };
       $21_1 = 0.0;
       label16 : while (1) {
        $21_1 = $21_1 + +HEAPF64[(($5_1 + 160 | 0) + ($11_1 << 3 | 0) | 0) >> 3];
        $2_1 = $11_1 >>> 0 > 2 >>> 0;
        $11_1 = $11_1 + -1 | 0;
        if ($2_1) {
         continue label16
        }
        break label16;
       };
      }
      $12_1 = +HEAPF64[($5_1 + 160 | 0) >> 3];
      if ($20_1) {
       break block33
      }
      HEAPF64[$1_1 >> 3] = $12_1;
      $12_1 = +HEAPF64[($5_1 + 168 | 0) >> 3];
      HEAPF64[($1_1 + 16 | 0) >> 3] = $21_1;
      HEAPF64[($1_1 + 8 | 0) >> 3] = $12_1;
      break block31;
     case 0:
      $12_1 = 0.0;
      block34 : {
       if (($11_1 | 0) < (0 | 0)) {
        break block34
       }
       label17 : while (1) {
        $2_1 = $11_1;
        $11_1 = $2_1 + -1 | 0;
        $12_1 = $12_1 + +HEAPF64[(($5_1 + 160 | 0) + ($2_1 << 3 | 0) | 0) >> 3];
        if ($2_1) {
         continue label17
        }
        break label17;
       };
      }
      HEAPF64[$1_1 >> 3] = $20_1 ? -$12_1 : $12_1;
      break block31;
     case 1:
     case 2:
      break block29;
     default:
      break block31;
     };
    }
    $12_1 = 0.0;
    block35 : {
     if (($11_1 | 0) < (0 | 0)) {
      break block35
     }
     $3_1 = $11_1;
     label18 : while (1) {
      $2_1 = $3_1;
      $3_1 = $2_1 + -1 | 0;
      $12_1 = $12_1 + +HEAPF64[(($5_1 + 160 | 0) + ($2_1 << 3 | 0) | 0) >> 3];
      if ($2_1) {
       continue label18
      }
      break label18;
     };
    }
    HEAPF64[$1_1 >> 3] = $20_1 ? -$12_1 : $12_1;
    $12_1 = +HEAPF64[($5_1 + 160 | 0) >> 3] - $12_1;
    $2_1 = 1;
    block36 : {
     if (($11_1 | 0) < (1 | 0)) {
      break block36
     }
     label19 : while (1) {
      $12_1 = $12_1 + +HEAPF64[(($5_1 + 160 | 0) + ($2_1 << 3 | 0) | 0) >> 3];
      $3_1 = ($2_1 | 0) != ($11_1 | 0);
      $2_1 = $2_1 + 1 | 0;
      if ($3_1) {
       continue label19
      }
      break label19;
     };
    }
    HEAPF64[($1_1 + 8 | 0) >> 3] = $20_1 ? -$12_1 : $12_1;
    break block31;
   }
   HEAPF64[$1_1 >> 3] = -$12_1;
   $12_1 = +HEAPF64[($5_1 + 168 | 0) >> 3];
   HEAPF64[($1_1 + 16 | 0) >> 3] = -$21_1;
   HEAPF64[($1_1 + 8 | 0) >> 3] = -$12_1;
  }
  global$0 = $5_1 + 560 | 0;
  return $7_1 & 7 | 0 | 0;
 }
 
 function $39($0_1, $1_1) {
  $0_1 = Math_fround($0_1);
  $1_1 = $1_1 | 0;
  var $4_1 = 0, $6_1 = 0.0, $2_1 = 0, $5_1 = 0.0, $7_1 = 0.0, $3_1 = 0, $8_1 = 0;
  $2_1 = global$0 - 16 | 0;
  global$0 = $2_1;
  block2 : {
   block : {
    $3_1 = (wasm2js_scratch_store_f32($0_1), wasm2js_scratch_load_i32(2));
    $4_1 = $3_1 & 2147483647 | 0;
    if ($4_1 >>> 0 > 1305022426 >>> 0) {
     break block
    }
    $5_1 = +$0_1;
    $6_1 = $5_1 * .6366197723675814 + 6755399441055744.0 + -6755399441055744.0;
    $7_1 = $5_1 + $6_1 * -1.5707963109016418 + $6_1 * -1.5893254773528196e-08;
    HEAPF64[$1_1 >> 3] = $7_1;
    $4_1 = ~~$6_1;
    block1 : {
     if (!($7_1 < -.7853981852531433)) {
      break block1
     }
     $6_1 = $6_1 + -1.0;
     HEAPF64[$1_1 >> 3] = $5_1 + $6_1 * -1.5707963109016418 + $6_1 * -1.5893254773528196e-08;
     $4_1 = $4_1 + -1 | 0;
     break block2;
    }
    if (!($7_1 > .7853981852531433)) {
     break block2
    }
    $6_1 = $6_1 + 1.0;
    HEAPF64[$1_1 >> 3] = $5_1 + $6_1 * -1.5707963109016418 + $6_1 * -1.5893254773528196e-08;
    $4_1 = $4_1 + 1 | 0;
    break block2;
   }
   block3 : {
    if ($4_1 >>> 0 < 2139095040 >>> 0) {
     break block3
    }
    HEAPF64[$1_1 >> 3] = +Math_fround($0_1 - $0_1);
    $4_1 = 0;
    break block2;
   }
   $8_1 = ($4_1 >>> 23 | 0) + -150 | 0;
   HEAPF64[($2_1 + 8 | 0) >> 3] = +(wasm2js_scratch_store_i32(2, $4_1 - ($8_1 << 23 | 0) | 0), wasm2js_scratch_load_f32());
   $4_1 = $38($2_1 + 8 | 0 | 0, $2_1 | 0, $8_1 | 0, 1 | 0, 0 | 0) | 0;
   $6_1 = +HEAPF64[$2_1 >> 3];
   block4 : {
    if (($3_1 | 0) > (-1 | 0)) {
     break block4
    }
    HEAPF64[$1_1 >> 3] = -$6_1;
    $4_1 = 0 - $4_1 | 0;
    break block2;
   }
   HEAPF64[$1_1 >> 3] = $6_1;
  }
  global$0 = $2_1 + 16 | 0;
  return $4_1 | 0;
 }
 
 function $40($0_1) {
  $0_1 = Math_fround($0_1);
  var $4_1 = Math_fround(0), $3_1 = 0, $5_1 = 0.0, $2_1 = 0, $1_1 = 0;
  $1_1 = global$0 - 16 | 0;
  global$0 = $1_1;
  block1 : {
   block : {
    $2_1 = (wasm2js_scratch_store_f32($0_1), wasm2js_scratch_load_i32(2));
    $3_1 = $2_1 & 2147483647 | 0;
    if ($3_1 >>> 0 > 1061752794 >>> 0) {
     break block
    }
    $4_1 = Math_fround(1.0);
    if ($3_1 >>> 0 < 964689920 >>> 0) {
     break block1
    }
    $4_1 = Math_fround($36(+(+$0_1)));
    break block1;
   }
   block2 : {
    if ($3_1 >>> 0 > 1081824209 >>> 0) {
     break block2
    }
    block3 : {
     if ($3_1 >>> 0 < 1075235812 >>> 0) {
      break block3
     }
     $4_1 = Math_fround(-Math_fround($36(+((($2_1 | 0) < (0 | 0) ? 3.141592653589793 : -3.141592653589793) + +$0_1))));
     break block1;
    }
    $5_1 = +$0_1;
    block4 : {
     if (($2_1 | 0) > (-1 | 0)) {
      break block4
     }
     $4_1 = Math_fround($37(+($5_1 + 1.5707963267948966)));
     break block1;
    }
    $4_1 = Math_fround($37(+(1.5707963267948966 - $5_1)));
    break block1;
   }
   block5 : {
    if ($3_1 >>> 0 > 1088565717 >>> 0) {
     break block5
    }
    block6 : {
     if ($3_1 >>> 0 < 1085271520 >>> 0) {
      break block6
     }
     $4_1 = Math_fround($36(+((($2_1 | 0) < (0 | 0) ? 6.283185307179586 : -6.283185307179586) + +$0_1)));
     break block1;
    }
    block7 : {
     if (($2_1 | 0) > (-1 | 0)) {
      break block7
     }
     $4_1 = Math_fround($37(+(-4.71238898038469 - +$0_1)));
     break block1;
    }
    $4_1 = Math_fround($37(+(+$0_1 + -4.71238898038469)));
    break block1;
   }
   block8 : {
    if ($3_1 >>> 0 < 2139095040 >>> 0) {
     break block8
    }
    $4_1 = Math_fround($0_1 - $0_1);
    break block1;
   }
   $3_1 = $39(Math_fround($0_1), $1_1 + 8 | 0 | 0) | 0;
   $5_1 = +HEAPF64[($1_1 + 8 | 0) >> 3];
   block12 : {
    switch ($3_1 & 3 | 0 | 0) {
    default:
     $4_1 = Math_fround($36(+$5_1));
     break block1;
    case 1:
     $4_1 = Math_fround($37(+-$5_1));
     break block1;
    case 2:
     $4_1 = Math_fround(-Math_fround($36(+$5_1)));
     break block1;
    case 3:
     break block12;
    };
   }
   $4_1 = Math_fround($37(+$5_1));
  }
  global$0 = $1_1 + 16 | 0;
  return Math_fround($4_1);
 }
 
 function $41($0_1) {
  $0_1 = +$0_1;
  return +Math_floor($0_1);
 }
 
 function $42($0_1, $1_1) {
  $0_1 = Math_fround($0_1);
  $1_1 = Math_fround($1_1);
  block : {
   if ((($43(Math_fround($0_1)) | 0) & 2147483647 | 0) >>> 0 > 2139095040 >>> 0) {
    break block
   }
   $1_1 = (($43(Math_fround($1_1)) | 0) & 2147483647 | 0) >>> 0 > 2139095040 >>> 0 ? $0_1 : Math_fround(Math_max($0_1, $1_1));
  }
  return Math_fround($1_1);
 }
 
 function $43($0_1) {
  $0_1 = Math_fround($0_1);
  return (wasm2js_scratch_store_f32($0_1), wasm2js_scratch_load_i32(2)) | 0;
 }
 
 function $44($0_1, $1_1) {
  $0_1 = Math_fround($0_1);
  $1_1 = Math_fround($1_1);
  block : {
   if ((($45(Math_fround($0_1)) | 0) & 2147483647 | 0) >>> 0 > 2139095040 >>> 0) {
    break block
   }
   $1_1 = (($45(Math_fround($1_1)) | 0) & 2147483647 | 0) >>> 0 > 2139095040 >>> 0 ? $0_1 : Math_fround(Math_min($0_1, $1_1));
  }
  return Math_fround($1_1);
 }
 
 function $45($0_1) {
  $0_1 = Math_fround($0_1);
  return (wasm2js_scratch_store_f32($0_1), wasm2js_scratch_load_i32(2)) | 0;
 }
 
 function $46() {
  var i64toi32_i32$0 = 0, i64toi32_i32$2 = 0, i64toi32_i32$5 = 0, i64toi32_i32$1 = 0, i64toi32_i32$3 = 0, i64toi32_i32$4 = 0, $6_1 = 0, $7_1 = 0;
  i64toi32_i32$2 = 0;
  i64toi32_i32$0 = HEAP32[(i64toi32_i32$2 + 4832 | 0) >> 2] | 0;
  i64toi32_i32$1 = HEAP32[(i64toi32_i32$2 + 4836 | 0) >> 2] | 0;
  $7_1 = i64toi32_i32$0;
  i64toi32_i32$0 = 1481765933;
  i64toi32_i32$0 = __wasm_i64_mul($7_1 | 0, i64toi32_i32$1 | 0, 1284865837 | 0, i64toi32_i32$0 | 0) | 0;
  i64toi32_i32$1 = i64toi32_i32$HIGH_BITS;
  i64toi32_i32$2 = i64toi32_i32$0;
  i64toi32_i32$0 = 0;
  i64toi32_i32$3 = 1;
  i64toi32_i32$4 = i64toi32_i32$2 + i64toi32_i32$3 | 0;
  i64toi32_i32$5 = i64toi32_i32$1 + i64toi32_i32$0 | 0;
  if (i64toi32_i32$4 >>> 0 < i64toi32_i32$3 >>> 0) {
   i64toi32_i32$5 = i64toi32_i32$5 + 1 | 0
  }
  i64toi32_i32$2 = 0;
  HEAP32[(i64toi32_i32$2 + 4832 | 0) >> 2] = i64toi32_i32$4;
  HEAP32[(i64toi32_i32$2 + 4836 | 0) >> 2] = i64toi32_i32$5;
  i64toi32_i32$1 = i64toi32_i32$4;
  i64toi32_i32$2 = 0;
  i64toi32_i32$3 = 33;
  i64toi32_i32$0 = i64toi32_i32$3 & 31 | 0;
  if (32 >>> 0 <= (i64toi32_i32$3 & 63 | 0) >>> 0) {
   i64toi32_i32$2 = 0;
   $6_1 = i64toi32_i32$5 >>> i64toi32_i32$0 | 0;
  } else {
   i64toi32_i32$2 = i64toi32_i32$5 >>> i64toi32_i32$0 | 0;
   $6_1 = (((1 << i64toi32_i32$0 | 0) - 1 | 0) & i64toi32_i32$5 | 0) << (32 - i64toi32_i32$0 | 0) | 0 | (i64toi32_i32$1 >>> i64toi32_i32$0 | 0) | 0;
  }
  return $6_1 | 0;
 }
 
 function $47($0_1, $1_1) {
  $0_1 = +$0_1;
  $1_1 = $1_1 | 0;
  var i64toi32_i32$4 = 0, i64toi32_i32$2 = 0, i64toi32_i32$1 = 0, i64toi32_i32$3 = 0, $8_1 = 0, $32_1 = 0.0, i64toi32_i32$0 = 0;
  block2 : {
   block : {
    if (($1_1 | 0) < (1024 | 0)) {
     break block
    }
    $0_1 = $0_1 * 8988465674311579538646525.0e283;
    block1 : {
     if ($1_1 >>> 0 >= 2047 >>> 0) {
      break block1
     }
     $1_1 = $1_1 + -1023 | 0;
     break block2;
    }
    $0_1 = $0_1 * 8988465674311579538646525.0e283;
    $1_1 = ($1_1 >>> 0 < 3069 >>> 0 ? $1_1 : 3069) + -2046 | 0;
    break block2;
   }
   if (($1_1 | 0) > (-1023 | 0)) {
    break block2
   }
   $0_1 = $0_1 * 2.004168360008973e-292;
   block3 : {
    if ($1_1 >>> 0 <= -1992 >>> 0) {
     break block3
    }
    $1_1 = $1_1 + 969 | 0;
    break block2;
   }
   $0_1 = $0_1 * 2.004168360008973e-292;
   $1_1 = ($1_1 >>> 0 > -2960 >>> 0 ? $1_1 : -2960) + 1938 | 0;
  }
  $32_1 = $0_1;
  i64toi32_i32$0 = 0;
  i64toi32_i32$2 = $1_1 + 1023 | 0;
  i64toi32_i32$1 = 0;
  i64toi32_i32$3 = 52;
  i64toi32_i32$4 = i64toi32_i32$3 & 31 | 0;
  if (32 >>> 0 <= (i64toi32_i32$3 & 63 | 0) >>> 0) {
   i64toi32_i32$1 = i64toi32_i32$2 << i64toi32_i32$4 | 0;
   $8_1 = 0;
  } else {
   i64toi32_i32$1 = ((1 << i64toi32_i32$4 | 0) - 1 | 0) & (i64toi32_i32$2 >>> (32 - i64toi32_i32$4 | 0) | 0) | 0 | (i64toi32_i32$0 << i64toi32_i32$4 | 0) | 0;
   $8_1 = i64toi32_i32$2 << i64toi32_i32$4 | 0;
  }
  wasm2js_scratch_store_i32(0 | 0, $8_1 | 0);
  wasm2js_scratch_store_i32(1 | 0, i64toi32_i32$1 | 0);
  return +($32_1 * +wasm2js_scratch_load_f64());
 }
 
 function $48($0_1) {
  $0_1 = Math_fround($0_1);
  var $4_1 = 0.0, $3_1 = 0, $2_1 = 0, $1_1 = 0;
  $1_1 = global$0 - 16 | 0;
  global$0 = $1_1;
  block1 : {
   block : {
    $2_1 = (wasm2js_scratch_store_f32($0_1), wasm2js_scratch_load_i32(2));
    $3_1 = $2_1 & 2147483647 | 0;
    if ($3_1 >>> 0 > 1061752794 >>> 0) {
     break block
    }
    if ($3_1 >>> 0 < 964689920 >>> 0) {
     break block1
    }
    $0_1 = Math_fround($37(+(+$0_1)));
    break block1;
   }
   block2 : {
    if ($3_1 >>> 0 > 1081824209 >>> 0) {
     break block2
    }
    $4_1 = +$0_1;
    block3 : {
     if ($3_1 >>> 0 > 1075235811 >>> 0) {
      break block3
     }
     block4 : {
      if (($2_1 | 0) > (-1 | 0)) {
       break block4
      }
      $0_1 = Math_fround(-Math_fround($36(+($4_1 + 1.5707963267948966))));
      break block1;
     }
     $0_1 = Math_fround($36(+($4_1 + -1.5707963267948966)));
     break block1;
    }
    $0_1 = Math_fround($37(+-((($2_1 | 0) > (-1 | 0) ? -3.141592653589793 : 3.141592653589793) + $4_1)));
    break block1;
   }
   block5 : {
    if ($3_1 >>> 0 > 1088565717 >>> 0) {
     break block5
    }
    block6 : {
     if ($3_1 >>> 0 > 1085271519 >>> 0) {
      break block6
     }
     $4_1 = +$0_1;
     block7 : {
      if (($2_1 | 0) > (-1 | 0)) {
       break block7
      }
      $0_1 = Math_fround($36(+($4_1 + 4.71238898038469)));
      break block1;
     }
     $0_1 = Math_fround(-Math_fround($36(+($4_1 + -4.71238898038469))));
     break block1;
    }
    $0_1 = Math_fround($37(+((($2_1 | 0) < (0 | 0) ? 6.283185307179586 : -6.283185307179586) + +$0_1)));
    break block1;
   }
   block8 : {
    if ($3_1 >>> 0 < 2139095040 >>> 0) {
     break block8
    }
    $0_1 = Math_fround($0_1 - $0_1);
    break block1;
   }
   $3_1 = $39(Math_fround($0_1), $1_1 + 8 | 0 | 0) | 0;
   $4_1 = +HEAPF64[($1_1 + 8 | 0) >> 3];
   block12 : {
    switch ($3_1 & 3 | 0 | 0) {
    default:
     $0_1 = Math_fround($37(+$4_1));
     break block1;
    case 1:
     $0_1 = Math_fround($36(+$4_1));
     break block1;
    case 2:
     $0_1 = Math_fround($37(+-$4_1));
     break block1;
    case 3:
     break block12;
    };
   }
   $0_1 = Math_fround(-Math_fround($36(+$4_1)));
  }
  global$0 = $1_1 + 16 | 0;
  return Math_fround($0_1);
 }
 
 function $49($0_1) {
  $0_1 = $0_1 | 0;
  global$0 = $0_1;
 }
 
 function $50($0_1) {
  $0_1 = $0_1 | 0;
  var $1_1 = 0;
  $1_1 = (global$0 - $0_1 | 0) & -16 | 0;
  global$0 = $1_1;
  return $1_1 | 0;
 }
 
 function $51() {
  return global$0 | 0;
 }
 
 function _ZN17compiler_builtins3int3mul3Mul3mul17h070e9a1c69faec5bE(var$0, var$0$hi, var$1, var$1$hi) {
  var$0 = var$0 | 0;
  var$0$hi = var$0$hi | 0;
  var$1 = var$1 | 0;
  var$1$hi = var$1$hi | 0;
  var i64toi32_i32$4 = 0, i64toi32_i32$0 = 0, i64toi32_i32$1 = 0, var$2 = 0, i64toi32_i32$2 = 0, i64toi32_i32$3 = 0, var$3 = 0, var$4 = 0, var$5 = 0, $21_1 = 0, $22_1 = 0, var$6 = 0, $24_1 = 0, $17_1 = 0, $18_1 = 0, $23_1 = 0, $29_1 = 0, $45_1 = 0, $56$hi = 0, $62$hi = 0;
  i64toi32_i32$0 = var$1$hi;
  var$2 = var$1;
  var$4 = var$2 >>> 16 | 0;
  i64toi32_i32$0 = var$0$hi;
  var$3 = var$0;
  var$5 = var$3 >>> 16 | 0;
  $17_1 = Math_imul(var$4, var$5);
  $18_1 = var$2;
  i64toi32_i32$2 = var$3;
  i64toi32_i32$1 = 0;
  i64toi32_i32$3 = 32;
  i64toi32_i32$4 = i64toi32_i32$3 & 31 | 0;
  if (32 >>> 0 <= (i64toi32_i32$3 & 63 | 0) >>> 0) {
   i64toi32_i32$1 = 0;
   $21_1 = i64toi32_i32$0 >>> i64toi32_i32$4 | 0;
  } else {
   i64toi32_i32$1 = i64toi32_i32$0 >>> i64toi32_i32$4 | 0;
   $21_1 = (((1 << i64toi32_i32$4 | 0) - 1 | 0) & i64toi32_i32$0 | 0) << (32 - i64toi32_i32$4 | 0) | 0 | (i64toi32_i32$2 >>> i64toi32_i32$4 | 0) | 0;
  }
  $23_1 = $17_1 + Math_imul($18_1, $21_1) | 0;
  i64toi32_i32$1 = var$1$hi;
  i64toi32_i32$0 = var$1;
  i64toi32_i32$2 = 0;
  i64toi32_i32$3 = 32;
  i64toi32_i32$4 = i64toi32_i32$3 & 31 | 0;
  if (32 >>> 0 <= (i64toi32_i32$3 & 63 | 0) >>> 0) {
   i64toi32_i32$2 = 0;
   $22_1 = i64toi32_i32$1 >>> i64toi32_i32$4 | 0;
  } else {
   i64toi32_i32$2 = i64toi32_i32$1 >>> i64toi32_i32$4 | 0;
   $22_1 = (((1 << i64toi32_i32$4 | 0) - 1 | 0) & i64toi32_i32$1 | 0) << (32 - i64toi32_i32$4 | 0) | 0 | (i64toi32_i32$0 >>> i64toi32_i32$4 | 0) | 0;
  }
  $29_1 = $23_1 + Math_imul($22_1, var$3) | 0;
  var$2 = var$2 & 65535 | 0;
  var$3 = var$3 & 65535 | 0;
  var$6 = Math_imul(var$2, var$3);
  var$2 = (var$6 >>> 16 | 0) + Math_imul(var$2, var$5) | 0;
  $45_1 = $29_1 + (var$2 >>> 16 | 0) | 0;
  var$2 = (var$2 & 65535 | 0) + Math_imul(var$4, var$3) | 0;
  i64toi32_i32$2 = 0;
  i64toi32_i32$1 = $45_1 + (var$2 >>> 16 | 0) | 0;
  i64toi32_i32$0 = 0;
  i64toi32_i32$3 = 32;
  i64toi32_i32$4 = i64toi32_i32$3 & 31 | 0;
  if (32 >>> 0 <= (i64toi32_i32$3 & 63 | 0) >>> 0) {
   i64toi32_i32$0 = i64toi32_i32$1 << i64toi32_i32$4 | 0;
   $24_1 = 0;
  } else {
   i64toi32_i32$0 = ((1 << i64toi32_i32$4 | 0) - 1 | 0) & (i64toi32_i32$1 >>> (32 - i64toi32_i32$4 | 0) | 0) | 0 | (i64toi32_i32$2 << i64toi32_i32$4 | 0) | 0;
   $24_1 = i64toi32_i32$1 << i64toi32_i32$4 | 0;
  }
  $56$hi = i64toi32_i32$0;
  i64toi32_i32$0 = 0;
  $62$hi = i64toi32_i32$0;
  i64toi32_i32$0 = $56$hi;
  i64toi32_i32$2 = $24_1;
  i64toi32_i32$1 = $62$hi;
  i64toi32_i32$3 = var$2 << 16 | 0 | (var$6 & 65535 | 0) | 0;
  i64toi32_i32$1 = i64toi32_i32$0 | i64toi32_i32$1 | 0;
  i64toi32_i32$2 = i64toi32_i32$2 | i64toi32_i32$3 | 0;
  i64toi32_i32$HIGH_BITS = i64toi32_i32$1;
  return i64toi32_i32$2 | 0;
 }
 
 function __wasm_i64_mul(var$0, var$0$hi, var$1, var$1$hi) {
  var$0 = var$0 | 0;
  var$0$hi = var$0$hi | 0;
  var$1 = var$1 | 0;
  var$1$hi = var$1$hi | 0;
  var i64toi32_i32$0 = 0, i64toi32_i32$1 = 0;
  i64toi32_i32$0 = var$0$hi;
  i64toi32_i32$0 = var$1$hi;
  i64toi32_i32$0 = var$0$hi;
  i64toi32_i32$1 = var$1$hi;
  i64toi32_i32$1 = _ZN17compiler_builtins3int3mul3Mul3mul17h070e9a1c69faec5bE(var$0 | 0, i64toi32_i32$0 | 0, var$1 | 0, i64toi32_i32$1 | 0) | 0;
  i64toi32_i32$0 = i64toi32_i32$HIGH_BITS;
  i64toi32_i32$HIGH_BITS = i64toi32_i32$0;
  return i64toi32_i32$1 | 0;
 }
 
 bufferView = HEAPU8;
 initActiveSegments(imports);
 var FUNCTION_TABLE = Table([]);
 function __wasm_memory_size() {
  return buffer.byteLength >> 16;
 }
 
 function __wasm_memory_grow(pagesToAdd) {
  pagesToAdd = pagesToAdd | 0;
  var oldPages = __wasm_memory_size() | 0;
  var newPages = oldPages + pagesToAdd | 0;
  if ((oldPages < newPages) && (newPages < 65536) && (newPages <= 32768)) {
   var newBuffer = new ArrayBuffer(newPages << 16);
   var newHEAP8 = new Int8Array(newBuffer);
   newHEAP8.set(HEAP8);
   HEAP8 = new Int8Array(newBuffer);
   HEAP16 = new Int16Array(newBuffer);
   HEAP32 = new Int32Array(newBuffer);
   HEAPU8 = new Uint8Array(newBuffer);
   HEAPU16 = new Uint16Array(newBuffer);
   HEAPU32 = new Uint32Array(newBuffer);
   HEAPF32 = new Float32Array(newBuffer);
   HEAPF64 = new Float64Array(newBuffer);
   buffer = newBuffer;
   bufferView = HEAPU8;
  }
  return oldPages;
 }
 
 return {
  "memory": Object.create(Object.prototype, {
   "grow": {
    "value": __wasm_memory_grow
   }, 
   "buffer": {
    "get": function () {
     return buffer;
    }
    
   }
  }), 
  "__wasm_call_ctors": $0, 
  "init_game": $1, 
  "launch_ball": $2, 
  "update_game": $3, 
  "_init_game": $4, 
  "_update_game": $5, 
  "_move_paddle": $6, 
  "_launch_ball": $7, 
  "_reset_game": $8, 
  "_get_game_state": $9, 
  "_get_lives": $10, 
  "_get_score": $11, 
  "_get_level": $12, 
  "_get_bricks_remaining": $13, 
  "_get_total_bricks": $14, 
  "_get_launch_ready": $15, 
  "_get_paddle_x": $16, 
  "_get_paddle_y": $17, 
  "_get_paddle_w": $18, 
  "_get_paddle_h": $19, 
  "_get_ball_x": $20, 
  "_get_ball_y": $21, 
  "_get_ball_active": $22, 
  "_get_ball_count": $23, 
  "_get_brick_alive": $24, 
  "_get_brick_color_idx": $25, 
  "_get_brick_x": $26, 
  "_get_brick_y": $27, 
  "_get_brick_w": $28, 
  "_get_brick_h": $29, 
  "_get_screen_w": $30, 
  "_get_screen_h": $31, 
  "_get_brick_rows": $32, 
  "_get_brick_cols": $33, 
  "_get_ball_r": $34, 
  "_get_paddle_y_val": $35, 
  "__indirect_function_table": FUNCTION_TABLE, 
  "_emscripten_stack_restore": $49, 
  "_emscripten_stack_alloc": $50, 
  "emscripten_stack_get_current": $51
 };
}

var retasmFunc = asmFunc({
});
export var memory = retasmFunc.memory;
export var __wasm_call_ctors = retasmFunc.__wasm_call_ctors;
export var init_game = retasmFunc.init_game;
export var launch_ball = retasmFunc.launch_ball;
export var update_game = retasmFunc.update_game;
export var _init_game = retasmFunc._init_game;
export var _update_game = retasmFunc._update_game;
export var _move_paddle = retasmFunc._move_paddle;
export var _launch_ball = retasmFunc._launch_ball;
export var _reset_game = retasmFunc._reset_game;
export var _get_game_state = retasmFunc._get_game_state;
export var _get_lives = retasmFunc._get_lives;
export var _get_score = retasmFunc._get_score;
export var _get_level = retasmFunc._get_level;
export var _get_bricks_remaining = retasmFunc._get_bricks_remaining;
export var _get_total_bricks = retasmFunc._get_total_bricks;
export var _get_launch_ready = retasmFunc._get_launch_ready;
export var _get_paddle_x = retasmFunc._get_paddle_x;
export var _get_paddle_y = retasmFunc._get_paddle_y;
export var _get_paddle_w = retasmFunc._get_paddle_w;
export var _get_paddle_h = retasmFunc._get_paddle_h;
export var _get_ball_x = retasmFunc._get_ball_x;
export var _get_ball_y = retasmFunc._get_ball_y;
export var _get_ball_active = retasmFunc._get_ball_active;
export var _get_ball_count = retasmFunc._get_ball_count;
export var _get_brick_alive = retasmFunc._get_brick_alive;
export var _get_brick_color_idx = retasmFunc._get_brick_color_idx;
export var _get_brick_x = retasmFunc._get_brick_x;
export var _get_brick_y = retasmFunc._get_brick_y;
export var _get_brick_w = retasmFunc._get_brick_w;
export var _get_brick_h = retasmFunc._get_brick_h;
export var _get_screen_w = retasmFunc._get_screen_w;
export var _get_screen_h = retasmFunc._get_screen_h;
export var _get_brick_rows = retasmFunc._get_brick_rows;
export var _get_brick_cols = retasmFunc._get_brick_cols;
export var _get_ball_r = retasmFunc._get_ball_r;
export var _get_paddle_y_val = retasmFunc._get_paddle_y_val;
export var _emscripten_stack_restore = retasmFunc._emscripten_stack_restore;
export var _emscripten_stack_alloc = retasmFunc._emscripten_stack_alloc;
export var emscripten_stack_get_current = retasmFunc.emscripten_stack_get_current;
