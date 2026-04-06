const { test, expect } = require('@playwright/test');

test('Brick Breaker WASM - loads and renders without errors', async ({ page }) => {
  const errors = [];
  page.on('pageerror', e => errors.push(e.message));
  page.on('console', msg => {
    if (msg.type() === 'error') errors.push(msg.text());
  });

  await page.goto('http://localhost:8765/index.html');
  await page.waitForTimeout(2000);

  const canvas = page.locator('#gameCanvas');
  await expect(canvas).toBeVisible();

  const pageContent = await page.content();
  expect(pageContent).toContain('Score:');

  const criticalErrors = errors.filter(e =>
    !e.includes('warning') &&
    !e.includes('CORS') &&
    !e.includes('Cross-Origin')
  );
  expect(criticalErrors).toHaveLength(0);
  console.log('Brick Breaker WASM: PASS');
});

test('Brick Breaker WASM - game functions exported correctly', async ({ page }) => {
  await page.goto('http://localhost:8765/index.html');
  await page.waitForTimeout(2000);

  const gameReady = await page.evaluate(() =>
    typeof Module !== 'undefined' &&
    Module._restart !== undefined &&
    Module._update_game !== undefined &&
    Module._get_score !== undefined &&
    Module._get_lives !== undefined &&
    Module._is_game_over !== undefined
  );

  expect(gameReady).toBe(true);
  console.log('Brick Breaker WASM functions: PASS');
});
