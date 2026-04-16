# Instructions

- Following Playwright test failed.
- Explain why, be concise, respect Playwright best practices.
- Provide a snippet of code with the fix, if possible.

# Test info

- Name: test.spec.js >> Battleship WASM tests >> Battleship WASM - Shooting hits or misses
- Location: test.spec.js:64:5

# Error details

```
Test timeout of 30000ms exceeded.
```

```
Error: locator.click: Test timeout of 30000ms exceeded.
Call log:
  - waiting for locator('#e-0-0')

```

# Test source

```ts
  1  | const { test, expect } = require('@playwright/test');
  2  | const path = require('path');
  3  | const http = require('http');
  4  | const fs = require('fs');
  5  | 
  6  | const PORT = 18765;
  7  | 
  8  | test.describe('Battleship WASM tests', () => {
  9  |     let server;
  10 | 
  11 |     test.beforeAll(async () => {
  12 |         server = http.createServer((req, res) => {
  13 |             let urlPath = req.url === '/' ? '/index.html' : req.url;
  14 |             let filePath = path.join(__dirname, urlPath);
  15 |             const ext = path.extname(filePath);
  16 |             const mimeTypes = {
  17 |                 '.html': 'text/html',
  18 |                 '.js': 'application/javascript',
  19 |                 '.wasm': 'application/wasm',
  20 |             };
  21 |             fs.readFile(filePath, (err, data) => {
  22 |                 if (err) {
  23 |                     res.writeHead(404);
  24 |                     res.end('Not found: ' + urlPath);
  25 |                     return;
  26 |                 }
  27 |                 res.writeHead(200, { 'Content-Type': mimeTypes[ext] || 'application/octet-stream' });
  28 |                 res.end(data);
  29 |             });
  30 |         });
  31 |         await new Promise(resolve => server.listen(PORT, resolve));
  32 |     });
  33 | 
  34 |     test.afterAll(async () => {
  35 |         server.close();
  36 |     });
  37 | 
  38 |     test('Battleship WASM - Load and initialize', async ({ page }) => {
  39 |         await page.goto(`http://localhost:${PORT}/`);
  40 |         await page.waitForTimeout(3000);
  41 | 
  42 |         await expect(page.locator('h1')).toContainText('Battleship WASM');
  43 |         await expect(page.locator('#player-board')).toBeVisible();
  44 |         await expect(page.locator('#enemy-board')).toBeVisible();
  45 |         await expect(page.locator('#phase-banner')).toContainText('Place Your Ships');
  46 |         const shipBtns = page.locator('.ship-btn');
  47 |         await expect(shipBtns).toHaveCount(5);
  48 |         await expect(page.locator('#hits')).toHaveText('0');
  49 |         await expect(page.locator('#misses')).toHaveText('0');
  50 |         await expect(page.locator('#sunk')).toHaveText('0');
  51 |     });
  52 | 
  53 |     test('Battleship WASM - Auto place and enter battle', async ({ page }) => {
  54 |         await page.goto(`http://localhost:${PORT}/`);
  55 |         await page.waitForTimeout(3000);
  56 | 
  57 |         await page.locator('button:has-text("Auto Place")').click();
  58 |         await page.waitForTimeout(800);
  59 | 
  60 |         const banner = await page.locator('#phase-banner').textContent();
  61 |         expect(banner).toContain('Battle');
  62 |     });
  63 | 
  64 |     test('Battleship WASM - Shooting hits or misses', async ({ page }) => {
  65 |         await page.goto(`http://localhost:${PORT}/`);
  66 |         await page.waitForTimeout(3000);
  67 | 
  68 |         await page.locator('button:has-text("Auto Place")').click();
  69 |         await page.waitForTimeout(800);
  70 | 
> 71 |         await page.locator('#e-0-0').click();
     |                                      ^ Error: locator.click: Test timeout of 30000ms exceeded.
  72 |         await page.waitForTimeout(400);
  73 | 
  74 |         const hits = await page.locator('#hits').textContent();
  75 |         const misses = await page.locator('#misses').textContent();
  76 |         expect(parseInt(hits) + parseInt(misses)).toBeGreaterThanOrEqual(1);
  77 | 
  78 |         const cellClass = await page.locator('#e-0-0').getAttribute('class');
  79 |         expect(cellClass).toMatch(/hit|miss/);
  80 |     });
  81 | 
  82 |     test('Battleship WASM - Reset game works', async ({ page }) => {
  83 |         await page.goto(`http://localhost:${PORT}/`);
  84 |         await page.waitForTimeout(3000);
  85 | 
  86 |         await page.locator('button:has-text("Auto Place")').click();
  87 |         await page.waitForTimeout(500);
  88 |         await page.locator('#e-0-0').click();
  89 |         await page.waitForTimeout(300);
  90 | 
  91 |         await page.locator('button:has-text("Reset")').click();
  92 |         await page.waitForTimeout(400);
  93 | 
  94 |         await expect(page.locator('#phase-banner')).toContainText('Place Your Ships');
  95 |         await expect(page.locator('#hits')).toHaveText('0');
  96 |     });
  97 | });
  98 | 
```