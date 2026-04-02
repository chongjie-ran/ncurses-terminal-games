import { chromium } from '/opt/homebrew/lib/node_modules/playwright/index.mjs';
import { createServer } from 'http';
import { readFileSync } from 'fs';
import { join, extname } from 'path';

const PORT = 8767;
const gameDir = '/Users/chongjieran/.openclaw/workspace-developer/projects/gomoku-wasm';

// Simple HTTP server for the game
const server = createServer((req, res) => {
    const urlPath = req.url === '/' ? '/index.html' : req.url;
    const filePath = join(gameDir, urlPath);
    const mimeTypes = {
        '.html': 'text/html',
        '.js': 'application/javascript',
        '.wasm': 'application/wasm',
    };
    try {
        const data = readFileSync(filePath);
        res.writeHead(200, { 'Content-Type': mimeTypes[extname(filePath)] || 'application/octet-stream' });
        res.end(data);
    } catch {
        res.writeHead(404);
        res.end('Not found: ' + urlPath);
    }
});

await new Promise(resolve => server.listen(PORT, resolve));
console.log(`Server running on port ${PORT}`);

const browser = await chromium.launch();
const page = await browser.newPage();

const errors = [];
page.on('console', msg => { if (msg.type() === 'error') errors.push(msg.text()); });
page.on('pageerror', err => errors.push(err.message));

try {
    await page.goto(`http://localhost:${PORT}/`, { waitUntil: 'domcontentloaded' });
    await page.waitForTimeout(2000);

    // Test 1: Canvas renders
    const canvas = await page.$('canvas#c');
    console.log('Test 1 - Canvas exists:', canvas ? 'PASS ✅' : 'FAIL ❌');

    // Test 2: Title
    const title = await page.title();
    console.log('Test 2 - Title contains Gomoku:', title.includes('Gomoku') ? 'PASS ✅' : 'FAIL ❌');

    // Test 3: Player indicator shows Black
    const playerText = await page.locator('#player-text').innerText();
    console.log('Test 3 - Starts with Black:', playerText === 'Black' ? 'PASS ✅' : 'FAIL ❌');

    // Test 4: Click on board - place a stone
    const box = await canvas.boundingBox();
    const CELL = 32, PADDING = 32;

    await page.mouse.click(box.x + PADDING + 7 * CELL, box.y + PADDING + 7 * CELL);
    await page.waitForTimeout(300);

    const playerAfterFirst = await page.locator('#player-text').innerText();
    console.log('Test 4 - Player changes to White after Black places:', playerAfterFirst === 'White' ? 'PASS ✅' : 'FAIL ❌');

    // Test 5: Click on another cell - White places
    await page.mouse.click(box.x + PADDING + 8 * CELL, box.y + PADDING + 7 * CELL);
    await page.waitForTimeout(300);

    const playerAfterSecond = await page.locator('#player-text').innerText();
    console.log('Test 5 - Player changes back to Black after White places:', playerAfterSecond === 'Black' ? 'PASS ✅' : 'FAIL ❌');

    // Test 6: Restart button works
    await page.locator('#restart-btn').click();
    await page.waitForTimeout(300);
    const playerAfterRestart = await page.locator('#player-text').innerText();
    console.log('Test 6 - Restart resets to Black:', playerAfterRestart === 'Black' ? 'PASS ✅' : 'FAIL ❌');

    // Test 7: Console errors check
    const criticalErrors = errors.filter(e => !e.includes('favicon') && !e.includes('DevTools') && !e.includes('net::ERR'));
    console.log('Test 7 - No console errors:', criticalErrors.length === 0 ? 'PASS ✅' : `FAIL ❌ (${criticalErrors.join('; ')})`);

    // Overall result
    const allPassed = canvas && title.includes('Gomoku') && playerText === 'Black' &&
        playerAfterFirst === 'White' && playerAfterSecond === 'Black' &&
        playerAfterRestart === 'Black' && criticalErrors.length === 0;
    console.log('\n============================');
    console.log('Overall:', allPassed ? 'ALL TESTS PASSED ✅' : 'SOME TESTS FAILED ❌');
    console.log('============================');

} catch (e) {
    console.error('Test error:', e.message);
} finally {
    await browser.close();
    server.close();
}
