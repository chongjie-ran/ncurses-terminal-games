import { chromium } from '/opt/homebrew/lib/node_modules/playwright/index.mjs';
import { createServer } from 'http';
import { readFileSync } from 'fs';
import { fileURLToPath } from 'url';
import { dirname, join } from 'path';

const __dirname = dirname(fileURLToPath(import.meta.url));

const server = createServer((req, res) => {
  try {
    const filePath = join(__dirname, req.url === '/' ? 'index.html' : req.url);
    const content = readFileSync(filePath);
    const ext = filePath.split('.').pop();
    const mimeTypes = { html: 'text/html', js: 'application/javascript', wasm: 'application/wasm' };
    res.writeHead(200, { 'Content-Type': mimeTypes[ext] || 'text/plain' });
    res.end(content);
  } catch (e) {
    res.writeHead(404);
    res.end('Not found');
  }
});

server.listen(8765, async () => {
  const browser = await chromium.launch();
  const page = await browser.newPage();
  
  const errors = [];
  page.on('pageerror', err => errors.push(err.message));
  page.on('console', msg => { if (msg.type() === 'error') errors.push(msg.text()); });
  
  try {
    await page.goto('http://localhost:8765', { waitUntil: 'networkidle', timeout: 10000 });
    await page.waitForTimeout(2000);
    
    const title = await page.title();
    const gameInfo = await page.$eval('#gameInfo', el => el.textContent).catch(() => 'N/A');
    const board = await page.$eval('#board', el => el.textContent).catch(() => 'N/A');
    const humanHand = await page.$eval('#humanHand', el => el.textContent).catch(() => 'N/A');
    
    console.log('=== Dominoes WASM Test ===');
    console.log('Title:', title);
    console.log('Game Info:', gameInfo);
    console.log('Board:', board);
    console.log('Human Hand:', humanHand ? humanHand.substring(0, 100) + '...' : 'N/A');
    console.log('Errors:', errors.length ? errors : 'None');
    console.log('Status:', errors.length === 0 ? 'PASS' : 'FAIL');
  } catch (e) {
    console.error('Test error:', e.message);
  }
  
  await browser.close();
  server.close();
  process.exit(errors.length ? 1 : 0);
});
