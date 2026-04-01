// Playwright test for Mahjong Solitaire WASM
import pkg from '/Users/chongjieran/.openclaw/workspace-developer/projects/wasm-game-builder/node_modules/@playwright/test/index.js';
const { chromium } = pkg;
import { createServer } from 'http';
import { readFileSync } from 'fs';
import { join, extname } from 'path';

const PORT = 8766;
const DIR = '.';

const MIME = {
    '.html': 'text/html',
    '.js': 'application/javascript',
    '.wasm': 'application/wasm'
};

async function startServer(port) {
    return new Promise(resolve => {
        const server = createServer((req, res) => {
            const file = join(DIR, req.url === '/' ? 'index.html' : req.url);
            try {
                const data = readFileSync(file);
                const ext = extname(file);
                res.writeHead(200, {
                    'Content-Type': MIME[ext] || 'application/octet-stream',
                    'Cross-Origin-Opener-Policy': 'same-origin',
                    'Cross-Origin-Embedder-Policy': 'require-corp'
                });
                res.end(data);
            } catch {
                res.writeHead(404);
                res.end('Not found');
            }
        });
        server.listen(port, () => resolve(server));
    });
}

async function main() {
    const server = await startServer(PORT);
    console.log(`Server started on port ${PORT}`);
    
    const browser = await chromium.launch({ headless: true });
    const page = await browser.newPage();
    
    const errors = [];
    page.on('console', msg => {
        if (msg.type() === 'error') errors.push(msg.text());
    });
    page.on('pageerror', err => errors.push(err.message));
    
    await page.goto(`http://localhost:${PORT}/`);
    await page.waitForTimeout(2000);
    
    const title = await page.title();
    console.log('Title:', title);
    
    const canvas = await page.$('canvas#game');
    console.log('Canvas found:', !!canvas);
    
    const score = await page.$eval('#score', el => el.textContent);
    const tiles = await page.$eval('#tiles', el => el.textContent);
    console.log('Score:', score);
    console.log('Tiles:', tiles);
    
    if (errors.length > 0) {
        console.log('Console errors:', errors);
    } else {
        console.log('Console errors: NONE ✅');
    }
    
    await browser.close();
    server.close();
    
    // Summary
    console.log('\n=== Test Summary ===');
    console.log('Title: ' + (title.includes('Mahjong') ? '✅' : '❌'));
    console.log('Canvas: ' + (canvas ? '✅' : '❌'));
    console.log('Score display: ' + (score !== undefined ? '✅' : '❌'));
    console.log('Tiles display: ' + (tiles !== undefined ? '✅' : '❌'));
    console.log('Console errors: ' + (errors.length === 0 ? 'NONE ✅' : errors.join('; ')));
    
    process.exit(errors.length > 0 ? 1 : 0);
}

main().catch(e => { console.error(e); process.exit(1); });
