import { chromium } from '/opt/homebrew/lib/node_modules/playwright/index.mjs';

const url = 'http://localhost:8765/index.html';

(async () => {
    const browser = await chromium.launch();
    const page = await browser.newPage();
    
    const errors = [];
    page.on('console', msg => { if (msg.type() === 'error') errors.push(msg.text()); });
    page.on('pageerror', err => errors.push(err.message));

    await page.goto(url);
    await page.waitForTimeout(3000);

    const canvas = await page.$('canvas#c');
    console.log('Canvas exists:', !!canvas);
    const title = await page.title();
    console.log('Title:', title);
    const hud = await page.$eval('#hud', el => el.innerText);
    console.log('HUD:', hud);

    // Simulate space to start
    await page.keyboard.press('Space');
    await page.waitForTimeout(1000);
    const hudAfter = await page.$eval('#hud', el => el.innerText);
    console.log('HUD after Space:', hudAfter);

    console.log('Console errors:', errors.length === 0 ? 'NONE' : errors.join(' | '));
    console.log('TEST:', errors.length === 0 ? 'PASS ✅' : 'FAIL ❌');
    await browser.close();
    process.exit(0);
})();
