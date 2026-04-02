const { test, expect } = require('@playwright/test');
const path = require('path');
const http = require('http');
const fs = require('fs');

test.describe('connect-four-wasm WASM game tests', () => {
    let server;

    test.beforeAll(async () => {
        server = http.createServer((req, res) => {
            let urlPath = req.url === '/' ? '/index.html' : req.url;
            let filePath = path.join(__dirname, urlPath);
            const ext = path.extname(filePath);
            const mimeTypes = {
                '.html': 'text/html',
                '.js': 'application/javascript',
                '.wasm': 'application/wasm',
            };
            fs.readFile(filePath, (err, data) => {
                if (err) {
                    res.writeHead(404);
                    res.end('Not found: ' + urlPath);
                    return;
                }
                res.writeHead(200, { 'Content-Type': mimeTypes[ext] || 'application/octet-stream' });
                res.end(data);
            });
        });
        await new Promise(resolve => server.listen(8777, resolve));
    });

    test.afterAll(async () => {
        await new Promise(resolve => server.close(resolve));
    });

    test('should load without console errors', async ({ page }) => {
        const errors = [];
        page.on('console', msg => {
            if (msg.type() === 'error') errors.push(msg.text());
        });
        page.on('pageerror', err => errors.push(err.message));

        await page.goto('http://localhost:8777/', { waitUntil: 'domcontentloaded' });
        await page.waitForTimeout(2000);

        await expect(page).toHaveTitle(/connect four/i);

        const criticalErrors = errors.filter(e =>
            !e.includes('favicon') && !e.includes('DevTools') && !e.includes('net::ERR')
        );
        expect(criticalErrors, 'Console errors: ' + criticalErrors.join('; ')).toHaveLength(0);
    });

    test('should render canvas and HUD', async ({ page }) => {
        await page.goto('http://localhost:8777/', { waitUntil: 'domcontentloaded' });
        await page.waitForTimeout(500);

        const canvas = page.locator('#canvas');
        await expect(canvas).toBeVisible();
        const box = await canvas.boundingBox();
        console.log('Canvas:', box.width, 'x', box.height);

        const status = page.locator('#status');
        await expect(status).toBeVisible();
        const initialStatus = await status.innerText();
        console.log('Initial status:', initialStatus);
    });

    test('should start game and drop discs', async ({ page }) => {
        await page.goto('http://localhost:8777/', { waitUntil: 'domcontentloaded' });
        await page.waitForTimeout(1000);

        const canvas = page.locator('#canvas');
        const box = await canvas.boundingBox();
        const cellW = box.width / 7;

        // Click to start
        await canvas.click();
        await page.waitForTimeout(500);

        const status = page.locator('#status');
        const playingStatus = await status.innerText();
        console.log('After start status:', playingStatus);

        // Drop disc in col 3
        await page.mouse.click(box.x + 3 * cellW + cellW / 2, box.y + box.height / 2);
        await page.waitForTimeout(200);

        // Drop in col 3 again (yellow)
        await page.mouse.click(box.x + 3 * cellW + cellW / 2, box.y + box.height / 2);
        await page.waitForTimeout(200);

        // Build 4-in-a-row: red at 0,1,2,3
        await page.mouse.click(box.x + 0 * cellW + cellW / 2, box.y + box.height / 2);
        await page.waitForTimeout(100);
        await page.mouse.click(box.x + 0 * cellW + cellW / 2, box.y + box.height / 2);
        await page.waitForTimeout(100);
        await page.mouse.click(box.x + 1 * cellW + cellW / 2, box.y + box.height / 2);
        await page.waitForTimeout(100);
        await page.mouse.click(box.x + 1 * cellW + cellW / 2, box.y + box.height / 2);
        await page.waitForTimeout(100);
        await page.mouse.click(box.x + 2 * cellW + cellW / 2, box.y + box.height / 2);
        await page.waitForTimeout(100);
        await page.mouse.click(box.x + 2 * cellW + cellW / 2, box.y + box.height / 2);
        await page.waitForTimeout(100);
        // Red wins by dropping in col 3 (row 5)
        await page.mouse.click(box.x + 3 * cellW + cellW / 2, box.y + box.height / 2);
        await page.waitForTimeout(500);

        const finalStatus = await status.innerText();
        console.log('Final status:', finalStatus);
    });
});
