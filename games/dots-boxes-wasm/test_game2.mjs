import { readFileSync } from 'fs';

const wasmBuffer = readFileSync('./dots_boxes.wasm');

async function runTests() {
  const { instance } = await WebAssembly.instantiate(wasmBuffer, {});
  const { db_reset, db_click_h, db_click_v, db_score, db_phase, db_boxes_done } = instance.exports;

  console.log('=== Game Over Test ===\n');
  db_reset();
  
  // Draw ALL horizontal edges (rows 0..4, cols 0..3)
  for (let r = 0; r <= 4; r++)
    for (let c = 0; c < 4; c++)
      db_click_h(r, c);
  
  // Draw ALL vertical edges (rows 0..3, cols 0..4)
  for (let r = 0; r < 4; r++)
    for (let c = 0; c <= 4; c++)
      db_click_v(r, c);
  
  console.log('After completing all edges:');
  console.log('  Phase:', db_phase(), '(expected 1=GAME_OVER)');
  console.log('  Boxes done:', db_boxes_done(), '(expected 16)');
  console.log('  Score(0):', db_score(0), '(expected 16)');
  
  if (db_phase() === 1 && db_boxes_done() === 16) {
    console.log('\n✅ All Tests PASSED');
  } else {
    console.log('\n❌ Tests FAILED');
  }
}

runTests().catch(console.error);
