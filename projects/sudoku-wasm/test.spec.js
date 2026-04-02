const { test, expect } = require('@playwright/test');
const path = require('path');
const http = require('http');
const fs = require('fs');

test.describe('Sudoku WASM tests', () => {
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
        await new Promise(resolve => server.listen(8778, resolve));
    });

    test.afterAll(async () => {
        server.close();
    });

    test('loads and starts Easy game', async ({ page }) => {
        const errors = [];
        page.on('pageerror', e => errors.push(e.message));

        await page.goto('http://localhost:8778/');
        await page.waitForFunction(() => window.sudokuReady === true, { timeout: 10000 });
        await page.waitForTimeout(500);

        const title = await page.textContent('h1');
        console.log('Title:', title);
        expect(title).toBe('SUDOKU');

        // Click Easy difficulty
        await page.click('.diff-btn.easy');
        await page.waitForTimeout(2000);

        // Check diff label
        const diffLabel = await page.textContent('#diffLabel');
        console.log('Difficulty:', diffLabel);
        expect(diffLabel).toBe('Easy');

        // Canvas visible
        const canvas = page.locator('#board');
        await expect(canvas).toBeVisible();

        // Click a cell (non-fixed area - bottom right)
        await canvas.click({ position: { x: 400, y: 400 } });
        await page.waitForTimeout(300);

        // Input a digit
        await page.keyboard.press('5');
        await page.waitForTimeout(300);

        // Check errors display
        const errorsTxt = await page.textContent('#errors');
        console.log('Errors count:', errorsTxt);

        // Toggle candidates
        await page.click('button:has-text("Candidates")');
        await page.waitForTimeout(300);

        // Hint button
        await page.click('button:has-text("Hint")');
        await page.waitForTimeout(300);
        const hintsUsed = await page.textContent('#hints');
        console.log('Hints used:', hintsUsed);

        // Timer is running
        const timer = await page.textContent('#timer');
        console.log('Timer:', timer);
        expect(timer).toMatch(/\d+:\d+/);

        // Critical JS errors
        const critical = errors.filter(e => !e.includes('warning') && !e.includes('Warning'));
        console.log('Critical errors:', critical);
        expect(critical.length).toBe(0);
    });

    test('starts different difficulty levels', async ({ page }) => {
        await page.goto('http://localhost:8778/');
        await page.waitForFunction(() => window.sudokuReady === true, { timeout: 10000 });
        await page.waitForTimeout(500);

        const difficulties = [
            { btn: '.diff-btn.medium', label: 'Medium' },
            { btn: '.diff-btn.hard', label: 'Hard' },
            { btn: '.diff-btn.expert', label: 'Expert' },
        ];

        for (const { btn, label } of difficulties) {
            await page.click(btn);
            await page.waitForTimeout(2000);
            const diff = await page.textContent('#diffLabel');
            console.log('Selected:', diff);
            expect(diff).toBe(label);
            // Go back to menu
            await page.click('button:has-text("Menu")');
            await page.waitForTimeout(300);
        }
    });

    test('solve button completes puzzle', async ({ page }) => {
        const errors = [];
        page.on('pageerror', e => errors.push(e.message));

        await page.goto('http://localhost:8778/');
        await page.waitForFunction(() => window.sudokuReady === true, { timeout: 10000 });
        await page.waitForTimeout(500);

        await page.click('.diff-btn.easy');
        await page.waitForTimeout(3000);

        // Click solve
        await page.click('button:has-text("Solve")');
        await page.waitForTimeout(5000);

        // Should show solved message
        const msg = await page.textContent('#msg');
        console.log('Message:', msg);
        expect(msg).toContain('Solved');

        const critical = errors.filter(e => !e.includes('warning'));
        expect(critical.length).toBe(0);
    });
});
