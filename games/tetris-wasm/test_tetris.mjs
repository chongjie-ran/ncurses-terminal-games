import { chromium } from 'playwright';

(async () => {
  const browser = await chromium.launch();
  const page = await browser.newPage();
  
  const errors = [];
  page.on('console', msg => {
    if (msg.type() === 'error') errors.push(msg.text());
  });
  page.on('pageerror', err => errors.push(err.message));
  
  await page.goto('http://localhost:8765/');
  // Wait for WASM to load
  await page.waitForTimeout(3000);
  
  if (errors.length > 0) {
    console.error('ERRORS:', errors);
    process.exit(1);
  }
  
  // Check canvas exists
  const canvas = await page.$('#main-canvas');
  if (!canvas) {
    console.error('Canvas not found');
    process.exit(1);
  }
  
  console.log('✅ Tetris WASM loads without errors');
  await browser.close();
  process.exit(0);
})();
