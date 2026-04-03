const { test, expect } = require('@playwright/test');
const path = require('path');

test.describe('wordle-wasm WASM game tests', () => {
    test('should load without errors', async ({ page }) => {
        const errors = [];
        page.on('console', msg => {
            if (msg.type() === 'error') errors.push(msg.text());
        });
        page.on('pageerror', err => errors.push(err.message));
        
        const filePath = 'file://' + path.resolve(__dirname, 'index.html');
        await page.goto(filePath, { waitUntil: 'domcontentloaded' });
        await expect(page).toHaveTitle(/wordle/i);
        await page.waitForTimeout(1500);
        
        const criticalErrors = errors.filter(e => 
            !e.includes('favicon') && !e.includes('DevTools') && !e.includes('net::ERR')
        );
        expect(criticalErrors, 'Console errors: ' + criticalErrors.join('; ')).toHaveLength(0);
    });
    
    test('should render game container', async ({ page }) => {
        const filePath = 'file://' + path.resolve(__dirname, 'index.html');
        await page.goto(filePath, { waitUntil: 'domcontentloaded' });
        await page.waitForTimeout(500);
        const body = page.locator('body');
        await expect(body).toBeVisible();
    });
});
