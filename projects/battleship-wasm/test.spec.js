const { test, expect } = require('@playwright/test');
const path = require('path');
const http = require('http');
const fs = require('fs');

const PORT = 18765;

test.describe('Battleship WASM tests', () => {
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
        await new Promise(resolve => server.listen(PORT, resolve));
    });

    test.afterAll(async () => {
        server.close();
    });

    test('Battleship WASM - Load and initialize', async ({ page }) => {
        await page.goto(`http://localhost:${PORT}/`);
        await page.waitForTimeout(3000);

        await expect(page.locator('h1')).toContainText('Battleship WASM');
        await expect(page.locator('#player-board')).toBeVisible();
        await expect(page.locator('#enemy-board')).toBeVisible();
        await expect(page.locator('#phase-banner')).toContainText('Place Your Ships');
        const shipBtns = page.locator('.ship-btn');
        await expect(shipBtns).toHaveCount(5);
        await expect(page.locator('#hits')).toHaveText('0');
        await expect(page.locator('#misses')).toHaveText('0');
        await expect(page.locator('#sunk')).toHaveText('0');
    });

    test('Battleship WASM - Auto place and enter battle', async ({ page }) => {
        await page.goto(`http://localhost:${PORT}/`);
        await page.waitForTimeout(3000);

        await page.locator('button:has-text("Auto Place")').click();
        await page.waitForTimeout(800);

        const banner = await page.locator('#phase-banner').textContent();
        expect(banner).toContain('Battle');
    });

    test('Battleship WASM - Shooting hits or misses', async ({ page }) => {
        await page.goto(`http://localhost:${PORT}/`);
        await page.waitForTimeout(3000);

        await page.locator('button:has-text("Auto Place")').click();
        await page.waitForTimeout(800);

        await page.locator('#e-0-0').click();
        await page.waitForTimeout(400);

        const hits = await page.locator('#hits').textContent();
        const misses = await page.locator('#misses').textContent();
        expect(parseInt(hits) + parseInt(misses)).toBeGreaterThanOrEqual(1);

        const cellClass = await page.locator('#e-0-0').getAttribute('class');
        expect(cellClass).toMatch(/hit|miss/);
    });

    test('Battleship WASM - Reset game works', async ({ page }) => {
        await page.goto(`http://localhost:${PORT}/`);
        await page.waitForTimeout(3000);

        await page.locator('button:has-text("Auto Place")').click();
        await page.waitForTimeout(500);
        await page.locator('#e-0-0').click();
        await page.waitForTimeout(300);

        await page.locator('button:has-text("Reset")').click();
        await page.waitForTimeout(400);

        await expect(page.locator('#phase-banner')).toContainText('Place Your Ships');
        await expect(page.locator('#hits')).toHaveText('0');
    });
});
