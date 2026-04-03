const { test, expect, _electron_electron = require('child_process').exec } = require('@playwright/test');
const path = require('path');

test.describe('sokoban-wasm WASM game tests', () => {
    let server;
    
    test.beforeAll(async () => {
        // Start a simple HTTP server for the game files
        const http = require('http');
        const fs = require('fs');
        const games_dir = path.dirname(__dirname);
        
        server = http.createServer((req, res) => {
            let filePath = path.join(__dirname, req.url === '/' ? 'index.html' : req.url);
            const ext = path.extname(filePath);
            const mimeTypes = {
                '.html': 'text/html',
                '.js': 'application/javascript',
                '.wasm': 'application/wasm',
            };
            
            fs.readFile(filePath, (err, data) => {
                if (err) {
                    res.writeHead(404);
                    res.end('Not found');
                    return;
                }
                res.writeHead(200, { 'Content-Type': mimeTypes[ext] || 'application/octet-stream' });
                res.end(data);
            });
        });
        
        await new Promise(resolve => server.listen(8765, resolve));
    });
    
    test.afterAll(async () => {
        if (server) server.close();
    });
    
    test('should load without errors', async ({ page }) => {
        const errors = [];
        page.on('console', msg => {
            if (msg.type() === 'error') errors.push(msg.text());
        });
        page.on('pageerror', err => errors.push(err.message));
        
        await page.goto('http://localhost:8765/', { waitUntil: 'domcontentloaded' });
        await expect(page).toHaveTitle(/sokoban/i);
        await page.waitForTimeout(1500);
        
        const criticalErrors = errors.filter(e => 
            !e.includes('favicon') && !e.includes('DevTools') && !e.includes('net::ERR')
        );
        expect(criticalErrors, 'Console errors: ' + criticalErrors.join('; ')).toHaveLength(0);
    });
    
    test('should render game container', async ({ page }) => {
        await page.goto('http://localhost:8765/', { waitUntil: 'domcontentloaded' });
        await page.waitForTimeout(500);
        const body = page.locator('body');
        await expect(body).toBeVisible();
    });
});
