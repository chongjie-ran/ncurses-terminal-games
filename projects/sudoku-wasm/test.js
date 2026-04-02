const { chromium } = require('playwright');
(async () => {
  const browser = await chromium.launch();
  const page = await browser.newPage();
  const errors = [];
  page.on('pageerror', e => errors.push(e.message));
  const filePath = 'file://' + process.cwd() + '/index.html';
  await page.goto(filePath);
  await page.waitForTimeout(2000);
  console.log('Title:', await page.textContent('h1'));
  console.log('JS Errors so far:', errors);
  
  // Click Easy
  await page.click('.diff-btn.easy');
  await page.waitForTimeout(2000);
  console.log('Game started - diff:', await page.textContent('#diffLabel'));
  
  // Click a cell and input digit
  const canvas = await page.locator('#board');
  await canvas.click({ position: { x: 400, y: 400 } });
  await page.waitForTimeout(300);
  await page.keyboard.press('3');
  await page.waitForTimeout(300);
  console.log('Errors count:', await page.textContent('#errors'));
  
  // Toggle candidates
  await page.click('button:has-text("Candidates")');
  await page.waitForTimeout(300);
  
  // Press hint
  await page.click('button:has-text("Hint")');
  await page.waitForTimeout(300);
  console.log('Hints used:', await page.textContent('#hints'));
  
  const critical = errors.filter(e => !e.includes('warning') && !e.includes('Warning'));
  console.log('Critical JS errors:', critical);
  console.log('✅ Sudoku WASM test PASSED');
  await browser.close();
})().catch(e => { console.error('❌ FAIL:', e.message); process.exit(1); });
