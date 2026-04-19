// Test Dots and Boxes WASM game logic
import { readFileSync } from 'fs';

// Load the WASM module
const wasmBuffer = readFileSync('./dots_boxes.wasm');

// Simple WASM interpreter test
async function runTests() {
  const { instance } = await WebAssembly.instantiate(wasmBuffer, {});
  const { db_reset, db_click_h, db_click_v, db_cell, db_score, db_player, db_phase, db_boxes_done } = instance.exports;

  console.log('=== Dots and Boxes WASM Tests ===\n');

  // Test 1: Initial state
  db_reset();
  console.log('Test 1: Initial state');
  console.log('  Phase:', db_phase(), '(expected 0 = PHASE_PLACING)');
  console.log('  Player:', db_player(), '(expected 0)');
  console.log('  Boxes done:', db_boxes_done(), '(expected 0)');
  console.log('  Score(0):', db_score(0), '(expected 0)');

  // Test 2: Cell encoding (dots at even positions)
  console.log('\nTest 2: Cell encoding');
  // Dot at (0,0) -> getCell(0,0) should be CELL_DOT
  let dotCell = db_cell(0, 0);
  console.log(`  Dot at (0,0): cell=${dotCell}, isDot=${dotCell === 1}`);

  // Empty at undrawn edge
  let emptyEdge = db_cell(0, 1);  // horizontal edge between (0,0) and (0,1)
  console.log(`  Undrawn H-edge at (0,0)-(0,1): cell=${emptyEdge} (expected 0=EMPTY)`);

  // Test 3: Draw a horizontal edge
  console.log('\nTest 3: Draw horizontal edge');
  let rv = db_click_h(0, 0);
  console.log('  draw_h(0,0) returned:', rv, '(expected 0=success)');
  let drawnCell = db_cell(0, 1);
  console.log('  Cell after draw:', drawnCell, '(expected 2-4=H_LINE)');
  console.log('  Player still:', db_player(), '(expected 0, no box completed)');

  // Test 4: Draw edge that completes a box
  console.log('\nTest 4: Box completion test');
  db_reset();
  // Complete top-left box (0,0): need (0,0)[h], (0,0)[v], (0,1)[v], (1,0)[h]
  db_click_h(0, 0);  // top edge of box (0,0)
  db_click_v(0, 0);  // left edge of box (0,0)
  db_click_v(0, 1);  // right edge of box (0,0)
  let rv4 = db_click_h(1, 0);  // bottom edge -> should complete box
  console.log('  draw_h(1,0) returned:', rv4, '(expected 0=success)');
  console.log('  Player:', db_player(), '(expected 0, same player scores)');
  console.log('  Score(0):', db_score(0), '(expected 1)');
  console.log('  Boxes done:', db_boxes_done(), '(expected 1)');
  let boxCell = db_cell(1, 1);
  console.log('  Box cell ownership:', boxCell, '(expected 8=BOX_0)');

  // Test 5: Game over
  console.log('\nTest 5: Game over detection');
  db_reset();
  // Fill all 16 boxes (for testing, we'll just check phase logic)
  let allDrawn = true;
  for (let r = 0; r < 4; r++) {
    for (let c = 0; c < 4; c++) {
      db_click_h(r, c);
      db_click_v(r, c);
    }
  }
  console.log('  Phase after all edges:', db_phase(), '(expected 1=GAME_OVER)');

  console.log('\n=== All basic tests completed ===');
}

runTests().catch(console.error);
