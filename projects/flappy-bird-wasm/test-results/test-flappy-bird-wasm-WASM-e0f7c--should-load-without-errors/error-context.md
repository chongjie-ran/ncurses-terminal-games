# Instructions

- Following Playwright test failed.
- Explain why, be concise, respect Playwright best practices.
- Provide a snippet of code with the fix, if possible.

# Test info

- Name: test.spec.js >> flappy-bird-wasm WASM game tests >> should load without errors
- Location: test.spec.js:40:5

# Error details

```
Error: expect(page).toHaveTitle(expected) failed

Expected pattern: /flappy-bird/i
Received string:  "Flappy Bird - WASM"
Timeout: 5000ms

Call log:
  - Expect "toHaveTitle" with timeout 5000ms
    9 × unexpected value "Flappy Bird - WASM"

```

# Test source

```ts
  1  | const { test, expect, _electron_electron = require('child_process').exec } = require('@playwright/test');
  2  | const path = require('path');
  3  | 
  4  | test.describe('flappy-bird-wasm WASM game tests', () => {
  5  |     let server;
  6  |     
  7  |     test.beforeAll(async () => {
  8  |         // Start a simple HTTP server for the game files
  9  |         const http = require('http');
  10 |         const fs = require('fs');
  11 |         const games_dir = path.dirname(__dirname);
  12 |         
  13 |         server = http.createServer((req, res) => {
  14 |             let filePath = path.join(__dirname, req.url === '/' ? 'index.html' : req.url);
  15 |             const ext = path.extname(filePath);
  16 |             const mimeTypes = {
  17 |                 '.html': 'text/html',
  18 |                 '.js': 'application/javascript',
  19 |                 '.wasm': 'application/wasm',
  20 |             };
  21 |             
  22 |             fs.readFile(filePath, (err, data) => {
  23 |                 if (err) {
  24 |                     res.writeHead(404);
  25 |                     res.end('Not found');
  26 |                     return;
  27 |                 }
  28 |                 res.writeHead(200, { 'Content-Type': mimeTypes[ext] || 'application/octet-stream' });
  29 |                 res.end(data);
  30 |             });
  31 |         });
  32 |         
  33 |         await new Promise(resolve => server.listen(8765, resolve));
  34 |     });
  35 |     
  36 |     test.afterAll(async () => {
  37 |         if (server) server.close();
  38 |     });
  39 |     
  40 |     test('should load without errors', async ({ page }) => {
  41 |         const errors = [];
  42 |         page.on('console', msg => {
  43 |             if (msg.type() === 'error') errors.push(msg.text());
  44 |         });
  45 |         page.on('pageerror', err => errors.push(err.message));
  46 |         
  47 |         await page.goto('http://localhost:8765/', { waitUntil: 'domcontentloaded' });
> 48 |         await expect(page).toHaveTitle(/flappy-bird/i);
     |                            ^ Error: expect(page).toHaveTitle(expected) failed
  49 |         await page.waitForTimeout(1500);
  50 |         
  51 |         const criticalErrors = errors.filter(e => 
  52 |             !e.includes('favicon') && !e.includes('DevTools') && !e.includes('net::ERR')
  53 |         );
  54 |         expect(criticalErrors, 'Console errors: ' + criticalErrors.join('; ')).toHaveLength(0);
  55 |     });
  56 |     
  57 |     test('should render game container', async ({ page }) => {
  58 |         await page.goto('http://localhost:8765/', { waitUntil: 'domcontentloaded' });
  59 |         await page.waitForTimeout(500);
  60 |         const body = page.locator('body');
  61 |         await expect(body).toBeVisible();
  62 |     });
  63 | });
  64 | 
```