const { test, expect } = require('@playwright/test');
const path = require('path');
const http = require('http');
const fs = require('fs');

test.describe('gomoku-wasm WASM game tests', () => {
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
        await new Promise(resolve => server.listen(8766, resolve));
    });

    test.afterAll(async () => {
        if (server) server.close();
    });

    test('should load without console errors', async ({ page }) => {
        const errors = [];
        page.on('console', msg => {
            if (msg.type() === 'error') errors.push(msg.text());
        });
        page.on('pageerror', err => errors.push(err.message));

        await page.goto('http://localhost:8766/', { waitUntil: 'domcontentloaded' });
        await expect(page).toHaveTitle(/gomoku/i);
        await page.waitForTimeout(2000);

        const criticalErrors = errors.filter(e =>
            !e.includes('favicon') && !e.includes('DevTools') && !e.includes('net::ERR')
        );
        expect(criticalErrors, 'Console errors: ' + criticalErrors.join('; ')).toHaveLength(0);
    });

    test('should render canvas and HUD', async ({ page }) => {
        await page.goto('http://localhost:8766/', { waitUntil: 'domcontentloaded' });
        await page.waitForTimeout(500);

        const canvas = page.locator('canvas#c');
        await expect(canvas).toBeVisible();

        const hud = page.locator('#hud');
        await expect(hud).toBeVisible();

        const playerText = await page.locator('#player-text').innerText();
        expect(playerText).toBe('Black');
    });

    test('should place stones on click and update player', async ({ page }) => {
        await page.goto('http://localhost:8766/', { waitUntil: 'domcontentloaded' });
        await page.waitForTimeout(1000);

        const canvas = page.locator('canvas#c');
        const box = await canvas.boundingBox();

        // Cell size = 32, padding = 32
        // Click at board intersection (7, 7) - roughly center
        const CELL = 32;
        const PADDING = 32;
        const clickX = box.x + PADDING + 7 * CELL;
        const clickY = box.y + PADDING + 7 * CELL;

        // First click - Black places
        await page.mouse.click(clickX, clickY);
        await page.waitForTimeout(200);

        let playerText = await page.locator('#player-text').innerText();
        expect(playerText).toBe('White');

        // Second click - White places at (8, 7)
        const clickX2 = box.x + PADDING + 8 * CELL;
        const clickY2 = box.y + PADDING + 7 * CELL;
        await page.mouse.click(clickX2, clickY2);
        await page.waitForTimeout(200);

        playerText = await page.locator('#player-text').innerText();
        expect(playerText).toBe('Black');

        // Third click - Black at (6, 7)
        const clickX3 = box.x + PADDING + 6 * CELL;
        const clickY3 = box.y + PADDING + 7 * CELL;
        await page.mouse.click(clickX3, clickY3);
        await page.waitForTimeout(200);

        playerText = await page.locator('#player-text').innerText();
        expect(playerText).toBe('White');
    });

    test('should detect win condition - 5 in a row', async ({ page }) => {
        await page.goto('http://localhost:8766/', { waitUntil: 'domcontentloaded' });
        await page.waitForTimeout(1000);

        const canvas = page.locator('canvas#c');
        const box = await canvas.boundingBox();

        const CELL = 32;
        const PADDING = 32;

        // Helper to click a board intersection
        async function clickCell(cx, cy) {
            const x = box.x + PADDING + cx * CELL;
            const y = box.y + PADDING + cy * CELL;
            await page.mouse.click(x, y);
            await page.waitForTimeout(100);
        }

        // Build a winning line for Black: (0,0)-(4,0)
        // B at (0,0), W at (0,5), B at (1,0), W at (1,5), B at (2,0), W at (2,5), B at (3,0), W at (3,5), B at (4,0)
        await clickCell(0, 0); // B
        await page.waitForTimeout(100);
        let player = await page.locator('#player-text').innerText();
        expect(player).toBe('White');

        await clickCell(0, 5); // W
        await page.waitForTimeout(100);
        player = await page.locator('#player-text').innerText();
        expect(player).toBe('Black');

        await clickCell(1, 0); // B
        await page.waitForTimeout(100);

        await clickCell(1, 5); // W
        await page.waitForTimeout(100);

        await clickCell(2, 0); // B
        await page.waitForTimeout(100);

        await clickCell(2, 5); // W
        await page.waitForTimeout(100);

        await clickCell(3, 0); // B
        await page.waitForTimeout(100);

        await clickCell(3, 5); // W
        await page.waitForTimeout(100);

        await clickCell(4, 0); // B wins!
        await page.waitForTimeout(500);

        const message = await page.locator('#message').innerText();
        expect(message).toContain('Black Wins');
    });

    test('should restart game on button click', async ({ page }) => {
        await page.goto('http://localhost:8766/', { waitUntil: 'domcontentloaded' });
        await page.waitForTimeout(1000);

        const canvas = page.locator('canvas#c');
        const box = await canvas.boundingBox();

        const CELL = 32;
        const PADDING = 32;

        // Place a few stones
        await page.mouse.click(box.x + PADDING + 7 * CELL, box.y + PADDING + 7 * CELL);
        await page.waitForTimeout(200);
        await page.mouse.click(box.x + PADDING + 8 * CELL, box.y + PADDING + 7 * CELL);
        await page.waitForTimeout(200);

        let player = await page.locator('#player-text').innerText();
        expect(player).toBe('Black');

        // Click restart
        await page.locator('#restart-btn').click();
        await page.waitForTimeout(300);

        // Player should be reset to Black
        player = await page.locator('#player-text').innerText();
        expect(player).toBe('Black');

        // Message should be empty
        const message = await page.locator('#message').innerText();
        expect(message).toBe('');
    });

    test('should reject clicking on occupied cells', async ({ page }) => {
        await page.goto('http://localhost:8766/', { waitUntil: 'domcontentloaded' });
        await page.waitForTimeout(1000);

        const canvas = page.locator('canvas#c');
        const box = await canvas.boundingBox();

        const CELL = 32;
        const PADDING = 32;

        // Click at (7,7) - Black places
        await page.mouse.click(box.x + PADDING + 7 * CELL, box.y + PADDING + 7 * CELL);
        await page.waitForTimeout(200);

        // Click same cell again - should stay White's turn (occupied)
        await page.mouse.click(box.x + PADDING + 7 * CELL, box.y + PADDING + 7 * CELL);
        await page.waitForTimeout(200);

        // Still White's turn because click was rejected
        const player = await page.locator('#player-text').innerText();
        expect(player).toBe('White');
    });
});
