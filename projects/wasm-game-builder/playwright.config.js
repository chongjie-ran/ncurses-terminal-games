module.exports = {
  testDir: '.',
  testMatch: '**/test.spec.js',
  timeout: 30000,
  use: {
    headless: true,
    viewport: { width: 800, height: 600 },
  },
  projects: [],
};
