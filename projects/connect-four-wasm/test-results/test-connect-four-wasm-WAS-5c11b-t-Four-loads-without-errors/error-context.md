# Instructions

- Following Playwright test failed.
- Explain why, be concise, respect Playwright best practices.
- Provide a snippet of code with the fix, if possible.

# Test info

- Name: test.spec.js >> connect-four-wasm WASM game tests >> Connect Four loads without errors
- Location: test.spec.js:36:5

# Error details

```
ReferenceError: chromium is not defined
```

# Test source

```ts
  1  | const { test, expect } = require('@playwright/test');
  2  | const path = require('path');
  3  | const http = require('http');
  4  | const fs = require('fs');
  5  | 
  6  | test.describe('connect-four-wasm WASM game tests', () => {
  7  |     let server;
  8  | 
  9  |     test.beforeAll(async () => {
  10 |         server = http.createServer((req, res) => {
  11 |             let urlPath = req.url === '/' ? '/index.html' : req.url;
  12 |             let filePath = path.join(__dirname, urlPath);
  13 |             const ext = path.extname(filePath);
  14 |             const mimeTypes = {
  15 |                 '.html': 'text/html',
  16 |                 '.js': 'application/javascript',
  17 |                 '.wasm': 'application/wasm',
  18 |             };
  19 |             fs.readFile(filePath, (err, data) => {
  20 |                 if (err) {
  21 |                     res.writeHead(404);
  22 |                     res.end('Not found: ' + urlPath);
  23 |                     return;
  24 |                 }
  25 |                 res.writeHead(200, { 'Content-Type': mimeTypes[ext] || 'application/octet-stream' });
  26 |                 res.end(data);
  27 |             });
  28 |         });
  29 |         await new Promise(resolve => server.listen(8777, resolve));
  30 |     });
  31 | 
  32 |     test.afterAll(async () => {
  33 |         await new Promise(resolve => server.close(resolve));
  34 |     });
  35 | 
  36 |     test('Connect Four loads without errors', async () => {
> 37 |         const browser = await chromium.launch();
     |                         ^ ReferenceError: chromium is not defined
  38 |         const page = await browser.newPage();
  39 |         const errors = [];
  40 |         page.on('console', msg => {
  41 |             if (msg.type() === 'error') errors.push(msg.text());
  42 |         });
  43 | 
  44 |         await page.goto('http://localhost:8777/');
  45 |         await page.waitForTimeout(2000);
  46 | 
  47 |         const title = await page.title();
  48 |         console.log('Title:', title);
  49 |         expect(title).toBe('Connect Four - WASM');
  50 | 
  51 |         const canvas = page.locator('#canvas');
  52 |         await expect(canvas).toBeVisible();
  53 |         const canvasBox = await canvas.boundingBox();
  54 |         console.log('Canvas size:', canvasBox.width, 'x', canvasBox.height);
  55 | 
  56 |         const status = page.locator('#status');
  57 |         const statusText = await status.textContent();
  58 |         console.log('Initial status:', statusText);
  59 | 
  60 |         // Click to start game
  61 |         await canvas.click();
  62 |         await page.waitForTimeout(500);
  63 | 
  64 |         const statusPlaying = await status.textContent();
  65 |         console.log('Status after click:', statusPlaying);
  66 | 
  67 |         // Drop discs in columns 0-3 for red player
  68 |         const cellW = canvasBox.width / 7;
  69 |         await page.mouse.click(canvasBox.x + 1 * cellW + cellW / 2, canvasBox.y + canvasBox.height / 2);
  70 |         await page.waitForTimeout(200);
  71 |         await page.mouse.click(canvasBox.x + 1 * cellW + cellW / 2, canvasBox.y + canvasBox.height / 2);
  72 |         await page.waitForTimeout(200);
  73 |         await page.mouse.click(canvasBox.x + 2 * cellW + cellW / 2, canvasBox.y + canvasBox.height / 2);
  74 |         await page.waitForTimeout(200);
  75 |         await page.mouse.click(canvasBox.x + 2 * cellW + cellW / 2, canvasBox.y + canvasBox.height / 2);
  76 |         await page.waitForTimeout(200);
  77 |         await page.mouse.click(canvasBox.x + 3 * cellW + cellW / 2, canvasBox.y + canvasBox.height / 2);
  78 |         await page.waitForTimeout(200);
  79 |         await page.mouse.click(canvasBox.x + 3 * cellW + cellW / 2, canvasBox.y + canvasBox.height / 2);
  80 |         await page.waitForTimeout(200);
  81 |         // Red drops col 3 and wins
  82 |         await page.mouse.click(canvasBox.x + 4 * cellW + cellW / 2, canvasBox.y + canvasBox.height / 2);
  83 |         await page.waitForTimeout(500);
  84 | 
  85 |         const finalStatus = await status.textContent();
  86 |         console.log('Final status:', finalStatus);
  87 | 
  88 |         console.log('Console errors:', errors);
  89 |         expect(errors.length).toBe(0);
  90 | 
  91 |         await browser.close();
  92 |     });
  93 | });
  94 | 
```