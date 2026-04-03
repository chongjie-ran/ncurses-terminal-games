const { test, expect } = require("@playwright/test");

test("Connect Four AI WASM loads and plays", async ({ page }) => {
  const errors = [];
  page.on("console", msg => { if (msg.type() === "error") errors.push(msg.text()); });
  page.on("pageerror", err => errors.push(err.message));
  
  await page.goto("http://localhost:18778/index.html");
  
  // Wait for WASM to load (gameLoaded becomes true)
  await page.waitForFunction(() => {
    const status = document.getElementById("status");
    return status && status.textContent === "Choose game mode";
  }, { timeout: 8000 });
  console.log("WASM loaded");
  
  const title = await page.textContent("h1");
  expect(title).toContain("Connect Four");
  
  // Click vs AI Easy
  await page.click("text=vs AI");
  await page.click("text=Easy");
  await page.waitForTimeout(400);
  
  await expect(page.locator("#canvas")).toBeVisible();
  await expect(page.locator("#status")).toContainText("Red");
  
  // Click on canvas (col 3)
  const canvas = page.locator("#canvas");
  const canvasBox = await canvas.boundingBox();
  await page.mouse.click(canvasBox.x + 3 * 70 + 35, canvasBox.y + 300);
  await page.waitForTimeout(1200);
  
  console.log("Status after move:", await page.textContent("#status"));
  console.log("Console errors:", errors);
  expect(errors.length).toBe(0);
});
