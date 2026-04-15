const { test, expect } = require('@playwright/test');
const path = require('path');

test.describe('WASM Game Builder', () => {
    test('build-all.sh exists and is executable', async () => {
        const fs = require('fs');
        const buildScript = path.join(__dirname, 'build-all.sh');
        expect(fs.existsSync(buildScript)).toBe(true);
        const stats = fs.statSync(buildScript);
        expect(stats.mode & 0o111).not.toBe(0);
    });

    test('package.json has valid structure', async () => {
        const pkg = require('./package.json');
        expect(pkg.dependencies).toBeDefined();
        expect(pkg.dependencies['@playwright/test']).toBeDefined();
    });

    test('playwright.config.js is valid', async () => {
        const config = require('./playwright.config.js');
        expect(config.testDir).toBe('.');
        expect(config.testMatch).toBeTruthy();
    });
});
